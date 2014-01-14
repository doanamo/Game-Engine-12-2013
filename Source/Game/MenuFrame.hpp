#pragma once

#include "Precompiled.hpp"
#include "Graphics/Font.hpp"
#include "BaseFrame.hpp"

//
// Menu Frame
//

class MenuFrame : public BaseFrame
{
public:
    MenuFrame();
    ~MenuFrame();

    bool Initialize();
    void Cleanup();

    bool Process(const SDL_Event& event);
    void Update(float dt);
    void Draw(const glm::mat4& transform);

public:
    // Menu elements.
    struct MenuElements
    {
        enum Type
        {
            Continue,
            NewGame,
            Options,
            Credits,
            Quit,
    
            Count,
            None,
        };
    };

private:
    // Menu graphics.
    Font m_fontTitle;
    Font m_fontElement;

    // Menu elements.
    struct ElementData
    {
        bool        enabled;
        const char* text;
        glm::vec2   position;
        glm::vec4   boundingBox;
    };

    typedef std::array<ElementData, MenuElements::Count> ElementArray;

    ElementArray m_elements;
    int          m_elementSelected;

    bool m_initialized;
};
