#pragma once

#include "include/cef_app.h"
#include "include/cef_render_process_handler.h"
#include "include/wrapper/cef_helpers.h"

// Renderer process application handler
// Handles renderer-specific functionality like JavaScript extensions,
// DOM access, and render process communication
class AppRenderer : public CefApp, public CefRenderProcessHandler {
public:
    AppRenderer() = default;

    // CefApp methods
    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
        return this;
    }

    // CefRenderProcessHandler methods
    void OnContextCreated(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         CefRefPtr<CefV8Context> context) override;

    void OnContextReleased(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> frame,
                          CefRefPtr<CefV8Context> context) override;

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefProcessId source_process,
                                 CefRefPtr<CefProcessMessage> message) override;

    void OnWebKitInitialized() override;

private:
    IMPLEMENT_REFCOUNTING(AppRenderer);
    DISALLOW_COPY_AND_ASSIGN(AppRenderer);
};
