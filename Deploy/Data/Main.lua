require("StateMachine")
require("MenuFrame")

-- Package namespace.
Main = {}

-- Define functions.
function Main.Initialize()
    Log("Main context has been initialized.")
    
    -- Setup state machine.
    Main.stateMachine = StateMachine()
    Main.stateMachine:Change(MenuFrame())
    
    return true
end

function Main.Cleanup()
    Log("Cleaning up the main context...")
    
    Main.stateMachine = nil
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
