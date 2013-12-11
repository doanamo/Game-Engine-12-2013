#include "Utility.hpp"
#include <fstream>

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
