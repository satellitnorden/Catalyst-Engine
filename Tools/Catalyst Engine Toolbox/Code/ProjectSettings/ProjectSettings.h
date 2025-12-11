#pragma once

//Core.
#include <Core/Core.h>

//STD.
#include <string>

class ProjectSettings final
{

public:

	//Enumeration covering all physics.
	enum class Physics : uint8
	{
		NONE,

		JOLT
	};

	//The devloper name.
	std::string _DeveloperName;

	//The developer name, with no spaces.
	std::string _DeveloperNameNoSpaces;

	//The project name.
	std::string _ProjectName;

	//The projet name, with no spaces.
	std::string _ProjectNameNoSpaces;

	//The physics.
	Physics _Physics;

	//Denotes whether or not to include the ONNX Runtime library.
	bool _IncludeONNXRuntimeLibrary{ false };

	//Denotes whether or not to add static builds.
	bool _AddStaticBuilds{ false };

	//The script game include.
	std::string _ScriptGameInclude;

	/*
	*	Reads the project settings.
	*/
	void Read(const char *file_path);

};