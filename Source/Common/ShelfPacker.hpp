#pragma once

#include "Precompiled.hpp"

//
// Shelf Packer
//  Packs rectangles together to save as muhc space as possible.
//  It's not the most optimal algorithm, but it's very straight forward.
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

public:
    void Serialize(std::ofstream& stream);
    void Deserialize(std::ifstream& stream);

private:
    glm::ivec2 m_storeSize;
    glm::ivec2 m_storeSpacing;
    glm::ivec2 m_shelfPosition;
    glm::ivec2 m_shelfElement;
    int        m_shelfHeight;
};
