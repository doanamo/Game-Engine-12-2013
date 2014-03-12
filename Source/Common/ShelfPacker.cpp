#include "Precompiled.hpp"
#include "ShelfPacker.hpp"

ShelfPacker::ShelfPacker() :
    m_storeSize(0, 0),
    m_storeSpacing(0, 0),
    m_shelfPosition(0, 0),
    m_shelfElement(0, 0),
    m_shelfHeight(0)
{
}

ShelfPacker::~ShelfPacker()
{
    Cleanup();
}

void ShelfPacker::Create(glm::ivec2 size, glm::ivec2 spacing)
{
    Cleanup();

    m_storeSize = size;
    m_storeSpacing = spacing;
    m_shelfPosition = spacing;
}

void ShelfPacker::Cleanup()
{
    m_storeSize = glm::ivec2(0, 0);
    m_storeSpacing = glm::ivec2(0, 0);
    m_shelfPosition = glm::ivec2(0, 0);
    m_shelfElement = glm::ivec2(0, 0);
    m_shelfHeight = 0;
}

bool ShelfPacker::AddElement(glm::ivec2 size)
{
    // Add previous shelf element.
    if(m_shelfElement != glm::ivec2(0, 0))
    {
        m_shelfPosition.x += m_shelfElement.x + m_storeSpacing.x;
        m_shelfHeight = std::max(m_shelfHeight, m_shelfElement.y);
        m_shelfElement = glm::ivec2(0, 0);
    }

    // Check space on the current shelf.
    if(m_shelfPosition.x + size.x + m_storeSpacing.x > m_storeSize.x)
    {
        // Move to next shelf.
        m_shelfPosition.x = m_storeSpacing.x;
        m_shelfPosition.y = m_shelfPosition.y + m_shelfHeight + m_storeSpacing.y;
        m_shelfHeight = 0;
    }
    
    if(m_shelfPosition.y + size.y + m_storeSpacing.y > m_storeSize.y)
    {
        // Not enough space on the atlas for this glyph.
        return false;
    }
    
    // Set the current shelf element.
    m_shelfElement = size;

    return true;
}

const glm::ivec2& ShelfPacker::GetPosition()
{
    return m_shelfPosition;
}
