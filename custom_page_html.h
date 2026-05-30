#ifndef CUSTOM_PAGE_HTML_H
#define CUSTOM_PAGE_HTML_H

const char CUSTOM_PAGE_HTML[] PROGMEM = R"HTML_WRAPPER(
<!DOCTYPE html>
<html>
<!-- ********************* HTML START ********************* -->
<head class="container">
    <!--Add any metadata here-->
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">

    <title>Team LockBusters Homepage</title>

    <!--Link css stylesheet-->
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
</head>

<body>
    <header>
        <!--Nav icon-->
        <div class="nav-bar">
            <i class="fa-solid fa-bars icon" id="menu-icon"></i>
        </div>


        <div class="logo">
            <i class="fa-solid fa-shield-halved icon" id="lock-icon"></i>
        </div>
        
        <h1 class="title">Team LockBusters</h1>
    </header>
    
    <main>
        <!--connect to esp-->
        <div class="connection-container">
            <p>Connect to esp32 here</p>
            <!--Show connection status?-->
        </div>

        <!--on/off button for security system-->
        <div class="security-button-container">
            <div class="alarm-system">
                <h3>Toggle security system</h3>
                <label for="alarm-toggle" id="alarm-label">
                    ALARM-STATUS:
                </label>
                <button id="alarm-toggle">
                    <span id="alarm-state" value="ON">
                        ON
                    </span>
                </button>
            </div>
        </div>

        <!--Notification method (email, phone, app notif)-->
        <div class="notif-container">
            <label for="email-update">Enter Your Email Address:</label>
            <input type="text" id="email-info" placeholder="your-email@gmail.com">
            <br>
            <p>Set up email notifications to be alerted when your scooter is in danger!</p>
        </div>
    </main>
    <!-- ********************* HTML END ********************* -->

    <!-- ********************* JS START ********************* -->
    <script>
        // WebSocket message identifier for this custom app
        const APP_IDENTIFIER = 'CUSTOM:';
        
        let ws = null;
        
        function connectWebSocket() {
            ws = new WebSocket('ws://' + location.hostname + ':81');
            
            ws.onopen = function() {
                //esp is connected
                let connection = document.querySelector('.connection-container');
                connection.style.color = "--success-green";
            };
            
            ws.onclose = function() {
                //esp is disconnected
                let connection = document.querySelector('.connection-container');
                connection.style.color = "--disconnect-orange";
                // Auto reconnect after 5 seconds
                setTimeout(connectWebSocket, 5000);
            };
            
            ws.onmessage = function(event) {
                // Only process messages that start with our app identifier
                if (event.data.startsWith(APP_IDENTIFIER)) {
                    // Remove the identifier and display the message
                    let message = event.data.substring(APP_IDENTIFIER.length);
                }
                // Ignore messages that don't belong to our app
            };
            
            ws.onerror = function(error) {
                //esp error
                let connection = document.querySelector('.connection-container');
                connection.style.color = "--fail-red";
            };
        }
        
        function send(message) {
            if (ws && ws.readyState === WebSocket.OPEN) {
                ws.send(APP_IDENTIFIER + message);
            }
        }
        
        function sendMessage() {
            let input = document.getElementById('messageInput');
            if (input.value && ws && ws.readyState === WebSocket.OPEN) {
                ws.send(APP_IDENTIFIER + input.value);
                input.value = '';
            }
        }
        
        // Start connection when page loads
        window.addEventListener('load', function() {
            connectWebSocket();
        });
        
        // Reconnect when page becomes visible
        document.addEventListener('visibilitychange', function() {
            if (!document.hidden && (!ws || ws.readyState !== WebSocket.OPEN)) {
                setTimeout(connectWebSocket, 1000);
            }
        });

        //event listener for alarm toggle system on/off
        const alarmButton = document.querySelector("#alarm-toggle");
        const alarmTxt = document.querySelector("#alarm-state"); //text for on/off
        const alarmStyle = alarmButton.style;

        alarmButton.addEventListener('click', () => alarmToggle(alarmTxt, alarmStyle));

        function alarmToggle(alarmTxt, alarmStyle) {
            console.log("Alarm toggled.");
            let message = "ALARM_TOGGLE:";
            //if alarm is on, turn off
            if (alarmTxt.innerText === "ON") {
                console.log("alarm is on. Switching to off...");
                alarmTxt.innerText = "OFF";
                alarmStyle.backgroundColor = "red";
                //turn off led
                message = message.concat("OFF");
            }
            //if alarm is off, turn on
            else if (alarmTxt.innerText === "OFF") {
                console.log("alarm is off. Switching to on...");
                alarmTxt.innerText = "ON";
                alarmStyle.backgroundColor = "green";
                //turn on led
                message = message.concat("ON");
            }
            console.log("alarm message: " + message);
            send(message);
        }

        //event listener for email notifications
        const emailAddr = document.querySelector("#email-info");
        const emailButton = document.querySelector("#email-button");

        emailAddr.addEventListener('keypress', function(event) {
            console.log("Updating email...");
            console.log("fetched email: " + this.value);
            if (event.key === "Enter") {
                let message = "EMAIL_UPDATE:";
                message = message.concat(this.value);
                console.log("Email message: " + message);
                emailAddr.value = "";
                send(message);
            }
        });
    </script>
    <!-- ********************* JS END ********************* -->
</body>

<!-- ********************* CSS START ********************* -->
<style> 
/* Google fonts */
/* We import inter which is a popular tech font */
@import url('https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600;700&display=swap');


* {
   margin: 0;
   padding: 0;
   box-sizing: border-box;
   font-family: "Inter", sans-serif;
   color:var(--text-color);
}

