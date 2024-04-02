#pragma once

//Catalyst code generator.
#include "CatalystCodeGeneratorCore.h"
#include "json.hpp"

class SettingsGenerator final
{

public:

	/*
	*	Runs this settings generator.
	*/
	void Run();

private:

	//The file paths.
	std::vector<std::string> _FilePaths;

	//Denotes if settings need to be compiled.
	bool _CompileSettings{ false };

	/*
	*	Gathers settings definitions in the given directory.
	*/
	void GatherSettingsDefinitions(const char *const directory_path, nlohmann::ordered_json &cache);

	/*
	*	Generates code.
	*/
	void GenerateCode();

};