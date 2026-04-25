//  Komga Switch Reader :  main.cpp
//  Build with devkitPro:  make
//  Required packages   :  switch-dev switch-sdl2 switch-sdl2_image switch-sdl2_ttf
//                         switch-curl switch-mbedtls switch-libjpeg-turbo switch-libpng

#include "config.h"

#include "ui.h"

#include "screens.h"

#include "komga_api.h"

#include "http_client.h"

#include "texture_cache.h"

#include "config_store.h"

#include <switch.h>

#include <SDL2/SDL.h>

#include <SDL2/SDL_ttf.h>

#include <SDL2/SDL_image.h>

#include <string>

#include <algorithm>

using namespace std;

// ---- Font path (bundled in romfs) ----
#define FONT_PATH "romfs:/fonts/Roboto-Regular.ttf"

// ---- Pad state ----
static PadState padState;

// ---- Forward declarations ----
static void handleInputLogin   (u64 btn, AppState& state);

static void handleInputLibrary (u64 btn, AppState& state);

static void handleInputSeries  (u64 btn, AppState& state);

static void handleInputBooks   (u64 btn, AppState& state);

static void handleInputReader  (u64 btn, AppState& state);

static void loadLibraries      (AppState& state);

static void loadSeriesPage     (AppState& state, const string& libId, int page);

static void loadBooksPage      (AppState& state, const string& seriesId, int page);

static void loadBookPages      (AppState& state, const string& bookId);

// ---- Main ---- 
int main(int /*argc*/, char** /*argv*/) 
{

    // ---- Init horizon services ----
    romfsInit();

    socketInitializeDefault();

    // ---- Init pad input ----
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    
    padInitializeDefault(&padState);

    // ---- Init HTTP Client
    HttpClient::init();

    // ---- Load persisted config and push to API client ----
    AppConfig cfg = ConfigStore::load();
    
    KomgaApi::setApiBase(ConfigStore::apiBase(cfg));

    // ----SDL init ----
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
    
    TTF_Init();
    
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    SDL_Window*   window   = SDL_CreateWindow("Komga", 0, 0, SCREEN_W, SCREEN_H, SDL_WINDOW_FULLSCREEN);
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // ---- Fonts ----
    TTF_Font* fontLg = TTF_OpenFont(FONT_PATH, FONT_SIZE_LARGE);
    
    TTF_Font* fontMd = TTF_OpenFont(FONT_PATH, FONT_SIZE_MEDIUM);
    
    TTF_Font* fontSm = TTF_OpenFont(FONT_PATH, FONT_SIZE_SMALL);

    // ---- Joystick (Joy-Con) ----
    for (int i = 0; i < SDL_NumJoysticks(); ++i)  
    {
    
        SDL_JoystickOpen(i);

    }

    TextureCache cache(renderer, 60);
    
    AppState     state;

    // ---- Pre-fill login fields from saved config ----
    state.inputServer      = cfg.serverHost;
    
    state.inputPort        = cfg.serverPort;
    
    state.saveCredentials  = cfg.saveCredentials;
    
    if (cfg.saveCredentials) 
    {
    
        state.inputUser = cfg.username;
    
        state.inputPass = cfg.password;
    
    }

    // ---- Main Loop ----
    while (appletMainLoop() && state.running) 
    {
        
        // ---- Read gamepad ----
        padUpdate(&padState);
        
        u64 buttonsDown = padGetButtonsDown(&padState);

        // ---- Screen-specific input ----
        switch (state.screen) 
        {
            
            case Screen::LOGIN:    handleInputLogin   (buttonsDown, state); break;
            
            case Screen::LIBRARIES: handleInputLibrary(buttonsDown, state); break;
            
            case Screen::SERIES:   handleInputSeries  (buttonsDown, state); break;
            
            case Screen::BOOKS:    handleInputBooks   (buttonsDown, state); break;
            
            case Screen::READER:   handleInputReader  (buttonsDown, state); break;
        
        }

        // ---- Choose render destination ----
        SDL_SetRenderTarget(renderer, nullptr);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        
        SDL_SetRenderDrawColor(renderer, 18, 18, 24, 255);
        
        SDL_RenderClear(renderer);

        switch (state.screen) 
        {
        
            case Screen::LOGIN:
        
                drawLoginScreen(renderer, fontLg, fontMd, fontSm, state);
            
                break;
            
            case Screen::LIBRARIES:
            
                drawLibraryScreen(renderer, fontLg, fontMd, fontSm, state);
                
                break;
            
            case Screen::SERIES:
        
                drawSeriesScreen(renderer, fontLg, fontMd, fontSm, state, cache);
            
                break;
            
            case Screen::BOOKS:
        
                drawBooksScreen(renderer, fontLg, fontMd, fontSm, state, cache);
            
                break;
            
            case Screen::READER:
        
                drawReaderScreen(renderer, fontMd, fontSm, state, cache);
            
                break;
        
        }

        SDL_RenderPresent(renderer);
    
    }

    // ---- Cleanup ----
    cache.clear();

    if (fontLg) TTF_CloseFont(fontLg);
    
    if (fontMd) TTF_CloseFont(fontMd);
    
    if (fontSm) TTF_CloseFont(fontSm);
    
    SDL_DestroyRenderer(renderer);
    
    SDL_DestroyWindow(window);
    
    IMG_Quit();
    
    TTF_Quit();
    
    SDL_Quit();
    
    HttpClient::cleanup();
    
    socketExit();
    
    romfsExit();
    
    return 0;

}

