// @cursor:phase-1
// Client implementation - Main CEF client handler
// CEF 137 compatible implementation with runtime style support

#include "client.h"
#include "include/cef_app.h"
#include "include/cef_callback.h"
#include "include/wrapper/cef_helpers.h"
#include <iostream>
#include <list>

#include <sstream>
#include <string>

#include "include/base/cef_callback.h"
#include "include/cef_parser.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_closure_task.h"

namespace {

Client* g_instance = nullptr;

// Returns a data: URI with the specified contents.
std::string GetDataURI(const std::string& data, const std::string& mime_type) {
  return "data:" + mime_type + ";base64," +
         CefURIEncode(CefBase64Encode(data.data(), data.size()), false)
             .ToString();
}

}  // namespace

Client::Client(bool is_alloy_style)
    : is_alloy_style_(is_alloy_style) {
  DCHECK(!g_instance);
  g_instance = this;
}

Client::~Client() {
  g_instance = nullptr;
}

// static
Client* Client::GetInstance() {
  return g_instance;
}

// CefDisplayHandler methods
void Client::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) {
    CEF_REQUIRE_UI_THREAD();

    if (auto browser_view = CefBrowserView::GetForBrowser(browser)) {
        // Set the title of the window using the Views framework.
        CefRefPtr<CefWindow> window = browser_view->GetWindow();
        if (window) {
            window->SetTitle(title);
        }
    } else if (is_alloy_style_) {
        // Set the title of the window using platform APIs.
        PlatformTitleChange(browser, title);
    }
}



void Client::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();

    // Sanity-check the configured runtime style.
    CHECK_EQ(is_alloy_style_ ? CEF_RUNTIME_STYLE_ALLOY : CEF_RUNTIME_STYLE_CHROME,
             browser->GetHost()->GetRuntimeStyle());

    // Add to the list of existing browsers.
    browser_list_.push_back(browser);
}

bool Client::DoClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();

    // Closing the main window requires special handling. See the DoClose()
    // documentation in the CEF header for a detailed destription of this
    // process.
    if (browser_list_.size() == 1) {
        // Set a flag to indicate that the window close should be allowed.
        is_closing_ = true;
    }

    // Allow the close. For windowed browsers this will result in the OS close
    // event being sent.
    return false;
}

void Client::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();

    // Remove from the list of existing browsers.
    BrowserList::iterator bit = browser_list_.begin();
    for (; bit != browser_list_.end(); ++bit) {
        if ((*bit)->IsSame(browser)) {
            browser_list_.erase(bit);
            break;
        }
    }

    if (browser_list_.empty()) {
        // All browser windows have closed. Quit the application message loop.
        CefQuitMessageLoop();
    }
}



void Client::OnLoadError(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        ErrorCode errorCode,
                        const CefString& errorText,
                        const CefString& failedUrl) {
    CEF_REQUIRE_UI_THREAD();

    // Allow Chrome to show the error page.
    if (!is_alloy_style_) {
        return;
    }

    // Don't display an error for downloaded files.
    if (errorCode == ERR_ABORTED) {
        return;
    }

    // Display a load error message using a data: URI.
    std::stringstream ss;
    ss << "<html><body bgcolor=\"white\">"
          "<h2>Failed to load URL "
       << std::string(failedUrl) << " with error " << std::string(errorText)
       << " (" << errorCode << ").</h2></body></html>";

    frame->LoadURL(GetDataURI(ss.str(), "text/html"));
}



void Client::ShowMainWindow() {
    if (!CefCurrentlyOn(TID_UI)) {
        // Execute on the UI thread.
        CefPostTask(TID_UI, base::BindOnce(&Client::ShowMainWindow, this));
        return;
    }

    if (browser_list_.empty()) {
        return;
    }

    auto main_browser = browser_list_.front();

    if (auto browser_view = CefBrowserView::GetForBrowser(main_browser)) {
        // Show the window using the Views framework.
        if (auto window = browser_view->GetWindow()) {
            window->Show();
        }
    } else if (is_alloy_style_) {
        PlatformShowWindow(main_browser);
    }
}

void Client::CloseAllBrowsers(bool force_close) {
    if (!CefCurrentlyOn(TID_UI)) {
        // Execute on the UI thread.
        CefPostTask(TID_UI, base::BindOnce(&Client::CloseAllBrowsers, this,
                                           force_close));
        return;
    }

    if (browser_list_.empty()) {
        return;
    }

    BrowserList::const_iterator it = browser_list_.begin();
    for (; it != browser_list_.end(); ++it) {
        (*it)->GetHost()->CloseBrowser(force_close);
    }
}

#if !defined(OS_MAC)
void Client::PlatformShowWindow(CefRefPtr<CefBrowser> browser) {
    NOTIMPLEMENTED();
}

void Client::PlatformTitleChange(CefRefPtr<CefBrowser> browser,
                                 const CefString& title) {
    NOTIMPLEMENTED();
}
#endif 

