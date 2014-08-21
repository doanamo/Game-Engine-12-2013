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
    setmetatable(self, MenuFrame)
    
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
    
    -- Setup continue button.
    local buttonContinueText = "Continue"
    
    buttonTextInfo.position = buttonPosition
    buttonTextMetrics = TextRenderer:Measure(buttonTextInfo, buttonContinueText)
    
    buttonSize.x = buttonTextMetrics.textArea.z - buttonTextMetrics.textArea.x
    buttonSize.y = buttonTextMetrics.textArea.w - buttonTextMetrics.textArea.y
    
    self.buttonContinue:SetEnabled(false)
    self.buttonContinue:SetText(buttonContinueText)
    self.buttonContinue:SetPosition(buttonPosition)
    self.buttonContinue:SetSize(buttonSize)
    
    buttonPosition.y = buttonPosition.y - buttonSize.y
    
    -- Setup new game button.
    local buttonNewGameText = "New Game"
    
    buttonTextInfo.position = buttonPosition
    buttonTextMetrics = TextRenderer:Measure(buttonTextInfo, buttonNewGameText)
    
    buttonSize.x = buttonTextMetrics.textArea.z - buttonTextMetrics.textArea.x
    buttonSize.y = buttonTextMetrics.textArea.w - buttonTextMetrics.textArea.y
    
    self.buttonNewGame:SetEnabled(true)
    self.buttonNewGame:SetText(buttonNewGameText)
    self.buttonNewGame:SetPosition(buttonPosition)
    self.buttonNewGame:SetSize(buttonSize)
    
    buttonPosition.y = buttonPosition.y - buttonSize.y
    
    -- Setup options button.
    local buttonOptionsText = "Options"
    
    buttonTextInfo.position = buttonPosition
    buttonTextMetrics = TextRenderer:Measure(buttonTextInfo, buttonOptionsText)
    
    buttonSize.x = buttonTextMetrics.textArea.z - buttonTextMetrics.textArea.x
    buttonSize.y = buttonTextMetrics.textArea.w - buttonTextMetrics.textArea.y
    
    self.buttonOptions:SetEnabled(false)
    self.buttonOptions:SetText(buttonOptionsText)
    self.buttonOptions:SetPosition(buttonPosition)
    self.buttonOptions:SetSize(buttonSize)
    
    buttonPosition.y = buttonPosition.y - buttonSize.y
    
    -- Setup credits button.
    local buttonCreditsText = "Credits"
    
    buttonTextInfo.position = buttonPosition
    buttonTextMetrics = TextRenderer:Measure(buttonTextInfo, buttonCreditsText)
    
    buttonSize.x = buttonTextMetrics.textArea.z - buttonTextMetrics.textArea.x
    buttonSize.y = buttonTextMetrics.textArea.w - buttonTextMetrics.textArea.y
    
    self.buttonCredits:SetEnabled(false)
    self.buttonCredits:SetText(buttonCreditsText)
    self.buttonCredits:SetPosition(buttonPosition)
    self.buttonCredits:SetSize(buttonSize)
    
    buttonPosition.y = buttonPosition.y - buttonSize.y
    
    -- Setup quit button.
    local buttonQuitText = "Quit"
    
    buttonTextInfo.position = buttonPosition
    buttonTextMetrics = TextRenderer:Measure(buttonTextInfo, buttonQuitText)
    
    buttonSize.x = buttonTextMetrics.textArea.z - buttonTextMetrics.textArea.x
    buttonSize.y = buttonTextMetrics.textArea.w - buttonTextMetrics.textArea.y
    
    self.buttonQuit:SetEnabled(true)
    self.buttonQuit:SetText(buttonQuitText)
    self.buttonQuit:SetPosition(buttonPosition)
    self.buttonQuit:SetSize(buttonSize)
    
    buttonPosition.y = buttonPosition.y - buttonSize.y
    
    -- Setup screen space.
    local windowWidth = Console.GetInteger("r_width")
    local windowHeight = Console.GetInteger("r_height")
    
    self.screenSpace:SetSourceSize(windowWidth, windowHeight)
    self.screenSpace:SetTargetSize(ViewWidth, ViewHeight)
    
    -- Setup interface.
    self.interfaceRoot:SetScreenSpace(self.screenSpace)
    self.interfaceRoot:AddChild(self.buttonContinue)
    self.interfaceRoot:AddChild(self.buttonNewGame)
    self.interfaceRoot:AddChild(self.buttonOptions)
    self.interfaceRoot:AddChild(self.buttonCredits)
    self.interfaceRoot:AddChild(self.buttonQuit)
    
    -- Bind delegates.
    self.buttonContinue:OnEventAction(MenuFrame.ButtonContinue, self)
    self.buttonNewGame:OnEventAction(MenuFrame.ButtonNewGame, self)
    self.buttonQuit:OnEventAction(MenuFrame.ButtonQuit, self)
    
    return self
