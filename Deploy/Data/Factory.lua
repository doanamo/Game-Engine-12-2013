require("Scripts.Common")
require("Scripts.Player")
require("Scripts.Projectile")

Factory = Factory or {}

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
    collision:SetType(CollisionTypes.Player)
    collision:SetMask(CollisionTypes.Enemy)
    
    local script = ComponentSystem:CreateScript(entity)
    script:AddScript(Scripts.Player())
    script:AddScript(Scripts.DamageOnCollision(10, 0.2))
    script:AddScript(Scripts.FlashOnDamage())
    
    local render = ComponentSystem:CreateRender(entity)
    render:SetDiffuseColor(Vec4(0.0, 1.0, 0.0, 1.0))
end

function Factory.CreateBounds()
    -- Create projectile bounds.
    do
        local entity = EntitySystem:CreateEntity()
    
        local transform = ComponentSystem:CreateTransform(entity)
        transform:SetPosition(Vec2(0.0, 0.0))
        
        local collision = ComponentSystem:CreateCollision(entity)
        collision:SetBoundingBox(Vec4(0.0, 0.0, 1024.0, 576.0))
        collision:SetType(CollisionTypes.None)
        collision:SetMask(CollisionTypes.Projectile)
        collision:SetFlags(bit.bor(CollisionFlags.Default, CollisionFlags.Reversed))
        
        local script = ComponentSystem:CreateScript(entity)
        script:AddScript(Scripts.DestroyOnCollision())
    end

    -- Create entity bounds.
    do
        local entity = EntitySystem:CreateEntity()
    
        local transform = ComponentSystem:CreateTransform(entity)
        transform:SetPosition(Vec2(0.0, 0.0))
        
        local collision = ComponentSystem:CreateCollision(entity)
        collision:SetBoundingBox(Vec4(0.0, 0.0, 1024.0 + 200.0, 576.0))
        collision:SetType(CollisionTypes.None)
        collision:SetMask(bit.bxor(CollisionTypes.All, CollisionTypes.Player))
        collision:SetFlags(bit.bor(CollisionFlags.Default, CollisionFlags.Reversed))
        
        local script = ComponentSystem:CreateScript(entity)
        script:AddScript(Scripts.DestroyOnCollision())
    end
end

function Factory.CreateProjectile(position, velocity, damage, mask)
    local entity = EntitySystem:CreateEntity()
    
    local transform = ComponentSystem:CreateTransform(entity)
    transform:SetPosition(position)
    transform:SetScale(Vec2(30.0, 30.0))
    transform:SetRotation(0.0)
    
    local collision = ComponentSystem:CreateCollision(entity)
    collision:SetBoundingBox(Vec4(-15.0, -15.0, 15.0, 15.0))
    collision:SetType(CollisionTypes.Projectile)
    collision:SetMask(mask)
    
    local script = ComponentSystem:CreateScript(entity)
    script:AddScript(Scripts.Projectile(damage))
    script:AddScript(Scripts.ConstantVelocity(velocity))
    
    local render = ComponentSystem:CreateRender(entity)
    render:SetDiffuseColor(Vec4(1.0, 1.0, 0.0, 1.0))
end
