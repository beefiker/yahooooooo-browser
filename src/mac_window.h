#ifndef MAC_WINDOW_H
#define MAC_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

// Create a native macOS window and return its content view handle
void* CreateNativeWindow(int width, int height, const char* title);

#ifdef __cplusplus
}
#endif

#endif // MAC_WINDOW_H
