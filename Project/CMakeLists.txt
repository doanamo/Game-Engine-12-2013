#
# CMake
#

CMake_Minimum_Required(VERSION 2.8.12)

#
# Project
#

# Project name.
set(ProjectName "Karolina")

Project(${ProjectName})

# Enable project folders.
Set_Property(GLOBAL PROPERTY USE_FOLDERS ON)

#
# GLM
#

# Add include directory.
Include_Directories("../External/GLM-0.9.4.6")

#
# SDL2
#

# We are going to use the static library.
Set(SDL_SHARED OFF CACHE BOOL "")
Set(SDL_STATIC ON CACHE BOOL "")

# Add library directory.
Add_SubDirectory("../External/SDL-2.0.1" "SDL2" EXCLUDE_FROM_ALL)

# Add include directory.
Include_Directories("../External/SDL-2.0.1/include")

# Add additional link dependencies.
Link_Directories("$ENV{DXSDK_DIR}/Lib/x86")

# Disable annoying warnings that we can't do anything about.
If(MSVC)
	# C4996: "'x' was declared deprecated."
	Set_Property(TARGET "SDL2-static" APPEND_STRING PROPERTY COMPILE_FLAGS "/wd4996 ")

	# C4018: "Signed/unsigned mismatch."
	Set_Property(TARGET "SDL2-static" APPEND_STRING PROPERTY COMPILE_FLAGS "/wd4018 ")
EndIf()

#
# GLEW
#

# Add library directory.
Add_SubDirectory("../External/GLEW-1.10.0" "GLEW" EXCLUDE_FROM_ALL)

# Add include directory.
Include_Directories("../External/GLEW-1.10.0/include")

#
# FreeType
#

# Add library directory.
Add_SubDirectory("../External/FreeType-2.5.2" "FreeType" EXCLUDE_FROM_ALL)

# Add include directory.
Include_Directories("../External/FreeType-2.5.2/include")

# Disable annoying warnings that we can't do anything about.
If(MSVC)
	# C4996: "This function or variable may be unsafe. Consider using 'x' instead."
	Set_Property(TARGET "freetype" APPEND_STRING PROPERTY COMPILE_FLAGS "/wd4996 ")

	# C4244: "Conversion from 'x' to 'y', possible loss of data."
	Set_Property(TARGET "freetype" APPEND_STRING PROPERTY COMPILE_FLAGS "/wd4244 ")
EndIf()

#
# Source
#

# Source files.
Set(SourceDir "../Source")
Set(SourceFiles_All "")
Set(PrecompiledHeader "Precompiled.hpp")
Set(PrecompiledSource "Precompiled.cpp")

Set(SourceFiles_Main
	"${SourceDir}/${PrecompiledHeader}"
	"${SourceDir}/${PrecompiledSource}"
	"${SourceDir}/Main.cpp"
	"${SourceDir}/Context.hpp"
	"${SourceDir}/Context.cpp"
	"${SourceDir}/Logger.hpp"
	"${SourceDir}/Logger.cpp"
	"${SourceDir}/Font.hpp"
	"${SourceDir}/Font.cpp"
)

Source_Group("Source" FILES ${SourceFiles_Main})
Set(SourceFiles_All ${SourceFiles_All} ${SourceFiles_Main})

Set(SourceFiles_Common
	"${SourceDir}/Common/NonCopyable.hpp"
	"${SourceDir}/Common/ScopeGuard.hpp"
	"${SourceDir}/Common/Utility.hpp"
	"${SourceDir}/Common/Utility.cpp"
)

Source_Group("Source\\Common" FILES ${SourceFiles_Common})
Set(SourceFiles_All ${SourceFiles_All} ${SourceFiles_Common})

Set(SourceFiles_Console
	"${SourceDir}/Console/ConsoleSystem.hpp"
	"${SourceDir}/Console/ConsoleSystem.cpp"
	"${SourceDir}/Console/ConsoleDefinition.hpp"
	"${SourceDir}/Console/ConsoleDefinition.cpp"
	"${SourceDir}/Console/ConsoleCommand.hpp"
	"${SourceDir}/Console/ConsoleCommand.cpp"
	"${SourceDir}/Console/ConsoleVariable.hpp"
	"${SourceDir}/Console/ConsoleVariable.cpp"
	"${SourceDir}/Console/ConsoleFrame.hpp"
	"${SourceDir}/Console/ConsoleFrame.cpp"
)

Source_Group("Source\\Console" FILES ${SourceFiles_Console})
Set(SourceFiles_All ${SourceFiles_All} ${SourceFiles_Console})

Set(SourceFiles_Graphics
	"${SourceDir}/Graphics/Shader.hpp"
	"${SourceDir}/Graphics/Shader.cpp"
	"${SourceDir}/Graphics/Buffer.hpp"
	"${SourceDir}/Graphics/Buffer.cpp"
	"${SourceDir}/Graphics/VertexInput.hpp"
	"${SourceDir}/Graphics/VertexInput.cpp"
	"${SourceDir}/Graphics/Texture.hpp"
	"${SourceDir}/Graphics/Texture.cpp"
)

Source_Group("Source\\Graphics" FILES ${SourceFiles_Graphics})
Set(SourceFiles_All ${SourceFiles_All} ${SourceFiles_Graphics})

# Add the main source folder as an include directory.
Include_Directories(${SourceDir})

#
# Target
#

# Create an executable target.
Set(TargetName "Game")

Add_Executable(${TargetName} ${SourceFiles_All})

# Link SDL2 library.
Add_Dependencies(${TargetName} "SDL2-static" "SDL2main")
Target_Link_Libraries(${TargetName} "SDL2-static" "SDL2main")

# Link GLEW library.
Add_Dependencies(${TargetName} "glew32s")
Target_Link_Libraries(${TargetName} "glew32s")

# Link FreeType library.
Add_Dependencies(${TargetName} "freetype")
Target_Link_Libraries(${TargetName} "freetype")

# Set unicode support.
Add_Definitions(-DUNICODE -D_UNICODE)

# Group dependencies in a single folder.
Set_Property(TARGET "SDL2-static" PROPERTY FOLDER "Dependencies")
Set_Property(TARGET "SDL2main" PROPERTY FOLDER "Dependencies")
Set_Property(TARGET "glew32s" PROPERTY FOLDER "Dependencies")
Set_Property(TARGET "freetype" PROPERTY FOLDER "Dependencies")

#
# Debugging
#

# Write a text file in the build's directory that will point the executable to it's working directory.
Get_Filename_Component(WorkingDir "${CMAKE_CURRENT_LIST_DIR}/../Base" ABSOLUTE)

File(WRITE "${CMAKE_BINARY_DIR}/WorkingDir.txt" "${WorkingDir}/")

#
# Windows
#

# Visual C++ specific.
If(MSVC)
	# Remove the console window.
	#Set_Property(TARGET ${TargetName} APPEND_STRING PROPERTY LINK_FLAGS "/SUBSYSTEM:Windows ")
	
	# Restore default main() entry instead of WinMain().
	#Set_Property(TARGET ${TargetName} APPEND_STRING PROPERTY LINK_FLAGS "/ENTRY:mainCRTStartup ")

	# Disable unsafe Standard C function warnings.
	Set_Property(TARGET ${TargetName} APPEND_STRING PROPERTY COMPILE_DEFINITIONS "_CRT_SECURE_NO_WARNINGS")
	
	# Set the precompiled header.
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
