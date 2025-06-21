// CEF 137 Compatible Client - macOS platform implementation
// Platform-specific methods for macOS

#include "client.h"

#import <Cocoa/Cocoa.h>

#include "include/cef_browser.h"

void Client::PlatformTitleChange(CefRefPtr<CefBrowser> browser,
                                 const CefString& title) {
  NSView* view = (NSView*)browser->GetHost()->GetWindowHandle();
  NSWindow* window = [view window];
  std::string titleStr(title);
  NSString* str = [NSString stringWithUTF8String:titleStr.c_str()];
  [window setTitle:str];
}

void Client::PlatformShowWindow(CefRefPtr<CefBrowser> browser) {
  NSView* view = (NSView*)browser->GetHost()->GetWindowHandle();
  NSWindow* window = [view window];
  [window makeKeyAndOrderFront:nil];
}
