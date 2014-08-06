Scripts = Scripts or {}

function Scripts.Projectile(damage)
    local script = {}
    m_damage = damage or 10
    
    function script.OnCollision(self, other)
        -- Apply damage to other entity.
        HealthSystem:Damage(other.entity, m_damage)
        
        -- Destroy projectile.
        EntitySystem:DestroyEntity(self.entity)
    end
    
    return script
end
