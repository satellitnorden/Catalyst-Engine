//Header file.
#include <GenerateProjects/GenerateCopyDynamicLibraries.h>

//Core.
#include <Core/Utilities.h>

//STD.
#include <cstring>
#include <filesystem>

//Macros.
#define CHECK_ERROR_CODE() if (error_code) { std::cerr << "Error at " << __LINE__ << ": " << error_code.message() << std::endl; }

/*
*	Returns the configuration path.
*/
std::string GetConfigurationPath
(
	const GenerateProjectParameters &parameters,
	const GenerateProjectParameters::Configuration &configuration,
	const bool allow_spaces,
	const char *const build_directory
)
{
	std::string result;

	result = parameters._DirectoryPath;

	if (build_directory)
	{
		result += "/";
		result += build_directory;
	}

	result += "/";

	if (allow_spaces)
	{
		result += configuration._Name;
	}
	
	else
	{
		result += Utilities::RemoveSpaces(configuration._Name);
	}

	return result;
}

/*
*	Chooses the appropriate 'assimp.dll' file for the given configuration.
*/
const char *GetAssimpDLLFile(const GenerateProjectParameters::Configuration &configuration)
{
	if
	(
		std::strcmp(configuration._Name, "Debug") == 0
		||
		std::strcmp(configuration._Name, "Debug Editor") == 0
		||
		std::strcmp(configuration._Name, "Standalone Debug") == 0
	)
	{
		return "../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Debug/assimp.dll";
	}

	if
	(
		std::strcmp(configuration._Name, "Profile") == 0
		||
		std::strcmp(configuration._Name, "Profile Editor") == 0
		||
		std::strcmp(configuration._Name, "Standalone Profile") == 0
	)
	{
		return "../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Profile/assimp.dll";
	}

	if
	(
		std::strcmp(configuration._Name, "Final") == 0
		||
		std::strcmp(configuration._Name, "Standalone Final") == 0
		||
		std::strcmp(configuration._Name, "CLAP Debug") == 0
		||
		std::strcmp(configuration._Name, "CLAP Final") == 0
		||
		std::strcmp(configuration._Name, "VST3 Debug") == 0
		||
		std::strcmp(configuration._Name, "VST3 Final") == 0
	)
	{
		return nullptr;
	}

	ASSERT(false, "Unknown configuration!");

	return nullptr;
}

/*
*	Generates!
*/
void GenerateCopyDynamicLibraries::Generate(const ProjectSettings &project_settings, const GenerateProjectParameters &parameters, const bool allow_spaces, const char *const build_directory)
{
	//Remember the error code for filesystem functions.
	std::error_code error_code;

	//Create the directory for all configurations.
	for (const GenerateProjectParameters::Configuration &configuration : parameters._Configurations)
	{
		std::filesystem::create_directories(GetConfigurationPath(parameters, configuration, allow_spaces, build_directory), error_code); CHECK_ERROR_CODE();
	}

#if defined(PLATFORM_WIN64)
	//Copy 'assimp.dll'.
	for (const GenerateProjectParameters::Configuration &configuration : parameters._Configurations)
	{
		const char *source_file_path{ GetAssimpDLLFile(configuration) };

		if (!source_file_path)
		{
			continue;
		}

		std::filesystem::copy
		(
			source_file_path,
			GetConfigurationPath(parameters, configuration, allow_spaces, build_directory),
			std::filesystem::copy_options::overwrite_existing,
			error_code
		);
		
		CHECK_ERROR_CODE();
	}
#endif

#if defined(PLATFORM_WIN64)
	//Copy 'onnxruntime.dll'.
	for (const GenerateProjectParameters::Configuration &configuration : parameters._Configurations)
	{
		std::filesystem::copy
		(
			"../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/onnxruntime.dll",
			GetConfigurationPath(parameters, configuration, allow_spaces, build_directory),
			std::filesystem::copy_options::overwrite_existing,
			error_code
		);

		CHECK_ERROR_CODE();
	}
#endif
}