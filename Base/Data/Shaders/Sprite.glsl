#version 330

#if defined(VERTEX_SHADER)
    uniform mat4 viewTransform;

    layout(location = 0) in vec2  vertexPosition;
    layout(location = 1) in vec2  vertexTexture;
    layout(location = 2) in mat4  instanceTransform;
    layout(location = 6) in vec4  instanceDiffuseColor;
    layout(location = 7) in vec3  instanceEmissionColor;
    layout(location = 8) in float instanceEmissionPower;

    out vec2  fragmentTexture;
    out vec4  fragmentDiffuseColor;
    out vec3  fragmentEmissionColor;
    out float fragmentEmissionPower;

    void main()
    {
        gl_Position = viewTransform * instanceTransform * vec4(vertexPosition, 0.0f, 1.0f);

        fragmentTexture = vertexTexture;
        fragmentDiffuseColor = instanceDiffuseColor;
        fragmentEmissionColor = instanceEmissionColor;
        fragmentEmissionPower = instanceEmissionPower;
    }
#endif

#if defined(FRAGMENT_SHADER)
    uniform sampler2D texture;
    
    in vec2  fragmentTexture;
    in vec4  fragmentDiffuseColor;
    in vec3  fragmentEmissionColor;
    in float fragmentEmissionPower;
    
    out vec4 outputColor;

    void main()
    {
        float diffusePower = 1.0f - clamp(fragmentEmissionPower, 0.0f, 1.0f);
        
        // Calculate diffuse color.
        outputColor = texture2D(texture, fragmentTexture) * fragmentDiffuseColor * diffusePower;
        
        // Apply emission.
        outputColor += vec4(fragmentEmissionColor, 1.0f) * fragmentEmissionPower;
    }
#endif
