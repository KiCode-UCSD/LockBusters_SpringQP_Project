/*
 * DIYables WebApp Library - Custom WebApp
 *
 * Setup:
 * 1. Change WiFi name and password below
 * 2. Upload to your Arduino
 * 3. Open Serial Monitor to see IP address
 * 4. Go to http://[IP_ADDRESS]/custom in your browser
 */

#include <DIYables_ESP32_Platform.h>
#include <DIYablesWebApps.h>
#include <EMailSender.h>
#include <FastLED.h>
#include <SPI.h>
#include <MFRC522.h>

#include "CustomWebApp.h"

// CHANGE THESE TO YOUR WIFI DETAILS
const char WIFI_SSID[] = "Minh";
const char WIFI_PASSWORD[] = "12345678";

// Create server and pages
ESP32ServerFactory serverFactory;
DIYablesWebAppServer webAppsServer(serverFactory, 80, 81);
DIYablesHomePage homePage;
CustomWebAppPage customPage;

// ************** KI VARIABLES ************** //
//using my personal email to send messages
//(ucsd emails have too many restrictions to work)
const char* SENDER_EMAIL      = "keeper.of.keys777@gmail.com";
const char* SENDER_EMAIL_NAME = "Team LockBusters";
const char* SENDER_PASS       = "aeex gnxo erix lzcc";
static String RECEIVER_EMAIL  = "";

// Create EMailSender with Gmail SMTP settings
EMailSender emailSend(SENDER_EMAIL, SENDER_PASS,
                      SENDER_EMAIL, SENDER_EMAIL_NAME,
                      "smtp.gmail.com", 465);  // Port 465 for WifiClientSecure

//custom payload signatures
const String ALARM_DATA = "ALARM_TOGGLE:"; //receive on/off signal from webapp
const String EMAIL_DATA = "EMAIL_UPDATE:"; //update receiver email based on input from webapp

// ************** MINH & DANIEL VARIABLES ************** //
//  PIN DEFINITIONS (ESP32)
#define LED_PIN1      12
#define LED_PIN2      13
#define NUM_LEDS      10
#define LED_TYPE      WS2812B
#define COLOR_ORDER   GRB
#define BUZZER_PIN    23    // Change to your buzzer pin
#define VIB_PIN       15     // SW-420
#define SS_PIN        5
#define RST_PIN       4
#define SCK_PIN       18
#define MISO_PIN      21
#define MOSI_PIN      19

//  AUTHORIZED CARD UIDs
const String AUTHORIZED_UIDS[] = {
    "04 3F 43 5A FB 76 80" // ← replace with your card UID
     // ← add more if needed
};
const int NUM_AUTHORIZED = sizeof(AUTHORIZED_UIDS) / sizeof(AUTHORIZED_UIDS[0]);
const String MASTER_UIDS = "0A 0B 0C 0D";

//  CONFIGURATION
#define WINDOW_MS         30000
#define HIT_THRESHOLD     300

#define BLINK_INTERVAL_MS 200
#define BUZZER_FREQ       1000
#define BUZZER_ON_MS      100
#define BUZZER_OFF_MS     100

//  GLOBALS
CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];
MFRC522 rfid(SS_PIN, RST_PIN);

int hitCount   = 0; //Ki: removed "volatile" since you can't increment volatile vars
bool alarmTriggered     = false;
bool alarmDisarmed      = false;
bool systemOn           = true;
bool vibrationWarningSent     = false; //keeps track of if warning email has been sent already (prevents multiple repeat emails)
bool vibrationWarningCleared  = false; //keeps track of when email notifying cleared warning has sent
bool badUserWarningSent       = false; //keeps track of when to send warning email
bool badUserWarningCleared    = false; //keeps track of when email notifying cleared warning has sent

unsigned long windowStart   = 0;
unsigned long lastBlinkTime = 0;
bool ledsOn                 = false;
int unauthorizedcount       = 0;

