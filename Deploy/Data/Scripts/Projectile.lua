Scripts = Scripts or {}

local Projectile = {}
Projectile.__index = Projectile
Scripts.Projectile = Projectile

function Projectile:New(damage)
    local self = {}
    self.damage = damage or 1

    return setmetatable(self, Projectile)
end

function Projectile:OnCollision(collisionSelf, collisionOther)
    -- Apply damage to other entity.
    HealthSystem:Damage(collisionOther.entity, self.damage)
    
    -- Destroy projectile.
    EntitySystem:DestroyEntity(collisionSelf.entity)
end

setmetatable(Projectile, { __call = Projectile.New })
