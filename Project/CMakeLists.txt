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

#
# SDL2
#

# We are going to use the static library.
Set(SDL_SHARED OFF CACHE BOOL "")
Set(SDL_STATIC ON CACHE BOOL "")

# Look for targets in the main directory where CMakeLists.txt is located.
Add_SubDirectory("../External/SDL-2.0.1" "SDL2" EXCLUDE_FROM_ALL)

# Add include directory.
Include_Directories("../External/SDL-2.0.1/include")

# Add directory of library dependencies.
Link_Directories("$ENV{DXSDK_DIR}/Lib/x86")

#
# GLEW
#

# Add project directory.
Add_SubDirectory("../External/GLEW-1.10.0" "GLEW" EXCLUDE_FROM_ALL)

# Add include directory.
Include_Directories("../External/GLEW-1.10.0/include")

#
# Target
#

# Target source files.
Set(SourceDir "../Source")
Set(PrecompiledHeader "Precompiled.hpp")
Set(PrecompiledSource "Precompiled.cpp")

Set(SourceFiles
	"${SourceDir}/${PrecompiledHeader}"
	"${SourceDir}/${PrecompiledSource}"
	"${SourceDir}/Main.cpp"
	"${SourceDir}/Context.hpp"
	"${SourceDir}/Context.cpp"
	"${SourceDir}/ConsoleSystem.hpp"
	"${SourceDir}/ConsoleSystem.cpp"
	"${SourceDir}/ConsoleDefinition.hpp"
	"${SourceDir}/ConsoleDefinition.cpp"
	"${SourceDir}/ConsoleCommand.hpp"
	"${SourceDir}/ConsoleCommand.cpp"
	"${SourceDir}/ConsoleVariable.hpp"
	"${SourceDir}/ConsoleVariable.cpp"
	"${SourceDir}/Common/NonCopyable.hpp"
	"${SourceDir}/Common/ScopeGuard.hpp"
)

Source_Group("Source" FILES ${SourceFiles})

# Create an executable target.
Set(TargetName "Game")

Add_Executable(${TargetName} ${SourceFiles})

# Link SDL2 library.
Add_Dependencies(${TargetName} "SDL2-static" "SDL2main")
Target_Link_Libraries(${TargetName} "SDL2-static" "SDL2main")

# Link GLEW library.
Add_Dependencies(${TargetName} "glew32s")
Target_Link_Libraries(${TargetName} "glew32s")

# Set unicode support.
Add_Definitions(-DUNICODE -D_UNICODE)

#
# Windows
#

# Visual C++ specific.
If(MSVC)
	# Remove the console window.
	#Set_Property(TARGET ${TargetName} APPEND_STRING PROPERTY LINK_FLAGS "/SUBSYSTEM:Windows ")
	
	# Restore default main() entry instead of WinMain().
	#Set_Property(TARGET ${TargetName} APPEND_STRING PROPERTY LINK_FLAGS "/ENTRY:mainCRTStartup ")
	
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
