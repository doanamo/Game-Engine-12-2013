#pragma once

#include "Precompiled.hpp"

//
// Vertex Input
//

// Forward declarations.
class VertexBuffer;

// Vertex attribute types.
enum class VertexAttributeTypes
{
	Invalid,

	Float1,
	Float2,
	Float3,
	Float4,

	Count,
};

// Vertex attribute structure.
struct VertexAttribute
{
	int location;
	VertexBuffer* buffer;
	VertexAttributeTypes type;
};

// Vertex input class.
class VertexInput
{
public:
	VertexInput();
	~VertexInput();

	// Initializes the vertex input.
	bool Initialize(VertexAttribute* attributes, unsigned int count);

	// Restores class instance to it's original state.
	void Cleanup();

	// Gets the vertex array object handle.
	GLuint GetHandle() const
	{
		return m_handle;
	}

private:
	// Vertex array object.
	GLuint m_handle;
};
