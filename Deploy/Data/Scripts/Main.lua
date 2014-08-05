local Bit = require("bit")

GameState = {}

CollisionTypes = {}
CollisionTypes.None = 0
CollisionTypes.Player = Bit.lshift(1, 0)
CollisionTypes.Enemy = Bit.lshift(1, 1)
CollisionTypes.Projectile = Bit.lshift(1, 2)
CollisionTypes.Pickup = Bit.lshift(1, 3)
CollisionTypes.Environment = Bit.lshift(1, 4)

Scripts = {}
Scripts.DestroyOnDeath = {}

function Scripts.DestroyOnDeath.OnDamaged(self, value, alive)
    if not alive then
        EntitySystem:DestroyEntity(self)
    end
end

function GameState.Initialize()
    -- Print log message.
    Log("Calling GameState.Initialize() function....")
    
    -- Create player entity.
    local entity = EntitySystem:CreateEntity()
    
    IdentitySystem:SetEntityName(entity, "Test")
    
    local transform = ComponentSystem:CreateTransform(entity)
    transform:SetPosition(Vec2(50.0, 275.0))
    transform:SetScale(Vec2(50.0, 50.0))
    transform:SetRotation(0.0)
    
    local input = ComponentSystem:CreateInput(entity)
    input:SetStateReference(InputSystem)
    
    local health = ComponentSystem:CreateHealth(entity)
    health:SetMaximumHealth(100)
    health:SetCurrentHealth(100)
    
    local collision = ComponentSystem:CreateCollision(entity)
    collision:SetBoundingBox(Vec4(-25.0, -25.0, 25.0, 25.0))
    collision:SetType(CollisionTypes.Player)
    collision:SetMask(CollisionTypes.Enemy)
    
    local script = ComponentSystem:CreateScript(entity)
    script:AddScript(Scripts.DestroyOnDeath);
    
    --[[ Not functional yet!
    script:AddScript(Scripts.PlayerScript())
    script:AddScript(Scripts.DamageOnCollision(10, 0.2))
    script:AddScript(Scripts.FlashOnDamageScript())
    --]]
    
    local render = ComponentSystem:CreateRender(entity)
    render:SetDiffuseColor(Vec4(1.0, 0.0, 1.0, 1.0))
end

Log("Main scripting entry has been loaded.")
