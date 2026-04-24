#pragma once

#include <SDL2/SDL.h>

#include <SDL2/SDL_ttf.h>

#include "ui.h"

#include "texture_cache.h"

#include "komga_api.h"

void drawLoginScreen  (SDL_Renderer* r, TTF_Font* fontLg, TTF_Font* fontMd, TTF_Font* fontSm, AppState& state);

void drawLibraryScreen(SDL_Renderer* r, TTF_Font* fontLg, TTF_Font* fontMd, TTF_Font* fontSm, AppState& state);

void drawSeriesScreen (SDL_Renderer* r, TTF_Font* fontLg, TTF_Font* fontMd, TTF_Font* fontSm, AppState& state, TextureCache& cache);

void drawBooksScreen  (SDL_Renderer* r, TTF_Font* fontLg, TTF_Font* fontMd, TTF_Font* fontSm, AppState& state, TextureCache& cache);

void drawReaderScreen (SDL_Renderer* r, TTF_Font* fontMd, TTF_Font* fontSm, AppState& state, TextureCache& cache);