#include "Precompiled.hpp"
#include "Font.hpp"

#include "MainGlobal.hpp"
#include "System/CacheManager.hpp"

namespace
{
    // Log error messages.
    #define LogSaveCacheError(filename) "Failed to save a cache for \"" << filename << "\" font file! "
    #define LogLoadCacheError(filename) "Failed to load a cache for \"" << filename << "\" font file! "

    // Cache file header.
    struct CacheHeader
    {
        boost::uuids::uuid identifier;
        int version;
        boost::crc_32_type::value_type checksum;
        int glyphs;
    };

    const char* CacheIdentifier = "cd1f3240-fbab-11e3-a3ac-0800200c9a66";
    const unsigned int CacheVersion = 5;
}

bool Font::SaveCache()
{
    if(!m_initialized)
        return false;

    // Don't update the cache if no new glyphs has been added.
    if(!m_cacheUpdate)
        return true;

    // Get the cache file.
    std::string cacheFilename = Main::GetCacheManager().Lookup(m_fontFilename);

    // Open the cache file for writing.
    std::ofstream file(Main::GetCacheDir() + cacheFilename, std::ios::binary);

    if(!file.is_open())
    {
        Log() << LogSaveCacheError(m_fontFilename) << "Couldn't create the cache file.";
        return false;
    }

    // Calculate font file CRC.
    boost::crc_32_type crc;

    if(!CalculateFileCRC(Main::GetWorkingDir() + m_fontFilename, &crc))
    {
        Log() << LogSaveCacheError(m_fontFilename) << "Couldn't calculate file CRC.";
        return false;
    }

    // Write the hader.
    CacheHeader header;
    header.identifier = boost::uuids::string_generator()(CacheIdentifier);
    header.version = CacheVersion;
    header.checksum = crc.checksum();
    header.glyphs = m_glyphCache.size();

    StreamWrite(file, header);

    // Write glyph entries.
    for(auto it = m_glyphCache.begin(); it != m_glyphCache.end(); ++it)
    {
        StreamWrite(file, it->first);
        StreamWrite(file, it->second);
    }

    // Write the font atlas.
    SDL_LockSurface(m_atlasSurface);

    char* surfacePixels = reinterpret_cast<char*>(m_atlasSurface->pixels);
    unsigned long surfaceSize = m_atlasSurface->h * m_atlasSurface->pitch;

    file.write((char*)surfacePixels, surfaceSize);

    SDL_UnlockSurface(m_atlasSurface);

    // Write the packer.
    m_atlasPacker.Serialize(file);

    // Check if any stream errors occured.
    if(!file.good())
    {
        Log() << LogSaveCacheError(m_fontFilename) << "There was an error while writting to the stream.";
        return false;
    }

    // Don't update the cache until a new glyph is added.
    m_cacheUpdate = false;

    return true;
}

bool Font::LoadCache()
{
    if(!m_initialized)
        return false;

    // Get the cache identifier.
    std::string identifier = Main::GetCacheManager().Lookup(m_fontFilename);

    // Open the cache file for reading.
    std::ifstream file(Main::GetCacheDir() + identifier, std::ios::binary);

    if(!file.is_open())
    {
        // No cache to load.
        return false;
    }

    // Calculate font file CRC.
    boost::crc_32_type crc;

    if(!CalculateFileCRC(Main::GetWorkingDir() + m_fontFilename, &crc))
    {
        Log() << LogLoadCacheError(m_fontFilename) << "Couldn't calculate file CRC.";
        return false;
    }

    // Read the header.
    CacheHeader header;
    StreamRead(file, &header);

    if(!file.good())
    {
        Log() << LogLoadCacheError(m_fontFilename) << "Couldn't read the header.";
        return false;
    }

    // Validate the header.
    if(header.identifier != boost::uuids::string_generator()(CacheIdentifier))
    {
        Log() << LogLoadCacheError(m_fontFilename) << "Invalid class identifier.";
        return false;
    }

    if(header.version != CacheVersion)
    {
        Log() << LogLoadCacheError(m_fontFilename) << "Invalid class version.";
        return false;
    }

    if(header.checksum != crc.checksum())
    {
        Log() << LogLoadCacheError(m_fontFilename) << "Invalid file checksum.";
        return false;
    }

    // Clear read glyphs in case of failure.
    auto clearReadGlyphs = MakeScopeGuard([&]()
    {
        ClearContainer(m_glyphCache);
    });

    // Read glyph entries.
    for(int i = 0; i < header.glyphs; ++i)
    {
        // Read the glyph entry.
        FT_ULong character;
        Glyph glyph;

        StreamRead(file, &character);
        StreamRead(file, &glyph);

        if(!file.good())
        {
            Log() << LogLoadCacheError(m_fontFilename) << "Couldn't read a glyph entry.";
            return false;
        }

        // Add glyph to the list.
        m_glyphCache.insert(std::make_pair(character, glyph));
    }

    // Clear surface in case of failure.
    auto clearReadPixels = MakeScopeGuard([&]()
    {
        SDL_LockSurface(m_atlasSurface);

        char* surfacePixels = reinterpret_cast<char*>(m_atlasSurface->pixels);
        unsigned long surfaceSize = m_atlasSurface->h * m_atlasSurface->pitch;

        memset(surfacePixels, 0, surfaceSize);

        SDL_UnlockSurface(m_atlasSurface);
    });

    // Read the font atlas.
    SDL_LockSurface(m_atlasSurface);

    char* surfacePixels = reinterpret_cast<char*>(m_atlasSurface->pixels);
    unsigned long surfaceSize = m_atlasSurface->h * m_atlasSurface->pitch;

    file.read((char*)surfacePixels, surfaceSize);

    SDL_UnlockSurface(m_atlasSurface);

    if(!file.good())
    {
        Log() << LogLoadCacheError(m_fontFilename) << "Couldn't read the font atlas.";
        return false;
    }

    // Clear read packer in case of failure.
    auto clearReadPacker = MakeScopeGuard([&]()
    {
        m_atlasPacker.Cleanup();
    });

    // Read the packer state.
    m_atlasPacker.Deserialize(file);

    if(!file.good())
    {
        Log() << LogLoadCacheError(m_fontFilename) << "Couldn't read the atlas packer.";
        return false;
    } 

    // Update the atlas texture.
    m_atlasUpload = true;

    // Don't update cache unless new glyph is cached.
    m_cacheUpdate = false;

    // Disable the scope guard.
    clearReadGlyphs.Disable();
    clearReadPixels.Disable();
    clearReadPacker.Disable();

    return true;
}
