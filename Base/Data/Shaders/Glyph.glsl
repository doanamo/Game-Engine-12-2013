#version 330

#if defined(VERTEX_SHADER)
    uniform mat4 vertexTransform;

    layout(location = 0) in vec2 vertexPosition;
    layout(location = 1) in vec2 vertexTexture;
    layout(location = 2) in vec4 vertexColor;

    out vec2 fragmentTexture;
    out vec4 fragmentColor;

    void main()
    {
        gl_Position = vertexTransform * vec4(vertexPosition, 0.0f, 1.0f);
        fragmentTexture = vertexTexture;
        fragmentColor = vertexColor;
    }
#endif

#if defined(FRAGMENT_SHADER)
    uniform sampler2D texture;

    in vec2 fragmentTexture;
    in vec4 fragmentColor;

    out vec4 outputColor;

    void main()
    {
        outputColor = vec4(1.0f, 1.0f, 1.0f, texture2D(texture, fragmentTexture)) * fragmentColor;
    }
#endif
