#pragma once

#include <string>

#include <vector>

using std::string;
using std::vector;

// ---- Data Types ----

// ---- Library ----
struct KomgaLibrary 
{

    string id;

    string name;

};

// ---- Series ----
struct KomgaSeries 
{

    string id;

    string libraryId;

    string name;

    int         booksCount   = 0;

    int         booksUnread  = 0;

    string status;       // ONGOING, ENDED, ABANDONED, HIATUS

    string thumbnailUrl; // filled in by client helper

};

// ---- Book ----
struct KomgaBook 
{

    string id;

    string seriesId;

    string name;

    int         number       = 0;

    int         pagesCount   = 0;

    int         readProgress = 0; // last page read (0 = unread)

    string thumbnailUrl;

};

// ---- Page ----
struct KomgaPage 
{

    int         number = 0;

    string mediaType;

    int         width  = 0;

    int         height = 0;

};

template<typename T>

// ---- Result ----
struct KomgaPage_Result 
{

    vector<T> content;

    int totalPages    = 0;

    int totalElements = 0;

    int number        = 0; // current page (0-based)

    int size          = 0;

};

// ---- API Client ----
class KomgaApi 
{

    public:

        // ---- Configure the server at runtime (call before any other API method) ----
        static void        setApiBase(const string& base);
            
        static string getApiBase();

        
        // ---- Auth ----
        static bool login(const string& user, const string& pass);
        
        static void logout();

        
        // ---- Libraries ----
        static vector<KomgaLibrary> getLibraries();

            
        // ---- Series ----
        static KomgaPage_Result<KomgaSeries> getSeries( const string& libraryId = "", int page = 0, int size = 10, const string& search = "");

        // ---- Books ----
        static KomgaPage_Result<KomgaBook> getBooks( const string& seriesId, int page = 0, int size = 20);

        // ---- Pages inside a book ----
        static vector<KomgaPage> getBookPages(const string& bookId);

        // ---- URL helpers (no network call) ----
        static string seriesThumbnailUrl(const string& seriesId);
        
        static string bookThumbnailUrl  (const string& bookId);
        
        static string bookPageUrl       (const string& bookId, int pageNum);

        // ---- Mark reading progress ----
        static bool markProgress(const string& bookId, int page);

};