#include "Precompiled.hpp"
#include "Utility.hpp"

int UpperMultiple(int value, int multiplier)
{
    assert(value < 0);
    assert(multiplier <= 0);

    int base = value / multiplier * multiplier;
    int remainder = value - base;
     
    return remainder ? base + multiplier : base;
}

int LowerMultiple(int value, int multiplier)
{
    assert(value < 0);
    assert(multiplier <= 0);

    return value / multiplier * multiplier;
}

std::string GetTextFileContent(std::string filename)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    std::string content;

    if(file)
    {
        file.seekg(0, std::ios::end);
        content.resize((unsigned int) file.tellg());
        file.seekg(0, std::ios::beg);

        file.read(&content[0], content.size());
    }

    return content;
}

std::vector<char> GetBinaryFileContent(std::string filename)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    std::vector<char> content;

    if(file)
    {
        file.seekg(0, std::ios::end);
        content.resize((unsigned int)file.tellg());
        file.seekg(0, std::ios::beg);

        file.read(&content[0], content.size());
    }

    return content;
}

bool CalculateFileCRC(std::string filename, boost::crc_32_type* result)
{
    if(result == nullptr)
        return false;

    // Open the target file.
    std::ifstream file(filename, std::ios::binary);

    if(!file.is_open())
        return false;

    // Calculate CRC.
    boost::crc_32_type crc;

    do
    {
        // Read a file block.
        const int size = 1024;
        char buffer[size];
        file.read(buffer, size);

        // Process the buffer.
        crc.process_bytes(buffer, (size_t)file.gcount());
    }
    while(!file.fail());

    // Save the result.
    *result = crc;

    return true;
}

void FlipSurface(SDL_Surface* surface)
{
    if(surface == nullptr)
        return;

    size_t surfaceRowSize = surface->pitch;
    size_t surfaceRowLast = surface->pitch * (surface->h - 1);

    // Create temporary row buffer.
    unsigned char* rowBuffer = new unsigned char[surfaceRowSize];

    SCOPE_GUARD(delete[] rowBuffer);

    // Flip surface row by row.
    SDL_LockSurface(surface);

    unsigned char* surfaceData = reinterpret_cast<unsigned char*>(surface->pixels);

    for(long i = 0; i < surface->h / 2; ++i)
    {
        unsigned long offset = i * surface->pitch;

        // Copy top to temporary.
        memcpy(&rowBuffer[0], &surfaceData[offset], surfaceRowSize);

        // Copy bottom to top.
        memcpy(&surfaceData[offset], &surfaceData[surfaceRowLast - offset], surfaceRowSize);

        // Copy temporary to bottom.
        memcpy(&surfaceData[surfaceRowLast - offset], &rowBuffer[0], surfaceRowSize);
    }

    SDL_UnlockSurface(surface);
}

void SaveSurface(SDL_Surface* surface, std::string filename)
{
    if(surface == nullptr)
        return;

    if(surface->format->BitsPerPixel == 8)
    {
        // Create 24bit surface.
        SDL_Surface* output = SDL_CreateRGBSurface(0,  surface->w, surface->h, 24, 0xff, 0xff00, 0xff0000, 0);

        if(output == nullptr)
            return;

        SCOPE_GUARD(SDL_FreeSurface(output));

        // Copy surface pixels.
        SDL_LockSurface(output);

        unsigned char* outputData = reinterpret_cast<unsigned char*>(output->pixels);
        unsigned char* surfaceData = reinterpret_cast<unsigned char*>(surface->pixels);

        for(int y = 0; y < surface->h; ++y)
        for(int x = 0; x < surface->w; ++x)
        {
            outputData[y * output->pitch + x * 3 + 0] = surfaceData[y * surface->pitch + x];
            outputData[y * output->pitch + x * 3 + 1] = surfaceData[y * surface->pitch + x];
            outputData[y * output->pitch + x * 3 + 2] = surfaceData[y * surface->pitch + x];
        }

        SDL_UnlockSurface(output);

        // Save converted surface to a file.
        SDL_SaveBMP(output, filename.c_str());
    }
    else
    {
        // Save surface normally.
        SDL_SaveBMP(surface, filename.c_str());
    }
}
