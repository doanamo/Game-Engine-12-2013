#include "Precompiled.hpp"
#include "Texture.hpp"

namespace
{
	// Log error messages.
	#define LogInitializeError() "Failed to initialize a texture! "
	
	// Invalid types.
	const GLuint InvalidHandle = 0;
}

Texture::Texture() :
	m_handle(InvalidHandle),
	m_width(0),
	m_height(0)
{
}

Texture::~Texture()
{
	Cleanup();
}

bool Texture::Initialize(int width, int height, GLenum format, const void* data)
{
	Cleanup();

	// Validate arguments.
	if(width <= 0)
	{
		Log() << LogInitializeError() << "Invalid argument - \"width\" is invalid.";
		return false;
	}

	if(height <= 0)
	{
		Log() << LogInitializeError() << "Invalid argument - \"height\" is invalid.";
		return false;
	}

	m_width = width;
	m_height = height;

	// Create a texture handle.
	glGenTextures(1, &m_handle);

	if(m_handle == InvalidHandle)
	{
		Log() << LogInitializeError() << "Couldn't create a texture.";
		Cleanup();
		return false;
	}

	// Bind the texture.
	glBindTexture(GL_TEXTURE_2D, m_handle);

	// Set default sampling parameters.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Set packing aligment for provided data.
	/*
	if(format == GL_RED)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}
	*/

	// Allocated a texture surface on the hardware.
	glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);

	// Unbind the texture.
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

void Texture::Cleanup()
{
	glDeleteTextures(1, &m_handle);
	m_handle = InvalidHandle;

	m_width = 0;
	m_height = 0;
}
