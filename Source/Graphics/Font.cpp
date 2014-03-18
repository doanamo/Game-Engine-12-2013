#include "Precompiled.hpp"
#include "Font.hpp"
#include "MainContext.hpp"

namespace
{
    // Log error messages.
    #define LogLoadError(filename) "Failed to load a font from \"" << filename << "\" file! "

    // Size of atlas texture.
    const int AtlasWidth = 512;
    const int AtlasHeight = 512;

    // Spacing between characters on the atlas (to avoid filtering artifacts).
    const int AtlasGlyphSpacing = 1;

    // Signed distance field calculation parameters.
    const int BaseFontSize = 64;

    const int DistanceFieldDownscale = 8;
    const int DistanceFieldSpread = 2;

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
    if(FT_Set_Pixel_Sizes(m_fontFace, 0, BaseFontSize * DistanceFieldDownscale) != 0)
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

    // Checks if glyph bitmap pixel is inside the glyph.
    auto IsPixelInsideGlyph = [&](int x, int y) -> bool
    {
        uint8_t pixel = GetGlyphBitmapPixel(x, y);
        return pixel == 1;
    };

    // Calculates a square length of a vector.
    auto SquaredLength = [](float x, float y) -> float
    {
		return x * x + y * y;
    };

    //
    const float d1 = 1.0f;
    const float d2 = std::sqrt(2.0f);
    const float ds = 16.0f;

    //
    float* fieldBitmapDistances = new float[glyphBitmapWidth * glyphBitmapHeight];
    SCOPE_GUARD(delete[] fieldBitmapDistances);

    auto GetFieldBitmapDistance = [&](int x, int y) -> float
    {
        if(x < 0 || x > glyphBitmapWidth - 1)
            return std::numeric_limits<float>::max();

        if(y < 0 || y > glyphBitmapHeight - 1)
            return std::numeric_limits<float>::max();

        return fieldBitmapDistances[y * glyphBitmapWidth + x];
    };

    //
    glm::ivec2* fieldBitmapBorders = new glm::ivec2[glyphBitmapWidth * glyphBitmapHeight];
    SCOPE_GUARD(delete[] fieldBitmapBorders);

    auto GetFieldBitmapBorder = [&](int x, int y) -> glm::ivec2
    {
        if(x < 0 || x > glyphBitmapWidth - 1)
            return glm::ivec2(-1, -1);

        if(y < 0 || y > glyphBitmapHeight - 1)
            return glm::ivec2(-1, -1);

        return fieldBitmapBorders[y * glyphBitmapWidth + x];
    };

    //
    for(int y = 0; y < glyphBitmapHeight; ++y)
    for(int x = 0; x < glyphBitmapWidth; ++x)
    {
        unsigned index = y * glyphBitmapWidth + x;

        fieldBitmapDistances[index] = std::numeric_limits<float>::max();
        fieldBitmapBorders[index] = glm::ivec2(-1, -1);
    }

    //
    for(int y = 0; y < glyphBitmapHeight; ++y)
    for(int x = 0; x < glyphBitmapWidth; ++x)
    {
        unsigned index = y * glyphBitmapWidth + x;
        uint8_t glyphPixel = GetGlyphBitmapPixel(x, y);

        bool isAtBorder =
            GetGlyphBitmapPixel(x - 1, y) != glyphPixel ||
            GetGlyphBitmapPixel(x + 1, y) != glyphPixel ||
            GetGlyphBitmapPixel(x, y - 1) != glyphPixel ||
            GetGlyphBitmapPixel(x, y + 1) != glyphPixel;

        if(isAtBorder && GetGlyphBitmapPixel(x, y) != 0)
        {
            fieldBitmapDistances[index] = 0;
            fieldBitmapBorders[index] = glm::ivec2(x, y);
        }
    }

