#pragma once

#include <string>

#include <unordered_map>

#include <list>

#include <SDL2/SDL.h>

using namespace std;

// ---- LRU Texture Cache ----
// Downloads images from URLs and caches them as SDL textures.
// Evicts least-recently-used entries when capacity is exceeded.
class TextureCache 
{

    public:
        explicit TextureCache(SDL_Renderer* renderer, size_t maxEntries = 50);
        
        ~TextureCache();

        // Returns a cached (or freshly downloaded) texture, or nullptr on failure.
        // The cache owns the texture – do NOT SDL_DestroyTexture it yourself.
        SDL_Texture* get(const string& url);

        void clear();

    private:
        
        SDL_Renderer* m_renderer;
        
        size_t        m_max;

        // LRU: list front = most-recently-used
        using Key  = string;
        
        using Iter = list<Key>::iterator;

        list<Key>                             m_order;
        
        unordered_map<Key, SDL_Texture*>      m_textures;
        
        unordered_map<Key, Iter>              m_iters;

        void evict();
        
        SDL_Texture* loadFromUrl(const string& url);

};