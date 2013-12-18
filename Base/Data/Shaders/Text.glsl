#version 330

#if defined(VERTEX_SHADER)
	uniform mat4 vertexTransform;

	layout(location = 0) in vec2 vertexPosition;
	layout(location = 1) in vec2 vertexTexture;

	out vec2 fragmentTexture;

	void main()
	{
		gl_Position = vertexTransform * vec4(vertexPosition, 0.0f, 1.0f);
		fragmentTexture = vertexTexture;
	}
#endif

#if defined(FRAGMENT_SHADER)
	uniform sampler2D texture;

	in vec2 fragmentTexture;

	out vec4 outputColor;

	void main()
	{
		outputColor = texture2D(texture, fragmentTexture);
	}
#endif