    //
    for(int y = 0; y < glyphBitmapHeight; ++y)
    for(int x = 0; x < glyphBitmapWidth; ++x)
    {
        unsigned index = y * glyphBitmapWidth + x;

        if(GetFieldBitmapDistance(x - 1, y - 1) + d2 < GetFieldBitmapDistance(x, y))
        {
            glm::ivec2 border = GetFieldBitmapBorder(x - 1, y - 1);

            fieldBitmapBorders[index] = border;
            fieldBitmapDistances[index] = glm::distance(glm::vec2(x, y), glm::vec2((float)border.x, (float)border.y));
        }

        if(GetFieldBitmapDistance(x, y - 1) + d1 < GetFieldBitmapDistance(x, y))
        {
            glm::ivec2 border = GetFieldBitmapBorder(x, y - 1);

            fieldBitmapBorders[index] = border;
            fieldBitmapDistances[index] = glm::distance(glm::vec2(x, y), glm::vec2((float)border.x, (float)border.y));
        }

        if(GetFieldBitmapDistance(x + 1, y - 1) + d2 < GetFieldBitmapDistance(x, y))
        {
            glm::ivec2 border = GetFieldBitmapBorder(x + 1, y - 1);

            fieldBitmapBorders[index] = border;
            fieldBitmapDistances[index] = glm::distance(glm::vec2(x, y), glm::vec2((float)border.x, (float)border.y));
        }

        if(GetFieldBitmapDistance(x - 1, y) + d1 < GetFieldBitmapDistance(x, y))
        {
            glm::ivec2 border = GetFieldBitmapBorder(x - 1, y);

            fieldBitmapBorders[index] = border;
            fieldBitmapDistances[index] = glm::distance(glm::vec2(x, y), glm::vec2((float)border.x, (float)border.y));
        }
    }

    //
    for(int y = glyphBitmapHeight - 1; y >= 0; --y)
    for(int x = glyphBitmapWidth - 1; x >= 0; --x)
    {
        unsigned index = y * glyphBitmapWidth + x;

        if(GetFieldBitmapDistance(x + 1, y) + d1 < GetFieldBitmapDistance(x, y))
        {
            glm::ivec2 border = GetFieldBitmapBorder(x + 1, y);

            fieldBitmapBorders[index] = border;
            fieldBitmapDistances[index] = glm::distance(glm::vec2(x, y), glm::vec2((float)border.x, (float)border.y));
        }

        if(GetFieldBitmapDistance(x - 1, y + 1) + d2 < GetFieldBitmapDistance(x, y))
        {
            glm::ivec2 border = GetFieldBitmapBorder(x - 1, y + 1);

            fieldBitmapBorders[index] = border;
            fieldBitmapDistances[index] = glm::distance(glm::vec2(x, y), glm::vec2((float)border.x, (float)border.y));
        }

        if(GetFieldBitmapDistance(x, y + 1) + d1 < GetFieldBitmapDistance(x, y))
        {
            glm::ivec2 border = GetFieldBitmapBorder(x, y + 1);

            fieldBitmapBorders[index] = border;
            fieldBitmapDistances[index] = glm::distance(glm::vec2(x, y), glm::vec2((float)border.x, (float)border.y));
        }

        if(GetFieldBitmapDistance(x + 1, y + 1) + d2 < GetFieldBitmapDistance(x, y))
        {
            glm::ivec2 border = GetFieldBitmapBorder(x + 1, y + 1);

            fieldBitmapBorders[index] = border;
            fieldBitmapDistances[index] = glm::distance(glm::vec2(x, y), glm::vec2((float)border.x, (float)border.y));
        }
    }

    //
    for(int y = 0; y < glyphBitmapHeight; ++y)
    for(int x = 0; x < glyphBitmapWidth; ++x)
    {
        unsigned index = y * glyphBitmapWidth + x;

        if(GetGlyphBitmapPixel(x, y) == 0)
        {
            float& distance = fieldBitmapDistances[index];
            distance = -distance;
        }
    }

    //
    SDL_Surface* distanceSurfaceTemp = SDL_CreateRGBSurface(
        0, 
        glyphBitmapWidth, 
        glyphBitmapHeight, 
        8, 0, 0, 0, 0
    );

    if(distanceSurfaceTemp == nullptr)
        return nullptr;

    SCOPE_GUARD(SDL_FreeSurface(distanceSurfaceTemp));

    //
    SDL_LockSurface(distanceSurfaceTemp);

    uint8_t* fieldBitmapPixels = reinterpret_cast<uint8_t*>(distanceSurfaceTemp->pixels);

    for(int y = 0; y < glyphBitmapHeight; ++y)
    for(int x = 0; x < glyphBitmapWidth; ++x)
    {
        // Get the pixel pointer.
        uint8_t* fieldBitmapPixel = &fieldBitmapPixels[y * distanceSurfaceTemp->pitch + x];

        // Find distance to the nearest opposite pixel.
        float pixelDistance = GetFieldBitmapDistance(x, y);

        // Map the value spread values to 0.0 - 1.0 range.
        float alpha = 0.5f + 0.5f * (pixelDistance / ds);
        alpha = std::min(1.0f, std::max(0.0f, alpha));
        uint8_t byte = (uint8_t)(alpha * 0xFF);

        // Save the pixel value.
        *fieldBitmapPixel = byte;
    }

    SDL_UnlockSurface(distanceSurfaceTemp);

    std::string filename;
    filename += "df_";
    filename += (char)character;
    filename += ".bmp";

