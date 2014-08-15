#
# CMake
#

CMake_Minimum_Required(VERSION 2.8.12)

#
# Project
#

# Project name.
set(ProjectName "Project")
Project(${ProjectName})

#
# Source
#

# Source files.
Set(SourceDir "../Source")
Set(PrecompiledHeader "Precompiled.hpp")
Set(PrecompiledSource "Precompiled.cpp")

Set(SourceFiles
    "${PrecompiledHeader}"
    "${PrecompiledSource}"

    "Main.cpp"
    "MainGlobal.hpp"
    "MainGlobal.cpp"

    "Common/NonCopyable.hpp"
    "Common/ScopeGuard.hpp"
    "Common/Utility.hpp"
    "Common/Utility.cpp"
    "Common/StateMachine.hpp"
    "Common/ShelfPacker.hpp"
    "Common/ShelfPacker.cpp"
    "Common/Dispatcher.hpp"
    "Common/Dispatcher.inl"
    "Common/Receiver.hpp"
    "Common/Delegate.hpp"
    "Common/Services.hpp"

    "Logger/Logger.hpp"
    "Logger/Logger.cpp"
    "Logger/LoggerOutput.hpp"
    "Logger/LoggerOutputFile.hpp"
    "Logger/LoggerOutputFile.cpp"
    "Logger/LoggerOutputConsole.hpp"
    "Logger/LoggerOutputConsole.cpp"
    "Logger/LoggerMessage.hpp"
    "Logger/LoggerMessage.cpp"
    "Logger/LoggerScopedMessage.hpp"
    "Logger/LoggerScopedMessage.cpp"

    "Console/ConsoleSystem.hpp"
    "Console/ConsoleSystem.cpp"
    "Console/ConsoleHistory.hpp"
    "Console/ConsoleHistory.cpp"
    "Console/ConsoleDefinition.hpp"
    "Console/ConsoleDefinition.cpp"
    "Console/ConsoleCommand.hpp"
    "Console/ConsoleCommand.cpp"
    "Console/ConsoleVariable.hpp"
    "Console/ConsoleVariable.cpp"
    "Console/ConsoleUtility.cpp"
    "Console/ConsoleFrame.hpp"
    "Console/ConsoleFrame.cpp"

    "System/BaseFrame.hpp"
    "System/FrameCounter.hpp"
    "System/FrameCounter.cpp"
    "System/CacheManager.hpp"
    "System/CacheManager.cpp"

    "Graphics/ScreenSpace.hpp"
    "Graphics/ScreenSpace.cpp"
    "Graphics/CoreRenderer.hpp"
    "Graphics/CoreRenderer.cpp"
    "Graphics/Shader.hpp"
    "Graphics/Shader.cpp"
    "Graphics/Buffer.hpp"
    "Graphics/Buffer.cpp"
    "Graphics/VertexInput.hpp"
    "Graphics/VertexInput.cpp"
    "Graphics/Texture.hpp"
    "Graphics/Texture.cpp"
    "Graphics/BasicRenderer.hpp"
    "Graphics/BasicRenderer.cpp"
    "Graphics/Font.hpp"
    "Graphics/Font.cpp"
    "Graphics/FontCache.cpp"
    "Graphics/TextRenderer.hpp"
    "Graphics/TextRenderer.cpp"
    "Graphics/TextDrawState.hpp"
    "Graphics/TextDrawState.cpp"

    "Interface/Element.hpp"
    "Interface/Element.cpp"
    "Interface/Root.hpp"
    "Interface/Root.cpp"
    "Interface/Button.hpp"
    "Interface/Button.cpp"

    "Scripting/LuaEngine.hpp"
    "Scripting/LuaEngine.cpp"
    "Scripting/LuaMath.hpp"
    "Scripting/LuaMath.cpp"
    "Scripting/LuaLogger.hpp"
    "Scripting/LuaLogger.cpp"
    "Scripting/LuaSystem.hpp"
    "Scripting/LuaSystem.cpp"
    "Scripting/LuaGraphics.hpp"
    "Scripting/LuaGraphics.cpp"
    "Scripting/LuaGame.hpp"
    "Scripting/LuaGame.cpp"

    "Game/MainFrame.hpp"
    "Game/MainFrame.cpp"
    "Game/GameGlobal.hpp"
    "Game/GameGlobal.cpp"
    "Game/MenuFrame.hpp"
    "Game/MenuFrame.cpp"
    "Game/GameFrame.hpp"
    "Game/GameFrame.cpp"
    "Game/LoseFrame.hpp"
    "Game/LoseFrame.cpp"
    "Game/GameState.hpp"
    "Game/GameState.cpp"

    "Game/Entity/EntityHandle.hpp"
    "Game/Entity/EntitySystem.hpp"
    "Game/Entity/EntitySystem.cpp"

    "Game/Event/EventDefinitions.hpp"
    "Game/Event/EventSystem.hpp"

    "Game/Identity/IdentitySystem.hpp"
    "Game/Identity/IdentitySystem.cpp"

    "Game/Component/Component.hpp"
    "Game/Component/ComponentPool.hpp"
    "Game/Component/ComponentSystem.hpp"

    "Game/Input/InputState.hpp"
    "Game/Input/InputComponent.hpp"
    "Game/Input/InputComponent.cpp"
    "Game/Input/InputSystem.hpp"
    "Game/Input/InputSystem.cpp"

    "Game/Transform/TransformComponent.hpp"
    "Game/Transform/TransformComponent.cpp"

    "Game/Collision/CollisionDefinitions.hpp"
    "Game/Collision/CollisionComponent.hpp"
    "Game/Collision/CollisionComponent.cpp"
    "Game/Collision/CollisionObject.hpp"
    "Game/Collision/CollisionSystem.hpp"
    "Game/Collision/CollisionSystem.cpp"

    "Game/Health/HealthComponent.hpp"
    "Game/Health/HealthComponent.cpp"
    "Game/Health/HealthSystem.hpp"
    "Game/Health/HealthSystem.cpp"

    "Game/Script/ScriptComponent.hpp"
    "Game/Script/ScriptComponent.cpp"
    "Game/Script/ScriptSystem.hpp"
    "Game/Script/ScriptSystem.cpp"

    "Game/Render/RenderComponent.hpp"
    "Game/Render/RenderComponent.cpp"
    "Game/Render/RenderSystem.hpp"
    "Game/Render/RenderSystem.cpp"

    "Game/Interface/InterfaceSystem.hpp"
    "Game/Interface/InterfaceSystem.cpp"
    "Game/Interface/ValueBar.hpp"
    "Game/Interface/ValueBar.cpp"
    "Game/Interface/FloatingText.hpp"
    "Game/Interface/FloatingText.cpp"

    "Game/Spawn/SpawnSystem.hpp"
    "Game/Spawn/SpawnSystem.cpp"
)

