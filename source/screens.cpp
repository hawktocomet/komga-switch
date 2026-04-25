#include "screens.h"

#include "ui.h"

#include "config.h"

#include "config_store.h"

#include <algorithm>

#include <sstream>

using namespace std;

// ----------------------
//  LOGIN SCREEN
// ----------------------
void drawLoginScreen(SDL_Renderer* r, TTF_Font* fontLg, TTF_Font* fontMd, TTF_Font* fontSm, AppState& state) 
{

    fillRect(r, 0, 0, SCREEN_W, SCREEN_H, C_BG);

    // -- 5 Field Login Panel ---
    int panelW = 500, panelH = 520;

    int panelX = (SCREEN_W - panelW) / 2;
    
    int panelY = (SCREEN_H - panelH) / 2 - 10;

    fillRect(r, panelX, panelY, panelW, panelH, C_PANEL);
    
    drawRect(r, panelX, panelY, panelW, panelH, C_ACCENT, 2);

    drawText(r, fontLg, "Komga Reader", SCREEN_W/2, panelY + 22, C_ACCENT, true);

    // ---- Field helper ----
    struct FieldDef 
    {
     
        const char* label;
     
        string      value;
     
        bool        password;
     
        int         fieldIdx;
    
    };

    int fieldW = 400, fieldH = 42;
    
    int fieldX = (SCREEN_W - fieldW) / 2;
    
    int startY = panelY + 76;
    
    int gap    = fieldH + 32;

    FieldDef fields[5] = 
    {

        { "Server address", state.inputServer, false, 0 },

        { "Port",           state.inputPort,   false, 1 },

        { "Path",           state.inputPath,   false, 2 },

        { "Username",       state.inputUser,   false, 3 },

        { "Password",       state.inputPass,   true,  4 },

    };

    for (int i = 0; i < 5; ++i) 
    {
    
        int y      = startY + i * gap;
    
        bool focus = (state.loginField == i);

        fillRect(r, fieldX, y, fieldW, fieldH, {35, 35, 48, 255});
    
        drawRect(r, fieldX, y, fieldW, fieldH, focus ? C_ACCENT : C_SUBTEXT, focus ? 2 : 1);

        drawText(r, fontSm, fields[i].label, fieldX, y - 20, C_SUBTEXT);

        string disp;
        
        Color col;
        
        if (fields[i].value.empty()) 
        {
            if (i ==2 ) 
            {
                
                disp = focus ? "" : "Optional e.g. /komga";
            
            }
            else
            {

                disp = focus ? "" : string("Enter ") + fields[i].label;
            
            }

            col  = C_SUBTEXT;
        
        } 
        
        else if (fields[i].password && !state.showPassword) 
        {
        
            disp = string(fields[i].value.size(), '*');
        
            col  = C_TEXT;
        
        } 
        
        else 
        {
        
            disp = fields[i].value;
        
            col  = C_TEXT;
        
        }
        
        drawText(r, fontMd, disp, fieldX + 10, y + 8, col);

        if (focus) 
        {
            
            // Caret
            int cx = fieldX + 10 + (int)disp.size() * 11;
            
            fillRect(r, cx, y + 6, 2, 28, C_ACCENT);
     
        }

    }

    // ---- Save credentials checkbox ----
    int cbY = startY + 5 * gap - 8;
    
    fillRect(r, fieldX, cbY, 20, 20, {35, 35, 48, 255});
    
    drawRect(r, fieldX, cbY, 20, 20, C_SUBTEXT, 1);
    
    if (state.saveCredentials) 
    {
        
        fillRect(r, fieldX + 4, cbY + 4, 12, 12, C_ACCENT);
    
    }

    drawText(r, fontSm, "Save credentials", fieldX + 50, cbY + 1, C_SUBTEXT);

    // ---- Login button ----
    int btnW = 220, btnH = 44;
    
    int btnX = (SCREEN_W - btnW) / 2;
    
    int btnY = cbY + 32;
    
    fillRect(r, btnX, btnY, btnW, btnH, C_ACCENT);
    
    drawText(r, fontMd, state.loginLoading ? "Connecting..." : "Login (A)", btnX + btnW/2, btnY + 12, C_BG, true);

    // ---- Error ----
    if (!state.loginError.empty()) 
    {

        drawText(r, fontSm, state.loginError, SCREEN_W/2, btnY + btnH + 50, {220, 80, 80, 255}, true);
    
    }

    // ---- Controls hint ----
    drawText(r, fontSm, "[Up]/[Down] Field   [Y] Keyboard   [A] Login   [X] Toggle save creds", SCREEN_W/2, 30, C_SUBTEXT, true);

}


