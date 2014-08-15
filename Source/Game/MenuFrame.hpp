#pragma once

#include "Precompiled.hpp"

#include "System/BaseFrame.hpp"
#include "Graphics/ScreenSpace.hpp"
#include "Interface/Root.hpp"
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
    void ButtonContinue(const Interface::Button::EventAction& event);
    void ButtonNewGame(const Interface::Button::EventAction& event);
    void ButtonQuit(const Interface::Button::EventAction& event);

private:
    void OnEnter();

private:
    bool m_initialized;

    // Menu screen space.
    ScreenSpace m_screenSpace;

    // Interface elements.
    Interface::Root m_interfaceRoot;

    Interface::Button m_buttonContinue;
    Interface::Button m_buttonNewGame;
    Interface::Button m_buttonOptions;
    Interface::Button m_buttonCredits;
    Interface::Button m_buttonQuit;

    // Button event receivers.
    Receiver<Interface::Button::EventAction> m_actionContinue;
    Receiver<Interface::Button::EventAction> m_actionNewGame;
    Receiver<Interface::Button::EventAction> m_actionQuit;
};
