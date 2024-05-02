#pragma once

//Catalyst code generator.
#include "CatalystCodeGeneratorCore.h"
#include "json.hpp"

class UserInterfaceGenerator final
{

public:

	/*
	*	Runs this user interface generator.
	*/
	void Run();

private:

	/*
	*	Gathers user interface scenes in the given directory.
	*/
	bool GatherUserInterfaceScenes(const char *const directory_path, nlohmann::json *const JSON);

	/*
	*	Parses a user interface scene.
	*/
	void ParseUserInterfaceScene(std::ifstream &file, std::string &current_line, nlohmann::json *const JSON);

	/*
	*	Generates the files.
	*/
	void GenerateFiles(const nlohmann::json &JSON);

};