// ******** HELPER FUNCTIONS ************ //
// KI HELPERS
void checkEmailValid(EMailSender::Response resp) {
  Serial.println("Response Status:");
  Serial.print("  Code: ");
  Serial.println(resp.code);
  Serial.print("  Description: ");
  Serial.println(resp.desc);
  Serial.print("  Success: ");
  Serial.println(resp.status ? "YES" : "NO");

  if (resp.status) {
      Serial.println("\n Email sent successfully!");
  } else {
      Serial.println("\n Email sending failed!");
      Serial.println("\nTroubleshooting:");
      Serial.println("- Check if your SMTP server accepts non-encrypted connections");
      Serial.println("- Verify port number (25 for standard SMTP, or server-specific)");
      Serial.println("- Ensure firewall allows outgoing SMTP traffic");
      Serial.println("- Some providers block port 25 - try alternative ports");
  }
}

// MINH & DANIEL HELPERS
void IRAM_ATTR onVibration() {
  hitCount++;
}

void buzzerOn() {
  ledcAttach(BUZZER_PIN, BUZZER_FREQ, 8);  // pin, freq, resolution
  ledcWrite(BUZZER_PIN, 128);              // 50% duty cycle = sound
}

void buzzerOff() {
  ledcWrite(BUZZER_PIN, 0);               // 0 duty = silent
}

void clearAlarm() {
  alarmTriggered    = false;
  ledsOn            = false;
  unauthorizedcount = 0;
  buzzerOff();
  fill_solid(leds1, NUM_LEDS, CRGB::Black);
  fill_solid(leds2, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void flashGreen(int times) {
  for (int i = 0; i < times; i++) {
    fill_solid(leds1, NUM_LEDS, CRGB::Green);
    fill_solid(leds2, NUM_LEDS, CRGB::Green);
    FastLED.show(); delay(150);
    fill_solid(leds1, NUM_LEDS, CRGB::Black);
    fill_solid(leds2, NUM_LEDS, CRGB::Black);
    FastLED.show(); delay(150);
  }
}

void flashRed(int times) {
  for (int i = 0; i < times; i++) {
    fill_solid(leds1, NUM_LEDS, CRGB::Red);
    fill_solid(leds2, NUM_LEDS, CRGB::Red);
    FastLED.show(); delay(150);
    fill_solid(leds1, NUM_LEDS, CRGB::Black);
    fill_solid(leds2, NUM_LEDS, CRGB::Black);
    FastLED.show(); delay(150);
  }
}

String getCardUID() {
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) uid += " ";
  }
  uid.toUpperCase();
  return uid;
}

bool isAuthorized(String uid) {
  for (int i = 0; i < NUM_AUTHORIZED; i++) {
    if (uid == AUTHORIZED_UIDS[i]) return true;
  }
  return false;
}

