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
    uniform sampler2D fontTexture;

    in vec2 fragmentTexture;
    in vec4 fragmentColor;

    out vec4 outputColor;

    void main()
    {
        // Set initial foreground glyph color.
        outputColor = fragmentColor;

        // Get distance from the glyph's edge.
        float distance = texture2D(fontTexture, fragmentTexture).r;

        // Calculate edge smooth step.
        float step = clamp(16.0f * (abs(dFdx(fragmentTexture.x)) + abs(dFdy(fragmentTexture.y))), 0.0f, 0.5f);

        // Apply smoothed glyph edge alpha.
        outputColor.a *= smoothstep(0.5f - step, 0.5f + step, distance);

        // Create an outline.
        float size = 0.1f;
        float distanceMin = 0.5f - size;
        float distanceMax = 0.5f + size;

        if(distance >= (distanceMin - step) && distance <= (distanceMax + step))
        {
            float outline = 1.0f;

            if(distance <= 0.5f)
            {
                // Outside glyph.
                outline = smoothstep(distanceMin - step, distanceMin + step, distance);
                outputColor = vec4(1.0f, 0.0f, 0.0f, outline);
            }
            else
            {
                // Inside glyph.
                outline = smoothstep(distanceMax + step, distanceMax - step, distance);
                outputColor = mix(outputColor, vec4(1.0f, 0.0f, 0.0f, 1.0f), outline);
            }
        }

        // Gamma correction.
        outputColor.a = pow(outputColor.a, 1.0f / 2.2f);
    }
#endif
