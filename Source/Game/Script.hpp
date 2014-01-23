#pragma once

#include "Precompiled.hpp"
#include "Component.hpp"
#include "EntityHandle.hpp"

//
// Script Object
//

class ScriptObject
{
public:
    virtual void Execute(EntityHandle entity, float timeDelta) = 0;
};

//
// Script Component
//

class Script : public Component
{
public:
    // Friend declarations.
    friend class ScriptSystem;

    // Type declarations.
    typedef std::shared_ptr<ScriptObject> ScriptPtr;

public:
    Script();
    ~Script();

    void SetScript(ScriptPtr script)
    {
        m_script = script;
    }

    ScriptPtr GetScript()
    {
        return m_script;
    }
    
private:
    // Script object.
    ScriptPtr m_script;
};
