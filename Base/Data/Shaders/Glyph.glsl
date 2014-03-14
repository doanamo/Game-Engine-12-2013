#version 330

#if defined(VERTEX_SHADER)
    uniform mat4 vertexTransform;
    uniform vec2 texturePixelSize;

    layout(location = 0) in vec2 vertexPosition;
    layout(location = 1) in vec2 instancePosition;
    layout(location = 2) in vec2 instanceSize;
    layout(location = 3) in vec2 instanceScale;
    layout(location = 4) in vec2 instanceTexture;
    layout(location = 5) in vec4 instanceColor;

    out vec2 fragmentTexture;
    out vec4 fragmentColor;

    void main()
    {
        // Calculate vertex position.
        vec2 position = vertexPosition * instanceSize * instanceScale + instancePosition;

        // Transform vertex position.
        gl_Position = vertexTransform * vec4(position, 0.0f, 1.0f);

        // Calculate texture coordinates (vertex position is the same as vertex texture for a quad).
        fragmentTexture = (vertexPosition * instanceSize + instanceTexture) * texturePixelSize;
        
        // Set glyph color.
        fragmentColor = instanceColor;
    }
#endif

#if defined(FRAGMENT_SHADER)
    uniform sampler2D texture;

    in vec2 fragmentTexture;
    in vec4 fragmentColor;

    out vec4 outputColor;

    void main()
    {
        ///*
        // Use distance field to calculate glyph alpha value.
        const float smoothing = 1.0f / 4.0f;
        const float edge = 0.5f;

        float distance = texture2D(texture, fragmentTexture).r;
        float alpha = smoothstep(edge - smoothing, edge + smoothing, distance);

        // Output a final color.
        outputColor = vec4(1.0f, 1.0f, 1.0f, alpha) * fragmentColor;
        //*/

        //outputColor = vec4(1.0f, 1.0f, 1.0f, texture2D(texture, fragmentTexture).r) * fragmentColor;
    }
#endif
