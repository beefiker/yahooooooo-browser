#ifndef RENDER_HANDLER_H
#define RENDER_HANDLER_H

#include "include/cef_render_handler.h"
#include <iostream>

class SimpleRenderHandler : public CefRenderHandler {
public:
    SimpleRenderHandler() = default;

    // CefRenderHandler methods:
    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override {
        // Return a fixed size for the view
        rect.x = 0;
        rect.y = 0; 
        rect.width = 1200;
        rect.height = 800;
    }

    void OnPaint(CefRefPtr<CefBrowser> browser,
                 PaintElementType type,
                 const RectList& dirtyRects,
                 const void* buffer,
                 int width,
                 int height) override {
        // For now, just log that painting occurred
        // In a real implementation, we'd render this to a window/view
        static int frame_count = 0;
        frame_count++;
        if (frame_count % 30 == 0) {  // Log every 30 frames
            std::cout << "🎨 Paint frame " << frame_count << " (" << width << "x" << height << ")" << std::endl;
        }
    }

private:
    IMPLEMENT_REFCOUNTING(SimpleRenderHandler);
    DISALLOW_COPY_AND_ASSIGN(SimpleRenderHandler);
};

#endif // RENDER_HANDLER_H 
