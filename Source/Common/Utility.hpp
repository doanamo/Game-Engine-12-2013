#pragma once

#include "Precompiled.hpp"

// Gets the size of a static array.
template<typename Type, size_t Size>
size_t StaticArraySize(const Type(&)[Size])
{
    return Size;
}

// Clamps a value between lower and upper range.
template<typename Type>
Type Clamp(const Type& value, const Type& lower, const Type& upper)
{
    return std::max(lower, std::min(value, upper));
}

// Frees STL container memory (for std::vector, std::list, etc.).
template<typename Type>
void ClearContainer(Type& container)
{
    Type empty;
    container.swap(empty);
}

// Upper multiple of a number.
int UpperMultiple(int value, int multiplier);

// Lower multiple of a number.
int LowerMultiple(int value, int multiplier);

// Gets the content of a text file.
std::string GetTextFileContent(std::string filename);

// Gets the content of a binary file.
std::vector<char> GetBinaryFileContent(std::string filename);

// Calculates the CRC of a file.
bool CalculateFileCRC(std::string filename, boost::crc_32_type* result);

// Flips SDL surface upside down.
void FlipSurface(SDL_Surface* surface);

// Saves SDL surface to a file.
void SaveSurface(SDL_Surface* surface, std::string filename);
