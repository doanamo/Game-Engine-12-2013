MenuFrame = {}
MenuFrame.__index = MenuFrame

local ViewWidth = 1024
local ViewHeight = 576

local TitleFontSize = 176
local ButtonFontSize = 48

function MenuFrame:New()
    local self = {}
    self.screenSpace = ScreenSpace()
    self.interfaceRoot = Interface.Root()
    self.buttonContinue = Interface.Button()
    self.buttonNewGame = Interface.Button()
    self.buttonOptions = Interface.Button()
    self.buttonCredits = Interface.Button()
    self.buttonQuit = Interface.Button()
    
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
