Scripts = Scripts or {}

function Scripts.HealthPickup(heal)
    local script = {}
    local m_heal = heal or 0
    
    function script.OnCollision(self, other)
        -- Heal other entity.
        HealthSystem.Heal(other.entity, m_heal)
        
        -- Destroy health pickup.
        EntitySystem.DestroyEntity(self.entity)
    end
    
    return script
end
