#pragma once

#include "Precompiled.hpp"

//
// Clear Flags
//

struct ClearFlags
{
    enum Type
    {
        None = 0,
        Color = 1 << 0,
        Depth = 1 << 1,
    };
};

//
// Core Renderer
//

class CoreRenderer
{
public:
    CoreRenderer();
    ~CoreRenderer();

    bool Initialize();
    void Cleanup();

    void Clear(uint32_t flags);

    void SetClearColor(const glm::vec4& color);
    void SetClearDepth(float depth);

private:
    bool m_initialized;
};
