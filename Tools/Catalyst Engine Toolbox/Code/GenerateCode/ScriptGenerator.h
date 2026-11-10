#pragma once

//Core.
#include <Core/Core.h>

//Project settings.
#include <ProjectSettings/ProjectSettings.h>

//Third party.
#include <ThirdParty/json.hpp>

class ScriptGenerator final
{

public:

	/*
	*	Runs this script generator.
	*/
	void Run(const ProjectSettings &project_settings);

private:

	/*
	*	Script data class definition.
	*/
	class ScriptData final
	{

	public:

		//The file path.
		std::string _FilePath;

		//The name.
		std::string _Name;

		//The flags.
		uint32 _Flags;

		//The required data size.
		uint64 _RequiredDataSize;

		//The events.
		std::vector<std::string> _Events;

	};

	//Denotes if scripts need to be compile.
	bool _CompileScripts{ false };

	//The script data.
	std::vector<ScriptData> _ScriptData;

	/*
	*	Gathers scripts in the given directory.
	*/
	void GatherScripts(const char *const directory_path, nlohmann::json &script_cache);

	/*
	*	Generates the source file.
	*/
	void GenerateSourceFile(const ProjectSettings &project_settings);

	/*
	*	Generates the header file.
	*/
	void GenerateHeaderFile();

};