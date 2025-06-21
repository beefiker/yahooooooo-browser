// Copyright (c) 2017 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/cef_app.h"

// This file is shared by cefclient and cefsimple. It doesn't do anything but 
// it's required for CEF helper processes.

// Entry point function for sub-processes.
class MinimalOtherApp : public CefApp {
 public:
  MinimalOtherApp() {}

 private:
  IMPLEMENT_REFCOUNTING(MinimalOtherApp);
  DISALLOW_COPY_AND_ASSIGN(MinimalOtherApp);
};

// Return the application instance. Called from main().
CefRefPtr<CefApp> CreateOtherApplication() {
  return new MinimalOtherApp();
}
