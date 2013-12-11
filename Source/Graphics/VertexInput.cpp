#include "Precompiled.hpp"
#include "VertexInput.hpp"
#include "Buffer.hpp"

namespace
{
	// Log error messages.
	#define LogInitializeError() "Failed to initialize a vertex input! "

	// Gets the vertex attribute type size.
	GLint GetVertexAttributeTypeSize(VertexAttributeTypes type)
	{
		switch(type)
		{
			case VertexAttributeTypes::Float1:
				return 1;

			case VertexAttributeTypes::Float2:
				return 2;

			case VertexAttributeTypes::Float3:
				return 3;

			case VertexAttributeTypes::Float4:
				return 4;
		}

		return 0;
	}

	// Gets the vertex attribute type enum.
	GLenum GetVertexAttributeTypeEnum(VertexAttributeTypes type)
	{
		switch(type)
		{
			case VertexAttributeTypes::Float1:
			case VertexAttributeTypes::Float2:
			case VertexAttributeTypes::Float3:
			case VertexAttributeTypes::Float4:
				return GL_FLOAT;
		}

		return GL_INVALID_ENUM;
	}

	// Gets the vertex attribute type offset.
	GLint GetVertexAttributeTypeOffset(VertexAttributeTypes type)
	{
		switch(type)
		{
			case VertexAttributeTypes::Float1:
				return sizeof(float) * 1;

			case VertexAttributeTypes::Float2:
				return sizeof(float) * 2;

			case VertexAttributeTypes::Float3:
				return sizeof(float) * 3;

			case VertexAttributeTypes::Float4:
				return sizeof(float) * 4;
		}

		return 0;
	}
}

VertexInput::VertexInput() :
	m_handle(0)
{
}

VertexInput::~VertexInput()
{
	Cleanup();
}

bool VertexInput::Initialize(VertexAttribute* attributes, unsigned int count)
{
	Cleanup();

	// Validate arguments.
	if(attributes == nullptr)
	{
		Log() << LogInitializeError() << "Invalid argument - \"attributes\" is null.";
		return false;
	}

	if(count == 0)
	{
		Log() << LogInitializeError() << "Invalid argument - \"count\" is 0.";
		return false;
	}

	for(unsigned int i = 0; i < count; ++i)
	{
		VertexAttribute& attribute = attributes[i];

		if(attribute.location < 0)
		{
			Log() << LogInitializeError() << "Invalid argument - \"attribute[" << i << "].location\" is invalid.";
			return false;
		}

		if(attribute.buffer == nullptr)
		{
			Log() << LogInitializeError() << "Invalid argument - \"attribute[" << i << "].buffer\" is null.";
			return false;
		}

		if(!attribute.buffer->IsValid())
		{
			Log() << LogInitializeError() << "Invalid argument - \"attribute[" << i << "].buffer\" is invalid.";
			return false;
		}

		if(attribute.type == VertexAttributeTypes::Invalid)
		{
			Log() << LogInitializeError() << "Invalid argument - \"attribute[" << i << "].type\" is invalid.";
			return false;
		}
	}

	// Create a vertex array object.
	glGenVertexArrays(1, &m_handle);

	if(m_handle == 0)
	{
		Log() << LogInitializeError() << "Couldn't create a vertex array object.";
		Cleanup();
		return false;
	}

	// Bind the vertex array.
	glBindVertexArray(m_handle);

	// Set the vertex array state.
	VertexBuffer* currentBuffer = nullptr;
	unsigned int currentOffset = 0;

	for(unsigned int i = 0; i < count; ++i)
	{
		VertexAttribute& attribute = attributes[i];

		// Bind the vertex buffer.
		if(currentBuffer != attribute.buffer)
		{
			glBindBuffer(GL_ARRAY_BUFFER, attribute.buffer->GetHandle());

			currentBuffer = attribute.buffer;
			currentOffset = 0;
		}

		// Enable vertex attribute.
		glEnableVertexAttribArray(attribute.location);

		// Set vertex attribute pointer.
		glVertexAttribPointer(
			attribute.location,
			GetVertexAttributeTypeSize(attribute.type), 
			GetVertexAttributeTypeEnum(attribute.type),
			GL_FALSE,
			attribute.buffer->GetElementSize(),
			(void*)currentOffset
		);

		// Increment current offset.
		currentOffset += GetVertexAttributeTypeOffset(attribute.type);
	}

	// Restore default state.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

void VertexInput::Cleanup()
{
	glDeleteVertexArrays(1, &m_handle);
	m_handle = 0;
}