//  ---- DATA LOADERS ----

// ---- Load Library ----
static void loadLibraries(AppState& state) 
{
    
    state.libraries = KomgaApi::getLibraries();
    
    state.libSelected = 0;

    state.libScroll = 0;
}

// ---- Load Series ----
static void loadSeriesPage(AppState& state, const string& libId, int page) 
{
    
    state.seriesLoading   = true;
    
    state.seriesPageIndex = page;
    
    state.seriesPage = KomgaApi::getSeries(libId, page, PAGE_SIZE, state.seriesSearch);
    
    state.seriesSelected  = 0;
    
    state.seriesLoading   = false;

}

// ---- Load Books ----
static void loadBooksPage(AppState& state, const string& seriesId, int page) 
{
    
    state.booksLoading   = true;
    
    state.bookPageIndex  = page;
    
    state.booksPage = KomgaApi::getBooks(seriesId, page, PAGE_SIZE);
    
    state.bookSelected   = 0;
    
    state.booksLoading   = false;

}

// --- Load Individual Book Pages ----
static void loadBookPages(AppState& state, const string& bookId) 
{
    
    state.readerLoading = true;
    
    state.pages = KomgaApi::getBookPages(bookId);
    
    state.readerLoading = false;

}


// ---- INPUT HANDLERS ----

