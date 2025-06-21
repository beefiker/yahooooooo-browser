// CEF 137 Compatible App - Following cefsimple pattern
// Main application handler for browser process

#pragma once

#include "include/cef_app.h"

/**
 * CEF 137 compatible application class following SimpleApp pattern
 * Implements application-level callbacks for the browser process
 */
class App : public CefApp, public CefBrowserProcessHandler {
public:
    App();

    // CefApp methods
    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
        return this;
    }

    // CefBrowserProcessHandler methods
    void OnBeforeCommandLineProcessing(
        const CefString& process_type,
        CefRefPtr<CefCommandLine> command_line) override;
    void OnContextInitialized() override;
    CefRefPtr<CefClient> GetDefaultClient() override;

private:
    // Include the default reference counting implementation
    IMPLEMENT_REFCOUNTING(App);
};
