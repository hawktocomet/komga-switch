#pragma once

#include <string>

#include <vector>

#include <memory>

#include <SDL2/SDL.h>

#include <SDL2/SDL_ttf.h>

#include "komga_api.h"

#include "texture_cache.h"

#include "config.h"

using namespace std;

// ---- App Screens ----
enum class Screen 
{

    LOGIN,

    LIBRARIES,

    SERIES,

    BOOKS,

    READER,

};

// ---- UI Colours ----
struct Color 
{ 

    uint8_t r, g, b, a; 

};

constexpr Color C_BG        = COL_BG;

constexpr Color C_PANEL     = COL_PANEL;

constexpr Color C_ACCENT    = COL_ACCENT;

constexpr Color C_ACCENT2   = COL_ACCENT2;

constexpr Color C_TEXT      = COL_TEXT;

constexpr Color C_SUBTEXT   = COL_SUBTEXT;

constexpr Color C_HIGHLIGHT = COL_HIGHLIGHT;

constexpr Color C_SEL       = COL_SEL_BORDER;

// ---- App State ----
struct AppState 
{
    
    // ---- Navigation ----
    Screen screen = Screen::LOGIN;

    // ---- Login ----
    string inputServer;   // hostname / IP
    
    string inputPort;     // port number string
    
    string inputPath;     // optional base path e.g. /komga

    string inputUser;
    
    string inputPass;
    
    int loginField    = 0; // 0=server, 1=port, 2=user, 3=pass
    
    string loginError;
    
    bool loginLoading  = false;
    
    bool showPassword  = false;
    
    bool saveCredentials = false;

    // ---- Libraries ----
    vector<KomgaLibrary> libraries;
    
    int libSelected = 0;

    int libScroll = 0;

    // ---- Series ----
    KomgaPage_Result<KomgaSeries> seriesPage;
    
    string currentLibraryId;
    
    string currentLibraryName;
    
    int  seriesSelected   = 0;
    
    int  seriesPageIndex  = 0;
    
    bool seriesLoading    = false;
    
    string seriesSearch;
    
    bool seriesSearchMode = false;

    // ---- Books ----
    KomgaPage_Result<KomgaBook> booksPage;
    
    string currentSeriesId;
    
    string currentSeriesName;
    
    int  bookSelected    = 0;
    
    int  bookPageIndex   = 0;
    
    bool booksLoading    = false;

    // ---- Reader ----
    string currentBookId;
    
    string currentBookName;
    
    vector<KomgaPage> pages;
    
    int  readerPage      = 0;
    
    bool readerLoading   = false;
    
    bool readerShowHUD   = true;

    // ---- Reader Camera State ----
    float readerZoom      = 1.0f;

    float readerOffsetX   = 0.0f;
    
    float readerOffsetY   = 0.0f;

    bool  readerFitWidth  = false;

    // ---- UI rotation (applies to all screens) ----
    int  uiRotation      = 0;   // 0 = normal, 90 = landscape CW

    // ---- Keyboard (software) ----
    bool  kbVisible = false;

    string kbBuffer;
    
    string* kbTarget = nullptr; // pointer into inputUser or inputPass etc.

    // ---- General ----
    string statusMsg;
    
    bool  running = true;

};

// ---- UI Helpers ----
void setColor(SDL_Renderer* r, const Color& c);

void fillRect(SDL_Renderer* r, int x, int y, int w, int h, const Color& c);

void drawRect(SDL_Renderer* r, int x, int y, int w, int h, const Color& c, int thick = 2);

void drawText(SDL_Renderer* r, TTF_Font* f, const string& text, int x, int y, const Color& c, bool centerX = false, bool centerY = false);

SDL_Texture* renderText(SDL_Renderer* r, TTF_Font* f, const string& text, const Color& c);

void drawTextureScaled(SDL_Renderer* r, SDL_Texture* t, int x, int y, int w, int h);

void drawRoundRect(SDL_Renderer* r, int x, int y, int w, int h, int radius, const Color& c);

void drawPlaceholder(SDL_Renderer* r, int x, int y, int w, int h, const string& label, TTF_Font* font);

SDL_Rect getPortraitViewport();