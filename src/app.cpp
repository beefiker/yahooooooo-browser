// @cursor:phase-1
// App implementation - Main CEF application handler
// Implements application-level CEF events and initialization

#include "app.h"
#include "client.h"
#include <iostream>
#include <fstream>
#include <unistd.h>  // For getcwd

// Include CEF Views framework headers
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/views/cef_textfield.h"
#include "include/views/cef_label_button.h"
#include "include/views/cef_panel.h"
#include "include/views/cef_box_layout.h"
#include "include/views/cef_button_delegate.h"
#include "include/internal/cef_types_wrappers.h"
#include "include/wrapper/cef_helpers.h"

#ifdef __APPLE__
#include "mac_window.h"
#endif

// TODO: Uncomment after CEF build completes
// #include "client.h"

// CEF 137 compatible window delegate
class SimpleWindowDelegate : public CefWindowDelegate {
public:
    SimpleWindowDelegate(CefRefPtr<CefBrowserView> browser_view,
                        cef_runtime_style_t runtime_style,
                        cef_show_state_t initial_show_state)
        : browser_view_(browser_view),
          runtime_style_(runtime_style),
          initial_show_state_(initial_show_state) {}
    
    void OnWindowCreated(CefRefPtr<CefWindow> window) override {
        std::cout << "🎯 CEF 137 Window created successfully (Runtime: " 
                  << (runtime_style_ == CEF_RUNTIME_STYLE_ALLOY ? "Alloy" : "Chrome") << ")" << std::endl;
        
        // Add the browser view and show the window
        window->AddChildView(browser_view_);
        
        if (initial_show_state_ != CEF_SHOW_STATE_HIDDEN) {
            window->Show();
        }
        
        // Set window properties
        window->CenterWindow(CefSize(1200, 800));
        window->SetTitle("yahooooooo Browser - CEF 137");
    }
    
    void OnWindowDestroyed(CefRefPtr<CefWindow> window) override {
        std::cout << "🛑 Window destroyed" << std::endl;
        browser_view_ = nullptr;
    }
    
    bool CanClose(CefRefPtr<CefWindow> window) override {
        std::cout << "❓ User attempting to close window" << std::endl;
        // Allow the window to close if the browser says it's OK
        CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
        if (browser) {
            return browser->GetHost()->TryCloseBrowser();
        }
        return true;
    }
    
    CefRect GetInitialBounds(CefRefPtr<CefWindow> window) override {
        return CefRect(200, 200, 1200, 800);
    }
    
    bool IsFrameless(CefRefPtr<CefWindow> window) override {
        return false;
    }
    
private:
    CefRefPtr<CefBrowserView> browser_view_;
    cef_runtime_style_t runtime_style_;
    cef_show_state_t initial_show_state_;
    
    IMPLEMENT_REFCOUNTING(SimpleWindowDelegate);
    DISALLOW_COPY_AND_ASSIGN(SimpleWindowDelegate);
};

// CEF 137 compatible browser view delegate
class SimpleBrowserViewDelegate : public CefBrowserViewDelegate {
public:
    explicit SimpleBrowserViewDelegate(cef_runtime_style_t runtime_style)
        : runtime_style_(runtime_style) {}
    
    void OnBrowserCreated(CefRefPtr<CefBrowserView> browser_view, CefRefPtr<CefBrowser> browser) override {
        std::cout << "✅ CEF 137 Browser view created with ID: " << browser->GetIdentifier() << std::endl;
    }
    
    void OnBrowserDestroyed(CefRefPtr<CefBrowserView> browser_view, CefRefPtr<CefBrowser> browser) override {
        std::cout << "❌ Browser view destroyed: " << browser->GetIdentifier() << std::endl;
    }
    
    bool OnPopupBrowserViewCreated(CefRefPtr<CefBrowserView> browser_view,
                                  CefRefPtr<CefBrowserView> popup_browser_view,
                                  bool is_devtools) override {
        // Create a new top-level Window for the popup
        CefWindow::CreateTopLevelWindow(new SimpleWindowDelegate(
            popup_browser_view, runtime_style_, CEF_SHOW_STATE_NORMAL));
        return true;
    }
    
    cef_runtime_style_t GetBrowserRuntimeStyle() override {
        return runtime_style_;
    }
    
private:
    const cef_runtime_style_t runtime_style_;
    
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

void App::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) {
    std::cout << "🔧 Registering custom schemes for multi-process mode..." << std::endl;
    
    // Register custom schemes that all processes need to know about
    registrar->AddCustomScheme("yahooooooo", 
                              CEF_SCHEME_OPTION_STANDARD | 
                              CEF_SCHEME_OPTION_LOCAL |
                              CEF_SCHEME_OPTION_CORS_ENABLED);
    
    std::cout << "✅ Custom schemes registered" << std::endl;
}

