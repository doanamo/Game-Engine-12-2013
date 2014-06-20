#pragma once

#include "Precompiled.hpp"

//
// Cache Manager
//  Manages a registry of cached files, so resources can be loaded much faster directly
//  from the cache after they have been processed once and saved to the cache.
//

class CacheManager
{
public:
    // Cache registry list.
    typedef std::unordered_map<std::string, boost::uuids::uuid> CacheList;

public:
    CacheManager();
    ~CacheManager();

    bool Initialize();
    void Cleanup();

    // Lookups a cache entry. It creates a new entry if it doesn't exist.
    std::string Lookup(std::string filename);

private:
    bool LoadCache();
    bool RecreateCache();
    bool SaveCache();

    bool ValidateCache(std::string identifier);

private:
    CacheList m_registry;

    bool m_initialized;
};
