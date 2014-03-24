#pragma once

#include "Precompiled.hpp"

//
// Cache Manager
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
