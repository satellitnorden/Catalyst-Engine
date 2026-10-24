#pragma once

//Core.
#include <Core/Core.h>

//STD.
#include <string>
#include <vector>

class ProjectSettings final
{

public:

	//Enumeration covering all physics.
	enum class Physics : uint8
	{
		NONE,

		JOLT
	};

	/*
	*	Audio plugin class definition.
	*/
	class AudioPlugin final
	{

	public:

		//Denotes whether or not this audio plugin is locked.
		bool _Locked;

		//The name.
		std::string _Name;

		//The name with no spaces.
		std::string _NameNoSpaces;

		//The class name.
		std::string _ClassName;

		//The main category.
		std::string _MainCategory;

		//The sub category.
		std::string _SubCategory;

		//The resolution.
		struct
		{
			//The width.
			unsigned _Width;

			//The height.
			unsigned _Height;
		} _Resolution;

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

	//The audio plugins.
	std::vector<AudioPlugin> _AudioPlugins;

	/*
	*	Reads the project settings.
	*/
	void Read(const char *file_path);

};