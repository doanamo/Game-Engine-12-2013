#pragma once

#include "Precompiled.hpp"

// Forward delcarations.
class InterfaceRoot;

//
// Interface Element
//

class InterfaceElement
{
public:
    InterfaceElement(InterfaceRoot* root = nullptr);
    virtual ~InterfaceElement();

    void Cleanup();

    virtual bool Process(const SDL_Event& event);
    virtual void Update(float timeDelta);
    virtual void Draw();

    bool AddChild(InterfaceElement* element);
    bool RemoveChild(InterfaceElement* element);
    void RemoveAllChilds();

    InterfaceRoot* Root()
    {
        return m_root;
    }

    InterfaceElement* Parent()
    {
        return m_parent;
    }

    InterfaceElement* Next()
    {
        return m_next;
    }

    InterfaceElement* Begin()
    {
        return m_begin;
    }

    InterfaceElement* End()
    {
        return m_end;
    }

private:
    InterfaceElement* PreviousChild(InterfaceElement* element);

private:
    // Linked list root.
    InterfaceRoot* m_root;

    // Linked list element.
    InterfaceElement* m_parent;
    InterfaceElement* m_next;

    // Linked list of childs.
    InterfaceElement* m_begin;
    InterfaceElement* m_end;
};
