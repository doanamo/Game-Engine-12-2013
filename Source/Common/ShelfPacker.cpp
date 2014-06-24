#include "Precompiled.hpp"
#include "ShelfPacker.hpp"

namespace
{
    // Log error messages.
    #define LogSerializationError() "Failed to read an object from a stream! "

    // Class unique identifier.
    const boost::uuids::uuid ClassIdentifier = boost::uuids::string_generator()("db604c60-fb9f-11e3-a3ac-0800200c9a66");

    // Class version.
    unsigned int ClassVersion = 1;

}

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

void ShelfPacker::Serialize(std::ofstream& stream)
{
    // Write class info.
    StreamWrite(stream, ClassIdentifier);
    StreamWrite(stream, ClassVersion);

    // Write class members.
    StreamWrite(stream, m_storeSize);
    StreamWrite(stream, m_storeSpacing);
    StreamWrite(stream, m_shelfPosition);
    StreamWrite(stream, m_shelfElement);
    StreamWrite(stream, m_shelfHeight);
}

void ShelfPacker::Deserialize(std::ifstream& stream)
{
    if(!stream.good())
        return;

    // Read class info.
    boost::uuids::uuid identifier;
    unsigned int version;

    StreamRead(stream, &identifier);
    StreamRead(stream, &version);

    // Verify class info.
    if(identifier != ClassIdentifier)
    {
        Log() << LogSerializationError() << "Invalid class identifier.";
        stream.setstate(std::ios::badbit);
        return;
    }

    if(version != ClassVersion)
    {
        Log() << LogSerializationError() << "Invalid class version.";
        stream.setstate(std::ios::badbit);
        return;
    }

    // Read class members.
    StreamRead(stream, &m_storeSize);
    StreamRead(stream, &m_storeSpacing);
    StreamRead(stream, &m_shelfPosition);
    StreamRead(stream, &m_shelfElement);
    StreamRead(stream, &m_shelfHeight);

    // Check stream state.
    if(!stream.good())
    {
        Log() << LogSerializationError() << "There was an error while reading from the stream.";
        Cleanup();
    }
}
