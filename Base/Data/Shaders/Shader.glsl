#version 330

#if defined(VERTEX_SHADER)
    layout(location = 0) in vec3 vertexPosition;
    layout(location = 1) in vec3 vertexColor;

    out vec4 fragmentColor;

    void main()
    {
        gl_Position = vec4(vertexPosition, 1.0f);
        fragmentColor = vec4(vertexColor, 1.0f);
    }
#endif

#if defined(FRAGMENT_SHADER)
    in vec4 fragmentColor;

    out vec4 outputColor;

    void main()
    {
        outputColor = fragmentColor;
    }
#endif
