#ifndef CUSTOM_WEBAPP_H
#define CUSTOM_WEBAPP_H

#include <DIYablesWebApps.h>

/**
 * Simple Custom WebApp Page
 */
class CustomWebAppPage : public DIYablesWebAppPageBase {
private:
    // WebSocket message identifier for this custom app
    static const String APP_IDENTIFIER;
    
public:
    CustomWebAppPage();
    
    // ========================================
    // REQUIRED METHODS - USED BY LIBRARY - DON'T CHANGE THESE!
    // ========================================
    void handleHTTPRequest(IWebClient& client) override;
    void handleWebSocketMessage(IWebSocket& ws, const char* message, uint16_t length) override;
    const char* getPageInfo() const override;
    String getNavigationInfo() const override;
    
    // ========================================
    // YOUR METHODS - USE THESE IN YOUR CODE!
    // ========================================
    void onCustomMessageReceived(void (*callback)(const String& payload));
    void sendToWeb(const String& message);
};

#endif
