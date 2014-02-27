#include "Precompiled.hpp"
#include "CoreRenderer.hpp"

CoreRenderer::CoreRenderer() :
    m_initialized(false)
{
}

CoreRenderer::~CoreRenderer()
{
    Cleanup();
}

bool CoreRenderer::Initialize()
{
    Cleanup();

    m_initialized = true;

    return true;
}

void CoreRenderer::Cleanup()
{
    m_initialized = false;
}

void CoreRenderer::Clear(uint32_t flags)
{
    if(!m_initialized)
        return;

    // Get the clear mask.
    GLbitfield mask = 0;

    if(flags & ClearFlags::Color) mask |= GL_COLOR_BUFFER_BIT;
    if(flags & ClearFlags::Depth) mask |= GL_DEPTH_BUFFER_BIT;

    // Clear the buffer.
    glClear(mask);
}

void CoreRenderer::SetClearColor(const glm::vec4& color)
{
    if(!m_initialized)
        return;

    glClearColor(color.r, color.g, color.b, color.a);
}

void CoreRenderer::SetClearDepth(float depth)
{
    if(!m_initialized)
        return;

    glClearDepth(depth);
}