// ***************** SETUP ***************** //
void setup() {
  Serial.begin(9600);
  delay(1000);

  // ************* WEBAPP SETUP ************* //
  Serial.println("Starting Custom WebApp...");

  // Add pages to server
  webAppsServer.addApp(&homePage);
  webAppsServer.addApp(&customPage);

  // Start WiFi and web server
  if (!webAppsServer.begin(WIFI_SSID, WIFI_PASSWORD)) {
    while (1) {
      Serial.println("Failed to connect to WiFi!");
      delay(1000);
    }
  }
 
  // Set up what happens when web page sends commands
  customPage.onCustomMessageReceived([](const String& payload) {
    Serial.println("Received: " + payload);

    // Implement your message handling here
    //payload, ALARM_DATA = "ALARM_TOGGLE:ON" or "ALARM_TOGGLE:OFF"
    if (payload.startsWith(ALARM_DATA)) {
      String alarmData = payload.substring(ALARM_DATA.length()); //remove ALARM_TOGGLE


      if (alarmData.startsWith("ON")) {
        //turn on alarm
        flashRed(2);
        alarmDisarmed = false;
      }
      else if (alarmData.startsWith("OFF")) {
        //turn off alarm
        alarmTriggered = false;
        alarmDisarmed = true;
        flashGreen(3);
        buzzerOff();
        clearAlarm();
      }
      else {
        Serial.println("Corrupted Alarm Data Payload.");
      }
    }

    //payload, EMAIL_DATA = "EMAIL_UPDATE:abc@gmail.com"
    else if (payload.startsWith(EMAIL_DATA)) {
      RECEIVER_EMAIL = payload.substring(EMAIL_DATA.length()); //set receiver email to one received from payload

      //send email notification upon setup
      // Prepare email message
      EMailSender::EMailMessage message;
      message.subject = "Team LockBusters Alarm Notification System";
      message.message = "Hello! Your email has been successfully updated. You will now receive email alerts when your scooter is in danger.";
      message.mime = MIME_TEXT_PLAIN;
   
      // Send email
      Serial.println("\nSending email via simple SMTP...");
      Serial.println("RECEIVER_EMAIL: " + RECEIVER_EMAIL);
      EMailSender::Response resp = emailSend.send(RECEIVER_EMAIL, message);
   
      // Check result
      checkEmailValid(resp);
    }
    // For now, just echo the message back
    customPage.sendToWeb("Received: " + payload);
  });

  Serial.println("Custom WebApp ready!");
  customPage.sendToWeb("Arduino is ready!");

  // ************* ALARM SYSTEM SETUP ************* //
  // ── Buzzer ────────────────────────────────
  ledcAttach(BUZZER_PIN, BUZZER_FREQ, 8);
  buzzerOff();

  // ── Vibration sensor ──────────────────────
  pinMode(VIB_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(VIB_PIN), onVibration, FALLING);

  // ── RFID — set custom SPI pins before init ─
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  rfid.PCD_Init();

  // Check RFID connection
  byte ver = rfid.PCD_ReadRegister(MFRC522::VersionReg);
  if (ver == 0x00 || ver == 0xFF) {
    Serial.println("ERROR: RFID not detected! Check wiring.");
  } else {
    Serial.print("RFID OK — firmware v");
    Serial.println(ver, HEX);
  }

  // ── LED strip ─────────────────────────────
  FastLED.addLeds<LED_TYPE, LED_PIN1, COLOR_ORDER>(leds1, NUM_LEDS)
         .setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, LED_PIN2, COLOR_ORDER>(leds2, NUM_LEDS)
         .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(80);
  flashGreen(2);

  windowStart = millis();
  Serial.println("Alarm system ready.");
}

