#pragma once

#include "Precompiled.hpp"

#include "System/BaseFrame.hpp"
#include "Graphics/ScreenSpace.hpp"

//
// Menu Frame
//

class MenuFrame : public BaseFrame
{
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

public:
    MenuFrame();
    ~MenuFrame();

    bool Initialize();
    void Cleanup();

    bool Process(const SDL_Event& event);
    void Update(float dt);
    void Draw();

private:
    void OnEnter();

private:
    // Menu screen space.
    ScreenSpace m_screenSpace;

    // Render transforms.
    glm::mat4 m_projection;
    glm::mat4 m_view;
    glm::mat4 m_transform;

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
