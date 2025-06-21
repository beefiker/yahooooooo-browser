// @cursor:phase-2
// Browser Window - Main browser window with navigation UI
// Handles window creation, navigation buttons, address bar, and DevTools

#pragma once

#include <string>

// TODO: Uncomment after CEF build completes
// #include "include/cef_client.h"
// #include "include/cef_browser.h"
// #include "include/views/cef_browser_view.h"
// #include "include/views/cef_window.h"

/**
 * Main browser window class
 * Manages the browser UI including navigation, address bar, and DevTools
 */
class BrowserWindow {
public:
    BrowserWindow() = default;
    ~BrowserWindow() = default;

    // Window management
    void Create(const std::string& title = "yahooooooo-browser");
    void Show();
    void Close();
    
    // Navigation methods
    void LoadURL(const std::string& url);
    void GoBack();
    void GoForward();
    void Reload();
    void Stop();
    
    // DevTools
    void ShowDevTools();
    void CloseDevTools();
    
    // Address bar
    void SetAddressBarText(const std::string& url);
    std::string GetAddressBarText() const;
    
    // Window properties
    void SetTitle(const std::string& title);
    void SetSize(int width, int height);
    void Center();

    // TODO: Implement after CEF is available
    /*
    // CEF integration
    void SetBrowser(CefRefPtr<CefBrowser> browser);
    CefRefPtr<CefBrowser> GetBrowser() const;
    CefRefPtr<CefWindow> GetWindow() const;
    */

private:
    // Window properties
    std::string window_title_ = "yahooooooo-browser";
    int width_ = 1200;
    int height_ = 800;
    
    // Navigation state
    std::string current_url_;
    bool can_go_back_ = false;
    bool can_go_forward_ = false;
    bool is_loading_ = false;
    
    // UI state
    bool devtools_visible_ = false;
    
    // TODO: CEF components (uncomment after build)
    // CefRefPtr<CefBrowser> browser_;
    // CefRefPtr<CefWindow> window_;
    // CefRefPtr<CefBrowserView> browser_view_;
    
    // UI creation helpers
    void CreateMenuBar();
    void CreateToolbar();
    void CreateStatusBar();
    void UpdateNavigationButtons();
}; 