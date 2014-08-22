Scripts = Scripts or {}

local Player = {}
Player.__index = Player
Scripts.Player = Player

function Player:New()
    local self = {}
    self.shootTime = 0.0
    
    return setmetatable(self, Player)
end

function Player:OnUpdate(entitySelf, timeDelta)
    -- Get entity components.
    local transform = ComponentSystem:LookupTransform(entitySelf)
    local collision = ComponentSystem:LookupCollision(entitySelf)
    local input = ComponentSystem:LookupInput(entitySelf)

    -- Update shoot time delay.
    self.shootTime = math.max(0.0, self.shootTime - timeDelta)
    
    -- Shoot projectiles.
    if InputSystem:IsKeyDown(Key["Space"]) then
        if self.shootTime == 0.0 then
            local mask = bit.bor(CollisionTypes.Enemy, CollisionTypes.Environment)
            Factory.CreateProjectile(transform:GetPosition(), Vec2(700.0, 0.0), 10, mask)

            self.shootTime = 0.25
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

setmetatable(Player, { __call = Player.New })
