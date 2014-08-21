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
