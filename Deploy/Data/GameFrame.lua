require("Keyboard")
require("Collision")
require("Factory")

require("LoseFrame")

GameFrame = {}
GameFrame.__index = GameFrame

function GameFrame:New()
    local self = {}
    self.gameState = Game.GameState()
    self.isPlayerDead = false
    self.deathTimer = 0.0
    setmetatable(self, GameFrame)
    
    -- Initialize the game state.
    if self.gameState:Initialize() == false then
        self.gameState = nil
        System.Quit()
    end
    
    -- Create entity bounds.
    Factory.CreateBounds()
    
    -- Create player entity.
    Factory.CreatePlayer()
    
    -- Setup spawn system.
    local spawnArea = Vec4(1024.0 + 100.0, 50.0, 1024.0 + 100.0, 576.0 - 50.0)
    SpawnSystem:AddSpawn(Factory.CreateEnemy, spawnArea, 0.5, 1.0)
    
    return self
end

function GameFrame:Process(event)
    if event.type == Event.KeyDown then
        if event.key.scancode == Key.Escape then
            -- Prevent from quitting to the main menu if player is dead.
            if not self.isPlayerDead then
                Main.stateMachine:Change(Main.menuFrame)
            end
            
            return false
        end
    end

    return self.gameState:Process(event)
end

function GameFrame:Update(timeDelta)
    self.gameState:Update(timeDelta)
    
    -- Handle player death.
    if not self.isPlayerDead then
        -- Check if player entity still exists.
        local entity = IdentitySystem:GetEntityByName("Player")
        
        if not EntitySystem:IsHandleValid(entity) then
            self.isPlayerDead = true
        end
    else
        -- Update the death timer.
        self.deathTimer = self.deathTimer + timeDelta
        
        -- Change to lose frame.
        if self.deathTimer > 4.0 then
            -- Destroy the game frame.
            Main.gameFrame = nil
            
            -- Create and set the lose frame.
            Main.loseFrame = LoseFrame()
            Main.stateMachine:Change(Main.loseFrame)
        end
    end
end

function GameFrame:Draw()
    self.gameState:Draw()
end

setmetatable(GameFrame, { __call = GameFrame.New })
