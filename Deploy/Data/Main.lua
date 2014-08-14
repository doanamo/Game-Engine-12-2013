require("StateMachine")

-- Package namespace.
Main = {}

-- Define functions.
function Main.Initialize()
    Log("Main context has been initialized.")
    
    -- Create a state machine.
    Main.stateMachine = StateMachine()
    
    return true
end

function Main.Cleanup()
    Log("Cleaning up the main context...")
    
    Main.stateMachine = nil
end

function Main.Process(event)
    return false
end

function Main.Update(timeDelta)
end

function Main.Draw()
    -- Clear the screen buffer.
    CoreRenderer:SetClearColor(Vec4(1.0, 1.0, 1.0, 1.0))
    CoreRenderer:SetClearDepth(1.0)
    CoreRenderer:Clear(bit.bor(ClearFlags.Color, ClearFlags.Depth))
end
