
GameState = {}

function GameState.Initialize()
    -- Print log message.
    Log("Calling GameState.Initialize() function....")
    
    -- Test math.
    local position = Vec3(1.0, 2.0, 3.0)
    
    Log("Vec3(" .. position.x .. ", " .. position.y .. ", " .. position.z .. ")")
    
    -- Create player entity.
    local entity = EntitySystem:CreateEntity()
    
    IdentitySystem:SetEntityName(entity, "Test")
    
    --[[ Not functional yet!
    local transform = ComponentSystem.CreateTransform(entity)
    transform:SetPosition(Vector2(50.0, 275.0))
    transform:SetScale(Vector2(50.0, 50.0))
    transform:SetRotation(0.0)
    
    local input = ComponentSystem.CreateInput(entity)
    input:SetStateReference(InputSystem)
    
    local health = ComponentSystem.CreateHealth(entity)
    health:SetMaximumHealth(100)
    health:SetCurrentHealth(100)
    
    local collision = ComponentSystem.CreateCollision(entity)
    collision:SetBoundingBox(Vector4(-25.0, -25.0, 25.0, 25.0))
    collision:SetType(CollisionTypes.Player)
    collision:SetMask(CollisionTypes.Enemy)
    
    local script = ComponentSystem.CreateScript(entity)
    script:AddScript(Scripts.PlayerScript())
    script:AddScript(Scripts.DamageOnCollision(10, 0.2))
    script:AddScript(Scripts.FlashOnDamageScript())
    
    local render = ComponentSystem.CreateRender(entity)
    render:SetDiffuseColor(Vector4(0.0, 1.0, 0.0, 1.0))
    --]]
    
    EntitySystem:DestroyEntity(entity)
end

Log("Main scripting entry has been loaded.")
