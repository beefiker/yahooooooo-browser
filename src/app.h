// @cursor:phase-1
// App class - Main CEF application handler
// Handles application-level CEF events and initialization

#pragma once

#include "include/cef_app.h"

/**
 * Main application class that implements CefApp interface
 * Handles browser process setup and configuration
 */
class App : public CefApp, public CefBrowserProcessHandler {
public:
    App() = default;
    ~App() = default;

    // CefApp methods
    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
        return this;
    }

    // CefBrowserProcessHandler methods
    void OnBeforeCommandLineProcessing(
        const CefString& process_type,
        CefRefPtr<CefCommandLine> command_line) override;
    
    void OnContextInitialized() override;

    // Optional: Handle render process creation
    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

private:
    IMPLEMENT_REFCOUNTING(App);
    DISALLOW_COPY_AND_ASSIGN(App);
};