# Enable source folders.
Set_Property(GLOBAL PROPERTY USE_FOLDERS ON)

# Add the source directory prefix to each file path.
ForEach(SourceFile ${SourceFiles})
    List(APPEND SourceFilesTemp "${SourceDir}/${SourceFile}")
EndForEach()

Set(SourceFiles ${SourceFilesTemp})
Set(SourceFilesTemp)

# Setup automatic source grouping based on the file path.
ForEach(SourceFile ${SourceFiles})
    # Get the source file path.
    Get_Filename_Component(SourceFilePath ${SourceFile} PATH)
    
    # Remove the base path.
    String(REPLACE "${SourceDir}" "" SourceFilePath ${SourceFilePath})

    # Add a source file to a group.
    If(NOT ${SourceFilePath} STREQUAL "")
        # Replace folder separators.
        String(REPLACE "/" "\\" SourceFilePath ${SourceFilePath})

        # Remove leading folder separator.
        String(SUBSTRING ${SourceFilePath} 0 1 FirstCharacter)

        If(FirstCharacter STREQUAL "\\")
            String(LENGTH ${SourceFilePath} SourceFilePathLength)
            Math(EXPR SourceFilePathLength "${SourceFilePathLength} - 1")
            String(SUBSTRING ${SourceFilePath} 1 ${SourceFilePathLength} SourceFilePath)
        EndIf()

        # Add a source file to a group.
        Source_Group("Source Files\\${SourceFilePath}" FILES ${SourceFile})
    Else()
        # Add a source file to the default group.
        Source_Group("Source Files" FILES ${SourceFile})
    EndIf()
