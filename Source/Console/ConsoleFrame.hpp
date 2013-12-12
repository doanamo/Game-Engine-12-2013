#pragma once

#include "Precompiled.hpp"

#include "Graphics/Shader.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexInput.hpp"

//
// Console Frame
//

class ConsoleFrame
{
public:
	ConsoleFrame();
	~ConsoleFrame();

	bool Initialize();
	void Cleanup();

	void Open();
	void Close();
	void Toggle();

	void Draw();

private:
	bool m_valid;
	bool m_open;

	Shader			m_shader;
	VertexBuffer	m_vertexBuffer;
	VertexInput		m_vertexInput;
};
