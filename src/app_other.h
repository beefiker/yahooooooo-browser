#pragma once

#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"

// Other subprocess application handler
// Handles GPU process, Plugin process, and other non-renderer subprocesses
// These processes typically don't need complex handlers but may need
// specific initialization or cleanup
class AppOther : public CefApp {
public:
    AppOther() = default;

    // CefApp methods - most subprocesses don't need special handlers
    void OnBeforeCommandLineProcessing(const CefString& process_type,
                                     CefRefPtr<CefCommandLine> command_line) override;

    void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;

private:
    IMPLEMENT_REFCOUNTING(AppOther);
    DISALLOW_COPY_AND_ASSIGN(AppOther);
}; 
