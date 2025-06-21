// Helper main for CEF subprocesses
// This is used for the helper processes with specialized handlers
// for different process types (renderer, GPU, plugin, utility)

#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"
#include "app_renderer.h"
#include "app_other.h"
#include <iostream>

// Enhanced subprocess creation functions
CefRefPtr<CefApp> CreateOtherApplication() {
    return new AppOther();
}

CefRefPtr<CefApp> CreateRendererApplication() {
    return new AppRenderer();
}

int main(int argc, char* argv[]) {
    std::cout << "🚀 Helper Process Starting..." << std::endl;
    
    // Provide CEF with command-line arguments
    CefMainArgs main_args(argc, argv);

    // Create the appropriate app based on process type
    CefRefPtr<CefApp> app;
    
    // Check command line for process type
    CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
    command_line->InitFromArgv(argc, argv);
    
    if (command_line->HasSwitch("type")) {
        std::string process_type = command_line->GetSwitchValue("type");
        std::cout << "🔧 Helper Process Type: " << process_type << std::endl;
        
        if (process_type == "renderer") {
            std::cout << "🎨 Creating Renderer Process Handler" << std::endl;
            app = CreateRendererApplication();
        } else {
            std::cout << "🔧 Creating Other Process Handler (" << process_type << ")" << std::endl;
            app = CreateOtherApplication();
        }
    } else {
        std::cout << "⚠️ No process type specified, using default handler" << std::endl;
        app = CreateOtherApplication();
    }

    std::cout << "🔧 Executing subprocess..." << std::endl;
    
    // Execute the secondary process
    int exit_code = CefExecuteProcess(main_args, app, nullptr);
    
    std::cout << "✅ Helper Process finished with code: " << exit_code << std::endl;
    return exit_code;
} 