EndForEach()

# Add the main source folder as an include directory.
Include_Directories(${SourceDir})

#
# Boost
#

# Set Boost options.
Set(Boost_USE_STATIC_LIBS ON)
Set(Boost_USE_MULTITHREADED ON)
Set(Boost_USE_STATIC_RUNTIME OFF)

# Find installed Boost library.
Find_Package(Boost 1.55.0 COMPONENTS filesystem REQUIRED)

# Add include directory.
Include_Directories(${Boost_INCLUDE_DIRS})

# Add link directory.
Link_Directories(${Boost_LIBRARY_DIRS})

#
# GLM
#

# Add include directory.
Include_Directories("../External/GLM-0.9.4.6")

#
# SDL2
#

# Compile as a static library.
Set(SDL_SHARED OFF CACHE BOOL "")
Set(SDL_STATIC ON CACHE BOOL "")

# Add library directory.
Add_SubDirectory("../External/SDL-2.0.1" "SDL2" EXCLUDE_FROM_ALL)

# Move library targets to dependencies.
Set_Property(TARGET "SDL2-static" PROPERTY FOLDER "Dependencies")
Set_Property(TARGET "SDL2main" PROPERTY FOLDER "Dependencies")

# Add include directory.
Include_Directories("../External/SDL-2.0.1/include")

# Add additional link dependencies.
Link_Directories("$ENV{DXSDK_DIR}Lib/x86")

# Disable annoying warnings that we can't do anything about.
If(MSVC)
    # C4996: "Function was declared deprecated."
    Set_Property(TARGET "SDL2-static" APPEND_STRING PROPERTY COMPILE_FLAGS "/wd4996 ")

    # C4018: "Signed/unsigned mismatch."
    Set_Property(TARGET "SDL2-static" APPEND_STRING PROPERTY COMPILE_FLAGS "/wd4018 ")
EndIf()

#
# GLEW
#

# Add library directory.
Add_SubDirectory("../External/GLEW-1.10.0" "GLEW" EXCLUDE_FROM_ALL)

# Move library target to dependencies.
Set_Property(TARGET "glew32s" PROPERTY FOLDER "Dependencies")

# Add include directory.
Include_Directories("../External/GLEW-1.10.0/include")

#
# FreeType
#

# Add library directory.
Add_SubDirectory("../External/FreeType-2.5.2" "FreeType" EXCLUDE_FROM_ALL)

# Move library target to dependencies.
Set_Property(TARGET "freetype" PROPERTY FOLDER "Dependencies")

# Add include directory.
Include_Directories("../External/FreeType-2.5.2/include")

# Disable annoying warnings that we can't do anything about.
If(MSVC)
    # C4996: "This function or variable may be unsafe."
    Set_Property(TARGET "freetype" APPEND_STRING PROPERTY COMPILE_FLAGS "/wd4996 ")

    # C4244: "Conversion from one type to another, possible loss of data."
    Set_Property(TARGET "freetype" APPEND_STRING PROPERTY COMPILE_FLAGS "/wd4244 ")
EndIf()

#
# UTF8
#

# Add include directory.
Include_Directories("../External/UTF8-2.3.4/source")

