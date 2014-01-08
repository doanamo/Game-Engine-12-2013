#include "Precompiled.hpp"
#include "Font.hpp"
#include "Context.hpp"

namespace
{
    // Log error messages.
    #define LogLoadError(filename) "Failed to load a font from \"" << filename << "\" file! "

    // Spacing between characters on the atlas (to avoid filtering artifacts).
    const int AtlasGlyphSpacing = 1;

    // Default glyph code if caching fails.
    FT_ULong DefaultGlyph = '?';
}

Font::Font() :
    m_fontFace(nullptr),
    m_glyphCache(),
    m_glyphDefault(nullptr),
    m_atlasWidth(0),
    m_atlasHeight(0),
    m_atlasSurface(nullptr),
    m_atlasTexture(),
    m_atlasUpload(false),
    m_shelfPosition(AtlasGlyphSpacing, AtlasGlyphSpacing),
    m_shelfSize(0),
    m_initialized(false)
{
}

Font::~Font()
{
    Cleanup();
}

bool Font::Load(std::string filename, int size, int atlasWidth, int atlasHeight)
{
    Cleanup();

    assert(Context::FontLibrary() != nullptr);

    // Validate arguments.
    if(filename.empty())
    {
        Log() << LogLoadError(filename) << "Invalid argument - \"filename\" is invalid.";
        return false;
    }

    if(size <= 0)
    {
        Log() << LogLoadError(filename) << "Invalid argument - \"size\" is invalid.";
        return false;
    }

    if(atlasWidth <= 0)
    {
        Log() << LogLoadError(filename) << "Invalid argument - \"atlasWidth\" is invalid.";
        return false;
    }

    if(atlasHeight <= 0)
    {
        Log() << LogLoadError(filename) << "Invalid argument - \"atlasHeight\" is invalid.";
        return false;
    }

    m_atlasWidth = atlasWidth;
    m_atlasHeight = atlasHeight;

    // Load font face.
    if(FT_New_Face(Context::FontLibrary(), filename.c_str(), 0, &m_fontFace) != 0)
    {
        Log() << LogLoadError(filename) << "Couldn't load the font.";
        Cleanup();
        return false;
    }

    // Set font encoding.
    if(FT_Select_Charmap(m_fontFace, FT_ENCODING_UNICODE) != 0)
    {
        Log() << LogLoadError(filename) << "Couldn't set font encoding.";
        Cleanup();
        return false;
    }

    // Set font size.
    if(FT_Set_Pixel_Sizes(m_fontFace, 0, size) != 0)
    {
        Log() << LogLoadError(filename) << "Couldn't set font size.";
        Cleanup();
        return false;
    }

    // Create font atlas surface.
    m_atlasSurface = SDL_CreateRGBSurface(0, m_atlasWidth, m_atlasHeight, 8, 0, 0, 0, 0);

    if(m_atlasSurface == nullptr)
    {
        Log() << LogLoadError(filename) << "Couldn't create atlas surface.";
        Cleanup();
        return false;
    }

    // Create font atlas texture.
    if(!m_atlasTexture.Initialize(m_atlasWidth, m_atlasHeight, GL_RED, nullptr))
    {
        Log() << LogLoadError(filename) << "Couldn't create atlas texture.";
        Cleanup();
        return false;
    }

    // Set initialized state.
    m_initialized = true;

    // Cache the default glyph.
    m_glyphDefault = CacheGlyph(DefaultGlyph);

    if(m_glyphDefault == nullptr)
    {
        Log() << LogLoadError(filename) << "Couldn't cache the default glyph.";
        Cleanup();
        return false;
    }

    // Success!
    Log() << "Loaded font from \"" << filename << "\" file. (Size: " << size << ")";

    return true;
}

void Font::Cleanup()
{
    // Cleanup atlas shelf.
    m_shelfPosition = glm::ivec2(AtlasGlyphSpacing, AtlasGlyphSpacing);
    m_shelfSize = 0;

    // Cleanup font atlas.
    m_atlasWidth = 0;
    m_atlasHeight = 0;

    SDL_FreeSurface(m_atlasSurface);
    m_atlasSurface = nullptr;

    m_atlasTexture.Cleanup();

    m_atlasUpload = false;

    // Cleanup glyph registry.
    ClearContainer(m_glyphCache);

    m_glyphDefault = nullptr;

    // Cleanup loaded font.
    FT_Done_Face(m_fontFace);
    m_fontFace = nullptr;

    m_initialized = false;
}

void Font::CacheASCII()
{
    if(!m_initialized)
        return;

    for(int i = '!'; i <= '~'; ++i)
    {
        CacheGlyph((FT_ULong)i);
    }
}

void Font::CacheGlyphs(const char* characters)
{
    if(!m_initialized)
        return;

    if(characters != nullptr)
    {
        const char* it = characters;
        const char* end = it + strlen(characters);

        while(it != end)
        {
            CacheGlyph(utf8::next(it, end));
        }
    }
}

