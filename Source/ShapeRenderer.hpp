#pragma once

#include "Precompiled.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexInput.hpp"

//
// Shape Renderer
//

class ShapeRenderer
{
private:
	// Vertex structure.
	struct Vertex
	{
		glm::vec2 position;
		glm::vec2 texture;
		glm::vec4 color;
	};

public:
	struct Line
	{
		glm::vec2 begin;
		glm::vec2 end;
		glm::vec4 color;
	};

public:
	ShapeRenderer();
	~ShapeRenderer();

	bool Initialize(int bufferSize);
	void Cleanup();

	void DrawLines(const Line* data, int count, const glm::mat4& transform);
	void DrawBox();
	void DrawQuad();
	void DrawCircle();
	void DrawSphere();

private:
	Vertex*	m_bufferData;
	int		m_bufferSize;

	Shader			m_shader;
	VertexBuffer	m_vertexBuffer;
	VertexInput		m_vertexInput;

	bool m_initialized;
};
