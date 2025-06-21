// @cursor:phase-1
// Life span handler implementation
// Manages browser lifecycle events, creation, and cleanup

#include "handler_life.h"
#include <iostream>

// TODO: Implement after CEF is available
/*
bool LifeSpanHandler::OnBeforePopup(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& target_url,
    const CefString& target_frame_name,
    CefLifeSpanHandler::WindowOpenDisposition target_disposition,
    bool user_gesture,
    const CefPopupFeatures& popupFeatures,
    CefWindowInfo& windowInfo,
    CefRefPtr<CefClient>& client,
    CefBrowserSettings& settings,
    CefRefPtr<CefDictionaryValue>& extra_info,
    bool* no_javascript_access) {
    
    std::cout << "🪟 Popup creation requested: " << target_url.ToString() << std::endl;
    
    // TODO: Implement popup blocking logic
    // For development, allow popups
    return false;
}

void LifeSpanHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    std::cout << "✅ Browser instance created (ID: " << browser->GetIdentifier() << ")" << std::endl;
    
    // Add to browser list for tracking
    browser_list_.push_back(browser);
    
    // TODO: Update UI to reflect new browser
}

bool LifeSpanHandler::DoClose(CefRefPtr<CefBrowser> browser) {
    std::cout << "🚪 Browser close requested (ID: " << browser->GetIdentifier() << ")" << std::endl;
    
    // Remove from browser list
    auto it = std::find(browser_list_.begin(), browser_list_.end(), browser);
    if (it != browser_list_.end()) {
        browser_list_.erase(it);
    }
    
    // Allow the close
    return false;
}

void LifeSpanHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    std::cout << "👋 Browser closing (ID: " << browser->GetIdentifier() << ")" << std::endl;
    
    // If this was the last browser, quit the application
    if (browser_list_.empty()) {
        std::cout << "🛑 Last browser closed, shutting down..." << std::endl;
        CefQuitMessageLoop();
    }
}
*/ 
