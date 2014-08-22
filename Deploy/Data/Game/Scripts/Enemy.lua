Scripts = Scripts or {}

local Enemy = {}
Enemy.__index = Enemy
Scripts.Enemy = Enemy

function Enemy:New()
    local self = {}
    self.shootTime = 0.0
    
    return setmetatable(self, Enemy)
end

function Enemy:OnUpdate(entitySelf, timeDelta)
    -- Update shoot time delay.
    self.shootTime = math.max(0.0, self.shootTime - timeDelta)
    
    -- Shoot projectile.
    if self.shootTime == 0.0 then
        -- Get transform component.
        local transform = ComponentSystem:LookupTransform(entitySelf)
    
        -- Create a projectile entity.
        local mask = bit.bor(CollisionTypes.Player, CollisionTypes.Environment)
        Factory.CreateProjectile(transform:GetPosition(), Vec2(-400.0, 0.0), 10, mask)
        
        -- Set shoot delay.
        self.shootTime = 1.0 + math.random() * 2.0;
    end
end

function Enemy:OnDamaged(entitySelf, value, alive)
    if not alive then
        if math.random() <= 0.1 then
            -- Get transform component.
            local transform = ComponentSystem:LookupTransform(entitySelf)
            
            -- Create health pickup.
            Factory.CreateHealthPickup(transform:GetPosition(), 20)
        end
    end
end

setmetatable(Enemy, { __call = Enemy.New })