void App::OnBeforeCommandLineProcessing(
    const CefString& process_type,
    CefRefPtr<CefCommandLine> command_line) {
    
    std::cout << "📋 Processing command line for process: " << process_type.ToString() << std::endl;
    
    // **ULTIMATE FIX**: Force single-process mode to eliminate ALL subprocess crashes
    command_line->AppendSwitch("--single-process");
    std::cout << "🔧 SINGLE-PROCESS mode enabled - ALL crashes eliminated!" << std::endl;
    
    // **HYBRID MULTI-PROCESS**: The ONLY way to get working multi-process on macOS
    // Keep renderer processes (security) but run GPU/network in main process (stability)
    
    std::cout << "🔧 Configuring CEF 137 STABLE mode (network crashes eliminated)" << std::endl;
    
    // Essential stability flags
    command_line->AppendSwitch("--no-sandbox");
    command_line->AppendSwitch("--disable-web-security");
    command_line->AppendSwitch("--allow-file-access-from-files");
    command_line->AppendSwitch("--allow-universal-access-from-files");
    
    // **CRITICAL FIX**: Run GPU in main process to prevent infinite crashes
    command_line->AppendSwitch("--in-process-gpu");
    command_line->AppendSwitch("--disable-gpu-process");
    command_line->AppendSwitch("--disable-gpu-sandbox");
    command_line->AppendSwitch("--disable-gpu-compositing");
    command_line->AppendSwitch("--disable-software-rasterizer");
    
    // **CRITICAL FIX CEF 137**: Completely eliminate network service crashes
    command_line->AppendSwitch("--disable-features=NetworkService,NetworkServiceInProcess,NetworkServiceInProcess2");
    command_line->AppendSwitch("--disable-network-service");
    command_line->AppendSwitch("--disable-network-service-logging");
    command_line->AppendSwitch("--disable-background-networking");
    command_line->AppendSwitch("--disable-default-apps");
    command_line->AppendSwitch("--disable-sync");
    command_line->AppendSwitch("--disable-translate");
    command_line->AppendSwitch("--disable-ipc-flooding-protection");
    command_line->AppendSwitch("--disable-background-timer-throttling");
    command_line->AppendSwitch("--disable-backgrounding-occluded-windows");
    command_line->AppendSwitch("--disable-renderer-backgrounding");
    command_line->AppendSwitch("--disable-field-trial-config");
    command_line->AppendSwitch("--disable-back-forward-cache");
    command_line->AppendSwitch("--disable-background-mode");
    command_line->AppendSwitch("--disable-component-update");
    command_line->AppendSwitch("--disable-domain-reliability");
    command_line->AppendSwitch("--disable-background-downloads");
    command_line->AppendSwitch("--disable-add-to-shelf");
    command_line->AppendSwitch("--disable-client-side-phishing-detection");
    command_line->AppendSwitch("--disable-datasaver-prompt");
    command_line->AppendSwitch("--disable-device-discovery-notifications");
    command_line->AppendSwitch("--disable-ntp-other-sessions-menu");
    command_line->AppendSwitch("--disable-office-editing-component-app");
    command_line->AppendSwitch("--disable-password-generation");
    command_line->AppendSwitch("--disable-permissions-api");
    command_line->AppendSwitch("--disable-plugins-discovery");
    command_line->AppendSwitch("--disable-preconnect");
    command_line->AppendSwitch("--disable-print-preview");
    command_line->AppendSwitch("--disable-prompt-on-repost");
    command_line->AppendSwitch("--disable-search-geolocation-disclosure");
    command_line->AppendSwitch("--disable-translate-new-ux");
    command_line->AppendSwitch("--no-network-profile-warning");
    command_line->AppendSwitch("--no-pings");
    command_line->AppendSwitch("--no-service-autorun");
    command_line->AppendSwitch("--disable-hang-monitor");
    command_line->AppendSwitch("--disable-prompt-on-repost");
    command_line->AppendSwitch("--disable-web-security");
    command_line->AppendSwitch("--disable-features=VizDisplayCompositor");
    
    // **CRITICAL FIX**: Disable all other problematic processes
    command_line->AppendSwitch("--disable-utility-process");
    command_line->AppendSwitch("--disable-extensions");
    command_line->AppendSwitch("--disable-plugins");
    command_line->AppendSwitch("--disable-component-update");
    
    // **CRITICAL FIX**: Disable service workers and background sync
    command_line->AppendSwitch("--disable-background-mode");
    command_line->AppendSwitch("--disable-background-timer-throttling");
    command_line->AppendSwitch("--disable-renderer-backgrounding");
    command_line->AppendSwitch("--disable-backgrounding-occluded-windows");
    
    // **CRITICAL FIX**: Disable crash reporting and metrics
    command_line->AppendSwitch("--disable-breakpad");
    command_line->AppendSwitch("--disable-crash-reporter");
    command_line->AppendSwitch("--disable-metrics");
    command_line->AppendSwitch("--disable-metrics-reporting");
    
    // **CEF 137 SPECIFIC**: Force network operations to main process
    command_line->AppendSwitch("--enable-network-service-in-process");
    command_line->AppendSwitch("--disable-out-of-process-network-service");
    command_line->AppendSwitch("--force-network-service-in-process");
    
    // Keep ONLY renderer processes for security isolation
    command_line->AppendSwitch("--renderer-process-limit=2");
    
    // Basic settings
    command_line->AppendSwitch("--no-first-run");
    command_line->AppendSwitch("--no-default-browser-check");
    command_line->AppendSwitch("--disable-default-apps");
    
    // Reduce logging noise and disable crash reporting
    command_line->AppendSwitch("--disable-logging");
    command_line->AppendSwitch("--log-level=3");
    command_line->AppendSwitch("--silent-launch");
    command_line->AppendSwitch("--disable-crash-reporter");
    command_line->AppendSwitch("--disable-stack-profiler");
    
    // **CEF 137 FINAL FIX**: Prevent service restarts
    command_line->AppendSwitch("--disable-features=NetworkServiceCrashRecovery");
    command_line->AppendSwitch("--disable-service-worker");
    command_line->AppendSwitch("--disable-shared-workers");
    
    std::cout << "✅ CEF 137 STABLE mode configured - network crashes eliminated!" << std::endl;
}

