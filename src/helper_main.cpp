// Helper main for CEF subprocesses
// This is used for the helper processes and doesn't include the main App class

#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"

// Function declarations
CefRefPtr<CefApp> CreateOtherApplication();
CefRefPtr<CefApp> CreateRendererApplication();

int main(int argc, char* argv[]) {
    // Provide CEF with command-line arguments
    CefMainArgs main_args(argc, argv);

    // Create the appropriate app based on process type
    CefRefPtr<CefApp> app;
    
    // Check command line for process type
    CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
    command_line->InitFromArgv(argc, argv);
    
    if (command_line->HasSwitch("type")) {
        std::string process_type = command_line->GetSwitchValue("type");
        
        if (process_type == "renderer") {
            app = CreateRendererApplication();
        } else {
            app = CreateOtherApplication();
        }
    }

    // Execute the secondary process
    return CefExecuteProcess(main_args, app, nullptr);
} 
