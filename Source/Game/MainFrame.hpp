#pragma once

#include "Precompiled.hpp"

#include "System/BaseFrame.hpp"

//
// Main Frame
//

class MainFrame : public BaseFrame
{
public:
    MainFrame();
    ~MainFrame();

    bool Initialize();
    void Cleanup();

    bool Process(const SDL_Event& event);
    void Update(float timeDelta);
    void Draw();

private:

};
