#pragma once

//Catalyst code generator.
#include "CatalystCodeGeneratorCore.h"
#include "json.hpp"

class SystemGenerator final
{

public:

	/*
	*	Runs this system generator.
	*/
	void Run();

private:

	/*
	*	Gathers systems in the given directory.
	*/
	bool GatherSystems(const char *const directory_path, nlohmann::json *const JSON);

	/*
	*	Parses a system.
	*/
	void ParseSystem(std::ifstream &file, std::string &current_line, nlohmann::json *const JSON);

	/*
	*	Generates the source file.
	*/
	void GenerateSourceFile(const nlohmann::json &JSON);

};