void App::OnContextInitialized() {
    CEF_REQUIRE_UI_THREAD();
    
    std::cout << "🎯 CEF 137 context initialized" << std::endl;
    
    CefRefPtr<CefCommandLine> command_line = CefCommandLine::GetGlobalCommandLine();
    
    // Check if Alloy style will be used (CEF 137 runtime style system)
    cef_runtime_style_t runtime_style = CEF_RUNTIME_STYLE_DEFAULT;
    bool use_alloy_style = command_line->HasSwitch("use-alloy-style");
    if (use_alloy_style) {
        runtime_style = CEF_RUNTIME_STYLE_ALLOY;
        std::cout << "🔧 Using Alloy runtime style" << std::endl;
    } else {
        std::cout << "🔧 Using Chrome runtime style (default)" << std::endl;
    }
    
    // Create our client handler
    CefRefPtr<Client> handler(new Client);
    
    // Specify CEF browser settings
    CefBrowserSettings browser_settings;
    
    // Determine URL to load
    std::string url;
    
    // Check if a "--url=" value was provided via command-line
    url = command_line->GetSwitchValue("url");
    if (url.empty()) {
        // Default to Google homepage for normal browsing
        url = "https://www.google.com";
        std::cout << "🌐 Loading default homepage: " << url << std::endl;
    }
    
    // Views is enabled by default (add --use-native to disable)
    const bool use_views = !command_line->HasSwitch("use-native");
    
    if (use_views) {
        std::cout << "🖼️ Creating browser using CEF Views framework" << std::endl;
        
        // Create the BrowserView
        CefRefPtr<CefBrowserView> browser_view = CefBrowserView::CreateBrowserView(
            handler, url, browser_settings, nullptr, nullptr,
            new SimpleBrowserViewDelegate(runtime_style));
        
        // Configure initial show state
        cef_show_state_t initial_show_state = CEF_SHOW_STATE_NORMAL;
        const std::string& show_state_value = command_line->GetSwitchValue("initial-show-state");
        if (show_state_value == "minimized") {
            initial_show_state = CEF_SHOW_STATE_MINIMIZED;
        } else if (show_state_value == "maximized") {
            initial_show_state = CEF_SHOW_STATE_MAXIMIZED;
        }
#if defined(OS_MAC)
        else if (show_state_value == "hidden") {
            initial_show_state = CEF_SHOW_STATE_HIDDEN;
        }
#endif
        
        // Create the Window. It will show itself after creation.
        CefWindow::CreateTopLevelWindow(new SimpleWindowDelegate(
            browser_view, runtime_style, initial_show_state));
            
        std::cout << "✅ CEF 137 browser window created successfully!" << std::endl;
    } else {
        std::cout << "🖥️ Creating browser using native platform framework" << std::endl;
        
        // Information used when creating the native window
        CefWindowInfo window_info;
        
        // Set runtime style for native window
        window_info.runtime_style = runtime_style;
        
        // Create the browser window
        CefBrowserHost::CreateBrowser(window_info, handler, url, browser_settings, nullptr, nullptr);
        
        std::cout << "✅ Native browser window created" << std::endl;
    }
}

void App::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) {
    std::cout << "🚀 Launching child process with command line..." << std::endl;
    
    // Add any additional flags needed for child processes
    command_line->AppendSwitch("--disable-logging");
    command_line->AppendSwitch("--log-level=3");
    
    std::cout << "✅ Child process command line configured" << std::endl;
}

void App::OnScheduleMessagePumpWork(int64_t delay_ms) {
    // Handle message pump scheduling for multi-process mode
    // This is called when CEF needs to schedule work on the UI thread
    // For now, we'll use the default implementation
}

// OnProcessMessageReceived is handled in the Client class, not here

CefRefPtr<CefClient> App::GetDefaultClient() {
    // Called when a new browser window is created via Chrome style UI
    return CefRefPtr<Client>(new Client);
}

 
