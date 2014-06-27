#pragma once

#include "Precompiled.hpp"

#include "Common/ShelfPacker.hpp"
#include "Graphics/Texture.hpp"

//
// Glyph
//

struct Glyph
{
    // Position in pixels.
    glm::ivec2 position;

    // Size in pixels.
    glm::ivec2 size;

    // Offset from glyph origin.
    glm::vec2 offset;

    // Glyph advance.
    glm::vec2 advance;
};

//
// Font
//

class Font
{
public:
    Font();
    ~Font();

    bool Load(std::string filename);
    void Cleanup();

    void CacheASCII();
    void CacheGlyphs(const char* characters);
    void UpdateAtlasTexture();

    const Glyph* GetGlyph(FT_ULong character);

    float GetKerning(FT_ULong left, FT_ULong right) const;
    float GetLineSpacing() const;
    float GetAscender() const;
    float GetDescender() const;

    int GetAtlasWidth() const;
    int GetAtlasHeight() const;

    int GetBaseSize() const;

    float GetScaling(float size) const;

    const SDL_Surface* GetAtlasSurface() const
    {
        return m_atlasSurface;
    }

    const Texture* GetAtlasTexture() const
    {
        return &m_atlasTexture;
    }

private:
    const Glyph* CacheGlyph(FT_ULong character);

    // Font cache.
    bool SaveCache();
    bool LoadCache();

private:
    // Glyph cache type.
    typedef std::map<FT_ULong, Glyph> GlyphCache;

    // Font face.
    std::string  m_fontFilename;
    FT_Face      m_fontFace;

    // Glyph registry.
    GlyphCache   m_glyphCache;
    const Glyph* m_glyphDefault;
    bool         m_cacheUpdate;

    // Atlas texture.
    SDL_Surface* m_atlasSurface;
    Texture      m_atlasTexture;
    bool         m_atlasUpload;
    ShelfPacker  m_atlasPacker;
    bool         m_atlasOverflow;

    bool m_initialized;
};
