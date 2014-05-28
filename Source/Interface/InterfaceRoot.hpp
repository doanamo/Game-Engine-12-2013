#pragma once

#include "Precompiled.hpp"
#include "InterfaceElement.hpp"

// Forward declarations.
class ScreenSpace;

//
// Interface Root
//

class InterfaceRoot : public InterfaceElement
{
public:
    InterfaceRoot();
    ~InterfaceRoot();

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