end

function MenuFrame:ButtonContinue()
    Log("MenuFrame:ButtonContinue() called.")
end

function MenuFrame:ButtonNewGame()
    Log("MenuFrame:ButtonNewGame() called.")
end

function MenuFrame:ButtonQuit()
    System.Quit()
end

function MenuFrame:ButtonDraw(button)
    -- Draw a button.
    local info = Graphics.TextDrawInfo()
    info.font = Graphics.GetDefaultFont()
    info.position = button:GetPosition()
    info.size = ButtonFontSize
    info.align = TextDrawAlign.BottomLeft
    info.outlineColor = Vec4(0.0, 0.0, 0.0, 1.0)
    info.outlineRange = Vec2(0.4, 0.5)
    
    if button:IsHovered() then
        info.bodyColor = Vec4(1.0, 0.0, 0.0, 1.0)
    else
        if button:IsEnabled() then
            info.bodyColor = Vec4(1.0, 1.0, 1.0, 1.0)
        else
            info.bodyColor = Vec4(0.0, 0.0, 0.0, 0.4)
        end
    end
    
    TextRenderer:Draw(info, button:GetText(), self.screenSpace:GetTransform())
end

function MenuFrame:OnEnter()
end

function MenuFrame:Process(event)
    if self.interfaceRoot:Process(event) then
        return true
    end
    
    return false
end

function MenuFrame:Update(timeDelta)
end

function MenuFrame:Draw()
    -- Setup screen space.
    local windowWidth = Console.GetInteger("r_width")
    local windowHeight = Console.GetInteger("r_height")
    
    self.screenSpace:SetSourceSize(windowWidth, windowHeight)
    self.screenSpace:SetTargetSize(ViewWidth, ViewHeight)
    
    -- Clear the screen buffer.
    CoreRenderer:SetClearColor(Vec4(1.0, 1.0, 1.0, 1.0))
    CoreRenderer:SetClearDepth(1.0)
    CoreRenderer:Clear(bit.bor(ClearFlags.Color, ClearFlags.Depth))
    
    -- Draw the main title.
    do
        local info = Graphics.TextDrawInfo()
        info.font = Graphics.GetDefaultFont()
        info.size = TitleFontSize
        info.bodyColor = Vec4(1.0, 1.0, 1.0, 1.0)
        info.outlineColor = Vec4(0.0, 0.0, 0.0, 1.0)
        info.outlineRange = Vec2(0.45, 0.55)
        info.position = Vec2(50.0, ViewHeight + 10.0)
        
        TextRenderer:Draw(info, "Gunstar", self.screenSpace:GetTransform())
    end
    
    -- Draw buttons.
    self:ButtonDraw(self.buttonContinue)
    self:ButtonDraw(self.buttonNewGame)
    self:ButtonDraw(self.buttonOptions)
    self:ButtonDraw(self.buttonCredits)
    self:ButtonDraw(self.buttonQuit)
end

setmetatable(MenuFrame, { __call = MenuFrame.New })
