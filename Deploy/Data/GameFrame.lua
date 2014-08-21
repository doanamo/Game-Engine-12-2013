GameFrame = {}
GameFrame.__index = GameFrame

function GameFrame:New()
    local self = {}
    setmetatable(self, GameFrame)
    
    return self
end

function GameFrame:Draw()
    -- Clear the screen buffer.
    CoreRenderer:SetClearColor(Vec4(1.0, 1.0, 1.0, 1.0))
    CoreRenderer:SetClearDepth(1.0)
    CoreRenderer:Clear(bit.bor(ClearFlags.Color, ClearFlags.Depth))
end

setmetatable(GameFrame, { __call = GameFrame.New })
