Scripts = Scripts or {}

-- Constant velocity script.
local ConstantVelocity = {}
ConstantVelocity.__index = ConstantVelocity
Scripts.ConstantVelocity = ConstantVelocity

function ConstantVelocity:New(velocity)
    local self = {}
    self.velocity = velocity
    
    return setmetatable(self, ConstantVelocity)
end

function ConstantVelocity:OnUpdate(entitySelf, timeDelta)
    -- Get transform component.
    local transform = ComponentSystem:LookupTransform(entitySelf)
    
    -- Update position.
    local position = transform:GetPosition()
    position.x = position.x + self.velocity.x * timeDelta
    position.y = position.y + self.velocity.y * timeDelta
    transform:SetPosition(position)
end

setmetatable(ConstantVelocity, { __call = ConstantVelocity.New })

-- Destroy on death script.
local DestroyOnDeath = {}
DestroyOnDeath.__index = DestroyOnDeath
Scripts.DestroyOnDeath = DestroyOnDeath

function DestroyOnDeath:New()
    local self = {}

    return setmetatable(self, DestroyOnDeath)
end

function DestroyOnDeath:OnDamaged(entitySelf, value, alive)
    if not alive then
        EntitySystem:DestroyEntity(entitySelf)
    end
end

setmetatable(DestroyOnDeath, { __call = DestroyOnDeath.New })

-- Destroy on collision script.
local DestroyOnCollision = {}
DestroyOnCollision.__index = DestroyOnCollision
Scripts.DestroyOnCollision = DestroyOnCollision

function DestroyOnCollision:New()
    local self = {}

    return setmetatable(self, DestroyOnCollision)
end

function DestroyOnCollision:OnCollision(collisionSelf, collisionOther)
    EntitySystem:DestroyEntity(collisionOther.entity)
end

setmetatable(DestroyOnCollision, { __call = DestroyOnCollision.New })

-- Damage on collision script.
local DamageOnCollision = {}
DamageOnCollision.__index = DamageOnCollision
Scripts.DamageOnCollision = DamageOnCollision

function DamageOnCollision:New(damage, interval)
    local self = {}
    self.damage = damage
    self.interval = interval

    return setmetatable(self, DamageOnCollision)
end

function DamageOnCollision:OnCollision(collisionSelf, collisionOther)
    -- Apply damage to other entity.
    HealthSystem:Damage(collisionOther.entity, self.damage)
    
    -- Disable collision pair for a period of time.
    CollisionSystem:DisableCollisionResponse(collisionSelf.entity, collisionOther.entity, self.interval)
end

setmetatable(DamageOnCollision, { __call = DamageOnCollision.New })

-- Flash on damage script.
local FlashOnDamage = {}
FlashOnDamage.__index = FlashOnDamage
Scripts.FlashOnDamage = FlashOnDamage

function FlashOnDamage:New()
    local self = {}
    self.timer = 0.0

    return setmetatable(self, FlashOnDamage)
end

function FlashOnDamage:OnUpdate(entitySelf, timeDelta)
    -- Don't do anything if not needed.
    if self.timer == 0.0 then
        return
    end
    
    -- Update render component.
    local render = ComponentSystem:LookupRender(entitySelf)
    render:SetEmissionColor(Vec3(1.0, 1.0, 1.0))
    render:SetEmissionPower(self.timer)
    
    -- Update blink time.
    self.timer = math.max(0.0, self.timer - timeDelta)
end

function FlashOnDamage:OnDamaged(entitySelf, value, alive)
    -- Reset blink time.
    self.timer = 0.8
end

setmetatable(FlashOnDamage, { __call = FlashOnDamage.New })
