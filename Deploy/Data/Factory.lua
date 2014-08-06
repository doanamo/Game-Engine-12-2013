require("Scripts.Common")
require("Scripts.Player")

Factory = {}

function Factory.CreatePlayer()
    local entity = EntitySystem:CreateEntity()
    
    IdentitySystem:SetEntityName(entity, "Player")
    
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
    collision:SetType(CollisionType.Player)
    collision:SetMask(CollisionType.Enemy)
    
    local script = ComponentSystem:CreateScript(entity)
    script:AddScript(Script.Player())
    script:AddScript(Script.DamageOnCollision(10, 0.2))
    script:AddScript(Script.FlashOnDamage())
    
    local render = ComponentSystem:CreateRender(entity)
    render:SetDiffuseColor(Vec4(0.0, 1.0, 0.0, 1.0))
end