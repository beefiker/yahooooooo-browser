// @cursor:phase-1
// App class - Main CEF application handler
// Handles application-level CEF events and initialization

#pragma once

#include "include/cef_app.h"

/**
 * Main application class that implements CefApp interface
 * Handles browser process setup, configuration, and multi-process communication
 */
class App : public CefApp, public CefBrowserProcessHandler {
public:
    App() = default;
    ~App() = default;

    // CefApp methods
    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
        return this;
    }

    void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;

    // CefBrowserProcessHandler methods
    void OnBeforeCommandLineProcessing(
        const CefString& process_type,
        CefRefPtr<CefCommandLine> command_line) override;
    
    void OnContextInitialized() override;

    void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) override;

    void OnScheduleMessagePumpWork(int64_t delay_ms) override;
    
    // CEF 137 requirement - provide default client for Chrome style UI
    CefRefPtr<CefClient> GetDefaultClient() override;

    // Multi-process communication (handled in client, not app)
    // bool OnProcessMessageReceived is handled in the Client class

private:
    IMPLEMENT_REFCOUNTING(App);
    DISALLOW_COPY_AND_ASSIGN(App);
};