//  ---- LIBRARY SCREEN ----
void drawLibraryScreen(SDL_Renderer* r, TTF_Font* fontLg, TTF_Font* fontMd, TTF_Font* fontSm, AppState& state) 
{

    fillRect(r, 0, 0, SCREEN_W, SCREEN_H, C_BG);
    
    drawText(r, fontLg, "Libraries", 50, 24, C_ACCENT);

    int itemH = 70, itemW = SCREEN_W - 100, startY = 90;

    // ---- List Libraries ----
    for (int i = 0; i < (int)state.libraries.size(); ++i) 
    {
    
        int y = startY + i * (itemH + 12) - state.libScroll;
    
        if (y + itemH < 0 || y > SCREEN_H) continue;

        bool sel = (i == state.libSelected);
    
        Color bgCol = sel ? C_HIGHLIGHT : C_PANEL;
    
        fillRect(r, 50, y, itemW, itemH, bgCol);
    
        if (sel) 
        {
            
            drawRect(r, 50, y, itemW, itemH, C_SEL, 2);

        }

        drawText(r, fontMd, state.libraries[i].name, 80, y + 14, C_TEXT);
    
        drawText(r, fontSm, "ID: " + state.libraries[i].id, 80, y + 42, C_SUBTEXT);

        // Arrow
        drawText(r, fontMd, ">", 50 + itemW - 30, y + 20, C_ACCENT);
    
    }

    // ---- Control Hint ----
    drawText(r, fontSm, "[Up]/[Down] Navigate   [A] Open   [B] Logout", SCREEN_W/2, 30, C_SUBTEXT, true);

}


//  ---- SERIES SCREEN (grid) ----
void drawSeriesScreen(SDL_Renderer* r, TTF_Font* fontLg, TTF_Font* fontMd, TTF_Font* fontSm, AppState& state, TextureCache& cache) 
{
    
    fillRect(r, 0, 0, SCREEN_W, SCREEN_H, C_BG);

    // ---- Library Name Header -----
    string header = state.currentLibraryName;
    
    if (!state.seriesSearch.empty())
    {

        header += "  [" + state.seriesSearch + "]";
    }

    drawText(r, fontLg, header, 50, 18, C_ACCENT);
    
    // ---- Pages Text ----
    string paging = "Page " + to_string(state.seriesPageIndex + 1) + " / " + to_string(max(1, state.seriesPage.totalPages));
    
    drawText(r, fontSm, paging, SCREEN_W - 90, 26, C_SUBTEXT, false, true);

    if (state.seriesLoading) 
    {
        
        drawText(r, fontMd, "Loading...", SCREEN_W/2, SCREEN_H/2, C_SUBTEXT, true, true);
        
        return;
    
    }

    // ---- Series Grid ----
    auto& series = state.seriesPage.content;

    for (int i = 0; i < (int)series.size(); ++i) 
    {
    
        int col = i % GRID_COLS;
    
        int row = i / GRID_COLS;
    
        int x   = GRID_MARGIN_X + col * (THUMB_W + GRID_PAD_X);
    
        int y   = GRID_MARGIN_Y + row * (THUMB_H + GRID_PAD_Y + 36);

        bool sel = (i == state.seriesSelected);

        // ---- Thumbnail ----
        SDL_Texture* thumb = cache.get(series[i].thumbnailUrl);
    
        if (thumb) 
        {
    
            drawTextureScaled(r, thumb, x, y, THUMB_W, THUMB_H);
    
        } 
        
        else 
        {
    
            drawPlaceholder(r, x, y, THUMB_W, THUMB_H, series[i].name, fontSm);
    
        }

        // ---- Selection border ----
        if (sel) 
        {
        
            drawRect(r, x - 3, y - 3, THUMB_W + 6, THUMB_H + 6, C_SEL, 3);
        
        }

        // ---- Series Grid Unread badge ----
        if (series[i].booksUnread > 0) 
        {

            string badge = to_string(series[i].booksUnread);
            
            fillRect(r, x + THUMB_W - 28, y + 4, 24, 24, C_ACCENT2);
            
            drawText(r, fontSm, badge, x + THUMB_W - 16, y + 4, C_BG, true);
        
        }

        // ---- Title below thumb (truncated) ----
        string title = series[i].name;
        
        if (title.size() > 18) 
        {
         
            title = title.substr(0, 16) + "..";
        }

        drawText(r, fontSm, title, x + THUMB_W/2, y + THUMB_H + 4, C_TEXT, true);
    
    }

    // --- Control Hints ----
    drawText(r, fontSm, "[D-Pad] Navigate   [A] Open   [B] Back   [L]/[R] Prev/Next page   [Y] Search", SCREEN_W/2, 30, C_SUBTEXT, true);

}


