#include "Precompiled.hpp"
#include "Font.hpp"
#include "MainContext.hpp"

namespace
{
    // Log error messages.
    #define LogLoadError(filename) "Failed to load a font from \"" << filename << "\" file! "

    // Size of the atlas texture.
    const int AtlasWidth = 1024;
    const int AtlasHeight = 1024;

    // Spacing between glyphs on the atlas (to avoid filtering artifacts).
    const int AtlasGlyphSpacing = 1;

    // Signed distance field calculation parameters.
    const int DistanceFieldFontSize = 96;
    const int DistanceFieldDownscale = 8;
    const int DistanceFieldSpread = 4;

    const int DistanceFieldSpreadScaled = DistanceFieldSpread * DistanceFieldDownscale;

    // Inverted constant values.
    const float DistanceFieldDownscaleInverted = 1.0f / DistanceFieldDownscale;
    const float DistanceFieldSpreadScaledInverted = 1.0f / DistanceFieldSpreadScaled;

    // Default glyph code if caching fails.
    FT_ULong DefaultGlyph = '?';
}

Font::Font() :
    m_fontFace(nullptr),
    m_glyphCache(),
    m_glyphDefault(nullptr),
    m_atlasSurface(nullptr),
    m_atlasTexture(),
    m_atlasUpload(false),
    m_atlasOverflow(false),
    m_packer(),
    m_initialized(false)
{
}

Font::~Font()
{
    Cleanup();
}

bool Font::Load(std::string filename)
{
    Cleanup();

    assert(Main::FontLibrary() != nullptr);

    // Validate arguments.
    if(filename.empty())
    {
        Log() << LogLoadError(filename) << "Invalid argument - \"filename\" is invalid.";
        return false;
    }

    // Load font face.
    if(FT_New_Face(Main::FontLibrary(), filename.c_str(), 0, &m_fontFace) != 0)
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
    if(FT_Set_Pixel_Sizes(m_fontFace, 0, DistanceFieldFontSize * DistanceFieldDownscale) != 0)
    {
        Log() << LogLoadError(filename) << "Couldn't set font size.";
        Cleanup();
        return false;
    }

    // Create font atlas surface.
    m_atlasSurface = SDL_CreateRGBSurface(0, AtlasWidth, AtlasHeight, 8, 0, 0, 0, 0);

    if(m_atlasSurface == nullptr)
    {
        Log() << LogLoadError(filename) << "Couldn't create atlas surface.";
        Cleanup();
        return false;
    }

    // Create font atlas texture.
    if(!m_atlasTexture.Initialize(AtlasWidth, AtlasHeight, GL_RED, nullptr))
    {
        Log() << LogLoadError(filename) << "Couldn't create atlas texture.";
        Cleanup();
        return false;
    }

    // Create a shelf packer.
    m_packer.Create(
        glm::ivec2(AtlasWidth, AtlasHeight),
        glm::ivec2(AtlasGlyphSpacing, AtlasGlyphSpacing)
    );

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
    Log() << "Loaded font from \"" << filename << "\" file.";

    return true;
}

void Font::Cleanup()
{
    // Cleanup glyph packer.
    m_packer.Cleanup();

    // Cleanup font atlas.
    SDL_FreeSurface(m_atlasSurface);
    m_atlasSurface = nullptr;

    m_atlasTexture.Cleanup();

    m_atlasUpload = false;
    m_atlasOverflow = false;

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

    // Cache base ASCII characters.
    for(char c = '!'; c <= '~'; ++c)
    {
        CacheGlyph((FT_ULong)c);
    }
}

