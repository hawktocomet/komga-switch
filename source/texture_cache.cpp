#include "texture_cache.h"

#include "http_client.h"

#include <SDL2/SDL_image.h>

#include <SDL2/SDL_rwops.h>

using namespace std;

TextureCache::TextureCache(SDL_Renderer* renderer, size_t maxEntries) : m_renderer(renderer), m_max(maxEntries) 
{

}

TextureCache::~TextureCache() 
{ 

    clear(); 

}

void TextureCache::clear() 
{
    
    for (auto& [url, tex] : m_textures)
    {

        if (tex) 
        {

            SDL_DestroyTexture(tex);
        
        }
    
    }
    
    m_textures.clear();
    
    m_order.clear();
    
    m_iters.clear();

}

SDL_Texture* TextureCache::get(const string& url) 
{
    
    if (url.empty()) 
    {

        return nullptr;
    
    }
    
    auto it = m_textures.find(url);
    
    if (it != m_textures.end()) 
    {
    
        // Move to front (most recently used)
        m_order.erase(m_iters[url]);
    
        m_order.push_front(url);
    
        m_iters[url] = m_order.begin();
    
        return it->second;
    
    }

    // Not cached – download
    SDL_Texture* tex = loadFromUrl(url);

    if (!tex)
    {

        return nullptr;

    }

    if (m_textures.size() >= m_max) 
    {
     
        evict();
    
    }
    
    m_order.push_front(url);
    
    m_textures[url] = tex;
    
    m_iters[url] = m_order.begin();
    
    return tex;
}

void TextureCache::evict() 
{
    
    if (m_order.empty()) 
    {
    
        return;
    }
     
    const string& lru = m_order.back();
    
    auto texIt = m_textures.find(lru);
    
    if (texIt != m_textures.end()) 
    {
    
        if (texIt->second) 
        {
            
            SDL_DestroyTexture(texIt->second);
        
        }
        
        m_textures.erase(texIt);
    }
    
    m_iters.erase(lru);
    
    m_order.pop_back();

}

SDL_Texture* TextureCache::loadFromUrl(const string& url) 
{

    vector<uint8_t> bytes;

    if (!HttpClient::download(url, bytes) || bytes.empty()) 
    {

        return nullptr;
    }

    SDL_RWops* rw  = SDL_RWFromMem(bytes.data(), (int)bytes.size());
    
    if (!rw) 
    {
         
        return nullptr;
    }

    SDL_Surface* surf = IMG_Load_RW(rw, 1 /*freesrc*/);
    
    if (!surf) 
    {
        
        return nullptr;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(m_renderer, surf);
    
    SDL_FreeSurface(surf);
    
    return tex;

}