    SaveSurface(distanceSurfaceTemp, filename.c_str());

    // Create a distance field surface.
    SDL_Surface* distanceSurface = SDL_CreateRGBSurface(
        0, 
        glyphBitmapWidth / DistanceFieldDownscale + DistanceFieldSpread * 2, 
        glyphBitmapHeight / DistanceFieldDownscale + DistanceFieldSpread * 2, 
        8, 0, 0, 0, 0
    );

    if(distanceSurface == nullptr)
        return nullptr;

    SCOPE_GUARD(SDL_FreeSurface(distanceSurface));

    // Get distance bitmap parameters.
    int distanceBitmapWidth = distanceSurface->w;
    int distanceBitmapHeight = distanceSurface->h;
    int distanceBitmapPitch = distanceSurface->pitch;

    uint8_t* distanceBitmapPixels = reinterpret_cast<uint8_t*>(distanceSurface->pixels);

    // Find a distance to nearest opposite pixel (translated to glyph bitmap space).
    // This is a brute force approach that is very cache unfriendly.
    auto FindSignedDistance = [&](int x, int y) -> float
    {
        // Transform coordinates to glyph bitmap space.
        int gx = x * DistanceFieldDownscale + DistanceFieldDownscale / 2 - DistanceFieldSpreadScaled;
        int gy = y * DistanceFieldDownscale + DistanceFieldDownscale / 2 - DistanceFieldSpreadScaled;

        // Check if inside a glyph.
        bool insideGlyph = IsPixelInsideGlyph(gx, gy);

        // Set initial maximum distance.
        float distanceSquared = DistanceFieldSpreadScaled * DistanceFieldSpreadScaled;

        // Check neighbors from nearest to furthest.
        bool found = false;

        for(int offset = 1; offset <= DistanceFieldSpreadScaled; ++offset)
        {
            for(int ny = -offset; ny <= offset; ++ny)
            for(int nx = -offset; nx <= offset; ++nx)
            {
                // You are not your own neighbor.
                if(nx == 0 && ny == 0)
                    continue;

                // Only check at offset edges.
                if(std::abs(nx) != offset && std::abs(ny) != offset)
                    continue;

                // Get the glyph bitmap pixel.
                const uint8_t glyphBitmapPixel = GetGlyphBitmapPixel(gx + nx, gy + ny);

                // Check if it's an opposite pixel value.
                if(glyphBitmapPixel != (uint8_t)insideGlyph)
                {
                    distanceSquared = std::min(distanceSquared, SquaredLength((float)nx, (float)ny));
                    found = true;
                }
            }

            // Break if we found the nearest pixel.
            if(found) break;
        }

        // Return a signed distance.
        float distance = std::sqrtf(distanceSquared);
        return insideGlyph ? distance : distance * -1.0f;
    };

    // Calculate distance field values.
    SDL_LockSurface(distanceSurface);

    for(int y = 0; y < distanceBitmapHeight; ++y)
    for(int x = 0; x < distanceBitmapWidth; ++x)
    {
        // Get the pixel pointer.
        uint8_t* fieldBitmapPixel = &distanceBitmapPixels[y * distanceBitmapPitch + x];

        // Find distance to the nearest opposite pixel.
        float pixelDistance = FindSignedDistance(x, y);

        // Map the value spread values to 0.0 - 1.0 range.
        float alpha = 0.5f + 0.5f * (pixelDistance * DistanceFieldSpreadScaledInverted);
        alpha = std::min(1.0f, std::max(0.0f, alpha));
        uint8_t byte = (uint8_t)(alpha * 0xFF);

        // Save the pixel value.
        *fieldBitmapPixel = byte;
    }

    SDL_UnlockSurface(distanceSurface);

    // Flip surface (we will draw upside down on SDL surface).
    FlipSurface(distanceSurface);

    // Add element to the packer.
    if(!m_packer.AddElement(glm::ivec2(distanceBitmapWidth, distanceBitmapHeight)))
    {
        // Not enough space on the atlas for this glyph.
        return nullptr;
    }

    // Draw glyph surface on the atlas.
    SDL_Rect drawRect;
    drawRect.x = m_packer.GetPosition().x;
    drawRect.y = m_packer.GetPosition().y;
    drawRect.w = distanceBitmapWidth;
    drawRect.h = distanceBitmapHeight;

    SDL_BlitSurface(distanceSurface, nullptr, m_atlasSurface, &drawRect);

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
    return BaseFontSize;
}

float Font::GetScaling(float size) const
{
    static const float Inverse = 1.0f / BaseFontSize;
    return size * Inverse;
}
