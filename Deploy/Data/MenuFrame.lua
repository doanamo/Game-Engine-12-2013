MenuFrame = {}
MenuFrame.__index = MenuFrame

local ViewWidth = 1024
local ViewHeight = 576

local TitleFontSize = 176
local ButtonFontSize = 48

function MenuFrame:New()
    local self = {}
    self.screenSpace = Graphics.ScreenSpace()
    self.interfaceRoot = Interface.Root()
    self.buttonContinue = Interface.Button()
    self.buttonNewGame = Interface.Button()
    self.buttonOptions = Interface.Button()
    self.buttonCredits = Interface.Button()
    self.buttonQuit = Interface.Button()
    
    -- Button starting position.
    local buttonPosition = Vec2(175.0, ViewHeight - 260.0)
    
    -- Intermediate structures.
    local buttonTextInfo = Graphics.TextDrawInfo()
    local buttonTextMetrics = Graphics.TextDrawMetrics()
    local buttonSize = Vec2(0.0, 0.0)
    
    -- Set shared properties.
    buttonTextInfo.font = Graphics.GetDefaultFont()
    buttonTextInfo.align = TextDrawAlign.BottomLeft
    buttonTextInfo.size = ButtonFontSize
    return setmetatable(self, MenuFrame)
end

function MenuFrame:Process(event)
    return false
end

function MenuFrame:Update(timeDelta)
end

function MenuFrame:Draw()
    -- Clear the screen buffer.
    CoreRenderer:SetClearColor(Vec4(1.0, 1.0, 1.0, 1.0))
    CoreRenderer:SetClearDepth(1.0)
    CoreRenderer:Clear(bit.bor(ClearFlags.Color, ClearFlags.Depth))
end

setmetatable(MenuFrame, { __call = MenuFrame.New })