//  ---- BOOKS SCREEN (grid) ----
void drawBooksScreen(SDL_Renderer* r, TTF_Font* fontLg, TTF_Font* fontMd, TTF_Font* fontSm, AppState& state, TextureCache& cache) 
{
    
    // ---- Book Name Header ----
    fillRect(r, 0, 0, SCREEN_W, SCREEN_H, C_BG);

    drawText(r, fontLg, state.currentSeriesName, 50, 18, C_ACCENT);
    
    // ---- Pages Text -----
    string paging = "Page " + to_string(state.bookPageIndex + 1) + " / " + to_string(max(1, state.booksPage.totalPages));
    
    drawText(r, fontSm, paging, SCREEN_W - 90, 26, C_SUBTEXT, false, true);

    if (state.booksLoading) 
    {

        drawText(r, fontMd, "Loading...", SCREEN_W/2, SCREEN_H/2, C_SUBTEXT, true, true);
        
        return;
    
    }

    // ---- Book Grid ----
    auto& books = state.booksPage.content;

    for (int i = 0; i < (int)books.size(); ++i) 
    {
        
        int col = i % GRID_COLS;
        
        int row = i / GRID_COLS;
        
        int x   = GRID_MARGIN_X + col * (THUMB_W + GRID_PAD_X);
        
        int y   = GRID_MARGIN_Y + row * (THUMB_H + GRID_PAD_Y + 36);

        bool sel = (i == state.bookSelected);

        SDL_Texture* thumb = cache.get(books[i].thumbnailUrl);

        if (thumb) 
        {
        
            drawTextureScaled(r, thumb, x, y, THUMB_W, THUMB_H);
        
        } 
        
        else 
        {
        
            drawPlaceholder(r, x, y, THUMB_W, THUMB_H, books[i].name, fontSm);
        
        }

        if (sel) 
        {
            
            drawRect(r, x - 3, y - 3, THUMB_W + 6, THUMB_H + 6, C_SEL, 3);
        
        }

        // ---- Book Grid Item Read Progress Bar ----
        if (books[i].pagesCount > 0 && books[i].readProgress > 0) 
        {

            float progress = (float)books[i].readProgress / books[i].pagesCount;
            
            int barW = (int)(THUMB_W * progress);
            
            fillRect(r, x, y + THUMB_H - 5, THUMB_W, 5, C_PANEL);
            
            fillRect(r, x, y + THUMB_H - 5, barW,    5, C_ACCENT);
        
        }

        // ---- Book Grid Item Number Label ----
        string numLabel = "#" + to_string(books[i].number);

        drawText(r, fontSm, numLabel, x + THUMB_W/2, y + THUMB_H + 4, C_ACCENT, true);

    }

}


