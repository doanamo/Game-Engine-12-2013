require("Keyboard")
require("Collision")
require("Factory")

function Initialize()
    -- Print log message.
    Log("Calling Game.Initialize() function....")
    
    -- Create player entity.
    Factory.CreatePlayer()
end

Log("Main scripting entry has been loaded.")
