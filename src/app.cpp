// @cursor:phase-1
// App implementation - Main CEF application handler
// Implements application-level CEF events and initialization

#include "app.h"
#include "client.h"
#include <iostream>

// Include CEF Views framework headers
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/views/cef_textfield.h"
#include "include/views/cef_label_button.h"
#include "include/views/cef_panel.h"
#include "include/views/cef_box_layout.h"
#include "include/views/cef_button_delegate.h"
#include "include/internal/cef_types_wrappers.h"

#ifdef __APPLE__
#include "mac_window.h"
#endif

// TODO: Uncomment after CEF build completes
// #include "client.h"

// Simple window delegate for creating the main window
class SimpleWindowDelegate : public CefWindowDelegate {
public:
    SimpleWindowDelegate() = default;
    
    void OnWindowCreated(CefRefPtr<CefWindow> window) override {
        std::cout << "🎯 Window created successfully" << std::endl;
        // Make window more prominent
        window->CenterWindow(CefSize(1200, 800));
        window->SetAlwaysOnTop(true);  // Temporarily on top
        window->Activate();
    }
    
    void OnWindowDestroyed(CefRefPtr<CefWindow> window) override {
        std::cout << "🛑 Window destroyed" << std::endl;
    }
    
    bool CanClose(CefRefPtr<CefWindow> window) override {
        std::cout << "❓ User attempting to close window" << std::endl;
        return true;
    }
    
    CefRect GetInitialBounds(CefRefPtr<CefWindow> window) override {
        // Center the window on screen
        return CefRect(200, 200, 1200, 800);
    }
    
    bool IsFrameless(CefRefPtr<CefWindow> window) override {
        return false;  // Keep window frame and title bar
    }
    
private:
    IMPLEMENT_REFCOUNTING(SimpleWindowDelegate);
    DISALLOW_COPY_AND_ASSIGN(SimpleWindowDelegate);
};

// Simple browser view delegate
class SimpleBrowserViewDelegate : public CefBrowserViewDelegate {
public:
    SimpleBrowserViewDelegate() = default;
    
    void OnBrowserCreated(CefRefPtr<CefBrowserView> browser_view, CefRefPtr<CefBrowser> browser) override {
        std::cout << "✅ Browser view created with ID: " << browser->GetIdentifier() << std::endl;
    }
    
    void OnBrowserDestroyed(CefRefPtr<CefBrowserView> browser_view, CefRefPtr<CefBrowser> browser) override {
        std::cout << "❌ Browser view destroyed: " << browser->GetIdentifier() << std::endl;
    }
    
private:
    IMPLEMENT_REFCOUNTING(SimpleBrowserViewDelegate);
    DISALLOW_COPY_AND_ASSIGN(SimpleBrowserViewDelegate);
};

// Address bar delegate to handle URL input
class AddressBarDelegate : public CefTextfieldDelegate {
public:
    AddressBarDelegate(CefRefPtr<CefBrowserView> browser_view) : browser_view_(browser_view) {}
    
    bool OnKeyEvent(CefRefPtr<CefTextfield> textfield, const CefKeyEvent& event) override {
        // Handle Enter key to navigate to URL
        if (event.type == KEYEVENT_KEYUP && event.windows_key_code == 13) { // Enter key
            std::string url = textfield->GetText().ToString();
            if (!url.empty()) {
                // Add http:// if no protocol specified
                if (url.find("://") == std::string::npos) {
                    if (url.find("google.com") != std::string::npos || 
                        url.find("www.") == 0 || 
                        url.find(".com") != std::string::npos ||
                        url.find(".org") != std::string::npos ||
                        url.find(".net") != std::string::npos) {
                        url = "https://" + url;
                    } else {
                        // Treat as Google search
                        url = "https://www.google.com/search?q=" + url;
                    }
                }
                if (browser_view_ && browser_view_->GetBrowser()) {
                    browser_view_->GetBrowser()->GetMainFrame()->LoadURL(url);
                }
            }
            return true;
        }
        return false;
    }
    
private:
    CefRefPtr<CefBrowserView> browser_view_;
    IMPLEMENT_REFCOUNTING(AddressBarDelegate);
};

// Navigation button delegate
class NavButtonDelegate : public CefButtonDelegate {
public:
    NavButtonDelegate(CefRefPtr<CefBrowserView> browser_view, const std::string& action) 
        : browser_view_(browser_view), action_(action) {}
    
    void OnButtonPressed(CefRefPtr<CefButton> button) override {
        if (browser_view_ && browser_view_->GetBrowser()) {
            if (action_ == "back") {
                browser_view_->GetBrowser()->GoBack();
            } else if (action_ == "forward") {
                browser_view_->GetBrowser()->GoForward();
            } else if (action_ == "refresh") {
                browser_view_->GetBrowser()->Reload();
            }
        }
    }
    
private:
    CefRefPtr<CefBrowserView> browser_view_;
    std::string action_;
    IMPLEMENT_REFCOUNTING(NavButtonDelegate);
};

