#version 330

#if defined(VERTEX_SHADER)
    uniform mat4 vertexTransform;
    uniform vec2 texturePixelSize;

    layout(location = 0) in vec2 vertexPosition;
    layout(location = 1) in vec2 instancePosition;
    layout(location = 2) in vec2 instanceSize;
    layout(location = 3) in vec2 instanceScale;
    layout(location = 4) in vec2 instanceTexture;

    layout(location = 5) in vec4 instanceBodyColor;
    layout(location = 6) in vec4 instanceOutlineColor;
    layout(location = 7) in vec4 instanceGlowColor;

    layout(location = 8) in vec2 instanceOutlineRange;
    layout(location = 9) in vec2 instanceGlowRange;

    out vec2 fragmentTexture;

    out vec4 fragmentBodyColor;
    out vec4 fragmentOutlineColor;
    out vec4 fragmentGlowColor;

    out vec2 fragmentOutlineRange;
    out vec2 fragmentGlowRange;

    void main()
    {
        // Calculate vertex position.
        vec2 position = vertexPosition * instanceSize * instanceScale + instancePosition;

        // Transform vertex position.
        gl_Position = vertexTransform * vec4(position, 0.0f, 1.0f);

        // Calculate texture coordinates (vertex position is the same as vertex texture for a quad).
        fragmentTexture = (vertexPosition * instanceSize + instanceTexture) * texturePixelSize;

        // Set glyph parameters.
        fragmentBodyColor = instanceBodyColor;
        fragmentOutlineColor = instanceOutlineColor;
        fragmentGlowColor = instanceGlowColor;

        fragmentOutlineRange = instanceOutlineRange;
        fragmentGlowRange = instanceGlowRange;
    }
#endif

#if defined(FRAGMENT_SHADER)
    uniform sampler2D fontTexture;

    in vec2 fragmentTexture;

    in vec4 fragmentBodyColor;
    in vec4 fragmentOutlineColor;
    in vec4 fragmentGlowColor;

    in vec2 fragmentOutlineRange;
    in vec2 fragmentGlowRange;

    out vec4 outputColor;

    void main()
    {
        // Get glyph parameters.
        const float glyphThreshold = 0.5f;

        float outlineOuter = fragmentOutlineRange.x;
        float outlineInner = fragmentOutlineRange.y;

        float glowEnd = fragmentGlowRange.x;
        float glowBegin = fragmentGlowRange.y;

        // Get distance from the glyph's edge.
        float distance = texture2D(fontTexture, fragmentTexture).r;

        // Calculate edge smooth step.
        float step = clamp(32.0f * (abs(dFdx(fragmentTexture.x)) + abs(dFdy(fragmentTexture.y))), 0.0f, outlineOuter);

        // Set initial color.
        if(fragmentGlowColor.a > 0.0f)
        {
            outputColor = vec4(fragmentGlowColor.rgb, 0.0f);
        }
        else if(fragmentOutlineColor.a > 0.0f)
        {
            outputColor = vec4(fragmentOutlineColor.rgb, 0.0f);
        }
        else
        {
            outputColor = vec4(fragmentBodyColor.rgb, 0.0f);
        }

        // Create glyph glow.
        if(fragmentGlowColor.a > 0.0f)
        {
            vec2 glowOffset = vec2(0.0f, 0.0f);
            float glowDistance = texture2D(fontTexture, fragmentTexture + glowOffset).r;
            float glowAlpha = smoothstep(glowEnd, glowBegin, glowDistance);

            outputColor = mix(outputColor, fragmentGlowColor, glowAlpha);

            // Inverted gamma correction (it doesn't work well with this effect).
            outputColor.a = pow(outputColor.a, 2.2f);
        }

        // Create glyph body.
        if(fragmentBodyColor.a > 0.0f)
        {
            float glyphAlpha = smoothstep(glyphThreshold - step, glyphThreshold + step, distance);
            
            outputColor = mix(outputColor, fragmentBodyColor, glyphAlpha);
        }

        // Create glyph outline.
        float outlineMin = outlineOuter - step;
        float outlineMax = outlineInner + step;

        if(fragmentOutlineColor.a > 0.0f && outlineMin <= distance && distance <= outlineMax)
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

            outputColor = mix(outputColor, fragmentOutlineColor, outlineAlpha);
        }

        // Gamma correction.
        outputColor.a = pow(outputColor.a, 1.0f / 2.2f);

        // Premultiply alpha.
        outputColor.rgb *= outputColor.a;
    }
#endif
