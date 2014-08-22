require("Keyboard")
require("Collision")
require("Factory")

GameFrame = {}
GameFrame.__index = GameFrame

function GameFrame:New()
    local self = {}
    self.gameState = GameState()
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
    self.gameState:Process(event)
end

function GameFrame:Update(timeDelta)
    self.gameState:Update(timeDelta)
end

function GameFrame:Draw()
    self.gameState:Draw()
end

setmetatable(GameFrame, { __call = GameFrame.New })
