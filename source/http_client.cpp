#include "http_client.h"

#include "config.h"

#include <curl/curl.h>

#include <cstring>

using namespace std;

// ---- Static members ----
string HttpClient::s_user;

string HttpClient::s_pass;

// ---- curl write callbacks ----
static size_t writeString(void* ptr, size_t size, size_t nmemb, string* out) 
{
    
    out->append(static_cast<char*>(ptr), size * nmemb);
    
    return size * nmemb;

}

static size_t writeBytes(void* ptr, size_t size, size_t nmemb, vector<uint8_t>* out) 
{

    const uint8_t* data = static_cast<uint8_t*>(ptr);

    out->insert(out->end(), data, data + size * nmemb);

    return size * nmemb;

}

// ---- Init / Cleanup ----
void HttpClient::init() 
{ 
    
    curl_global_init(CURL_GLOBAL_DEFAULT); 

}

void HttpClient::cleanup() 
{ 
    
    curl_global_cleanup(); 

}

void HttpClient::setCredentials(const string& user, const string& pass) 
{

    s_user = user;

    s_pass = pass;

}

void HttpClient::clearCredentials() 
{

    s_user.clear(); s_pass.clear(); 

}

// ---- GET ----
HttpResponse HttpClient::get(const string& url) 
{
    
    HttpResponse resp;
    
    CURL* curl = curl_easy_init();
    
    if (!curl) 
    {
        
        return resp;
    
    }
    
    string body;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  writeString);
    
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
    
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, HTTP_TIMEOUT_SEC);
    
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    
    // ---- Accept JSON ----
    struct curl_slist* headers = nullptr;
    
    headers = curl_slist_append(headers, "Accept: application/json");
    
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    if (!s_user.empty()) 
    {
    
        string creds = s_user + ":" + s_pass;
    
        curl_easy_setopt(curl, CURLOPT_USERPWD, creds.c_str());
    
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    
    }

    CURLcode res = curl_easy_perform(curl);
    
    if (res == CURLE_OK) 
    {

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp.status);

    }

    resp.body = move(body);
    
    curl_slist_free_all(headers);
    
    curl_easy_cleanup(curl);
    
    return resp;

}

// ---- Binary download ----
bool HttpClient::download(const string& url, vector<uint8_t>& outBytes) 
{
    
    CURL* curl = curl_easy_init();
    
    if (!curl) 
    {
        
        return false;

    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeBytes);
    
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outBytes);
    
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, HTTP_TIMEOUT_SEC);
    
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    if (!s_user.empty()) 
    {
        
        string creds = s_user + ":" + s_pass;
        
        curl_easy_setopt(curl, CURLOPT_USERPWD,  creds.c_str());
        
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    
    }

    CURLcode res = curl_easy_perform(curl);
    
    long status  = 0;
    
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    
    curl_easy_cleanup(curl);
    
    return (res == CURLE_OK && status >= 200 && status < 300);

}

// ---- PATCH ----
bool HttpClient::patch(const string& url, const string& jsonBody) 
{
    
    CURL* curl = curl_easy_init();
    
    if (!curl) 
    { 
        
        return false;
    
    }

    struct curl_slist* headers = nullptr;
    
    headers = curl_slist_append(headers, "Content-Type: application/json");

    string ignored;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
    
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, HTTP_TIMEOUT_SEC);
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeString);
    
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ignored);

    if (!s_user.empty()) 
    {

        string creds = s_user + ":" + s_pass;
        
        curl_easy_setopt(curl, CURLOPT_USERPWD,  creds.c_str());
        
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    
    }

    curl_easy_perform(curl);
    
    long status = 0;
    
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    
    curl_slist_free_all(headers);
    
    curl_easy_cleanup(curl);
    
    return (status >= 200 && status < 300);

}