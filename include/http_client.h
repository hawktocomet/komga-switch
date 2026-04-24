#pragma once

#include <string>

#include <vector>

#include <functional>

using namespace std;

// ---- HTTP Response ----
struct HttpResponse 
{

    long        status  = 0;

    string body;

    bool        ok() const { return status >= 200 && status < 300; }

};

// ---- HttpClient ----
// Thin curl wrapper.  Call init() once at startup, cleanup() at exit.
class HttpClient 
{

    public:
        
        static void init();
        
        static void cleanup();

        // Set Basic-Auth credentials (called after login)
        static void setCredentials(const string& user, const string& pass);
        
        static void clearCredentials();

        // Synchronous GET – returns response struct
        static HttpResponse get(const string& url);

        // PATCH with a JSON body – returns true on 2xx
        static bool patch(const string& url, const string& jsonBody);

        // Download binary data (e.g. cover images) into a byte buffer
        static bool download(const string& url, vector<uint8_t>& outBytes);

    private:

        static string s_user;

        static string s_pass;

};