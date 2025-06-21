#include "app_other.h"
#include <iostream>

void AppOther::OnBeforeCommandLineProcessing(const CefString& process_type,
                                           CefRefPtr<CefCommandLine> command_line) {
    std::cout << "🔧 Other Process (" << process_type.ToString() << "): Command line processing" << std::endl;
    
    // Add any subprocess-specific command line flags
    // For example, GPU process might need specific flags
    if (process_type == "gpu-process") {
        // GPU-specific flags
        command_line->AppendSwitch("disable-gpu-sandbox");
        std::cout << "🎮 GPU Process: Added GPU-specific flags" << std::endl;
    } else if (process_type == "plugin") {
        // Plugin-specific flags
        std::cout << "🔌 Plugin Process: Initialized" << std::endl;
    } else if (process_type == "utility") {
        // Utility process flags
        std::cout << "🛠️ Utility Process: Initialized" << std::endl;
    } else {
        std::cout << "❓ Unknown Process Type: " << process_type.ToString() << std::endl;
    }
}

void AppOther::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) {
    std::cout << "🔧 Other Process: Registering custom schemes" << std::endl;
    
    // Register custom schemes that all processes need to know about
    // For example, a custom protocol for internal browser communication
    registrar->AddCustomScheme("yahooooooo", 
                              CEF_SCHEME_OPTION_STANDARD | 
                              CEF_SCHEME_OPTION_LOCAL |
                              CEF_SCHEME_OPTION_CORS_ENABLED);
    
    std::cout << "✅ Other Process: Custom schemes registered" << std::endl;
} 
