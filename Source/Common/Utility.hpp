#pragma once

#include <string>
#include <vector>

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

// Gets the content of a text file.
std::string GetTextFileContent(std::string filename);

// Gest the content of a binary file.
std::vector<char> GetBinaryFileContent(std::string filename);
