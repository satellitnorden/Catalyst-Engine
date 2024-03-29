#pragma once

//Catalyst code generator.
#include "CatalystCodeGeneratorCore.h"
#include "json.hpp"

class ComponentGenerator final
{

public:

	/*
	*	Runs this component generator.
	*/
	void Run();

private:

	/*
	*	Gathers components in the given directory.
	*/
	bool GatherComponents(const char *const directory_path, nlohmann::json *const JSON);

	/*
	*	Parses an component.
	*/
	void ParseComponent(std::ifstream &file, std::string &current_line, nlohmann::json *const JSON);

	/*
	*	Generates the source file.
	*/
	void GenerateSourceFile(const nlohmann::json &JSON);

};