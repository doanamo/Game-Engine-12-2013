Scripts = Scripts or {}

local HealthPickup = {}
HealthPickup.__index = HealthPickup
Scripts.HealthPickup = HealthPickup

function HealthPickup:New(heal)
    local self = {}
    self.heal = heal or 0

    return setmetatable(self, HealthPickup)
end

function HealthPickup:OnCollision(collisionSelf, collisionOther)
    -- Heal other entity.
    HealthSystem:Heal(collisionOther.entity, self.heal)
    
    -- Destroy health pickup.
    EntitySystem:DestroyEntity(collisionSelf.entity)
end

setmetatable(HealthPickup, { __call = HealthPickup.New })
