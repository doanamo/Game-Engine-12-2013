#pragma once

#include "Precompiled.hpp"
#include "Common/ShelfPacker.hpp"
#include "Graphics/Texture.hpp"

//
// Glyph
//

struct Glyph
{
    glm::ivec2 position;
    glm::ivec2 size;
    glm::ivec2 offset;
    glm::ivec2 advance;
};

//
// Font
//

class Font
{
public:
    typedef std::map<FT_ULong, Glyph> GlyphCache;

public:
    Font();
    ~Font();

    bool Load(std::string filename);
    void Cleanup();

    void CacheASCII();
    void CacheGlyphs(const char* characters);
    void UpdateAtlasTexture();

    const Glyph* GetGlyph(FT_ULong character);

    int GetKerning(FT_ULong left, FT_ULong right) const;
    int GetLineSpacing() const;
    int GetAscender() const;
    int GetDescender() const;

    int GetAtlasWidth() const;
    int GetAtlasHeight() const;

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

private:
    FT_Face m_fontFace;

    // Glyph registry.
    GlyphCache      m_glyphCache;
    const Glyph*    m_glyphDefault;

    // Font atlas where glyphs are stores.
    SDL_Surface*    m_atlasSurface;
    Texture         m_atlasTexture;
    bool            m_atlasUpload;

    // Glyph texture packer.
    ShelfPacker m_packer;

    bool m_initialized;
};
