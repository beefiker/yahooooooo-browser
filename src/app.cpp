// @cursor:phase-1
// App implementation - Main CEF application handler
// Implements application-level CEF events and initialization

#include "app.h"
#include "client.h"
#include <iostream>
#include <fstream>
#include <unistd.h>  // For getcwd

// Include CEF headers
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/views/cef_textfield.h"
#include "include/views/cef_label_button.h"
#include "include/views/cef_panel.h"
#include "include/views/cef_box_layout.h"
#include "include/views/cef_button_delegate.h"
#include "include/internal/cef_types_wrappers.h"
#include "include/wrapper/cef_helpers.h"

// CEF 137 Compatible App - Following cefsimple pattern
// Implementation of main application handler

namespace {

// CEF 137 Window Delegate - following SimpleApp pattern exactly
class SimpleWindowDelegate : public CefWindowDelegate {
public:
    SimpleWindowDelegate(CefRefPtr<CefBrowserView> browser_view,
                         cef_runtime_style_t runtime_style,
                         cef_show_state_t initial_show_state)
        : browser_view_(browser_view),
          runtime_style_(runtime_style),
          initial_show_state_(initial_show_state) {}
    
    void OnWindowCreated(CefRefPtr<CefWindow> window) override {
        // Add the browser view and show the window.
        window->AddChildView(browser_view_);
        
        if (initial_show_state_ != CEF_SHOW_STATE_HIDDEN) {
            window->Show();
        }
    }
    
    void OnWindowDestroyed(CefRefPtr<CefWindow> window) override {
        browser_view_ = nullptr;
    }
    
    bool CanClose(CefRefPtr<CefWindow> window) override {
        // Allow the window to close if the browser says it's OK.
        CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
        if (browser) {
            return browser->GetHost()->TryCloseBrowser();
        }
        return true;
    }
    
    CefSize GetPreferredSize(CefRefPtr<CefView> view) override {
        return CefSize(1200, 800);
    }
    
    cef_show_state_t GetInitialShowState(CefRefPtr<CefWindow> window) override {
        return initial_show_state_;
    }
    
    cef_runtime_style_t GetWindowRuntimeStyle() override {
        return runtime_style_;
    }
    
private:
    CefRefPtr<CefBrowserView> browser_view_;
    const cef_runtime_style_t runtime_style_;
    const cef_show_state_t initial_show_state_;
    
    IMPLEMENT_REFCOUNTING(SimpleWindowDelegate);
    DISALLOW_COPY_AND_ASSIGN(SimpleWindowDelegate);
};

class SimpleBrowserViewDelegate : public CefBrowserViewDelegate {
public:
    explicit SimpleBrowserViewDelegate(cef_runtime_style_t runtime_style)
        : runtime_style_(runtime_style) {}
    
    bool OnPopupBrowserViewCreated(CefRefPtr<CefBrowserView> browser_view,
                                   CefRefPtr<CefBrowserView> popup_browser_view,
                                   bool is_devtools) override {
        // Create a new top-level Window for the popup. It will show itself after
        // creation.
        CefWindow::CreateTopLevelWindow(new SimpleWindowDelegate(
            popup_browser_view, runtime_style_, CEF_SHOW_STATE_NORMAL));
        
        // We created the Window.
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

}  // namespace

App::App() = default;

void App::OnBeforeCommandLineProcessing(
    const CefString& process_type,
    CefRefPtr<CefCommandLine> command_line) {
  
  // Add command line flags to prevent crashes - essential for CEF 137 stability
  command_line->AppendSwitch("--single-process");
  command_line->AppendSwitch("--no-sandbox");
  command_line->AppendSwitch("--disable-web-security");
  command_line->AppendSwitch("--allow-file-access-from-files");
  command_line->AppendSwitch("--allow-universal-access-from-files");
  
  // macOS specific fixes for NSApplication issues
#ifdef __APPLE__
  command_line->AppendSwitch("--disable-features=kMacSystemMediaPermissionsInfoUi");
  command_line->AppendSwitch("--disable-mac-overlays");
  command_line->AppendSwitch("--disable-pinch");
  command_line->AppendSwitch("--disable-partial-raster");
#endif
  
  // Fix V8 Proxy resolver issues in single process mode
  command_line->AppendSwitch("--no-proxy-server");
  command_line->AppendSwitch("--disable-proxy-pac-http-fetching");
  command_line->AppendSwitch("--disable-proxy-pac-https-fetching");
  command_line->AppendSwitch("--disable-proxy-pac-dhcp-fetching");
  command_line->AppendSwitch("--disable-proxy-pac-dns-fetching");
  
  // Enable GPU for content rendering
  command_line->AppendSwitch("--in-process-gpu");
  command_line->AppendSwitch("--enable-gpu-rasterization");
  
  // Disable V8 features that cause issues in single process
  command_line->AppendSwitch("--disable-v8-idle-tasks");
  command_line->AppendSwitch("--disable-background-networking");
  command_line->AppendSwitch("--disable-sync");
  
  // Camera and media fixes
  command_line->AppendSwitch("--disable-media-stream");
  command_line->AppendSwitch("--disable-webrtc");
  command_line->AppendSwitch("--disable-camera");
  command_line->AppendSwitch("--disable-microphone");
  
  // Basic stability flags
  command_line->AppendSwitch("--no-first-run");
  command_line->AppendSwitch("--no-default-browser-check");
  command_line->AppendSwitch("--disable-crash-reporter");
  
  // Additional single-process stability
  command_line->AppendSwitch("--disable-dev-shm-usage");
}

void App::OnContextInitialized() {
  CEF_REQUIRE_UI_THREAD();

  CefRefPtr<CefCommandLine> command_line =
      CefCommandLine::GetGlobalCommandLine();

  // Check if Alloy style will be used.
  cef_runtime_style_t runtime_style = CEF_RUNTIME_STYLE_DEFAULT;
  bool use_alloy_style = command_line->HasSwitch("use-alloy-style");
  if (use_alloy_style) {
    runtime_style = CEF_RUNTIME_STYLE_ALLOY;
  }

  // Client implements browser-level callbacks.
  CefRefPtr<Client> handler(new Client(use_alloy_style));

  // Specify CEF browser settings here.
  CefBrowserSettings browser_settings;

  std::string url;

  // Check if a "--url=" value was provided via the command-line. If so, use
  // that instead of the default URL.
  url = command_line->GetSwitchValue("url");
  if (url.empty()) {
    url = "https://www.google.com";
  }

  // Force native windows to avoid NSApplication issues
  const bool use_views = false;

  // Use native platform framework instead of Views to avoid NSApplication crashes
  if (use_views) {
    // Views code disabled to avoid NSApplication issues
  } else {
    // Information used when creating the native window.
    CefWindowInfo window_info;

#if defined(OS_WIN)
    // On Windows we need to specify certain flags that will be passed to
    // CreateWindowEx().
    window_info.SetAsPopup(nullptr, "yahooooooo-browser");
#endif

    // Alloy style will create a basic native window. Chrome style will create a
    // fully styled Chrome UI window.
    window_info.runtime_style = runtime_style;

    // Create the first browser window.
    CefBrowserHost::CreateBrowser(window_info, handler, url, browser_settings,
                                  nullptr, nullptr);
  }
}

CefRefPtr<CefClient> App::GetDefaultClient() {
    // Called when a new browser window is created via Chrome style UI.
    return Client::GetInstance();
}

 