void App::OnBeforeCommandLineProcessing(
    const CefString& process_type,
    CefRefPtr<CefCommandLine> command_line) {
    
    std::cout << "📋 Processing command line..." << std::endl;
    
    // **CRITICAL FIX**: Use single-process mode to avoid network service crashes
    // Multi-process mode has network service issues on this macOS setup
    command_line->AppendSwitch("--single-process");
    
    // Essential stability flags
    command_line->AppendSwitch("--no-sandbox");
    command_line->AppendSwitch("--disable-web-security");
    
    // **COMPREHENSIVE**: GPU and rendering fixes for complex sites
    command_line->AppendSwitch("--disable-gpu");
    command_line->AppendSwitch("--disable-gpu-compositing");
    command_line->AppendSwitch("--disable-gpu-sandbox");
    command_line->AppendSwitch("--disable-software-rasterizer");
    command_line->AppendSwitch("--disable-accelerated-2d-canvas");
    command_line->AppendSwitch("--disable-accelerated-video-decode");
    command_line->AppendSwitch("--disable-accelerated-mjpeg-decode");
    
    // **NEW**: Frame management and timeout fixes
    command_line->AppendSwitch("--disable-features=VizDisplayCompositor,NetworkService,AudioServiceOutOfProcess");
    command_line->AppendSwitch("--disable-logging");
    command_line->AppendSwitch("--log-level=3");
    command_line->AppendSwitch("--disable-browser-info-timeout");
    command_line->AppendSwitch("--disable-ipc-flooding-protection");
    
    // **MEMORY**: Prevent memory-related crashes on complex sites
    command_line->AppendSwitch("--max-gum-memory-size=67108864");  // 64MB limit
    command_line->AppendSwitch("--disable-dev-shm-usage");
    command_line->AppendSwitch("--memory-pressure-off");
    
    // **JAVASCRIPT**: Essential for complex sites
    command_line->AppendSwitch("--disable-background-timer-throttling");
    command_line->AppendSwitch("--disable-backgrounding-occluded-windows");
    command_line->AppendSwitch("--disable-renderer-backgrounding");
    command_line->AppendSwitch("--disable-extensions");
    command_line->AppendSwitch("--disable-plugins");
    
    // **NETWORK**: Simplified networking
    command_line->AppendSwitch("--no-first-run");
    command_line->AppendSwitch("--no-default-browser-check");
    command_line->AppendSwitch("--allow-running-insecure-content");
    command_line->AppendSwitch("--disable-sync");
    
    std::cout << "✅ Command line configured for complex site compatibility" << std::endl;
}

void App::OnContextInitialized() {
    std::cout << "🎯 CEF context initialized" << std::endl;
    
    // **CRITICAL FIX**: Use proper windowed rendering approach
    CefWindowInfo window_info;
    CefBrowserSettings browser_settings;
    
    // **FIXED**: Create a proper windowed browser on macOS
    #ifdef __APPLE__
    // Use child window mode for macOS
    window_info.SetAsChild(nullptr, CefRect(100, 100, 1200, 800));
    #else
    window_info.SetAsPopup(nullptr, "yahooooooo-browser");
    #endif
    
    // **CRITICAL**: Browser settings for modern web compatibility
    browser_settings.javascript = STATE_ENABLED;
    browser_settings.javascript_close_windows = STATE_ENABLED;
    browser_settings.javascript_access_clipboard = STATE_ENABLED;
    browser_settings.javascript_dom_paste = STATE_ENABLED;
    
    // Create client handler
    CefRefPtr<Client> client(new Client);
    
    // Use a simpler site that works well - test different complexity levels
    std::string test_url = "https://www.wikipedia.org";  // More reliable than Google
    
    // Create browser using stable windowed approach
    CefBrowserHost::CreateBrowser(
        window_info,
        client,
        test_url,
        browser_settings,
        nullptr,
        nullptr
    );
    
    std::cout << "✅ Browser window created with stable windowed rendering" << std::endl;
}

CefRefPtr<CefRenderProcessHandler> App::GetRenderProcessHandler() {
    return nullptr;  // We'll implement this later if needed
}

/*
// TODO: Implement after CEF is available
void App::OnBeforeCommandLineProcessing(
    const CefString& process_type,
    CefRefPtr<CefCommandLine> command_line) {
    
    std::cout << "📋 Processing command line..." << std::endl;
    
    // Disable web security for development (remove in production)
    command_line->AppendSwitch("--disable-web-security");
    command_line->AppendSwitch("--disable-features=VizDisplayCompositor");
}

void App::OnContextInitialized() {
    std::cout << "🎯 CEF context initialized" << std::endl;
    
    // Create the main browser window
    CefWindowInfo window_info;
    CefBrowserSettings browser_settings;
    
    // Configure window
    window_info.SetAsPopup(nullptr, "yahooooooo-browser");
    window_info.width = 1200;
    window_info.height = 800;
    
    // Create client handler
    CefRefPtr<Client> client(new Client);
    
    // Create browser
    CefBrowserHost::CreateBrowser(
        window_info,
        client,
        "https://example.com",  // Default homepage
        browser_settings,
        nullptr,
        nullptr
    );
}

CefRefPtr<CefRenderProcessHandler> App::GetRenderProcessHandler() {
    return nullptr;  // We'll implement this later if needed
}
*/ 
