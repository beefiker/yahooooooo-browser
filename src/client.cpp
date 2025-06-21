// @cursor:phase-1
// Client implementation - Main CEF client handler
// CEF 137 compatible implementation with runtime style support

#include "client.h"
#include "include/cef_app.h"
#include "include/cef_callback.h"
#include "include/wrapper/cef_helpers.h"
#include <iostream>
#include <list>

Client::Client() {
    std::cout << "🎯 CEF 137 Client created" << std::endl;
}

// CefDisplayHandler methods
void Client::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) {
    CEF_REQUIRE_UI_THREAD();
    std::cout << "📋 Title changed: " << title.ToString() << std::endl;
}

void Client::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) {
    CEF_REQUIRE_UI_THREAD();
    if (frame->IsMain()) {
        std::cout << "🌐 Address changed: " << url.ToString() << std::endl;
    }
}

// CefLifeSpanHandler methods - CEF 137 compatible
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
    
    CEF_REQUIRE_UI_THREAD();
    std::cout << "🪟 CEF 137 Popup requested (ID: " << popup_id << "): " << target_url.ToString() << std::endl;
    
    // Allow popups but use the same client
    client = this;
    return false;
}

void Client::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
    std::cout << "✅ CEF 137 Browser created with ID: " << browser->GetIdentifier() << std::endl;
    
    // Keep track of the main browser window
    if (!main_browser_) {
        main_browser_ = browser;
        std::cout << "🎯 Main browser window established" << std::endl;
        std::cout << "🪟 CEF 137 window should now be visible!" << std::endl;
        
        // Test inter-process communication
        std::cout << "⏰ Testing IPC communication..." << std::endl;
        TestInterProcessCommunication();
    }
    
    // Add to browser list
    browser_list_.push_back(browser);
}

bool Client::DoClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
    std::cout << "🔒 Browser closing: " << browser->GetIdentifier() << std::endl;
    
    // Allow the close to proceed
    return false;
}

void Client::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
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
    CEF_REQUIRE_UI_THREAD();
    
    if (!browser || !frame) {
        std::cout << "⚠️ Null pointer in OnLoadStart" << std::endl;
        return;
    }
    
    if (frame->IsMain()) {
        std::string url = frame->GetURL();
        std::cout << "⏳ CEF 137 Loading started: " << url << std::endl;
    }
}

void Client::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) {
    CEF_REQUIRE_UI_THREAD();
    
    if (!browser || !frame) {
        std::cout << "⚠️ Null pointer in OnLoadEnd" << std::endl;
        return;
    }
    
    if (frame->IsMain()) {
        std::string url = frame->GetURL();
        std::cout << "✅ CEF 137 Loading completed: " << url 
                  << " (Status: " << httpStatusCode << ")" << std::endl;
    }
}

void Client::OnLoadError(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        ErrorCode errorCode,
                        const CefString& errorText,
                        const CefString& failedUrl) {
    CEF_REQUIRE_UI_THREAD();
    
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
    
    CEF_REQUIRE_UI_THREAD();
    
    if (!browser || !frame || !request) {
        std::cout << "⚠️ Null pointer in OnBeforeBrowse, blocking navigation" << std::endl;
        return true;
    }
    
    std::string url = request->GetURL();
    std::cout << "🔍 CEF 137 Before browse: " << url << std::endl;
    
    // Validate URL format
    if (url.empty() || url.length() > 2048) {
        std::cout << "⚠️ Invalid URL format, blocking navigation" << std::endl;
        return true;
    }
    
    // Allow navigation
    return false;
}

// Multi-process communication
bool Client::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefProcessId source_process,
                                     CefRefPtr<CefProcessMessage> message) {
    CEF_REQUIRE_UI_THREAD();
    
    const std::string& message_name = message->GetName();
    std::cout << "🔗 CEF 137 Client: Received message '" << message_name << "' from process " << source_process << std::endl;
    
    if (message_name == "yahooooooo_pong") {
        // Handle pong response from renderer
        CefRefPtr<CefListValue> args = message->GetArgumentList();
        if (args->GetSize() > 0) {
            std::string response = args->GetString(0);
            std::cout << "✅ CEF 137 Client: Renderer Response: " << response << std::endl;
        }
        return true;
    }
    
    return false;
}

void Client::TestInterProcessCommunication() {
    if (main_browser_ && main_browser_->GetMainFrame()) {
        std::cout << "🔗 Testing CEF 137 inter-process communication..." << std::endl;
        
        // Send ping message to renderer process
        CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create("yahooooooo_ping");
        message->GetArgumentList()->SetString(0, "Hello from browser process!");
        
        main_browser_->GetMainFrame()->SendProcessMessage(PID_RENDERER, message);
        std::cout << "✅ CEF 137 IPC message sent to renderer" << std::endl;
    }
}

void Client::CloseAllBrowsers(bool force_close) {
    if (browser_list_.empty()) {
        return;
    }

    BrowserList::const_iterator it = browser_list_.begin();
    for (; it != browser_list_.end(); ++it) {
        (*it)->GetHost()->CloseBrowser(force_close);
    }
} 
