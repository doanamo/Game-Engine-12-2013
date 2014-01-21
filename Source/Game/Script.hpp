#pragma once

#include "Precompiled.hpp"
#include "Component.hpp"

// Forward declarations.
class Entity;

//
// Script Object
//

class ScriptObject
{
public:
    virtual void Execute(Entity* entity, float timeDelta) = 0;
};

//
// Script Component
//

class Script : public Component
{
public:
    // Friend declarations.
    friend class ScriptSystem;

public:
    Script();
    ~Script();

    template<typename Type>
    void SetScript(std::unique_ptr<Type>& object);

private:
    // Script object.
    std::unique_ptr<ScriptObject> m_object;
};

template<typename Type>
void Script::SetScript(std::unique_ptr<Type>& object)
{
    m_object = std::move(object);
}