#
# LuaJIT
#

# We don't need these.
Set(LUA_BUILD_EXE OFF CACHE BOOL "")
Set(LUA_BUILD_WLUA OFF CACHE BOOL "")

# Add library directory.
Add_SubDirectory("../External/LuaJIT-2.0.3" "LuaJIT")

# Move library target to dependencies.
Set_Property(TARGET "minilua" PROPERTY FOLDER "Dependencies")
Set_Property(TARGET "buildvm" PROPERTY FOLDER "Dependencies")
Set_Property(TARGET "liblua" PROPERTY FOLDER "Dependencies")

# Add include directory.
Include_Directories("../External/LuaJIT-2.0.3/src")
Include_Directories("${CMAKE_CURRENT_BINARY_DIR}/LuaJIT")

#
# LuaBridge
#

# Add include directory.
Include_Directories("../External/LuaBridge-443/Source")

#
# Target
#

# Create an executable target.
Set(TargetName "Game")
Add_Executable(${TargetName} ${SourceFiles})

# Enable unicode support.
Add_Definitions(-DUNICODE -D_UNICODE)

# Link Boost library.
Target_Link_Libraries(${TargetName} ${Boost_LIBRARIES})

# Link SDL2 library.
Add_Dependencies(${TargetName} "SDL2-static" "SDL2main")
Target_Link_Libraries(${TargetName} "SDL2-static" "SDL2main")

# Link GLEW library.
Add_Dependencies(${TargetName} "glew32s")
Target_Link_Libraries(${TargetName} "glew32s")

# Link FreeType library.
Add_Dependencies(${TargetName} "freetype")
Target_Link_Libraries(${TargetName} "freetype")

# Link LuaJIT library.
Add_Dependencies(${TargetName} "liblua")
Target_Link_Libraries(${TargetName} "liblua")

#
# Debugging
#

# Write a file in the build directory that will point the executable to it's working directory.
Get_Filename_Component(WorkingDir "${CMAKE_CURRENT_LIST_DIR}/../Deploy" ABSOLUTE)
File(WRITE "${CMAKE_BINARY_DIR}/WorkingDir.txt" "${WorkingDir}")

# Write a file in the build directory that will point to source directory.
Get_Filename_Component(SourceDir "${CMAKE_CURRENT_LIST_DIR}/../Source" ABSOLUTE)
File(WRITE "${CMAKE_BINARY_DIR}/SourceDir.txt" "${SourceDir}")

#
# Windows
#

# Visual C++ specific.
If(MSVC)
    # Remove the console window.
    Set_Property(TARGET ${TargetName} APPEND_STRING PROPERTY LINK_FLAGS "/SUBSYSTEM:Windows ")
    
    # Restore default main() entry instead of WinMain().
    Set_Property(TARGET ${TargetName} APPEND_STRING PROPERTY LINK_FLAGS "/ENTRY:mainCRTStartup ")

    # Disable Standard C++ Library warnings.
    Set_Property(TARGET ${TargetName} APPEND_STRING PROPERTY COMPILE_DEFINITIONS "_CRT_SECURE_NO_WARNINGS")
    Set_Property(TARGET ${TargetName} APPEND_STRING PROPERTY COMPILE_DEFINITIONS "_SCL_SECURE_NO_WARNINGS")
    
    # Use the precompiled header.
    Get_Filename_Component(PrecompiledName ${PrecompiledHeader} NAME_WE)
    
    Set(PrecompiledBinary "$(IntDir)/${PrecompiledName}.pch")
    
    Set_Source_Files_Properties(${SourceFiles} PROPERTIES 
        COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
        OBJECT_DEPENDS "${PrecompiledBinary}"
    )
    
    Set_Source_Files_Properties("${SourceDir}/${PrecompiledSource}" PROPERTIES 
        COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
        OBJECT_OUTPUTS "${PrecompiledBinary}"
    )
EndIf()
