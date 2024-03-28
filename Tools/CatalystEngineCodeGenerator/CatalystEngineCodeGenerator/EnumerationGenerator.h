#pragma once

//Catalyst code generator.
#include "CatalystCodeGeneratorCore.h"
#include "json.hpp"

class EnumerationGenerator final
{

public:

	/*
	*	Runs this enumeration generator.
	*/
	void Run();

private:

	/*
	*	Gathers enumerations in the given directory.
	*/
	bool GatherEnumerations(const char *const directory_path, nlohmann::json *const JSON);

	/*
	*	Parses an enumeration.
	*/
	void ParseEnumeration(std::ifstream &file, std::string &current_line, nlohmann::json *const JSON);

	/*
	*	Generates the source file.
	*/
	void GenerateSourceFile(const nlohmann::json &JSON);

};