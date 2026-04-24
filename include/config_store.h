#pragma once

#include <string>

using std::string;

// ─── Persisted configuration (sdmc:/switch/komga-switch/config.json) ─────────
struct AppConfig 
{
    
    string serverHost;
    
    string serverPort;
    
    string username;
    
    string password;
    
    bool        saveCredentials = false;

};

namespace ConfigStore 
{
    // Load from SD card. Returns default AppConfig on failure.
    AppConfig load();

    // Save to SD card.
    void save(const AppConfig& cfg);

    // Build the base API URL from a config.
    string apiBase(const AppConfig& cfg);

}