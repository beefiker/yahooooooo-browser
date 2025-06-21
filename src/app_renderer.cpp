#include "app_renderer.h"
#include "include/cef_dom.h"
#include "include/wrapper/cef_helpers.h"
#include <iostream>

void AppRenderer::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefV8Context> context) {
    CEF_REQUIRE_RENDERER_THREAD();
    
    std::cout << "🎨 Renderer: Context created for frame " << frame->GetIdentifier() << std::endl;
    
    // Enter the V8 context
    CefRefPtr<CefV8Context> v8_context = frame->GetV8Context();
    if (v8_context && v8_context->Enter()) {
        // Get the global object
        CefRefPtr<CefV8Value> global = v8_context->GetGlobal();
        
        // Create yahooooooo browser object
        CefRefPtr<CefV8Value> yahooooooo = CefV8Value::CreateObject(nullptr, nullptr);
        
        // Add browser info
        yahooooooo->SetValue("name", CefV8Value::CreateString("yahooooooo-browser"), V8_PROPERTY_ATTRIBUTE_READONLY);
        yahooooooo->SetValue("version", CefV8Value::CreateString("1.0.0"), V8_PROPERTY_ATTRIBUTE_READONLY);
        yahooooooo->SetValue("multiProcess", CefV8Value::CreateBool(true), V8_PROPERTY_ATTRIBUTE_READONLY);
        
        // Add to global object
        global->SetValue("yahooooooo", yahooooooo, V8_PROPERTY_ATTRIBUTE_READONLY);
        
        // Exit the V8 context
        v8_context->Exit();
        
        std::cout << "✅ Renderer: yahooooooo browser object injected into page" << std::endl;
    }
}

void AppRenderer::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefV8Context> context) {
    CEF_REQUIRE_RENDERER_THREAD();
    
    std::cout << "🎨 Renderer: Context released for frame " << frame->GetIdentifier() << std::endl;
}

bool AppRenderer::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                          CefRefPtr<CefFrame> frame,
                                          CefProcessId source_process,
                                          CefRefPtr<CefProcessMessage> message) {
    CEF_REQUIRE_RENDERER_THREAD();
    
    const std::string& message_name = message->GetName();
    std::cout << "🎨 Renderer: Received message '" << message_name << "' from process " << source_process << std::endl;
    
    if (message_name == "yahooooooo_ping") {
        // Respond with pong
        CefRefPtr<CefProcessMessage> response = CefProcessMessage::Create("yahooooooo_pong");
        response->GetArgumentList()->SetString(0, "Hello from renderer process!");
        frame->SendProcessMessage(PID_BROWSER, response);
        return true;
    }
    
    return false;
}

void AppRenderer::OnWebKitInitialized() {
    CEF_REQUIRE_RENDERER_THREAD();
    
    std::cout << "🎨 Renderer: WebKit initialized - ready for JavaScript extensions" << std::endl;
    
    // Here you could register custom JavaScript extensions
    // For example, custom APIs that websites can call
    
    // Example: Register a custom JavaScript function
    std::string extension_code = 
        "var yahooooooo;"
        "if (!yahooooooo)"
        "  yahooooooo = {};"
        "(function() {"
        "  yahooooooo.log = function(message) {"
        "    native function yahooooooo_log();"
        "    return yahooooooo_log(message);"
        "  };"
        "})();";
    
    CefRegisterExtension("yahooooooo/v8", extension_code, nullptr);
    
    std::cout << "✅ Renderer: Custom JavaScript extensions registered" << std::endl;
} 
