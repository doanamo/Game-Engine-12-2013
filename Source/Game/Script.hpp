#pragma once

#include "Precompiled.hpp"
#include "Component.hpp"

// Forward declarations.
class Entity;

//
// Script Component
//

class Script : public Component
{
public:
    // Friend declarations.
    friend class ScriptSystem;

    // Type declarations.
    typedef void (*FunctionPtr)(Entity*, float);

public:
    Script();
    ~Script();

    void SetFunction(FunctionPtr function);

private:
    // Pointer to a script function.
    FunctionPtr m_function;
};
