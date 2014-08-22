require("Common.StateMachine")
require("Frames.MenuFrame")

Main = {}

function Main.Initialize()
    Log("Main context has been initialized.")
    
    -- Setup state machine.
    Main.stateMachine = StateMachine()
    
    -- Create initial frame.
    Main.menuFrame = MenuFrame()
    Main.stateMachine:Change(Main.menuFrame)
    
    return true
end

function Main.Cleanup()
    Log("Cleaning up the main context...")
end

function Main.Process(event)
    return Main.stateMachine:Call("Process", event)
end

function Main.Update(timeDelta)
    Main.stateMachine:Call("Update", timeDelta)
end

function Main.Draw()
    Main.stateMachine:Call("Draw")
end
