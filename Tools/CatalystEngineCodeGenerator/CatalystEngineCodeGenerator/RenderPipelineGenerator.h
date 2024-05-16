#pragma once

//Catalyst code generator.
#include "CatalystCodeGeneratorCore.h"
#include "json.hpp"

class RenderPipelineGenerator final
{

public:

	/*
	*	Runs this render pipeline generator.
	*/
	void Run();

private:

	/*
	*	Gathers render pipelines in the given directory.
	*/
	bool GatherRenderPipelines(const char *const directory_path, nlohmann::json &JSON);

	/*
	*	Parses a render pipeline.
	*/
	void ParseRenderPipeline(std::ifstream &file, nlohmann::json &JSON);

	/*
	*	Generates the source file.
	*/
	void GenerateSourceFile(const nlohmann::json &JSON);

};