:root {
    --background-color1: #fafaff;
    --background-color2: #519cff;
    --background-color4: #cad7fda4;
    --primary-color: #3b60d8;
    --secondary-color: #123883;

    --background-color1: #fafaff;
    --background-color2: #519cff;
    --background-color4: #cad7fda4;
    --primary-color: #3b60d8;
    --secondary-color: #123883;
    --background-dark: #0f172a; /* Dark Background */
    --card-background: #1e293b;
    --primary-blue: #3b82f6;
    --accent-cyan: #06b6d4;
    --success-green: #10b981;
    --disconnect-orange: #febd3b;
    --fail-red: #ef4444;
    --text-color: #f8fafc;
    --text-faded: #94a3b8;
}

body {
    background-color: var(--background-dark);
    color: var(--text-main);
    max-width: 100%;
    overflow-x: hidden;
    padding-bottom: 40px;
}

.icon {
    /*height: 50px;*/
    font-size: 2rem;
    color: #ffffff;
    filter: drop-shadow(0 2px 8px rgba(0,0,0,0.2));
    transition: color 0.2s ease, transform 0.2s ease;
}

.icon:hover {
   color: var(--accent-cyan);
   transform: scale(1.1);
}

header {
    padding: 1em 0.5em;
    margin-bottom: 10px;

    display: flex;
    align-items: center;
    /* the linear-gradient blends blue with cyan at a 135-degree angle */
    background-color: linear-gradient(135deg, var(--primary-blue), var(--accent-cyan));
    padding: 1.5em 2em;

    margin-bottom: 30px;

    /* box-shadow makes the header have some depth */
    box-shadow: 0 10px 25px -5px rgba(59,130,246,0.3);
    border-bottom: 1px solid rgba(255,255,255,0.1); /* dividing line */
}

.nav-bar, .logo{
    margin-right: 20px;
    cursor: pointer;
    /* animations take 0.2 seconds */
    transition: transform 0.2s;
}

/* Effect to change over menu icons to pop out slightly */
.nav-bar:hover, .logo:hover {
   transform: scale(1.1); /* scales the element up by 10% on hover */
}

.title {
   font-size: 1.8rem;
   font-weight: 700;
   letter-spacing: -0.5px;
   text-shadow: 0 2px 4px rgba(0,0,0,0.15);
}

main {
   max-width: 800px;
   margin: 0 auto;
   padding: 0 20px;
   display: grid;
   grid-template-columns: 1fr;
   gap: 20px; /* Spaces out everything */
}

.security-button-container, .connection-container,
.notif-container{
   background-color: var(--card-background);
   border: 1px solid rgba(255, 255, 255, 0.05); /* Outline border */
   border-radius: 16px; /* Smooth corners */
   padding: 24px;
   box-shadow: 0 4px 20px rgba(0,0,0,0.2); /* Shadow */
   /* Hovering feature */
   transition: transform 0.3s ease, box-shadown 0.3s ease;

   /* flexible alignment of elements within containers and spaces them out */
   display: flex;
   flex-direction: column;
   * {
      margin: 5px;
   }
}

/* Hovering over card */
.security-button-container:hover,
.connection-container:hover, .notif-container:hover {
   transform: translateY(-2px);
   box-shadow: 0 8px 30px rgba(0,0,0,0.3); /* darkens shadow */
}

label {
   display: block;
   font-size: 0.9rem;
   font-weight: 600;
   color: var(--text-faded);
   margin-bottom: 10px;
   text-transform: uppercase;
   letter-spacing: 0.5px;
}

/* -----------------------INTERACTIVE FORMS-----------------------*/

/* Changes the dropdown style */
select, input[type="text"] {
   width:100%;
   padding: 12px 16px;
   background-color: rgba(15,23,42,0.6);
   border: 2px solid rgba(255,255,255,0.1);
   border-radius: 10px;
   color: var(--text-main);
   font-size: 1rem;
   outline: none;
   transition: border-color 0.2s;
}

/* Design when user clicks on textbox or dropdown */
select:focus, input[type="text"]:focus {
   border-color: var(--primary-blue);
   box-shadow: 0 0 0 3px rgba(59,130,246,0.2);
}

.alarm-system {
   display: flex;
   flex-direction: column;
   align-items: flex-start;
   gap: 12px;
}

.alarm-system h3 {
   font-size: 1.2rem;
   font-weight: 600;
}

#alarm-label {
   margin-bottom: 0;
}

/* ----------------------------Alarm Button-----------------------*/
#alarm-toggle {
   cursor: pointer;
   border: none;
   padding: 14px 28px;
   font-size: 1.1rem;
   font-weight: 700;
   border-radius: 12px;
   color: rgb(160, 20, 220);
   width: 100%;
   max-width: 200px;
   text-shadow: 0 1px 2px rgba(0,0,0,0.2);
   background-color: green;

   /* Emerald Neon Glow */
   box-shadow: 0 4px 14px rgba(16, 185, 129, 0.4);
   /* Transition when clicked */
   transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
}

#alarm-toggle:active {
   transform: scale(0.96);
}

.connection-container p {
   font-weight: 600;
   color: var(--accent-cyan);
   display: flex;
   align-items: center;
   gap: 8px;
}

.connection-container::before {
   content: "●";
   color: var(--success-green);
   font-size: 1.2rem;
   animation: blink 1.5s infinite;
}

.nav-container {
   display: flex;
   justify-content: space-around;
   margin-top: 10px;
}

.nav-container h2 {
   font-size: 1rem;
   color: var(--text-faded);
   cursor: pointer;
   transition: color 0.2s;
}

.nav-container h2:hover {
   color: var(--accent-cyan);
}

</style>
<!-- ********************* CSS END ********************* -->

</html>
)HTML_WRAPPER";

#endif