// ---- Login Input Handler  ----
static void handleInputLogin(u64 btn, AppState& state) 
{

    // ---- Cycle through 5 fields: 0=server, 1=port, 2=path 3=user, 4=pass ----
    // ---- (ZL/ZR / Up/Down all cycle the same way) ----
    constexpr int NUM_FIELDS = 5;

    if (btn & HidNpadButton_ZL || btn & HidNpadButton_Up) 
    {

        state.loginField = (state.loginField - 1 + NUM_FIELDS) % NUM_FIELDS;
    }
        
    if (btn & HidNpadButton_ZR || btn & HidNpadButton_Down) 
    {

        state.loginField = (state.loginField + 1) % NUM_FIELDS;
    }

    // ---- A = Login ----
    if (btn & HidNpadButton_A) 
    {

        if (!state.inputUser.empty() && !state.inputPass.empty() && !state.inputServer.empty() && !state.inputPort.empty()) 
        {
            
            state.loginLoading = true;
            
            state.loginError   = "";

            // ---- Update API base from current fields ----
            KomgaApi::setApiBase("http://" + state.inputServer + ":" + state.inputPort + state.inputPath + "/api/v1");

            if (KomgaApi::login(state.inputUser, state.inputPass)) 
            {
                
                // ---- Persist config ----
                AppConfig cfg;
                
                cfg.serverHost      = state.inputServer;
                
                cfg.serverPort      = state.inputPort;
                
                cfg.saveCredentials = state.saveCredentials;

                cfg.username        = state.saveCredentials ? state.inputUser  : "";
                
                cfg.password        = state.saveCredentials ? state.inputPass  : "";
                
                ConfigStore::save(cfg);

                loadLibraries(state);
                
                state.screen = Screen::LIBRARIES;
            } 
            
            else 
            {
               
                state.loginError = "Login failed – check credentials or server.";
            
            }
            
            state.loginLoading = false;
        
        } 
        
        else 
        {
            
            state.loginError = "All fields are required.";
        
        }
   
    }

    // X = toggle save-credentials checkbox
    if (btn & HidNpadButton_X) 
    {

        state.saveCredentials = !state.saveCredentials;
    
    }

    // Plus = exit
    if (btn & HidNpadButton_Plus) 
    {
        
        state.running = false;
    
    }

    // Y = open system keyboard for focused field
    if (btn & HidNpadButton_Y) 
    {
        
        SwkbdConfig kbd;
        
        swkbdCreate(&kbd, 0);
        
        swkbdConfigMakePresetDefault(&kbd);

        const char* headers[] = { "Server address", "Port", "Username", "Password" };
        
        swkbdConfigSetHeaderText(&kbd, headers[state.loginField]);
        
        if (state.loginField == 4) 
        {
        
            swkbdConfigSetPasswordFlag(&kbd, true);

        }

        char buf[256] = {};
        
        if (R_SUCCEEDED(swkbdShow(&kbd, buf, sizeof(buf)))) 
        {
            
            string val(buf);
            
            switch (state.loginField) 
            {
                
                case 0: state.inputServer = val; break;
                
                case 1: state.inputPort   = val; break;
                
                case 2: state.inputPath   = val; break;

                case 3: state.inputUser   = val; break;
                
                case 4: state.inputPass   = val; break;
            
            }
        
        }
        
        swkbdClose(&kbd);
    
    }

}

// ---- Libraries Input Handler ----
static void handleInputLibrary(u64 btn, AppState& state) 
{
    
    int n = (int)state.libraries.size();
    
    if (btn & HidNpadButton_Up)
    {
    
        state.libSelected = max(0, state.libSelected - 1);

        // Scroll up
        int itemTop = 90 + state.libSelected * (70 + 12);
        
        if (itemTop < state.libScroll + 90)
        {
            state.libScroll = max(0, itemTop - 90);
        }
    
    }

    if (btn & HidNpadButton_Down) 
    {
        
        state.libSelected = min(n - 1, state.libSelected + 1);
    
        // Scroll down
        int itemBottom = 90 + state.libSelected * (70 + 12) + 70;
        
        if (itemBottom > SCREEN_H - 40)
        {
            
            state.libScroll = itemBottom - (SCREEN_H - 40);

        }
    
    }

    if (btn & HidNpadButton_A && n > 0) 
    {
        
        auto& lib = state.libraries[state.libSelected];
        
        state.currentLibraryId   = lib.id;
        
        state.currentLibraryName = lib.name;
        
        state.seriesSearch       = "";
        
        loadSeriesPage(state, lib.id, 0);
        
        state.screen = Screen::SERIES;
    
    }

    if (btn & HidNpadButton_B) 
    {
        
        KomgaApi::logout();
        
        state.screen     = Screen::LOGIN;
        
        // state.inputUser  = ""; //You could clear the login here.
        
        // state.inputPass  = ""; //You could clear the login here.
        
        state.loginError = "";
    }

    if (btn & HidNpadButton_Plus) 
    {
        
        state.running = false;

    }

}

