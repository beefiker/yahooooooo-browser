// CEF 137 Compatible Main - Following cefsimple pattern
// Main entry point for yahooooooo-browser

#include <iostream>
#include <string>
#include <cstdlib>

#include "include/cef_app.h"
#include "include/cef_client.h"
#include "include/wrapper/cef_helpers.h"

#ifdef __APPLE__
#include "include/wrapper/cef_library_loader.h"
#endif

#include "app.h"

// Function to get the absolute path to test HTML files
std::string getTestHtmlPath(const std::string& filename) {
    std::string currentFile = __FILE__;
    size_t pos = currentFile.find_last_of("/");
    if (pos != std::string::npos) {
        std::string projectDir = currentFile.substr(0, pos);
        return projectDir + "/resources/" + filename;
    }
    return "./src/resources/" + filename;
}

// Function to open test HTML in default browser
void openTestHtml(const std::string& filename, const std::string& testName) {
    std::string htmlPath = getTestHtmlPath(filename);
    
    std::string command = "open \"" + htmlPath + "\"";
    
    std::cout << "🧪 " << testName << " TEST MODE ACTIVATED" << std::endl;
    std::cout << "🌐 Opening test HTML in default browser..." << std::endl;
    std::cout << "📄 File: " << htmlPath << std::endl;
    
    int result = system(command.c_str());
    if (result == 0) {
        std::cout << "✅ Successfully opened test page!" << std::endl;
        std::cout << "🎯 Your browser should now display the " << testName << " test page!" << std::endl;
    } else {
        std::cout << "❌ Failed to open test page" << std::endl;
        std::cout << "💡 Try manually opening: " << htmlPath << std::endl;
    }
}

int main(int argc, char* argv[]) {
#ifdef __APPLE__
    // Load the CEF framework library at runtime instead of linking directly
    // as required by the macOS sandbox implementation.
    CefScopedLibraryLoader library_loader;
    if (!library_loader.LoadInMain()) {
        return 1;
    }
#endif

    // Provide CEF with command-line arguments.
    CefMainArgs main_args(argc, argv);

    // Check for test flags
    bool testMode = false;
    bool multiprocessTestMode = false;
    
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--test") {
            testMode = true;
            break;
        } else if (std::string(argv[i]) == "--test-multiprocess") {
            multiprocessTestMode = true;
            break;
        }
    }

    if (testMode) {
        openTestHtml("simple_test.html", "SIMPLE");
        return 0;
    }
    
    if (multiprocessTestMode) {
        openTestHtml("multiprocess_test.html", "MULTIPROCESS");
        return 0;
    }

    // Parse command-line arguments for use in this method.
    CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
    command_line->InitFromArgv(argc, argv);

    // CEF settings
    CefSettings settings;
    
    // Set cache path to fix warning about default cache path
    std::string cache_path = std::string(getenv("HOME")) + "/.yahooooooo-browser-cache";
    CefString(&settings.cache_path) = cache_path;
    
    // Disable logging to reduce noise
    settings.log_severity = LOGSEVERITY_DISABLE;
    
    // Disable sandbox for stability
    settings.no_sandbox = true;
    
    // macOS specific settings to fix NSApplication issues
#ifdef __APPLE__
    settings.external_message_pump = false;
    settings.multi_threaded_message_loop = false;
#endif

    // App implements application-level callbacks for the browser process.
    // It will create the first browser instance in OnContextInitialized() after
    // CEF has initialized.
    CefRefPtr<App> app(new App);

    // Initialize the CEF browser process. May return false if initialization
    // fails or if early exit is desired (for example, due to process singleton
    // relaunch behavior).
    if (!CefInitialize(main_args, settings, app.get(), nullptr)) {
        return CefGetExitCode();
    }

    // Run the CEF message loop. This will block until CefQuitMessageLoop() is
    // called.
    CefRunMessageLoop();

    // Shut down CEF.
    CefShutdown();

    return 0;
}
