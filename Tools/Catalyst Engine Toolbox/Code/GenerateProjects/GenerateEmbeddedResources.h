#pragma once

//Core.
#include <Core/Core.h>

//STD.
#include <string>
#include <vector>

class GenerateEmbeddedResources final
{

public:

	/*
	*	Generates!
	*/
	static void Generate(const std::vector<std::string> &embedded_resources, const char *const directory_path);

private:

#if defined(PLATFORM_WIN64)
	/*
	*	Generates for Win64.
	*/
	static void GenerateWin64(const std::vector<std::string>& embedded_resources, const char* const directory_path);
#endif

#if defined(PLATFORM_LINUX)
	/*
	*	Generates for Linux.
	*/
	static void GenerateLinux(const std::vector<std::string> &embedded_resources, const char *const directory_path);
#endif

};