local Bit = require("bit")

Game = {}

Key = {}
Key["A"] = 4
Key["B"] = 5
Key["C"] = 6
Key["D"] = 7
Key["E"] = 8
Key["F"] = 9
Key["G"] = 10
Key["H"] = 11
Key["I"] = 12
Key["J"] = 13
Key["K"] = 14
Key["L"] = 15
Key["M"] = 16
Key["N"] = 17
Key["O"] = 18
Key["P"] = 19
Key["Q"] = 20
Key["R"] = 21
Key["S"] = 22
Key["T"] = 23
Key["U"] = 24
Key["V"] = 25
Key["W"] = 26
Key["X"] = 27
Key["Y"] = 28
Key["Z"] = 29
Key["1"] = 30
Key["2"] = 31
Key["3"] = 32
Key["4"] = 33
Key["5"] = 34
Key["6"] = 35
Key["7"] = 36
Key["8"] = 37
Key["9"] = 38
Key["0"] = 39
Key["Return"] = 40
Key["Escape"] = 41
Key["Backspace"] = 42
Key["Tab"] = 43
Key["Space"] = 44

CollisionType = {}
CollisionType.None = 0
CollisionType.Player = Bit.lshift(1, 0)
CollisionType.Enemy = Bit.lshift(1, 1)
CollisionType.Projectile = Bit.lshift(1, 2)
CollisionType.Pickup = Bit.lshift(1, 3)
CollisionType.Environment = Bit.lshift(1, 4)

Script = {}

function Script.PlayerScript()
    -- Create class instance.
    local instance = {}
    
    -- Class members.
    local m_shootTime = 0.0
    
    -- Class methods.
    function instance.OnUpdate(self, timeDelta)
        -- Get entity components.
        local transform = ComponentSystem:LookupTransform(self)
        local collision = ComponentSystem:LookupCollision(self)
        local input = ComponentSystem:LookupInput(self)
    
        -- Update shoot time delay.
        m_shootTime = math.max(0.0, m_shootTime - timeDelta)
        
        -- Shoot projectiles.
        if InputSystem:IsKeyDown(Key["Space"]) then
            if m_shootTime == 0.0 then
                -- TODO: Spawn a projectile.
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
        
            -- Calculate and set new position.
            local position = transform:GetPosition()
            position.x = position.x + movement.x * 400.0 * timeDelta
            position.y = position.y + movement.y * 400.0 * timeDelta
            transform:SetPosition(position)
        end
    end
    
    function instance.OnDamaged(self, value, alive)
        if not alive then
            EntitySystem:DestroyEntity(self)
        end
    end
    
    -- Return instance.
    return instance
end

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

function Game.Initialize()
    -- Print log message.
    Log("Calling Game.Initialize() function....")
    
    -- Create player entity.
    local entity = EntitySystem:CreateEntity()
    
    IdentitySystem:SetEntityName(entity, "Player")
    
    local transform = ComponentSystem:CreateTransform(entity)
    transform:SetPosition(Vec2(50.0, 275.0))
    transform:SetScale(Vec2(50.0, 50.0))
    transform:SetRotation(0.0)
    
    local input = ComponentSystem:CreateInput(entity)
    input:SetStateReference(InputSystem)
    
    local health = ComponentSystem:CreateHealth(entity)
    health:SetMaximumHealth(100)
    health:SetCurrentHealth(100)
    
    local collision = ComponentSystem:CreateCollision(entity)
    collision:SetBoundingBox(Vec4(-25.0, -25.0, 25.0, 25.0))
    collision:SetType(CollisionType.Player)
    collision:SetMask(CollisionType.Enemy)
    
    local script = ComponentSystem:CreateScript(entity)
    script:AddScript(Script.PlayerScript())
    script:AddScript(Script.DamageOnCollision(10, 0.2))
    script:AddScript(Script.FlashOnDamage())
    
    local render = ComponentSystem:CreateRender(entity)
    render:SetDiffuseColor(Vec4(0.0, 1.0, 0.0, 1.0))
end

Log("Main scripting entry has been loaded.")
