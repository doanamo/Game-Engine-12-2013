Scripts = Scripts or {}

function Scripts.Player()
    local script = {}
    local m_shootTime = 0.0
    
    function script.OnUpdate(self, timeDelta)
        -- Get entity components.
        local transform = ComponentSystem:LookupTransform(self)
        local collision = ComponentSystem:LookupCollision(self)
        local input = ComponentSystem:LookupInput(self)
    
        -- Update shoot time delay.
        m_shootTime = math.max(0.0, m_shootTime - timeDelta)
        
        -- Shoot projectiles.
        if InputSystem:IsKeyDown(Key["Space"]) then
            if m_shootTime == 0.0 then
                local mask = bit.bor(CollisionTypes.Enemy, CollisionTypes.Environment)
                Factory.CreateProjectile(transform:GetPosition(), Vec2(700.0, 0.0), 10, mask)

                m_shootTime = 0.25
            end
        end
        
        -- Handle movement.
        local movement = Vec2(0.0, 0.0)
        
        if InputSystem:IsKeyDown(Key["D"]) then
            movement.x = movement.x + 1.0
        end
        
        if InputSystem:IsKeyDown(Key["A"]) then
            movement.x = movement.x - 1.0
        end
        
        if InputSystem:IsKeyDown(Key["W"]) then
            movement.y = movement.y + 1.0
        end
        
        if InputSystem:IsKeyDown(Key["S"]) then
            movement.y = movement.y - 1.0
        end
        
        -- Update player position.
        if not movement:Equals(Vec2(0.0, 0.0)) then
            -- Normalize movement vector.
            movement:Normalize()
        
            -- Calculate new position.
            local position = transform:GetPosition()
            position.x = position.x + movement.x * 400.0 * timeDelta
            position.y = position.y + movement.y * 400.0 * timeDelta
            
            -- Prevent from moving outside of the screen bounds.
            local boundingBox = collision:GetBoundingBox()
            position.x = math.max(0.0 - boundingBox.x, math.min(position.x, 1024.0 - boundingBox.z))
            position.y = math.max(0.0 - boundingBox.y, math.min(position.y, 576.0 - boundingBox.w))
            
            -- Set new position.
            transform:SetPosition(position)
        end
    end
    
    function script.OnDamaged(self, value, alive)
        if not alive then
            EntitySystem:DestroyEntity(self)
        end
    end

    return script
end
