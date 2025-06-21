// @cursor:phase-3
// Tab Manager - Handles multiple browser tabs
// Manages tab creation, switching, closing, and state

#pragma once

#include <string>
#include <vector>
#include <memory>

/**
 * Represents a single browser tab
 */
struct BrowserTab {
    int id;
    std::string title;
    std::string url;
    bool is_loading;
    bool is_active;
    
    BrowserTab(int tab_id, const std::string& tab_url = "about:blank") 
        : id(tab_id), url(tab_url), title("New Tab"), is_loading(false), is_active(false) {}
};

/**
 * Tab Manager - Handles multiple browser tabs
 * Provides tab creation, switching, and management functionality
 */
class TabManager {
public:
    TabManager();
    ~TabManager() = default;

    // Tab management
    int CreateTab(const std::string& url = "about:blank");
    bool CloseTab(int tab_id);
    bool SwitchToTab(int tab_id);
    
    // Tab operations
    void LoadUrlInTab(int tab_id, const std::string& url);
    void UpdateTabTitle(int tab_id, const std::string& title);
    void SetTabLoading(int tab_id, bool loading);
    
    // Tab information
    std::vector<BrowserTab> GetAllTabs() const;
    BrowserTab* GetActiveTab();
    BrowserTab* GetTab(int tab_id);
    int GetActiveTabId() const;
    int GetTabCount() const;
    
    // Tab navigation
    void SwitchToNextTab();
    void SwitchToPreviousTab();
    void MoveTab(int tab_id, int new_position);
    
    // Duplicate and restore
    int DuplicateTab(int tab_id);
    void RestoreClosedTab(); // TODO: Implement closed tab history

private:
    std::vector<std::unique_ptr<BrowserTab>> tabs_;
    int active_tab_id_;
    int next_tab_id_;
    
    // Helper methods
    int FindTabIndex(int tab_id) const;
    void EnsureActiveTab();
}; 