#pragma once

#include "Precompiled.hpp"

//
// Shelf Packer
//  - Simplest implemenation that packs rectangles.
//

class ShelfPacker
{
public:
    ShelfPacker();
    ~ShelfPacker();

    void Create(glm::ivec2 size, glm::ivec2 spacing);
    void Cleanup();

    bool AddElement(glm::ivec2 size);
    const glm::ivec2& GetPosition();

private:
    glm::ivec2 m_storeSize;
    glm::ivec2 m_storeSpacing;
    glm::ivec2 m_shelfPosition;
    glm::ivec2 m_shelfElement;
    int        m_shelfHeight;
};
