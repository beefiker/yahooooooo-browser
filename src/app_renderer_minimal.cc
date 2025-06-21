// Copyright (c) 2017 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/cef_app.h"

// Minimal renderer process application
class MinimalRendererApp : public CefApp, public CefRenderProcessHandler {
 public:
  MinimalRendererApp() {}

  // CefApp methods:
  CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
    return this;
  }

  // CefRenderProcessHandler methods:
  void OnWebKitInitialized() override {
    // Optional: Register custom JavaScript bindings here
  }

  void OnContextCreated(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefFrame> frame,
                       CefRefPtr<CefV8Context> context) override {
    // Optional: Setup V8 context here
  }

 private:
  IMPLEMENT_REFCOUNTING(MinimalRendererApp);
  DISALLOW_COPY_AND_ASSIGN(MinimalRendererApp);
};

// Return the renderer application instance. Called from main().
CefRefPtr<CefApp> CreateRendererApplication() {
  return new MinimalRendererApp();
}
