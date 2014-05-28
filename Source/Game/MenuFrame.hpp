#pragma once

#include "Precompiled.hpp"

#include "System/BaseFrame.hpp"
#include "Graphics/ScreenSpace.hpp"
#include "Interface/InterfaceRoot.hpp"
#include "Interface/InterfaceElement.hpp"
#include "Interface/Button.hpp"

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
    void Update(float timeDelta);
    void Draw();

private:
    // Button event handlers.
    void ButtonContinue(const Button::EventAction& event);
    void ButtonNewGame(const Button::EventAction& event);
    void ButtonQuit(const Button::EventAction& event);

private:
    void OnEnter();

private:
    bool m_initialized;

    // Menu screen space.
    ScreenSpace m_screenSpace;

    // Interface elements.
    InterfaceRoot m_interfaceRoot;

    Button m_buttonContinue;
    Button m_buttonNewGame;
    Button m_buttonOptions;
    Button m_buttonCredits;
    Button m_buttonQuit;

    // Button event receivers.
    Receiver<Button::EventAction> m_actionContinue;
    Receiver<Button::EventAction> m_actionNewGame;
    Receiver<Button::EventAction> m_actionQuit;
};
