#pragma once

#include "Precompiled.hpp"
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

	bool Load(std::string filename, int size, int atlasWidth, int atlasHeight);
	void Cleanup();

	void CacheGlyphs(const wchar_t* characters);
	void UpdateAtlasTexture();

	const Glyph* GetGlyph(FT_ULong character);

	int GetLineSpacing() const;

	int GetAtlasWidth() const
	{
		return m_atlasWidth;
	}

	int GetAtlasHeight() const
	{
		return m_atlasHeight;
	}

	const SDL_Surface* GetSurface() const
	{
		return m_atlasSurface;
	}

	const Texture* GetTexture() const
	{
		return &m_atlasTexture;
	}

private:
	const Glyph* CacheGlyph(FT_ULong character);

private:
	FT_Face m_fontFace;

	// Glyph registry.
	GlyphCache		m_glyphCache;
	const Glyph*	m_glyphDefault;

	// Font atlas where glyphs are stores.
	int				m_atlasWidth;
	int				m_atlasHeight;
	SDL_Surface*	m_atlasSurface;
	Texture			m_atlasTexture;
	bool			m_atlasUpload;

	// Current shelf used for packing glyphs.
	glm::ivec2	m_shelfPosition;
	int			m_shelfSize;

	bool m_initialized;
};
