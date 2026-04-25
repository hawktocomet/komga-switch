#include "config_store.h"

#include <nlohmann/json.hpp>

#include <fstream>

#include <sys/stat.h>   // mkdir

using namespace std;

using json = nlohmann::json;

static constexpr const char* CONFIG_DIR  = "sdmc:/switch/komga-switch";

static constexpr const char* CONFIG_PATH = "sdmc:/switch/komga-switch/config.json";

AppConfig ConfigStore::load() 
{

    AppConfig cfg;

    ifstream f(CONFIG_PATH);

    if (!f.is_open()) 
    {
        
        return cfg;
    
    }
    
    try 
    {
    
        json j;
    
        f >> j;
    
        if (j.contains("serverHost") && j["serverHost"].is_string()) 
        {
    
            cfg.serverHost = j["serverHost"].get<string>();

        }

        if (j.contains("serverPort") && j["serverPort"].is_string()) 
        {

            cfg.serverPort = j["serverPort"].get<string>();
        
        }

        if (j.contains("serverPath") && j["serverPath"].is_string()) 
        {

            cfg.serverPath = j["serverPath"].get<string>();
        
        }

        if (j.contains("saveCredentials") && j["saveCredentials"].is_boolean()) 
        {
         
            cfg.saveCredentials = j["saveCredentials"].get<bool>();
        
        }
        
        if (cfg.saveCredentials) 
        {
            
            if (j.contains("username") && j["username"].is_string()) 
            {

                cfg.username = j["username"].get<string>();
            
            }
                
            if (j.contains("password") && j["password"].is_string()) 
            {

                cfg.password = j["password"].get<string>();
            
            }
        
        }
    
    } 
    
    catch (...) 
    {

    }
    
    return cfg;
}

void ConfigStore::save(const AppConfig& cfg) 
{
    
    // Ensure directory exists
    mkdir(CONFIG_DIR, 0777);

    json j;
    
    j["serverHost"]       = cfg.serverHost;
    
    j["serverPort"]       = cfg.serverPort;

    j["serverPath"]       = cfg.serverPath;
    
    j["saveCredentials"]  = cfg.saveCredentials;
    
    if (cfg.saveCredentials) 
    {
    
        j["username"] = cfg.username;
    
        j["password"] = cfg.password;
    
    } 
    
    else 
    {
    
        j["username"] = "";
    
        j["password"] = "";
    
    }

    ofstream f(CONFIG_PATH);
    
    if (f.is_open()) 
    {
        
        f << j.dump(2);
    
    }
    
}

string ConfigStore::apiBase(const AppConfig& cfg) 
{
    
    return "http://" + cfg.serverHost + ":" + cfg.serverPort + cfg.serverPath + "/api/v1";

}