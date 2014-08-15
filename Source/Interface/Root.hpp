#pragma once

#include "Precompiled.hpp"
#include "Element.hpp"

// Forward declarations.
class ScreenSpace;

//
// Interface
//

namespace Interface
{
    //
    // Root
    //

    class Root : public Element
    {
    public:
        Root();
        ~Root();

        void Cleanup();

        bool Process(const SDL_Event& event);

        void SetScreenSpace(ScreenSpace* screenSpace);

        const glm::vec2& GetCursorPosition() const;

    private:
        void CalculateCursorPosition() const;

    private:
        // Screen space that interface resides in.
        ScreenSpace* m_screenSpace;

        // Transformed cursor position.
        glm::ivec2 m_windowCursorPosition;

        mutable glm::vec2 m_interfaceCursorPosition;
        mutable bool m_rebuildCursorPosition;
    };
} // namespace Interface
