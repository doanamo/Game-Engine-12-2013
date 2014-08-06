require("Keyboard")
require("Collision")
require("Factory")

function Initialize()
    -- Print log message.
    Log("Calling Game.Initialize() function....")
    
    -- Create entity bounds.
    Factory.CreateBounds()
    
    -- Create player entity.
    Factory.CreatePlayer()
    
    -- Define spawns.
    local spawnArea = Vec4(1024.0 + 100.0, 50.0, 1024.0 + 100.0, 576.0 - 50.0)
    SpawnSystem:AddSpawn(Factory.CreateEnemy, spawnArea, 0.5, 1.0)
end

Log("Main scripting entry has been loaded.")
