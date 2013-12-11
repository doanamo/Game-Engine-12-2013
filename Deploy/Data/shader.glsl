#version 330

#if defined(VERTEX_SHADER)
	layout(location = 0) in vec3 vertexPosition;

	void main()
	{
		gl_Position = vec4(vertexPosition, 1.0f);
	}
#endif

#if defined(FRAGMENT_SHADER)
	out vec4 fragmentColor;

	void main()
	{
		fragmentColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
#endif