// ***************** LOOP ***************** //
void loop() {
  // Handle web server
  webAppsServer.loop();

  unsigned long now = millis();

  // ── 1. Check RFID ─────────────────────────
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String uid = getCardUID();
    Serial.print("Card scanned: ");
    Serial.println(uid);
    if (uid == MASTER_UIDS ){
      systemOn = !systemOn;        
      buzzerOff();                    
      Serial.println(systemOn ? "System ON" : "System OFF");
    }
    if (isAuthorized(uid)) {
      Serial.println("Authorized — alarm disarmed!");
      clearAlarm();
      alarmDisarmed = true;  
      flashGreen(3);

      if (badUserWarningCleared == false) {
        // Prepare email message
        EMailSender::EMailMessage message;
        message.subject = "Team LockBusters - Unauthorized User Cleared!";
        message.message = "Authorized user has cleared unauthorized access. Your scooter is now safe.";
        message.mime = MIME_TEXT_PLAIN;
     
        // Send email
        Serial.println("\nSending email via simple SMTP...");
        Serial.println("RECEIVER_EMAIL: " + RECEIVER_EMAIL);
        EMailSender::Response resp = emailSend.send(RECEIVER_EMAIL, message);
     
        // Check result
        checkEmailValid(resp);

        badUserWarningCleared = true;
      }
    } else {
      Serial.println("Unauthorized card.");
      flashRed(2);
      unauthorizedcount++;
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
  if (!systemOn) {
    return;
  }
  if ( unauthorizedcount > 5 && !alarmTriggered) {
    alarmTriggered = true;
    Serial.println("!! ALARM — Please scan authorized cards");

    if (badUserWarningSent == false) {
        // Prepare email message
        EMailSender::EMailMessage message;
        message.subject = "Team LockBusters - Unauthorized User Detected!";
        message.message = "Unauthorized user has attempted to access your scooter! Please check your scooter's safety as soon as possible.";
        message.mime = MIME_TEXT_PLAIN;
     
        // Send email
        Serial.println("\nSending email via simple SMTP...");
        Serial.println("RECEIVER_EMAIL: " + RECEIVER_EMAIL);
        EMailSender::Response resp = emailSend.send(RECEIVER_EMAIL, message);
     
        // Check result
        checkEmailValid(resp);

        badUserWarningSent = true;
        badUserWarningCleared = false;
      }
  }
  // ── 2. Evaluate hit count every WINDOW_MS ──
  if (now - windowStart >= WINDOW_MS) {
    noInterrupts();
    int count = hitCount;
    hitCount  = 0;
    interrupts();

    Serial.print("Hits in last 30s: ");
    Serial.println(count);

    if (count >= HIT_THRESHOLD) {
      alarmDisarmed = false; // Re-arm on new strong vibration
    }
   
    if (!alarmTriggered && !alarmDisarmed && count >= HIT_THRESHOLD){
      alarmTriggered = true;
      Serial.println("!! ALARM — strong vibration detected !!");

      if (vibrationWarningSent == false) {
        // Prepare email message
        EMailSender::EMailMessage message;
        message.subject = "Team LockBusters - Disturbance Detected!";
        message.message = "Strong vibration has been detected! Please check your scooter's safety as soon as possible.";
        message.mime = MIME_TEXT_PLAIN;
     
        // Send email
        Serial.println("\nSending email via simple SMTP...");
        Serial.println("RECEIVER_EMAIL: " + RECEIVER_EMAIL);
        EMailSender::Response resp = emailSend.send(RECEIVER_EMAIL, message);
     
        // Check result
        checkEmailValid(resp);

        vibrationWarningSent = true;
        vibrationWarningCleared = false;
      }
    }
    else if (alarmTriggered && count < HIT_THRESHOLD) {
      clearAlarm();
      Serial.println("Vibration subsided — alarm cleared.");
      vibrationWarningSent = false;

      if (vibrationWarningCleared == false) {
        // Prepare email message
        EMailSender::EMailMessage message;
        message.subject = "Team LockBusters - Disturbance Cleared!";
        message.message = "Strong vibration has cleared! Your scooter is now safe.";
        message.mime = MIME_TEXT_PLAIN;
     
        // Send email
        Serial.println("\nSending email via simple SMTP...");
        Serial.println("RECEIVER_EMAIL: " + RECEIVER_EMAIL);
        EMailSender::Response resp = emailSend.send(RECEIVER_EMAIL, message);
     
        // Check result
        checkEmailValid(resp);

        vibrationWarningCleared = true;
      }
    }
    windowStart = now;
  }

  // ── 3. Blink LEDs ─────────────────────────
  if (alarmTriggered) {
    if (now - lastBlinkTime >= BLINK_INTERVAL_MS) {
      lastBlinkTime = now;
      ledsOn = !ledsOn;
      fill_solid(leds1, NUM_LEDS, ledsOn ? CRGB::Red : CRGB::Black);
      fill_solid(leds2, NUM_LEDS, ledsOn ? CRGB::Red : CRGB::Black);
      FastLED.show();
    }
    // ── 4. Beep buzzer ────────────────────────
    unsigned long phase = now % (BUZZER_ON_MS + BUZZER_OFF_MS);
    if (phase < BUZZER_ON_MS) buzzerOn();
    else buzzerOff();
  } else {
    buzzerOff();
  }
}
