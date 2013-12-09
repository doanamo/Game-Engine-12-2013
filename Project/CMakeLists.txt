CMake_Minimum_Required(VERSION 2.8.12)

Project("Karolina")

Set(SourceFiles
	"../Source/Main.cpp"
    "../Source/Precompiled.hpp"
    "../Source/Precompiled.cpp"
    "../Source/Context.hpp"
    "../Source/Context.cpp"
    "../Source/ConsoleSystem.hpp"
    "../Source/ConsoleSystem.cpp"
    "../Source/ConsoleDefinition.hpp"
    "../Source/ConsoleDefinition.cpp"
    "../Source/ConsoleCommand.hpp"
    "../Source/ConsoleCommand.cpp"
    "../Source/ConsoleVariable.hpp"
    "../Source/ConsoleVariable.cpp"
)

Source_Group("Source" FILES ${SourceFiles})

Add_Executable("Game" ${SourceFiles})
