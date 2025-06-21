// @cursor:phase-3
// Tab Manager implementation
// Handles multiple browser tabs, switching, and management

#include "tab_manager.h"
#include <iostream>
#include <algorithm>

TabManager::TabManager() : active_tab_id_(-1), next_tab_id_(1) {
    std::cout << "📑 Tab Manager initialized" << std::endl;
}

int TabManager::CreateTab(const std::string& url) {
    int new_tab_id = next_tab_id_++;
    auto new_tab = std::make_unique<BrowserTab>(new_tab_id, url);
    
    // Set as active if it's the first tab
    if (tabs_.empty()) {
        new_tab->is_active = true;
        active_tab_id_ = new_tab_id;
    }
    
    tabs_.push_back(std::move(new_tab));
    
    std::cout << "📑 Created new tab #" << new_tab_id << " with URL: " << url << std::endl;
    return new_tab_id;
}

bool TabManager::CloseTab(int tab_id) {
    int index = FindTabIndex(tab_id);
    if (index == -1) {
        std::cout << "❌ Tab #" << tab_id << " not found" << std::endl;
        return false;
    }
    
    bool was_active = tabs_[index]->is_active;
    
    std::cout << "❌ Closing tab #" << tab_id << ": " << tabs_[index]->title << std::endl;
    tabs_.erase(tabs_.begin() + index);
    
    // Handle active tab switching if we closed the active tab
    if (was_active) {
        if (tabs_.empty()) {
            active_tab_id_ = -1;
            std::cout << "📑 No tabs remaining" << std::endl;
        } else {
            // Switch to the tab to the right, or leftmost if we closed the rightmost
            int new_active_index = std::min(index, static_cast<int>(tabs_.size()) - 1);
            SwitchToTab(tabs_[new_active_index]->id);
        }
    }
    
    return true;
}

bool TabManager::SwitchToTab(int tab_id) {
    int index = FindTabIndex(tab_id);
    if (index == -1) {
        std::cout << "❌ Tab #" << tab_id << " not found" << std::endl;
        return false;
    }
    
    // Deactivate current tab
    if (active_tab_id_ != -1) {
        auto* current_tab = GetTab(active_tab_id_);
        if (current_tab) {
            current_tab->is_active = false;
        }
    }
    
    // Activate new tab
    tabs_[index]->is_active = true;
    active_tab_id_ = tab_id;
    
    std::cout << "📑 Switched to tab #" << tab_id << ": " << tabs_[index]->title << std::endl;
    return true;
}

void TabManager::LoadUrlInTab(int tab_id, const std::string& url) {
    auto* tab = GetTab(tab_id);
    if (!tab) {
        std::cout << "❌ Tab #" << tab_id << " not found" << std::endl;
        return;
    }
    
    tab->url = url;
    tab->is_loading = true;
    tab->title = "Loading...";
    
    std::cout << "🌐 Loading URL in tab #" << tab_id << ": " << url << std::endl;
}

void TabManager::UpdateTabTitle(int tab_id, const std::string& title) {
    auto* tab = GetTab(tab_id);
    if (!tab) return;
    
    tab->title = title;
    std::cout << "📋 Updated tab #" << tab_id << " title: " << title << std::endl;
}

void TabManager::SetTabLoading(int tab_id, bool loading) {
    auto* tab = GetTab(tab_id);
    if (!tab) return;
    
    tab->is_loading = loading;
    std::cout << "🔄 Tab #" << tab_id << " loading state: " << (loading ? "Yes" : "No") << std::endl;
}

std::vector<BrowserTab> TabManager::GetAllTabs() const {
    std::vector<BrowserTab> result;
    for (const auto& tab : tabs_) {
        result.push_back(*tab);
    }
    return result;
}

BrowserTab* TabManager::GetActiveTab() {
    return GetTab(active_tab_id_);
}

BrowserTab* TabManager::GetTab(int tab_id) {
    auto it = std::find_if(tabs_.begin(), tabs_.end(),
        [tab_id](const std::unique_ptr<BrowserTab>& tab) {
            return tab->id == tab_id;
        });
    
    return (it != tabs_.end()) ? it->get() : nullptr;
}

int TabManager::GetActiveTabId() const {
    return active_tab_id_;
}

int TabManager::GetTabCount() const {
    return static_cast<int>(tabs_.size());
}

void TabManager::SwitchToNextTab() {
    if (tabs_.size() <= 1) return;
    
    int current_index = FindTabIndex(active_tab_id_);
    if (current_index == -1) return;
    
    int next_index = (current_index + 1) % tabs_.size();
    SwitchToTab(tabs_[next_index]->id);
}

void TabManager::SwitchToPreviousTab() {
    if (tabs_.size() <= 1) return;
    
    int current_index = FindTabIndex(active_tab_id_);
    if (current_index == -1) return;
    
    int prev_index = (current_index - 1 + tabs_.size()) % tabs_.size();
    SwitchToTab(tabs_[prev_index]->id);
}

void TabManager::MoveTab(int tab_id, int new_position) {
    int current_index = FindTabIndex(tab_id);
    if (current_index == -1 || new_position < 0 || new_position >= tabs_.size()) {
        return;
    }
    
    auto tab = std::move(tabs_[current_index]);
    tabs_.erase(tabs_.begin() + current_index);
    tabs_.insert(tabs_.begin() + new_position, std::move(tab));
    
    std::cout << "📑 Moved tab #" << tab_id << " to position " << new_position << std::endl;
}

int TabManager::DuplicateTab(int tab_id) {
    auto* original_tab = GetTab(tab_id);
    if (!original_tab) return -1;
    
    int new_tab_id = CreateTab(original_tab->url);
    auto* new_tab = GetTab(new_tab_id);
    if (new_tab) {
        new_tab->title = original_tab->title + " (Copy)";
    }
    
    std::cout << "📑 Duplicated tab #" << tab_id << " as tab #" << new_tab_id << std::endl;
    return new_tab_id;
}

void TabManager::RestoreClosedTab() {
    // TODO: Implement closed tab history
    std::cout << "🔄 Restore closed tab functionality - TODO" << std::endl;
}

int TabManager::FindTabIndex(int tab_id) const {
    for (size_t i = 0; i < tabs_.size(); ++i) {
        if (tabs_[i]->id == tab_id) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void TabManager::EnsureActiveTab() {
    if (active_tab_id_ == -1 && !tabs_.empty()) {
        SwitchToTab(tabs_[0]->id);
    }
} 