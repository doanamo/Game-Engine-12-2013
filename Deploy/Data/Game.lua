require("Keyboard")
require("Collision")
require("Factory")

Game = {}

function Game.Initialize()    
    -- Create entity bounds.
    Factory.CreateBounds()
    
    -- Create player entity.
    Factory.CreatePlayer()
    
    -- Setup spawn system.
    local spawnArea = Vec4(1024.0 + 100.0, 50.0, 1024.0 + 100.0, 576.0 - 50.0)
    SpawnSystem:AddSpawn(Factory.CreateEnemy, spawnArea, 0.5, 1.0)
    
    -- Print a log message.
    Log("Game state has been initialized.")
end

function Game.Cleanup()
    -- Print a log message.
    Log("Cleaning up the game state...")
end
