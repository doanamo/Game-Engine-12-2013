Script = Script or {}

function Script.DamageOnCollision(damage, interval)
    -- Create class instance.
    local instance = {}
    
    -- Class members.
    local m_damage = damage
    local m_interval = interval
    
    -- Class methods.
    function instance.OnCollision(self, other)
        -- Apply damage to other entity.
        HealthSystem:Damage(other.entity, m_damage)
        
        -- Disable collision pair for a period of time.
        CollisionSystem:DisableCollisionResponse(self.entity, other.entity, m_interval)
    end
    
    -- Return instance.
    return instance
end

function Script.FlashOnDamage()
    -- Create class instance.
    local instance = {}
    
    -- Class members.
    local m_timer = 0.0
    
    -- Class methods.
    function instance.OnUpdate(self, timeDelta)
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
    
    function instance.OnDamaged(self, other)
        -- Reset blink time.
        m_timer = 0.8
    end
    
    -- Return instance.
    return instance
end
