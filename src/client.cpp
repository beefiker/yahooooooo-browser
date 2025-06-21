// @cursor:phase-1
// Client implementation - Main CEF client handler
// Implements browser events, UI interactions, and lifecycle management

#include "client.h"
#include "include/cef_app.h"
#include "include/cef_callback.h"
#include <iostream>
#include <list>

Client::Client() {
    std::cout << "🎯 Client created" << std::endl;
    // Remove render handler initialization for windowed mode
}

// CefDisplayHandler methods
void Client::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) {
    std::cout << "📋 Title changed: " << title.ToString() << std::endl;
}

void Client::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) {
    if (frame->IsMain()) {
        std::cout << "🌐 Address changed: " << url.ToString() << std::endl;
    }
}

// CefLifeSpanHandler methods - Updated with correct 13 parameters
bool Client::OnBeforePopup(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> frame,
                          int popup_id,
                          const CefString& target_url,
                          const CefString& target_frame_name,
                          WindowOpenDisposition target_disposition,
                          bool user_gesture,
                          const CefPopupFeatures& popupFeatures,
                          CefWindowInfo& windowInfo,
                          CefRefPtr<CefClient>& client,
                          CefBrowserSettings& settings,
                          CefRefPtr<CefDictionaryValue>& extra_info,
                          bool* no_javascript_access) {
    
    std::cout << "🪟 Popup requested (ID: " << popup_id << "): " << target_url.ToString() << std::endl;
    
    // Allow popups but use the same client
    client = this;
    return false;
}

void Client::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    std::cout << "✅ Browser created with ID: " << browser->GetIdentifier() << std::endl;
    
    // Keep track of the main browser window
    if (!main_browser_) {
        main_browser_ = browser;
        std::cout << "🎯 Main browser window established" << std::endl;
    }
    
    // Add to browser list
    browser_list_.push_back(browser);
}

bool Client::DoClose(CefRefPtr<CefBrowser> browser) {
    std::cout << "🔒 Browser closing: " << browser->GetIdentifier() << std::endl;
    
    // Allow the close to proceed
    return false;
}

void Client::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    std::cout << "❌ Browser closed: " << browser->GetIdentifier() << std::endl;
    
    // Remove from browser list
    BrowserList::iterator bit = browser_list_.begin();
    for (; bit != browser_list_.end(); ++bit) {
        if ((*bit)->IsSame(browser)) {
            browser_list_.erase(bit);
            break;
        }
    }

    // Clear main browser reference if it's closing
    if (main_browser_ && main_browser_->IsSame(browser)) {
        main_browser_ = nullptr;
    }

    // Quit when all browsers are closed
    if (browser_list_.empty()) {
        std::cout << "🛑 All browsers closed, shutting down CEF..." << std::endl;
        CefQuitMessageLoop();
    }
}

// CefLoadHandler methods
void Client::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type) {
    if (!browser || !frame) {
        std::cout << "⚠️ Null pointer in OnLoadStart" << std::endl;
        return;
    }
    
    if (frame->IsMain()) {
        std::string url = frame->GetURL();
        std::cout << "⏳ Loading started: " << url << std::endl;
        
        // **SAFETY**: Check for problematic URLs during load
        if (url.find("accounts.google.com") != std::string::npos) {
            std::cout << "⚠️ Detected Google auth during load - potential crash risk" << std::endl;
        }
    }
}

void Client::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) {
    if (!browser || !frame) {
        std::cout << "⚠️ Null pointer in OnLoadEnd" << std::endl;
        return;
    }
    
    if (frame->IsMain()) {
        std::string url = frame->GetURL();
        std::cout << "✅ Loading completed: " << url 
                  << " (Status: " << httpStatusCode << ")" << std::endl;
    }
}

void Client::OnLoadError(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        ErrorCode errorCode,
                        const CefString& errorText,
                        const CefString& failedUrl) {
    if (!browser || !frame) {
        std::cout << "⚠️ Null pointer in OnLoadError" << std::endl;
        return;
    }
    
    if (frame->IsMain()) {
        std::string url = failedUrl.ToString();
        std::cout << "❌ Load error: " << url 
                  << " - " << errorText.ToString() 
                  << " (Code: " << errorCode << ")" << std::endl;
    }
}

// CefRequestHandler methods
bool Client::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           CefRefPtr<CefRequest> request,
                           bool user_gesture,
                           bool is_redirect) {
    
    // **CRITICAL**: Add null pointer checks
    if (!browser || !frame || !request) {
        std::cout << "⚠️ Null pointer in OnBeforeBrowse, blocking navigation" << std::endl;
        return true; // Block navigation if any pointer is null
    }
    
    std::string url = request->GetURL();
    std::cout << "🔍 Before browse: " << url << std::endl;
    
    // **SAFETY**: Validate URL format
    if (url.empty() || url.length() > 2048) {
        std::cout << "⚠️ Invalid URL format, blocking navigation" << std::endl;
        return true; // Block invalid URLs
    }
    
    // Allow all other navigation
    return false;
}

// Helper methods
void Client::CloseAllBrowsers(bool force_close) {
    if (!browser_list_.empty()) {
        std::cout << "🔄 Closing all browsers..." << std::endl;
        
        BrowserList::const_iterator it = browser_list_.begin();
        for (; it != browser_list_.end(); ++it) {
            (*it)->GetHost()->CloseBrowser(force_close);
        }
    }
} 