const Glyph* Font::CacheGlyph(FT_ULong character)
{
    if(!m_initialized)
        return nullptr;

    // Find out if glyph is already cached.
    auto it = m_glyphCache.find(character);

    if(it != m_glyphCache.end())
    {
        return &it->second;
    }

    // Load font glyph.
    FT_ULong glyphIndex = FT_Get_Char_Index(m_fontFace, character);

    if(FT_Load_Glyph(m_fontFace, glyphIndex, FT_LOAD_DEFAULT) != 0)
        return nullptr;

    FT_GlyphSlot glyphSlot = m_fontFace->glyph;

    // Render font glyph.
    if(FT_Render_Glyph(m_fontFace->glyph, FT_RENDER_MODE_NORMAL) != 0)
        return nullptr;

    FT_Bitmap* glyphBitmap = &glyphSlot->bitmap;

    // Create a glyph surface.
    // Todo: Allocated a surface that can hold all glyph bitmaps once.
    SDL_Surface* glyphSurface = SDL_CreateRGBSurface(0, glyphBitmap->width, glyphBitmap->rows, 8, 0, 0, 0, 0);

    if(glyphSurface == nullptr)
        return nullptr;

    SCOPE_GUARD(SDL_FreeSurface(glyphSurface));

    // Copy glyph pixels.
    SDL_LockSurface(glyphSurface);

    unsigned char* glyphSurfaceData = reinterpret_cast<unsigned char*>(glyphSurface->pixels);

    for(long i = 0; i < glyphBitmap->rows; ++i)
    {
        memcpy(&glyphSurfaceData[i * glyphSurface->pitch], &glyphBitmap->buffer[i * glyphBitmap->pitch], glyphBitmap->width);
    }

    SDL_UnlockSurface(glyphSurface);

    // Flip glyph surface (we will draw upside down on SDL surface).
    FlipSurface(glyphSurface);

    // Check space on the current shelf.
    if(m_shelfPosition.x + glyphBitmap->width + AtlasGlyphSpacing > m_atlasWidth)
    {
        // Move to next shelf.
        m_shelfPosition.x = AtlasGlyphSpacing;
        m_shelfPosition.y = m_shelfPosition.y + m_shelfSize + AtlasGlyphSpacing;
        m_shelfSize = 0;
    }

    if(m_shelfPosition.y + glyphBitmap->rows + AtlasGlyphSpacing > m_atlasHeight)
    {
        // Not enough space on the atlas for this glyph.
        return nullptr;
    }

    // Draw glyph surface on the atlas.
    SDL_Rect drawRect;
    drawRect.x = m_shelfPosition.x;
    drawRect.y = m_shelfPosition.y;
    drawRect.w = glyphBitmap->width;
    drawRect.h = glyphBitmap->rows;

    SDL_BlitSurface(glyphSurface, nullptr, m_atlasSurface, &drawRect);

    // Update current shelf position and size.
    m_shelfPosition.x += glyphBitmap->width + AtlasGlyphSpacing;

    m_shelfSize = std::max(m_shelfSize, glyphBitmap->rows);

    // Fill glyph structure.
    glm::vec2 pixelSize(1.0f / m_atlasWidth, 1.0f / m_atlasHeight);

    Glyph glyph;
    glyph.position.x = drawRect.x;
    glyph.position.y = drawRect.y;
    glyph.size.x = drawRect.w;
    glyph.size.y = drawRect.h;
    glyph.offset.x = glyphSlot->bitmap_left;
    glyph.offset.y = glyphSlot->bitmap_top - glyphBitmap->rows;
    glyph.advance.x = glyphSlot->advance.x >> 6;
    glyph.advance.y = glyphSlot->advance.y >> 6;

    // Add glyph to the cache.
    auto result = m_glyphCache.insert(std::make_pair(character, glyph));

    // Upload font atlas on next use.
    m_atlasUpload = true;

    // Return inserted glyph.
    return &result.first->second;
}

void Font::UpdateAtlasTexture()
{
    if(!m_initialized)
        return;

    if(m_atlasUpload)
    {
        m_atlasTexture.Update(m_atlasSurface->pixels);
        m_atlasUpload = false;
    }
}

const Glyph* Font::GetGlyph(FT_ULong character)
{
    if(!m_initialized)
        return nullptr;

    // Get the character glyph (it will be cached if needed).
    const Glyph* glyph = CacheGlyph(character);

    if(glyph != nullptr)
    {
        return glyph;
    }
    else
    {
        return m_glyphDefault;
    }
}

int Font::GetKerning(FT_ULong left, FT_ULong right) const
{
    FT_Vector kerning;

    FT_UInt glyphLeft = FT_Get_Char_Index(m_fontFace, left);
    FT_UInt glyphRight = FT_Get_Char_Index(m_fontFace, right);

    if(FT_Get_Kerning(m_fontFace, glyphLeft, glyphRight, FT_KERNING_DEFAULT, &kerning) != 0)
        return 0;

    return kerning.x >> 6;
}

int Font::GetLineSpacing() const
{
    if(!m_initialized)
        return 0;

    return m_fontFace->size->metrics.height >> 6;
}

int Font::GetAscender() const
{
    if(!m_initialized)
        return 0;

    return m_fontFace->size->metrics.ascender >> 6;
}

int Font::GetDescender() const
{
    if(!m_initialized)
        return 0;

    return m_fontFace->size->metrics.descender >> 6;
}