void Font::CacheGlyphs(const char* characters)
{
    if(!m_initialized)
        return;

    // Cache a UTF8 string of characters.
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

    // Don't waste time rendering glyphs if the atlas if full.
    if(m_atlasOverflow)
        return nullptr;

    //
    // Render Glyph
    //

    // Load font glyph.
    FT_ULong glyphIndex = FT_Get_Char_Index(m_fontFace, character);

    if(FT_Load_Glyph(m_fontFace, glyphIndex, FT_LOAD_DEFAULT) != 0)
        return nullptr;

    FT_GlyphSlot glyphSlot = m_fontFace->glyph;

    // Render font glyph.
    if(FT_Render_Glyph(m_fontFace->glyph, FT_RENDER_MODE_MONO) != 0)
        return nullptr;

    // Get glyph bitmap parameters.
    int glyphBitmapWidth = glyphSlot->bitmap.width;
    int glyphBitmapHeight = glyphSlot->bitmap.rows;
    int glyphBitmapPitch = glyphSlot->bitmap.pitch;

    uint8_t* glyphBitmapBytes = glyphSlot->bitmap.buffer;

    // Get a pixel from the gylph surface.
    // Each pixel is stored in one bit, so every byte contains 8 pixels.
    auto GetGlyphBitmapPixel = [&](int x, int y) -> uint8_t
    {
        if(x < 0 || x > glyphBitmapWidth - 1)
            return 0;

        if(y < 0 || y > glyphBitmapHeight - 1)
            return 0;

        // Calculate byte index and bit offset.
        int byteIndex = x / 8;
        int bitOffset = x % 8;

        // Even though bits start from right, the pixels are 
        // packed from the left (just like they are laid out).
        bitOffset = 7 - bitOffset;

        // Get the glyph bitmap byte.
        uint8_t byte = glyphBitmapBytes[y * glyphBitmapPitch + byteIndex];

        // Extract the bit pixel.
        return (byte >> bitOffset) & 0x1;
    };

    //
    // Calculate Distance Field
    // - Uses Dead Reckoning algorithm to calculate the distance field.
    // - Some artifacts are present that arent visible on the technical paper,
    //   but I were unable to eliminate them. They arent that big, so they will
    //   be lost after downscaling.
    //

    // Algorithm variables.
    const float d1 = 1.0f;
    const float d2 = std::sqrt(2.0f);

    int fieldBitmapWidth = glyphBitmapWidth + DistanceFieldSpreadScaled * 2;
    int fieldBitmapHeight = glyphBitmapHeight + DistanceFieldSpreadScaled * 2;

    // Create a field bitmap containing distances.
    float* fieldBitmapDistances = new float[fieldBitmapWidth * fieldBitmapHeight];
    SCOPE_GUARD(delete[] fieldBitmapDistances);

    auto GetFieldBitmapDistance = [&](int x, int y) -> float
    {
        if(x < 0 || x > fieldBitmapWidth - 1)
            return std::numeric_limits<float>::max();

        if(y < 0 || y > fieldBitmapHeight - 1)
            return std::numeric_limits<float>::max();

        return fieldBitmapDistances[y * fieldBitmapWidth + x];
    };

    // Create a field bitmap containing nearest borders.
    glm::ivec2* fieldBitmapBorders = new glm::ivec2[fieldBitmapWidth * fieldBitmapHeight];
    SCOPE_GUARD(delete[] fieldBitmapBorders);

    auto GetFieldBitmapBorder = [&](int x, int y) -> glm::ivec2
    {
        if(x < 0 || x > fieldBitmapWidth - 1)
            return glm::ivec2(-1, -1);

        if(y < 0 || y > fieldBitmapHeight - 1)
            return glm::ivec2(-1, -1);

        return fieldBitmapBorders[y * fieldBitmapWidth + x];
    };

    // Initialize field bitmaps.
    for(int y = 0; y < fieldBitmapHeight; ++y)
    for(int x = 0; x < fieldBitmapWidth; ++x)
    {
        unsigned index = y * fieldBitmapWidth + x;

        fieldBitmapDistances[index] = std::numeric_limits<float>::max();
        fieldBitmapBorders[index] = glm::ivec2(-1, -1);
    }

    // Find glyph borders.
    for(int y = 0; y < fieldBitmapHeight; ++y)
    for(int x = 0; x < fieldBitmapWidth; ++x)
    {
        unsigned index = y * fieldBitmapWidth + x;

        int gx = x - DistanceFieldSpreadScaled;
        int gy = y - DistanceFieldSpreadScaled;

        uint8_t glyphPixel = GetGlyphBitmapPixel(gx, gy);

        if(glyphPixel != 0)
        {
            bool isAtBorder =
                GetGlyphBitmapPixel(gx - 1, gy) != glyphPixel ||
                GetGlyphBitmapPixel(gx + 1, gy) != glyphPixel ||
                GetGlyphBitmapPixel(gx, gy - 1) != glyphPixel ||
                GetGlyphBitmapPixel(gx, gy + 1) != glyphPixel;

            if(isAtBorder)
            {
                fieldBitmapDistances[index] = 0.0f;
                fieldBitmapBorders[index] = glm::ivec2(x, y);
            }
        }
    }

    // Perform the first forward pass.
    for(int y = 0; y < fieldBitmapHeight; ++y)
    for(int x = 0; x < fieldBitmapWidth; ++x)
    {
        unsigned index = y * fieldBitmapWidth + x;

        // x--
        // -c-
        // ---

        if(GetFieldBitmapDistance(x - 1, y - 1) + d2 < GetFieldBitmapDistance(x, y))
        {
            glm::ivec2 border = GetFieldBitmapBorder(x - 1, y - 1);

            fieldBitmapBorders[index] = border;
            fieldBitmapDistances[index] = glm::distance(glm::vec2(x, y), glm::vec2((float)border.x, (float)border.y));
        }

        // -x-
        // -c-
        // ---

        if(GetFieldBitmapDistance(x, y - 1) + d1 < GetFieldBitmapDistance(x, y))
        {
            glm::ivec2 border = GetFieldBitmapBorder(x, y - 1);

            fieldBitmapBorders[index] = border;
            fieldBitmapDistances[index] = glm::distance(glm::vec2(x, y), glm::vec2((float)border.x, (float)border.y));
        }

        // --x
        // -c-
        // ---

        if(GetFieldBitmapDistance(x + 1, y - 1) + d2 < GetFieldBitmapDistance(x, y))
        {
            glm::ivec2 border = GetFieldBitmapBorder(x + 1, y - 1);

            fieldBitmapBorders[index] = border;
            fieldBitmapDistances[index] = glm::distance(glm::vec2(x, y), glm::vec2((float)border.x, (float)border.y));
        }

        // ---
        // xc-
        // ---

        if(GetFieldBitmapDistance(x - 1, y) + d1 < GetFieldBitmapDistance(x, y))
        {
            glm::ivec2 border = GetFieldBitmapBorder(x - 1, y);

            fieldBitmapBorders[index] = border;
            fieldBitmapDistances[index] = glm::distance(glm::vec2(x, y), glm::vec2((float)border.x, (float)border.y));
        }
    }

    // Perform the second backward pass.
    for(int y = fieldBitmapHeight - 1; y >= 0; --y)
    for(int x = fieldBitmapWidth - 1; x >= 0; --x)
    {
        unsigned index = y * fieldBitmapWidth + x;

        // ---
        // -cx
        // ---

        if(GetFieldBitmapDistance(x + 1, y) + d1 < GetFieldBitmapDistance(x, y))
        {
            glm::ivec2 border = GetFieldBitmapBorder(x + 1, y);

            fieldBitmapBorders[index] = border;
            fieldBitmapDistances[index] = glm::distance(glm::vec2(x, y), glm::vec2((float)border.x, (float)border.y));
        }

        // ---
        // -c-
        // x--

        if(GetFieldBitmapDistance(x - 1, y + 1) + d2 < GetFieldBitmapDistance(x, y))
        {
            glm::ivec2 border = GetFieldBitmapBorder(x - 1, y + 1);

            fieldBitmapBorders[index] = border;
            fieldBitmapDistances[index] = glm::distance(glm::vec2(x, y), glm::vec2((float)border.x, (float)border.y));
        }

        // ---
        // -c-
        // -x-

        if(GetFieldBitmapDistance(x, y + 1) + d1 < GetFieldBitmapDistance(x, y))
        {
            glm::ivec2 border = GetFieldBitmapBorder(x, y + 1);

            fieldBitmapBorders[index] = border;
            fieldBitmapDistances[index] = glm::distance(glm::vec2(x, y), glm::vec2((float)border.x, (float)border.y));
        }

        // ---
        // -c-
        // --x

        if(GetFieldBitmapDistance(x + 1, y + 1) + d2 < GetFieldBitmapDistance(x, y))
        {
            glm::ivec2 border = GetFieldBitmapBorder(x + 1, y + 1);

            fieldBitmapBorders[index] = border;
            fieldBitmapDistances[index] = glm::distance(glm::vec2(x, y), glm::vec2((float)border.x, (float)border.y));
        }
    }

    // Denote distance field signs.
    for(int y = 0; y < fieldBitmapHeight; ++y)
    for(int x = 0; x < fieldBitmapWidth; ++x)
    {
        unsigned index = y * fieldBitmapWidth + x;

        int gx = x - DistanceFieldSpreadScaled;
        int gy = y - DistanceFieldSpreadScaled;

        if(GetGlyphBitmapPixel(gx, gy) == 0)
        {
            float& distance = fieldBitmapDistances[index];
            distance = -distance;
        }
    }

    //
    // Create Scaled Surface
    // - Scaling using SDL_SoftStretch() isn't ideal.
    //   Replace with high quality downscaling.
    //

    // Create the distance surface.
    SDL_Surface* distanceSurface = SDL_CreateRGBSurface(
        0, 
        fieldBitmapWidth, 
        fieldBitmapHeight, 
        8, 0, 0, 0, 0
    );

    if(distanceSurface == nullptr)
        return nullptr;

    SCOPE_GUARD(SDL_FreeSurface(distanceSurface));

    // Fill the surface.
    SDL_LockSurface(distanceSurface);

    uint8_t* distanceSurfacePixels = reinterpret_cast<uint8_t*>(distanceSurface->pixels);

    for(int y = 0; y < fieldBitmapHeight; ++y)
    for(int x = 0; x < fieldBitmapWidth; ++x)
    {
        // Get the pixel pointer.
        uint8_t* fieldBitmapPixel = &distanceSurfacePixels[y * distanceSurface->pitch + x];

        // Map the value spread values to 0 - 255 range.
        float alpha = 0.5f + 0.5f * (GetFieldBitmapDistance(x, y) * DistanceFieldSpreadScaledInverted);
        alpha = std::min(1.0f, std::max(0.0f, alpha));
        uint8_t byte = (uint8_t)(alpha * 0xFF);

        // Save the pixel value.
        *fieldBitmapPixel = byte;
    }

    SDL_UnlockSurface(distanceSurface);

    // Debug surface dump.
    #if 0
    {
        std::string filename;
        filename += "df_";
        filename += (char)character;
        filename += ".bmp";

        SaveSurface(distanceSurface, filename.c_str());
    }
    #endif

    // Create the scaled surface.
    SDL_Surface* scaledSurface = SDL_CreateRGBSurface(
        0, 
        fieldBitmapWidth / DistanceFieldDownscale, 
        fieldBitmapHeight / DistanceFieldDownscale, 
        8, 0, 0, 0, 0
    );

    if(scaledSurface == nullptr)
        return nullptr;

    SCOPE_GUARD(SDL_FreeSurface(scaledSurface));

    // Get distance bitmap parameters.
    int scaledSurfaceWidth = scaledSurface->w;
    int scaledSurfaceHeight = scaledSurface->h;
    int scaledSurfacePitch = scaledSurface->pitch;

    // Scale the distance surface.
    SDL_SoftStretch(distanceSurface, nullptr, scaledSurface, nullptr);

    // Debug surface dump.
    #if 0
    {
        std::string filename;
        filename += "dfs_";
        filename += (char)character;
        filename += ".bmp";

        SaveSurface(scaledSurface, filename.c_str());
    }
    #endif

    // Flip surface (we will draw upside down on SDL surface).
    FlipSurface(scaledSurface);

    //
    // Create Glyph Entry
    //

    // Add element to the packer.
    if(!m_packer.AddElement(glm::ivec2(scaledSurfaceWidth, scaledSurfaceHeight)))
    {
        // Not enough space on the atlas for this glyph.
        m_atlasOverflow = true;
        return nullptr;
    }

    // Draw glyph surface on the atlas.
    SDL_Rect drawRect;
    drawRect.x = m_packer.GetPosition().x;
    drawRect.y = m_packer.GetPosition().y;
    drawRect.w = scaledSurfaceWidth;
    drawRect.h = scaledSurfaceHeight;

    SDL_BlitSurface(scaledSurface, nullptr, m_atlasSurface, &drawRect);

    // Fill glyph structure.
    glm::vec2 pixelSize(1.0f / AtlasWidth, 1.0f / AtlasHeight);

    Glyph glyph;
    glyph.position.x = drawRect.x;
    glyph.position.y = drawRect.y;
    glyph.size.x = drawRect.w;
    glyph.size.y = drawRect.h;

    glyph.offset.x = (float)(glyphSlot->bitmap_left);
    glyph.offset.y = (float)(glyphSlot->bitmap_top - glyphSlot->bitmap.rows);
    glyph.advance.x = (float)(glyphSlot->advance.x >> 6);
    glyph.advance.y = (float)(glyphSlot->advance.y >> 6);

    glyph.offset.x = glyph.offset.x * DistanceFieldDownscaleInverted - DistanceFieldSpread;
    glyph.offset.y = glyph.offset.y * DistanceFieldDownscaleInverted - DistanceFieldSpread;
    glyph.advance.x = glyph.advance.x * DistanceFieldDownscaleInverted;
    glyph.advance.y = glyph.advance.y * DistanceFieldDownscaleInverted;

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

    // Upload atlas surface to GPU.
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

float Font::GetKerning(FT_ULong left, FT_ULong right) const
{
    FT_Vector kerning;

    // Get kerning value between two characters.
    FT_UInt glyphLeft = FT_Get_Char_Index(m_fontFace, left);
    FT_UInt glyphRight = FT_Get_Char_Index(m_fontFace, right);

    if(FT_Get_Kerning(m_fontFace, glyphLeft, glyphRight, FT_KERNING_DEFAULT, &kerning) != 0)
        return 0.0f;

    return (kerning.x >> 6) * DistanceFieldDownscaleInverted;
}

float Font::GetLineSpacing() const
{
    if(!m_initialized)
        return 0.0f;

    return (m_fontFace->size->metrics.height >> 6) * DistanceFieldDownscaleInverted;
}

float Font::GetAscender() const
{
    if(!m_initialized)
        return 0.0f;

    return (m_fontFace->size->metrics.ascender >> 6) * DistanceFieldDownscaleInverted;
}

float Font::GetDescender() const
{
    if(!m_initialized)
        return 0.0f;

    return (m_fontFace->size->metrics.descender >> 6) * DistanceFieldDownscaleInverted;
}

int Font::GetAtlasWidth() const
{
    return AtlasWidth;
}

int Font::GetAtlasHeight() const
{
    return AtlasHeight;
}

int Font::GetBaseSize() const
{
    return DistanceFieldFontSize;
}

float Font::GetScaling(float size) const
{
    static const float Inverse = 1.0f / DistanceFieldFontSize;
    return size * Inverse;
}
