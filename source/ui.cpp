#include "ui.h"

#include <algorithm>

using namespace std;

void setColor(SDL_Renderer* r, const Color& c) 
{

    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);

}

void fillRect(SDL_Renderer* r, int x, int y, int w, int h, const Color& c) 
{

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);

    SDL_Rect rect{x, y, w, h};

    SDL_RenderFillRect(r, &rect);

}

void drawRect(SDL_Renderer* r, int x, int y, int w, int h, const Color& c, int thick) 
{

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);

    for (int i = 0; i < thick; ++i) 
    {

        SDL_Rect rect{x+i, y+i, w-2*i, h-2*i};

        SDL_RenderDrawRect(r, &rect);

    }

}

void drawText(SDL_Renderer* r, TTF_Font* f, const string& text, int x, int y, const Color& c, bool centerX, bool centerY) 
{
    
    if (!f || text.empty()) 
    {
    
        return;
    
    }

    SDL_Color col{c.r, c.g, c.b, c.a};
    
    SDL_Surface* surf = TTF_RenderUTF8_Blended(f, text.c_str(), col);
    
    if (!surf)
    {
        
        return;
    
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);
    
    if (tex) 
    {
    
        int tw = surf->w, th = surf->h;
    
        SDL_Rect dst{centerX ? x - tw/2 : x, centerY ? y - th/2 : y, tw, th};
        
        SDL_RenderCopy(r, tex, nullptr, &dst);
        
        SDL_DestroyTexture(tex);

    }

    SDL_FreeSurface(surf);

}

void drawTextureScaled(SDL_Renderer* r, SDL_Texture* t, int x, int y, int w, int h) {
    
    if (!t)
    {
        
        return;
    
    }

    SDL_Rect dst{x, y, w, h};
    
    SDL_RenderCopy(r, t, nullptr, &dst);

}

void drawPlaceholder(SDL_Renderer* r, int x, int y, int w, int h, const string& label, TTF_Font* font) 
{
    
    fillRect(r, x, y, w, h, C_PANEL);
    
    drawRect(r, x, y, w, h, C_ACCENT, 1);
    
    if (font && !label.empty()) 
    {
    
        // Truncate label to ~20 chars
        string display = label.size() > 20 ? label.substr(0, 18) + ".." : label;
    
        drawText(r, font, display, x + w/2, y + h/2, C_SUBTEXT, true, true);
    
    }

}