//  ---- READER SCREEN ----
void drawReaderScreen(SDL_Renderer* r, TTF_Font* fontMd, TTF_Font* fontSm, AppState& state, TextureCache& cache) 
{

    // ---- Background ----
    SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
    
    SDL_RenderClear(r);

    if (state.readerLoading) 
    {
    
        drawText(r, fontMd, "Loading...", SCREEN_W/2, SCREEN_H/2, C_SUBTEXT, true, true);
        
        return;
    
    }

    if (state.pages.empty()) 
    {
    
        drawText(r, fontMd, "No pages found.", SCREEN_W/2, SCREEN_H/2, C_SUBTEXT, true, true);
        
        return;
    
    }

    string pageUrl = KomgaApi::bookPageUrl( state.currentBookId, state.readerPage + 1 );

    SDL_Texture* tex = cache.get( pageUrl );

    if (!tex) 
    {
    
        drawText(r, fontMd, "Loading...", SCREEN_W/2, SCREEN_H/2, C_SUBTEXT, true, true);
        
        return;
    
    }

    // ---- Draw Book Page ---
    int texW, texH;

    SDL_QueryTexture(tex, nullptr, nullptr, &texW, &texH);

    // ---- In portrait mode, swap width/height for scaling ----
    bool portrait = (state.readerRotation == 90);

    float baseScale = portrait ? (float)SCREEN_H / texW : (float)SCREEN_W / texW;   // rotated: texture width maps to screen height

    float scale = baseScale * state.readerZoom;
   
    int drawW = (int)(texW * scale);
   
    int drawH = (int)(texH * scale);

    // ---- Clamp Pan ----
    if (portrait)
    {
        // In portrait, drawH spans the horizontal axis and drawW spans the vertical
        float maxOffsetX = (float)max(0, (drawH - SCREEN_W) / 2);
     
        float maxOffsetY = (float)max(0, (drawW - SCREEN_H) / 2);

        state.readerOffsetX = clamp(state.readerOffsetX, -maxOffsetX, maxOffsetX);
     
        state.readerOffsetY = clamp(state.readerOffsetY, -maxOffsetY, maxOffsetY);
    }

    else
    {
        float maxOffsetX = (float)max(0, (drawW - SCREEN_W) / 2);
    
        float minOffsetY = (float)(SCREEN_H - drawH);

        state.readerOffsetX = clamp(state.readerOffsetX, -maxOffsetX, maxOffsetX);
    
        state.readerOffsetY = clamp(state.readerOffsetY, minOffsetY, 0.0f);
    
    }

    // ---- Position ----
    int dstX, dstY;

    if (portrait)
    {
        dstX = (SCREEN_W - drawW) / 2 + state.readerOffsetX;
        dstY = (SCREEN_H - drawH) / 2 + state.readerOffsetY;
    }
    else
    {
        dstX = (SCREEN_W - drawW) / 2 + state.readerOffsetX;
        dstY = state.readerOffsetY;
    }

    SDL_Rect dst{ dstX, dstY, drawW, drawH };
    SDL_RenderCopyEx(r, tex, nullptr, &dst, state.readerRotation, nullptr, SDL_FLIP_NONE);

    // ---- Reader HUD ----
    if (state.readerShowHUD)
    {
        string title = state.currentBookName;
        
        if (title.size() > 40) 
        {
         
            title = title.substr(0, 38) + "..";
        
        }
                
        if (portrait)
        {
            // Offscreen texture is SCREEN_H wide x SCREEN_W tall (720x1280)
            // We draw on it as if it's a normal landscape screen
            // Then rotate 90 degrees onto the real screen
            SDL_Texture* hudTex = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_H, SCREEN_W);

            SDL_SetTextureBlendMode(hudTex, SDL_BLENDMODE_BLEND);

            SDL_SetRenderTarget(r, hudTex);
            
            SDL_SetRenderDrawColor(r, 0, 0, 0, 0);
            
            SDL_RenderClear(r);

            // Top bar on offscreen texture
            fillRect(r, 0, 0, SCREEN_H, 44, {0,0,0,180});

            // Bottom bar on offscreen texture
            fillRect(r, 0, SCREEN_W - 36, SCREEN_H, 36, {0,0,0,180});

            // Title centered in top bar
            drawText(r, fontMd, title, SCREEN_H/2, 8, C_TEXT, true, false);

            // Page counter top right
            string pg = to_string(state.readerPage + 1) + " / " + to_string((int)state.pages.size());
            
            drawText(r, fontSm, pg, SCREEN_H - 90, 12, C_SUBTEXT, false, false);

            // Controls hint in bottom bar
            drawText(r, fontSm, "[LB] HUD  [U/D] Page  [L/R] Zoom  [Stick] Pan  [B] Back  [Minus] Rotate", SCREEN_H/2, SCREEN_W - 26, C_SUBTEXT, true, false);

            // Progress bar
            float prog = (float)(state.readerPage + 1) / state.pages.size();

            fillRect(r, 0, SCREEN_W - 4, SCREEN_H, 4, C_PANEL);
            
            fillRect(r, 0, SCREEN_W - 4, (int)(SCREEN_H * prog), 4, C_ACCENT);

            // Switch render target back to screen
            SDL_SetRenderTarget(r, nullptr);

            // Render the offscreen texture rotated 90 degrees
            // Center point for rotation is center of screen
            SDL_Point center
            { 
            
                SCREEN_W/2, SCREEN_H/2 
            
            };
            
            SDL_Rect hudDst
            { 
                
                SCREEN_W/2 - SCREEN_H/2,
                
                SCREEN_H/2 - SCREEN_W/2 + 560,   // empirical offset to center rotated HUD on screen
                
                SCREEN_H,                   // 720
                
                SCREEN_W                    // 1280
            
            };

            SDL_RenderCopyEx(r, hudTex, nullptr, &hudDst, 90, &center, SDL_FLIP_NONE);
            
            SDL_DestroyTexture(hudTex);
        
        }
        
        else
        {
            // ---- Normal landscape HUD ----
            fillRect(r, 0, 0, SCREEN_W, 44, {0,0,0,180});

            fillRect(r, 0, SCREEN_H - 36, SCREEN_W, 36, {0,0,0,180});
            
            drawText(r, fontSm, "[X] HUD : [LB/RB] Page : [ZL/ZR] Zoom : [L-Stick] Pan : [B] Back : [Minus] Rotate", SCREEN_W/2, SCREEN_H - 26, C_SUBTEXT, true);
            
            drawText(r, fontMd, title, SCREEN_W/2, 8, C_TEXT, true);

            string pg = to_string(state.readerPage + 1) + " / " + to_string((int)state.pages.size());
            
            drawText(r, fontSm, pg, SCREEN_W - 90, 12, C_SUBTEXT, false);
            
            float prog = (float)(state.readerPage + 1) / state.pages.size();
            
            fillRect(r, 0, SCREEN_H - 4, SCREEN_W, 4, C_PANEL);
            
            fillRect(r, 0, SCREEN_H - 4, (int)(SCREEN_W * prog), 4, C_ACCENT);
        }

    }

}