// ---- Series Input Handler ----
static void handleInputSeries(u64 btn, AppState& state) 
{
    
    int n   = (int)state.seriesPage.content.size();
    
    int sel = state.seriesSelected;

    if (btn & HidNpadButton_Right) 
    {
        
        sel = min(n - 1, sel + 1);
    
    }
    
    if (btn & HidNpadButton_Left)
    {
    
        sel = max(0,     sel - 1);
    
    }

    if (btn & HidNpadButton_Down) 
    {
        
        sel = min(n - 1, sel + GRID_COLS);
    
    }

    if (btn & HidNpadButton_Up) 
    {

        sel = max(0,     sel - GRID_COLS);
    
    }    
    
    state.seriesSelected = sel;

    // ---- Paginate Series across pages ----
    if (btn & HidNpadButton_R && state.seriesPageIndex + 1 < state.seriesPage.totalPages) 
    {

        loadSeriesPage(state, state.currentLibraryId, state.seriesPageIndex + 1);
    
    }
        
    if (btn & HidNpadButton_L && state.seriesPageIndex > 0) 
    {

        loadSeriesPage(state, state.currentLibraryId, state.seriesPageIndex - 1);

    }

    // ---- Search for Series with system keyboard ----
    if (btn & HidNpadButton_Y) 
    {

        SwkbdConfig kbd;
        
        swkbdCreate(&kbd, 0);
        
        swkbdConfigMakePresetDefault(&kbd);
        
        swkbdConfigSetHeaderText(&kbd, "Search series");
        
        char buf[256] = {};
        
        if (R_SUCCEEDED(swkbdShow(&kbd, buf, sizeof(buf)))) 
        {
        
            state.seriesSearch = string(buf);
        
            loadSeriesPage(state, state.currentLibraryId, 0);
        
        }
        
        swkbdClose(&kbd);
    
    }

    // ---- Open series ----
    if (btn & HidNpadButton_A && n > 0) 
    {
        
        auto& s = state.seriesPage.content[state.seriesSelected];
        
        state.currentSeriesId   = s.id;
        
        state.currentSeriesName = s.name;
        
        loadBooksPage(state, s.id, 0);
        
        state.screen = Screen::BOOKS;
    
    }

    if (btn & HidNpadButton_B) 
    {
    
        state.screen = Screen::LIBRARIES;
    
    }
        
    if (btn & HidNpadButton_Plus) 
    {
        
        state.running = false;
    
    }
}

// ---- Books Input Handler ----
static void handleInputBooks(u64 btn, AppState& state) 
{
    
    int n   = (int)state.booksPage.content.size();
    
    int sel = state.bookSelected;

    if (btn & HidNpadButton_Right) 
    {
    
        sel = min(n - 1, sel + 1);
    
    }
    
    if (btn & HidNpadButton_Left)  
    {
    
        sel = max(0,     sel - 1);
    
    }
    
    if (btn & HidNpadButton_Down) 
    {
    
        sel = min(n - 1, sel + GRID_COLS);
    
    }

    if (btn & HidNpadButton_Up) 
    {

        sel = max(0,     sel - GRID_COLS);
    }

    state.bookSelected = sel;

    if (btn & HidNpadButton_R && state.bookPageIndex + 1 < state.booksPage.totalPages) 
    {

        loadBooksPage(state, state.currentSeriesId, state.bookPageIndex + 1);
    
    }
       
    if (btn & HidNpadButton_L && state.bookPageIndex > 0) 
    {

        loadBooksPage(state, state.currentSeriesId, state.bookPageIndex - 1);
    
    }

    if (btn & HidNpadButton_A && n > 0) 
    {
        
        auto& b = state.booksPage.content[state.bookSelected];
        
        state.currentBookId   = b.id;
        
        state.currentBookName = b.name;
        
        // Start from last read page (or 0)
        state.readerPage = max(0, b.readProgress > 0 ? b.readProgress - 1 : 0);
        
        loadBookPages(state, b.id);
        
        state.readerShowHUD = true;
        
        state.screen = Screen::READER;
    
    }

    if (btn & HidNpadButton_B) 
    {
    
        state.screen = Screen::SERIES;
    
    }
    
    if (btn & HidNpadButton_Plus) 
    {
    
        state.running = false;
    
    }

}

