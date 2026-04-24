#include "komga_api.h"

#include "http_client.h"

#include "config.h"

#include <nlohmann/json.hpp>

#include <curl/curl.h>

#include <sstream>

using json = nlohmann::json;

using namespace std;

// ---- Runtime API base ----
static string s_apiBase; // overridden by setApiBase()

void KomgaApi::setApiBase(const string& base) 
{ 
    
    s_apiBase = base; 

}

string KomgaApi::getApiBase() 
{ 
    
    return s_apiBase; 

}

static string js(const json& j, const char* key, const string& def = "") 
{
    
    if (j.contains(key) && j[key].is_string()) 
    { 
    
        return j[key].get<string>();
    
    }

    return def;

}

static int ji(const json& j, const char* key, int def = 0) 
{

    if (j.contains(key) && j[key].is_number()) 
    {
        
        return j[key].get<int>();
    
    }

    return def;
}

bool KomgaApi::login(const string& user, const string& pass) 
{
    
    HttpClient::setCredentials(user, pass);

    auto url = s_apiBase + "/libraries";
    
    auto resp = HttpClient::get(url);

    if (!resp.ok()) 
    {
    
        HttpClient::clearCredentials();
    
        return false;
    
    }

    return true;

}

void KomgaApi::logout() 
{ 
    
    HttpClient::clearCredentials(); 

}

vector<KomgaLibrary> KomgaApi::getLibraries() 
{
    
    vector<KomgaLibrary> libs;
    
    auto resp = HttpClient::get(s_apiBase + "/libraries");
    
    if (!resp.ok()) 
    { 
        
        return libs;
    }
    
    try 
    {
        
        auto j = json::parse(resp.body);
        
        for (auto& item : j) 
        {
        
            KomgaLibrary lib;
        
            lib.id   = js(item, "id");
        
            lib.name = js(item, "name");
        
            libs.push_back(move(lib));
        
        }

    } 
    
    catch (...) 
    {
    
        printf("JSON parse error\n");
    
    }
    
    return libs;

}

KomgaPage_Result<KomgaSeries> KomgaApi::getSeries(const string& libraryId, int page, int size, const string& search)
{

    KomgaPage_Result<KomgaSeries> result;
    
    ostringstream url;
    
    url << s_apiBase << "/series?page=" << page << "&size=" << size << "&sort=metadata.titleSort,asc";
    
    if (!libraryId.empty()) 
    {
        
        url << "&library_id=" << libraryId;
    
    }
    
    if (!search.empty()) 
    {
        
        CURL* tmp = curl_easy_init();
        
        if (tmp) 
        {
        
            char* enc = curl_easy_escape(tmp, search.c_str(), (int)search.size());
        
            if (enc) 
            { 
            
                url << "&search=" << enc; curl_free(enc); 
            
            }
        
            curl_easy_cleanup(tmp);
        
        }
    
    }
    
    auto resp = HttpClient::get(url.str());
    
    if (!resp.ok()) 
    {
        
        return result;
    
    }

    try 
    {
        
        auto j = json::parse(resp.body);
        
        result.totalPages    = ji(j, "totalPages");
        
        result.totalElements = ji(j, "totalElements");
        
        result.number        = ji(j, "number");
        
        result.size          = ji(j, "size");
        
        for (auto& item : j["content"]) 
        {
            
            KomgaSeries s;
            
            s.id          = js(item, "id");
            
            s.libraryId   = js(item, "libraryId");
            
            s.booksCount  = ji(item, "booksCount");
            
            s.booksUnread = ji(item, "booksUnreadCount");
            
            if (item.contains("metadata")) 
            {
                
                s.name   = js(item["metadata"], "title");
                
                s.status = js(item["metadata"], "status");
            
            }
            
            s.thumbnailUrl = seriesThumbnailUrl(s.id);
            
            result.content.push_back(move(s));
        }

    } 
    
    catch (...) 
    {

        printf("JSON parse error\n");

    }
    
    return result;

}

KomgaPage_Result<KomgaBook> KomgaApi::getBooks(const string& seriesId, int page, int size)
{
    
    KomgaPage_Result<KomgaBook> result;
    
    ostringstream url;
    
    url << s_apiBase << "/series/" << seriesId << "/books?page=" << page << "&size=" << size << "&sort=metadata.numberSort,asc";
    
    auto resp = HttpClient::get(url.str());
    
    if (!resp.ok()) 
    {
        
        return result;
    }

    try 
    {
        
        auto j = json::parse(resp.body);
        
        result.totalPages    = ji(j, "totalPages");
        
        result.totalElements = ji(j, "totalElements");
        
        result.number        = ji(j, "number");
        
        result.size          = ji(j, "size");
        
        for (auto& item : j["content"]) 
        {
            
            KomgaBook b;
            
            b.id       = js(item, "id");
            
            b.seriesId = js(item, "seriesId");
            
            if (item.contains("media") && !item["media"].is_null()) 
            {

                b.pagesCount = ji(item["media"], "pagesCount");

            }

            if (item.contains("metadata")) 
            {
               
                auto& m = item["metadata"];

                // title fallback chain (Komga is inconsistent)
                b.name = js(m, "title", js(m, "name", js(item, "name", "Untitled")));

                // safe number parsing
                if (m.contains("number")) 
                {
                    try 
                    {
                        if (m["number"].is_string()) 
                        {

                            b.number = stoi(m["number"].get<string>());
                        
                        } 
                        else if (m["number"].is_number()) 
                        {

                            b.number = (int)m["number"].get<float>();
                        
                        }
                    
                    } 
                    
                    catch (...) 
                    {
                        
                        b.number = 0;
                    
                    }
                
                }

            }   

            if (item.contains("readProgress") && !item["readProgress"].is_null()) 
            {

                b.readProgress = ji(item["readProgress"], "page");
            
            }

            b.thumbnailUrl = bookThumbnailUrl(b.id);
            
            result.content.push_back(move(b));
        
        }
    
    } 
    
    catch (...) 
    {

        printf("JSON parse error\n");

    }
    
    return result;

}

vector<KomgaPage> KomgaApi::getBookPages(const string& bookId) {
    
    vector<KomgaPage> pages;
    
    auto resp = HttpClient::get(s_apiBase + "/books/" + bookId + "/pages");
    
    if (!resp.ok()) 
    {
        
        return pages;
    
    }

    try 
    {
        
        auto j = json::parse(resp.body);
        
        for (auto& item : j) 
        {
            
            KomgaPage p;
            
            p.number    = ji(item, "number");
            
            p.mediaType = js(item, "mediaType");
            
            p.width     = ji(item, "width");
            
            p.height    = ji(item, "height");
            
            pages.push_back(p);
        
        }

    } 
    
    catch (...) 
    {
            
        printf("JSON parse error\n");

    }
    
    return pages;
}

string KomgaApi::seriesThumbnailUrl(const string& id) 
{
    
    return s_apiBase + "/series/" + id + "/thumbnail";

}

string KomgaApi::bookThumbnailUrl(const string& id) 
{
    
    return s_apiBase + "/books/" + id + "/thumbnail";

}

string KomgaApi::bookPageUrl(const string& bookId, int pageNum) 
{

    return s_apiBase + "/books/" + bookId + "/pages/" + to_string(pageNum) + "?zero_based=true";

}

bool KomgaApi::markProgress(const string& bookId, int page) 
{
    
    return HttpClient::patch( s_apiBase + "/books/" + bookId + "/read-progress", "{\"page\":" + to_string(page) + "}" );

}