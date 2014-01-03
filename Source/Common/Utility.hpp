#pragma once

#include "Precompiled.hpp"

// Gets the size of a static array.
template<typename Type, size_t Size>
size_t StaticArraySize(const Type(&)[Size])
{
    return Size;
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

// Gest the content of a binary file.
std::vector<char> GetBinaryFileContent(std::string filename);

// Flips SDL surface upside down.
void FlipSurface(SDL_Surface* surface);

// Saves SDL surface to a file.
void SaveSurface(SDL_Surface* surface, std::string filename);
