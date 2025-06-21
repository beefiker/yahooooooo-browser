#import <Cocoa/Cocoa.h>
#include "mac_window.h"

@interface BrowserWindow : NSWindow
@end

@implementation BrowserWindow
- (BOOL)canBecomeKeyWindow {
    return YES;
}
- (BOOL)canBecomeMainWindow {
    return YES;
}
@end

void* CreateNativeWindow(int width, int height, const char* title) {
    @autoreleasepool {
        NSRect frame = NSMakeRect(100, 100, width, height);
        
        NSWindowStyleMask styleMask = NSWindowStyleMaskTitled |
                                      NSWindowStyleMaskClosable |
                                      NSWindowStyleMaskMiniaturizable |
                                      NSWindowStyleMaskResizable;
        
        BrowserWindow* window = [[BrowserWindow alloc] initWithContentRect:frame
                                                                 styleMask:styleMask
                                                                   backing:NSBackingStoreBuffered
                                                                     defer:NO];
        
        [window setTitle:[NSString stringWithUTF8String:title]];
        [window makeKeyAndOrderFront:nil];
        [window center];
        
        return (__bridge void*)window.contentView;
    }
} 
