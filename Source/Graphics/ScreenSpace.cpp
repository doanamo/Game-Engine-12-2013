#include "Precompiled.hpp"
#include "ScreenSpace.hpp"

ScreenSpace::ScreenSpace() :
    m_rebuild(true),
    m_sourceSize(4.0f, 4.0f),
    m_targetSize(4.0f, 4.0f),
    m_rectangle(0.0f, 0.0f, 0.0f, 0.0f),
    m_offset(0.0f, 0.0f),
    m_projection(1.0f),
    m_view(1.0f),
    m_transform(1.0f)
{
}

ScreenSpace::~ScreenSpace()
{
}

void ScreenSpace::Cleanup()
{
    m_rebuild = true;

    m_sourceSize = glm::vec2(4.0f, 4.0f);
    m_targetSize = glm::vec2(4.0f, 4.0f);
    m_rectangle = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    m_offset = glm::vec2(0.0f, 0.0f);
    m_projection = glm::mat4(1.0f);
    m_view = glm::mat4(1.0f);
    m_transform = glm::mat4(1.0f);
}

void ScreenSpace::SetSourceSize(float width, float height)
{
    // Floor and make a multiple of 2.
    int floorWidth = (int)width;
    int floorHeight = (int)height;

    if(floorWidth % 2) floorWidth += 1;
    if(floorHeight % 2) floorHeight += 1;

    m_sourceSize.x = (float)floorWidth;
    m_sourceSize.y = (float)floorHeight;

    m_rebuild = true;
}

void ScreenSpace::SetTargetSize(float width, float height)
{
    m_targetSize.x = width;
    m_targetSize.y = height;

    m_rebuild = true;
}

void ScreenSpace::SetTargetAspect(float aspect)
{
    // Set target size equal to source.
    m_targetSize = m_sourceSize;

    // Adjust target size aspect ratio.
    float sourceAspect = m_sourceSize.x / m_sourceSize.y;

    if(sourceAspect > aspect)
    {
        m_targetSize.x = m_sourceSize.x / (sourceAspect / aspect);
    }
    else
    {
        m_targetSize.y = m_sourceSize.y * (sourceAspect / aspect);
    }

    // Floor and make a multiple of 2.
    // This is done to retain pixel perfect rendering.
    int floorWidth = (int)m_targetSize.x;
    int floorHeight = (int)m_targetSize.y;

    if(floorWidth % 2) floorWidth += 1;
    if(floorHeight % 2) floorHeight += 1;

    m_targetSize.x = (float)floorWidth;
    m_targetSize.y = (float)floorHeight;

    m_rebuild = true;
}

void ScreenSpace::Rebuild() const
{
    if(m_rebuild)
    {
        // Calculate apsect ratios.
        float sourceAspectRatio = m_sourceSize.x / m_sourceSize.y;
        float targetAspectRatio = m_targetSize.x / m_targetSize.y;

        float aspectRatio = sourceAspectRatio / targetAspectRatio;

        // Calculate screen space coordinates.
        m_rectangle.x = -m_targetSize.x * 0.5f;
        m_rectangle.y =  m_targetSize.x * 0.5f;
        m_rectangle.z = -m_targetSize.y * 0.5f;
        m_rectangle.w =  m_targetSize.y * 0.5f;

        if(sourceAspectRatio > targetAspectRatio)
        {
            // Scale screen space coordinates.
            m_rectangle.x *= aspectRatio;
            m_rectangle.y *= aspectRatio;
        }
        else
        {
            // Scale screen space coordinates.
            m_rectangle.z /= aspectRatio;
            m_rectangle.w /= aspectRatio;
        }

        // Calculate screen space offset.
        m_offset.x = -m_targetSize.x * 0.5f;
        m_offset.y = -m_targetSize.y * 0.5f;

        // Calculate screen space matrices.
        m_projection = glm::ortho(m_rectangle.x, m_rectangle.y, m_rectangle.z, m_rectangle.w);
        m_view = glm::translate(glm::mat4(1.0f), glm::vec3(m_offset, 0.0f));
        m_transform = m_projection * m_view;

        m_rebuild = false;
    }
}

const glm::vec2& ScreenSpace::GetSourceSize() const
{
    return m_sourceSize;
}

const glm::vec2& ScreenSpace::GetTargetSize() const
{
    return m_targetSize;
}

const glm::vec4& ScreenSpace::GetRectangle() const
{
    Rebuild();
    return m_rectangle;
}

const glm::vec2& ScreenSpace::GetOffset() const
{
    Rebuild();
    return m_offset;
}

const glm::mat4& ScreenSpace::GetProjection() const
{
    Rebuild();
    return m_projection;
}

const glm::mat4& ScreenSpace::GetView() const
{
    Rebuild();
    return m_view;
}

const glm::mat4& ScreenSpace::GetTransform() const
{
    Rebuild();
    return m_transform;
}
