LoseFrame = {}
LoseFrame.__index = LoseFrame

local ViewWidth = 1024
local ViewHeight = 576

local FrameCooldown = 1.0

function LoseFrame:New()
    local self = {}
    self.screenSpace = Graphics.ScreenSpace()
    self.timer = 0.0
    setmetatable(self, LoseFrame)
    
    return self
end

function LoseFrame:Process(event)
    if event.type == Event.KeyDown then
        if self.timer > FrameCooldown then
            -- Change state to menu frame.
            Main.loseFrame = nil
            Main.stateMachine:Change(Main.menuFrame)
        end
        
        return true
    end
    
    return false
end

function LoseFrame:Update(timeDelta)
    -- Update the frame transition cooldown.
    self.timer = self.timer + timeDelta
end

function LoseFrame:Draw()
     -- Setup screen space.
    local windowWidth = Console.GetInteger("r_width")
    local windowHeight = Console.GetInteger("r_height")
    
    self.screenSpace:SetSourceSize(windowWidth, windowHeight)
    self.screenSpace:SetTargetSize(ViewWidth, ViewHeight)
    
    -- Clear the screen buffer.
    CoreRenderer:SetClearColor(Vec4(1.0, 1.0, 1.0, 1.0))
    CoreRenderer:SetClearDepth(1.0)
    CoreRenderer:Clear(bit.bor(ClearFlags.Color, ClearFlags.Depth))
    
    -- Draw the game over text.
    do
        local info = Graphics.TextDrawInfo()
        info.font = Graphics.GetDefaultFont()
        info.size = 128
        info.align = TextDrawAlign.Centered
        info.bodyColor = Vec4(1.0, 1.0, 1.0, 1.0)
        info.outlineColor = Vec4(0.0, 0.0, 0.0, 1.0)
        info.outlineRange = Vec2(0.45, 0.55)
        info.position = Vec2(ViewWidth * 0.5, ViewHeight * 0.7)
        
        TextRenderer:Draw(info, "Game Over", self.screenSpace:GetTransform())
    end
    
    do
        local info = Graphics.TextDrawInfo()
        info.font = Graphics.GetDefaultFont()
        info.size = 48
        info.align = TextDrawAlign.Centered
        info.bodyColor = Vec4(0.0, 0.0, 0.0, 1.0)
        info.position = Vec2(ViewWidth * 0.5, ViewHeight * 0.5)
        
        TextRenderer:Draw(info, "Press any key to continue...", self.screenSpace:GetTransform())
    end
end

setmetatable(LoseFrame, { __call = LoseFrame.New })
