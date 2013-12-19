#pragma once

#include "Precompiled.hpp"
#include "Graphics/Texture.hpp"

//
// Glyph
//

struct Glyph
{
	glm::ivec2 drawingOffset;
	glm::ivec2 drawingAdvance;
	glm::vec4 textureCoords;
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

	const Texture* GetTexture() const
	{
		return &m_atlasTexture;
	}

private:
	const Glyph* CacheGlyph(FT_ULong character);

private:
	FT_Face m_fontFace;

	// Glyph registry.
	GlyphCache m_glyphCache;

	// Font atlas where glyphs are stores.
	int				m_atlasWidth;
	int				m_atlasHeight;
	SDL_Surface*	m_atlasSurface;
	Texture			m_atlasTexture;
	bool			m_atlasUpload;

	// Current shelf used for packing glyphs.
	glm::ivec2	m_shelfPosition;
	int			m_shelfSize;

	bool m_Initialized;
};
