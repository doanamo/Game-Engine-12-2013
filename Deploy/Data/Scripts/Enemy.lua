Scripts = Scripts or {}

function Scripts.Enemy()
    local script = {}
    local m_shootTime = 0
    
    function script.OnUpdate(self, timeDelta)
        -- Update shoot time delay.
        m_shootTime = math.max(0.0, m_shootTime - timeDelta)
        
        -- Shoot projectile.
        if m_shootTime == 0.0 then
            -- Get transform component.
            local transform = ComponentSystem:LookupTransform(self)
        
            -- Create a projectile entity.
            local mask = bit.bor(CollisionTypes.Player, CollisionTypes.Environment)
            Factory.CreateProjectile(transform:GetPosition(), Vec2(-400.0, 0.0), 10, mask)
            
            -- Set shoot delay.
            m_shootTime = 1.0 + math.random() * 2.0;
        end
    end
    
    function script.OnDamaged(self, value, alive)
        if not alive then
            if math.random() <= 0.1 then
                -- Get transform component.
                local transform = ComponentSystem:LookupTransform(self)
                
                -- Create health pickup.
                Factory.CreateHealthPickup(transform:GetPosition(), 20)
            end
        end
    end
    
    return script
end
