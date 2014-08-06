Scripts = Scripts or {}

function Scripts.ConstantVelocity(velocity)
    local script = {}
    m_velocity = velocity
    
    function script.OnUpdate(self, timeDelta)
        -- Get transform component.
        local transform = ComponentSystem:LookupTransform(self)
        
        -- Update position.
        local position = transform:GetPosition()
        position.x = position.x + m_velocity.x * timeDelta
        position.y = position.y + m_velocity.y * timeDelta
        transform:SetPosition(position)
    end
    
    return script
end

function Scripts.DestroyOnDeath()
    local script = {}
    
    function script.OnDamaged(self, value, alive)
        if not alive then
            EntitySystem:DestroyEntity(self)
        end
    end
    
    return script
end

function Scripts.DestroyOnCollision()
    local script = {}
    
    function script.OnCollision(self, other)
        EntitySystem:DestroyEntity(other.entity)
    end
    
    return script
end

function Scripts.DamageOnCollision(damage, interval)
    local script = {}
    local m_damage = damage
    local m_interval = interval
    
    function script.OnCollision(self, other)
        -- Apply damage to other entity.
        HealthSystem:Damage(other.entity, m_damage)
        
        -- Disable collision pair for a period of time.
        CollisionSystem:DisableCollisionResponse(self.entity, other.entity, m_interval)
    end

    return script
end

function Scripts.FlashOnDamage()
    local script = {}
    local m_timer = 0.0
    
    function script.OnUpdate(self, timeDelta)
        -- Don't do anything if not needed.
        if m_timer == 0.0 then
            return
        end
        
        -- Update render component.
        local render = ComponentSystem:LookupRender(self)
        render:SetEmissionColor(Vec3(1.0, 1.0, 1.0))
        render:SetEmissionPower(m_timer)
        
        -- Update blink time.
        m_timer = math.max(0.0, m_timer - timeDelta)
    end
    
    function script.OnDamaged(self, other)
        -- Reset blink time.
        m_timer = 0.8
    end

    return script
end