// ---- Reader Input Handler ----
static void handleInputReader(u64 btn, AppState& state) 
{
    
    int total = (int)state.pages.size();
    
    if (total == 0) 
    {
    
        return;
    
    }
    
    // ---- Helper sub-functions -----
    auto resetView = [&]() 
    {

        state.readerOffsetY = 0;   // go to top
        
        state.readerOffsetX = 0;   // optional but cleaner
    
    };

    auto pageRight = [&]() 
    {
    
        if (state.readerPage < total - 1) 
        {
    
            state.readerPage++;
    
            resetView();
    
        }
    
    };

    auto pageLeft = [&]() 
    {
    
        if (state.readerPage > 0) 
        {
    
            state.readerPage--;
    
            resetView();
    
        }
    
    };

    // ---- Page Turning ----
    bool didPageTurn = false;

    if (btn & HidNpadButton_L) 
    {

        if (state.readerPage > 0) 
        { 
            
            pageLeft(); 
            
            didPageTurn = true; 
        
        }
    
    }

    if (btn & HidNpadButton_R) 
    {
        
        if (state.readerPage < total - 1) 
        { 
            
            pageRight(); 
            
            didPageTurn = true; 
        
        }
    
    }

    if (btn & HidNpadButton_Left) 
    {
        
        if (state.readerPage > 0) 
        {
            
            pageLeft(); 
            
            didPageTurn = true; 
        }
    
    }

    if (btn & HidNpadButton_Right) 
    {
        
        if (state.readerPage < total - 1) 
        { 
            
            pageRight(); 
            
            didPageTurn = true; 
        }
    
        
    }

    if (didPageTurn) 
    { 
    
        return;
    
    }
    
    // --- Zoom (ZL/ZR) ----
    if (btn & HidNpadButton_ZL) 
    {

        state.readerZoom -= 0.1f;

    }

    if (btn & HidNpadButton_ZR) 
    {

        state.readerZoom += 0.1f;
 
    }

    state.readerZoom = max(0.5f, min(3.0f, state.readerZoom));

    
    // ---- D-PAD PAN (VERTICAL ONLY) ----
    if (btn & HidNpadButton_Up) 
    {
        
        state.readerOffsetY += 20;

    }

    if (btn & HidNpadButton_Down) 
    {
        
        state.readerOffsetY -= 20;

    }

    // ---- ANALOG STICK PAN ----
    HidAnalogStickState stick = padGetStickPos(&padState, 0);

    const float deadzone = 8000.0f;

    if (abs(stick.x) > deadzone) 
    {

        state.readerOffsetX -= (int)(stick.x / 4000);

    }

    if (abs(stick.y) > deadzone) 
    {

        state.readerOffsetY += (int)(stick.y / 4000);
   
    }

    // ---- HUD TOGGLE (X) ----
    if (btn & HidNpadButton_X) 
    {

        state.readerShowHUD = !state.readerShowHUD;

    }

    // ---- Exit Book ----
    if (btn & HidNpadButton_B) 
    {

        KomgaApi::markProgress(state.currentBookId, state.readerPage + 1);
        
        loadBooksPage(state, state.currentSeriesId, state.bookPageIndex);  // ← reload
        
        state.screen = Screen::BOOKS;
    
    }

    // ---- Exit App ----
    if (btn & HidNpadButton_Plus) 
    {

        KomgaApi::markProgress(state.currentBookId, state.readerPage + 1);
        
        state.running = false;
    
    }

}