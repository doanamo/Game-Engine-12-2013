#pragma once

#include "Precompiled.hpp"

//
// Interface
//

namespace Interface
{
    // Forward delcarations.
    class Root;

    //
    // Element
    //

    class Element
    {
    public:
        Element(Root* root = nullptr);
        virtual ~Element();

        void Cleanup();

        virtual bool Process(const SDL_Event& event);
        virtual void Update(float timeDelta);
        virtual void Draw();

        bool AddChild(Element* element);
        bool RemoveChild(Element* element);
        void RemoveAllChilds();

        Root* GetRoot()
        {
            return m_root;
        }

        Element* GetParent()
        {
            return m_parent;
        }

        Element* GetNext()
        {
            return m_next;
        }

        Element* Begin()
        {
            return m_begin;
        }

        Element* End()
        {
            return m_end;
        }

    private:
        Element* PreviousChild(Element* element);

    private:
        // Linked list root.
        Root* m_root;

        // Linked list element.
        Element* m_parent;
        Element* m_next;

        // Linked list of childs.
        Element* m_begin;
        Element* m_end;
    };
} // namespace Interface
