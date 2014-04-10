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
        vec4 glyphColor = fragmentColor;
        vec4 outlineColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);

        float outlineOuter = 0.5f;
        float outlineInner = 0.5f;

        // Get distance from the glyph's edge.
        float distance = texture2D(fontTexture, fragmentTexture).r;

        // Calculate edge smooth step.
        float step = clamp(32.0f * (abs(dFdx(fragmentTexture.x)) + abs(dFdy(fragmentTexture.y))), 0.0f, outlineOuter);

        // Set foreground color.
        if(outlineColor.a > 0.0f)
        {
            outputColor = vec4(outlineColor.rgb, 0.0f);
        }
        else
        {
            outputColor = vec4(glyphColor.rgb, 0.0f);
        }

        // Create glyph outline.
        float outlineMin = outlineOuter - step;
        float outlineMax = outlineInner + step;

        if(outlineColor.a > 0.0f && outlineMin <= distance && distance <= outlineMax)
        {
            float outlineAlpha = 1.0f;

            if(distance <= outlineOuter)
            {
                outlineAlpha = smoothstep(outlineOuter - step, outlineOuter + step, distance);
            }
            else if(distance >= outlineInner)
            {
                outlineAlpha = smoothstep(outlineInner + step, outlineInner - step, distance);
            }

            outputColor = mix(outputColor, outlineColor, outlineAlpha);
        }

        // Create glyph body.
        if(glyphColor.a > 0.0f)
        {
            float glyphAlpha = smoothstep(outlineInner - step, outlineInner + step, distance);
            outputColor = mix(outputColor, glyphColor, glyphAlpha);
        }

        // Gamma correction.
        outputColor.a = pow(outputColor.a, 1.0f / 2.2f);

        // Premultiply alpha.
        outputColor.rgb *= outputColor.a;
    }
#endif
