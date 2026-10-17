//Header file.
#include <GenerateSolution/GenerateSolution.h>

//Generate solution.
#include <GenerateSolution//GenerateVisualStudioSolution.h>

//STD.
#include <filesystem>
#include <fstream>
#include <iostream>

//Preprocessor defines.
#define DIRECTORY "Win64\\Win64"

//Macros.
#define CHECK_ERROR_CODE() if (error_code) { std::cerr << "Error at " << __LINE__ << ": " << error_code.message() << std::endl; }

/*
*	Returns the directory for the given audio plugin.
*/
std::string AudioPluginDirectory(const ProjectSettings::AudioPlugin & audio_plugin)
{
	return audio_plugin._NameNoSpaces + "\\Win64";
}

/*
*	Adds preprocessor definitions.
*/
void AddPreprocessorDefinitions(const ProjectSettings &project_settings, const char *configuration_name, std::vector<std::string> *preprocessor_definitions)
{
	//Add configuration specific preprocessor definitions.
	if (strcmp(configuration_name, "Debug") == 0)
	{
		preprocessor_definitions->emplace_back("_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_ENABLE_RESOURCE_BUILDING");
		preprocessor_definitions->emplace_back("CATALYST_HAS_ASSIMP");
		preprocessor_definitions->emplace_back("TRACY_ENABLE");
		preprocessor_definitions->emplace_back("TRACY_ON_DEMAND");
	}

	else if (strcmp(configuration_name, "Debug Editor") == 0)
	{
		preprocessor_definitions->emplace_back("_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_ENABLE_RESOURCE_BUILDING");
		preprocessor_definitions->emplace_back("CATALYST_HAS_ASSIMP");
		preprocessor_definitions->emplace_back("CATALYST_EDITOR");
		preprocessor_definitions->emplace_back("TRACY_ENABLE");
		preprocessor_definitions->emplace_back("TRACY_ON_DEMAND");
	}

	else if (strcmp(configuration_name, "Profile") == 0)
	{
		preprocessor_definitions->emplace_back("NDEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_PROFILE");
		preprocessor_definitions->emplace_back("CATALYST_ENABLE_RESOURCE_BUILDING");
		preprocessor_definitions->emplace_back("CATALYST_HAS_ASSIMP");
		preprocessor_definitions->emplace_back("TRACY_ENABLE");
		preprocessor_definitions->emplace_back("TRACY_ON_DEMAND");
	}

	else if (strcmp(configuration_name, "Profile Editor") == 0)
	{
		preprocessor_definitions->emplace_back("NDEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_PROFILE");
		preprocessor_definitions->emplace_back("CATALYST_ENABLE_RESOURCE_BUILDING");
		preprocessor_definitions->emplace_back("CATALYST_HAS_ASSIMP");
		preprocessor_definitions->emplace_back("CATALYST_EDITOR");
		preprocessor_definitions->emplace_back("TRACY_ENABLE");
		preprocessor_definitions->emplace_back("TRACY_ON_DEMAND");
	}

	else if (strcmp(configuration_name, "Final") == 0)
	{
		preprocessor_definitions->emplace_back("NDEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_FINAL");
	}

	else if (strcmp(configuration_name, "Debug Static") == 0)
	{
		preprocessor_definitions->emplace_back("_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_DEBUG");
		preprocessor_definitions->emplace_back("TRACY_ENABLE");
		preprocessor_definitions->emplace_back("TRACY_ON_DEMAND");
	}

	else if (strcmp(configuration_name, "Profile Static") == 0)
	{
		preprocessor_definitions->emplace_back("NDEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_PROFILE");
		preprocessor_definitions->emplace_back("TRACY_ENABLE");
		preprocessor_definitions->emplace_back("TRACY_ON_DEMAND");
	}

	else if (strcmp(configuration_name, "Final Static") == 0)
	{
		preprocessor_definitions->emplace_back("NDEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_FINAL");
	}

	else if (strcmp(configuration_name, "Standalone Debug") == 0)
	{
		preprocessor_definitions->emplace_back("_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_ENABLE_RESOURCE_BUILDING");
		preprocessor_definitions->emplace_back("CATALYST_HAS_ASSIMP");
		preprocessor_definitions->emplace_back("TRACY_ENABLE");
		preprocessor_definitions->emplace_back("TRACY_ON_DEMAND");
	}

	else if (strcmp(configuration_name, "Standalone Profile") == 0)
	{
		preprocessor_definitions->emplace_back("NDEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_PROFILE");
		preprocessor_definitions->emplace_back("CATALYST_ENABLE_RESOURCE_BUILDING");
		preprocessor_definitions->emplace_back("CATALYST_HAS_ASSIMP");
		preprocessor_definitions->emplace_back("TRACY_ENABLE");
		preprocessor_definitions->emplace_back("TRACY_ON_DEMAND");
	}

	else if (strcmp(configuration_name, "Standalone Final") == 0)
	{
		preprocessor_definitions->emplace_back("NDEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_FINAL");
	}

	else if (strcmp(configuration_name, "CLAP Debug") == 0)
	{
		preprocessor_definitions->emplace_back("_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_DEBUG");
	}

	else if (strcmp(configuration_name, "CLAP Final") == 0)
	{
		preprocessor_definitions->emplace_back("NDEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_FINAL");
	}

	else if (strcmp(configuration_name, "VST3 Debug") == 0)
	{
		preprocessor_definitions->emplace_back("_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_DEBUG");
		}

	else if (strcmp(configuration_name, "VST3 Final") == 0)
	{
		preprocessor_definitions->emplace_back("NDEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_FINAL");
		}

	else
	{
		std::cerr << "Unknown configuration type " << configuration_name << "!" << std::endl;
	}

	//Add global preprocessor definitions.
	preprocessor_definitions->emplace_back("__WINDOWS_WASAPI__");
	preprocessor_definitions->emplace_back("__WINDOWS_ASIO__");
	preprocessor_definitions->emplace_back("__WINDOWS_ASIO__");
	preprocessor_definitions->emplace_back("__WINDOWS_MM__");
	preprocessor_definitions->emplace_back("CATALYST_DISTRIBUTION_NULL");
	preprocessor_definitions->emplace_back("CATALYST_PLATFORM_WINDOWS");
	preprocessor_definitions->emplace_back("CATALYST_RENDERING_VULKAN");
	preprocessor_definitions->emplace_back("NAM_ENABLE_A2_FAST");

	switch (project_settings._Physics)
	{
		case ProjectSettings::Physics::NONE:
		{
			preprocessor_definitions->emplace_back("CATALYST_PHYSICS_NULL");

			break;
		}

		case ProjectSettings::Physics::JOLT:
		{
			preprocessor_definitions->emplace_back("CATALYST_PHYSICS_JOLT");

			break;
		}

		default:
		{
			std::cerr << "Unknown physics!" << std::endl;

			break;
		}
	}

	//Add special preprocessor definitions.
	if (project_settings._IncludeONNXRuntimeLibrary)
	{
		preprocessor_definitions->emplace_back("CATALYST_HAS_ONNX");
	}
}

/*
*	Adds library includes
*/
void AddLibraryIncludes(const ProjectSettings &project_settings, const char *configuration_name, std::vector<std::string> *library_includes)
{
	//Add configuration specific library includes.
	if (strcmp(configuration_name, "Debug") == 0)
	{
		library_includes->emplace_back("Debug\\shaderc_shared.lib");
		library_includes->emplace_back("Debug\\libsoundwave.lib");
		library_includes->emplace_back("Debug\\assimp-vc143-mtd.lib");
	}

	else if (strcmp(configuration_name, "Debug Editor") == 0)
	{
		library_includes->emplace_back("Debug\\shaderc_shared.lib");
		library_includes->emplace_back("Debug\\libsoundwave.lib");
		library_includes->emplace_back("Debug\\assimp-vc143-mtd.lib");
	}

	else if (strcmp(configuration_name, "Profile") == 0)
	{
		library_includes->emplace_back("Profile\\shaderc_shared.lib");
		library_includes->emplace_back("Profile\\libsoundwave.lib");
		library_includes->emplace_back("Profile\\assimp-vc143-mt.lib");
	}

	else if (strcmp(configuration_name, "Profile Editor") == 0)
	{
		library_includes->emplace_back("Profile\\shaderc_shared.lib");
		library_includes->emplace_back("Profile\\libsoundwave.lib");
		library_includes->emplace_back("Profile\\assimp-vc143-mt.lib");
	}

	else if (strcmp(configuration_name, "Final") == 0)
	{
		library_includes->emplace_back("Final\\libsoundwave.lib");
	}

	else if (strcmp(configuration_name, "Debug Static") == 0)
	{
		library_includes->emplace_back("Debug\\shaderc_shared.lib");
		library_includes->emplace_back("Debug\\libsoundwave.lib");
		library_includes->emplace_back("Debug\\assimp-vc143-mtd.lib");
	}

	else if (strcmp(configuration_name, "Profile Static") == 0)
	{
		library_includes->emplace_back("Profile\\shaderc_shared.lib");
		library_includes->emplace_back("Profile\\libsoundwave.lib");
		library_includes->emplace_back("Profile\\assimp-vc143-mt.lib");
	}

	else if (strcmp(configuration_name, "Final Static") == 0)
	{
		library_includes->emplace_back("Final\\libsoundwave.lib");
	}

	else if (strcmp(configuration_name, "Standalone Debug") == 0)
	{
		library_includes->emplace_back("Debug\\shaderc_shared.lib");
		library_includes->emplace_back("Debug\\libsoundwave.lib");
		library_includes->emplace_back("Debug\\assimp-vc143-mtd.lib");
	}

	else if (strcmp(configuration_name, "Standalone Profile") == 0)
	{
		library_includes->emplace_back("Profile\\shaderc_shared.lib");
		library_includes->emplace_back("Profile\\libsoundwave.lib");
		library_includes->emplace_back("Profile\\assimp-vc143-mt.lib");
	}

	else if (strcmp(configuration_name, "Standalone Final") == 0)
	{
		library_includes->emplace_back("Final\\libsoundwave.lib");
	}

	else if (strcmp(configuration_name, "CLAP Debug") == 0)
	{
		library_includes->emplace_back("Debug\\shaderc_shared.lib");
		library_includes->emplace_back("Debug\\libsoundwave.lib");
	}

	else if (strcmp(configuration_name, "CLAP Final") == 0)
	{
		library_includes->emplace_back("Final\\libsoundwave.lib");
	}

	else if (strcmp(configuration_name, "VST3 Debug") == 0)
	{
		library_includes->emplace_back("Debug\\shaderc_shared.lib");
		library_includes->emplace_back("Debug\\libsoundwave.lib");
	}

	else if (strcmp(configuration_name, "VST3 Final") == 0)
	{
		library_includes->emplace_back("Final\\libsoundwave.lib");
	}

	else
	{
		std::cerr << "Unknown configuration type " << configuration_name << "!" << std::endl;
	}

	//Add global library includes.
	library_includes->emplace_back("vulkan-1.lib");
	library_includes->emplace_back("Winmm.lib");
	library_includes->emplace_back("xinput.lib");

	//Add special library includes.
	if (project_settings._IncludeONNXRuntimeLibrary)
	{
		library_includes->emplace_back("onnxruntime.lib");
	}
}

/*
*	Scans the given directory.
*/
void ScanDirectory(GenerateVisualStudioSolution::Parameters::Directory *directory, const char *path)
{
	if (!std::filesystem::exists(path))
	{
		return;
	}

	for (auto &entry : std::filesystem::directory_iterator(path))
	{
		const std::string entry_file_path{ entry.path().string() };

		if (entry.is_directory())
		{
			GenerateVisualStudioSolution::Parameters::Directory &new_directory{ directory->_Directories.emplace_back() };
			new_directory._Name = entry_file_path.substr(entry_file_path.find_last_of('\\') + 1);
			ScanDirectory(&new_directory, entry_file_path.c_str());
		}

		else
		{
			const std::string extension{ entry_file_path.substr(entry_file_path.find_last_of('.')) };

			if (extension == ".h"
				|| extension == ".inl"
				|| extension == ".cpp"
				|| extension == ".c"
				|| extension == ".common_shader"
				|| extension == ".global_render_data"
				|| extension == ".render_pipeline"
				|| extension == ".shader_function_library"
				|| extension == ".storage_buffer_definition"
				|| extension == ".uniform_buffer_definition")
			{
				GenerateVisualStudioSolution::Parameters::File &new_file{ directory->_Files.emplace_back() };

				new_file._Path = std::string("..\\..\\") + entry_file_path;
				new_file._EnableWarnings = true;
			}
		}
	}
}

/*
*	Returns whether or not the given third party directory should be included.
*/
bool ShouldIncludeThirdPartyDirectory(const ProjectSettings &project_settings, const std::string &path)
{
	if (path == "..\\..\\Catalyst-Engine\\Engine\\Third Party\\Jolt")
	{
		return project_settings._Physics == ProjectSettings::Physics::JOLT;
	}

	return true;
}

/*
*	Scans the given third party directory.
*/
void ScanThirdPartyDirectory(const ProjectSettings &project_settings, GenerateVisualStudioSolution::Parameters *parameters, GenerateVisualStudioSolution::Parameters::Directory *directory, const char *path, const bool is_top_level)
{
	if (!std::filesystem::exists(path))
	{
		return;
	}

	for (auto &entry : std::filesystem::directory_iterator(path))
	{
		const std::string entry_file_path{ entry.path().string() };

		if (entry.is_directory())
		{
			if (ShouldIncludeThirdPartyDirectory(project_settings, entry_file_path))
			{
				if (is_top_level)
				{
					parameters->_CodeIncludePaths.emplace_back(std::string("..\\..\\") + entry_file_path);
				}

				GenerateVisualStudioSolution::Parameters::Directory &new_directory{ directory->_Directories.emplace_back() };
				new_directory._Name = entry_file_path.substr(entry_file_path.find_last_of('\\') + 1);
				ScanThirdPartyDirectory(project_settings, parameters, &new_directory, entry_file_path.c_str(), false);
			}
		}

		else
		{
			const std::string extension{ entry_file_path.substr(entry_file_path.find_last_of('.')) };

			if (extension == ".h"
				|| extension == ".inl"
				|| extension == ".cpp"
				|| extension == ".c")
			{
				GenerateVisualStudioSolution::Parameters::File &new_file{ directory->_Files.emplace_back() };

				new_file._Path = std::string("..\\..\\") + entry_file_path;
				new_file._EnableWarnings = false;
			}
		}
	}
}

/*
*	Generates the "RunCodeGeneration.bat" file.
*/
void GenerateRunCodeGenerationBatchFile(const std::string &directory, const ProjectSettings &project_settings, const bool for_pre_generation)
{
	//Add the "RunCodeGeneration.bat" file.
	std::ofstream run_code_generation_file{ directory + "\\RunCodeGeneration.bat" };

	if (for_pre_generation)
	{
		run_code_generation_file << "cd " << directory.c_str() << "\\" << std::endl;
	}

	run_code_generation_file << "\"../../../../Catalyst-Engine/Tools/Binaries/CatalystEngineCodeGenerator.exe\"";

	if (!project_settings._ScriptGameInclude.empty())
	{
		run_code_generation_file << " -script_generator_game_include " << project_settings._ScriptGameInclude.c_str();
	}

	run_code_generation_file.close();
}

/*
*	Adds common parameters.
*/
void AddCommonParameters(const ProjectSettings &project_settings, GenerateVisualStudioSolution::Parameters *const parameters)
{
	//Fill in the core code include paths.
	parameters->_CodeIncludePaths.emplace_back("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Include");
	parameters->_CodeIncludePaths.emplace_back("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Third Party");
	parameters->_CodeIncludePaths.emplace_back("..\\..\\..\\Code\\Include");

	//Fill in the third party code include paths.
	parameters->_CodeIncludePaths.emplace_back("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Include\\ThirdParty\\assimp");
	parameters->_CodeIncludePaths.emplace_back("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Include\\ThirdParty\\imguinodeeditor");

	//Fill in the library include paths.
	parameters->_LibraryIncludePaths.emplace_back("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Libraries\\Static");

	//Fill in the directories.
	{
		GenerateVisualStudioSolution::Parameters::Directory &directory{ parameters->_Directories.emplace_back() };
		directory._Name = "Catalyst Engine Include";
		ScanDirectory(&directory, "..\\..\\Catalyst-Engine\\Engine\\Include");
	}

	{
		GenerateVisualStudioSolution::Parameters::Directory &directory{ parameters->_Directories.emplace_back() };
		directory._Name = "Catalyst Engine Third Party";
		ScanThirdPartyDirectory(project_settings, parameters, &directory, "..\\..\\Catalyst-Engine\\Engine\\Third Party", true);
	}

	{
		GenerateVisualStudioSolution::Parameters::Directory &directory{ parameters->_Directories.emplace_back() };
		directory._Name = "Catalyst Engine Source";
		ScanDirectory(&directory, "..\\..\\Catalyst-Engine\\Engine\\Source");
	}

	{
		GenerateVisualStudioSolution::Parameters::Directory &directory{ parameters->_Directories.emplace_back() };
		directory._Name = "Catalyst Engine Rendering";
		ScanDirectory(&directory, "..\\..\\Catalyst-Engine\\Engine\\Rendering");
	}

	{
		GenerateVisualStudioSolution::Parameters::Directory &directory{ parameters->_Directories.emplace_back() };
		directory._Name = project_settings._ProjectName + std::string(" Include");
		ScanDirectory(&directory, "..\\Code\\Include");
	}

	{
		GenerateVisualStudioSolution::Parameters::Directory &directory{ parameters->_Directories.emplace_back() };
		directory._Name = project_settings._ProjectName + std::string(" Source");
		ScanDirectory(&directory, "..\\Code\\Source");
	}

	{
		GenerateVisualStudioSolution::Parameters::Directory &directory{ parameters->_Directories.emplace_back() };
		directory._Name = project_settings._ProjectName + std::string(" Rendering");
		ScanDirectory(&directory, "..\\Rendering");
	}
}

/*
*	Generates the default solution with the given parameters.
*/
void GenerateDefaultSolution(const ProjectSettings &project_settings)
{
	//Figure out the project name.
	char project_name[256];
	sprintf_s(project_name, "%s_Win64", project_settings._ProjectNameNoSpaces.c_str());

	//Set up the parameters.
	GenerateVisualStudioSolution::Parameters parameters;

	parameters._DirectoryPath = DIRECTORY;
	parameters._ProjectName = project_name;

	{
		GenerateVisualStudioSolution::Parameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "Debug";
		configuration._Type = GenerateVisualStudioSolution::Parameters::Configuration::Type::APPLICATION;
		configuration._TargetFileExtension = ".exe";
		configuration._UseDebugLibraries = true;
		configuration._RuntimeLibrary = "MultiThreadedDebug";
		configuration._SubSystem = "Windows";
		AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
		AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
		configuration._PreBuildEvent = "call RunCodeGeneration.bat";
	}

	{
		GenerateVisualStudioSolution::Parameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "Debug Editor";
		configuration._Type = GenerateVisualStudioSolution::Parameters::Configuration::Type::APPLICATION;
		configuration._TargetFileExtension = ".exe";
		configuration._UseDebugLibraries = true;
		configuration._RuntimeLibrary = "MultiThreadedDebug";
		configuration._SubSystem = "Windows";
		AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
		AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
		configuration._PreBuildEvent = "call RunCodeGeneration.bat";
	}

	{
		GenerateVisualStudioSolution::Parameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "Profile";
		configuration._Type = GenerateVisualStudioSolution::Parameters::Configuration::Type::APPLICATION;
		configuration._TargetFileExtension = ".exe";
		configuration._UseDebugLibraries = false;
		configuration._RuntimeLibrary = "MultiThreaded";
		configuration._SubSystem = "Windows";
		AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
		AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
		configuration._PreBuildEvent = "call RunCodeGeneration.bat";
	}

	{
		GenerateVisualStudioSolution::Parameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "Profile Editor";
		configuration._Type = GenerateVisualStudioSolution::Parameters::Configuration::Type::APPLICATION;
		configuration._TargetFileExtension = ".exe";
		configuration._UseDebugLibraries = false;
		configuration._RuntimeLibrary = "MultiThreaded";
		configuration._SubSystem = "Windows";
		AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
		AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
		configuration._PreBuildEvent = "call RunCodeGeneration.bat";
	}

	{
		GenerateVisualStudioSolution::Parameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "Final";
		configuration._Type = GenerateVisualStudioSolution::Parameters::Configuration::Type::APPLICATION;
		configuration._TargetFileExtension = ".exe";
		configuration._UseDebugLibraries = false;
		configuration._RuntimeLibrary = "MultiThreaded";
		configuration._SubSystem = "Windows";
		AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
		AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
		configuration._PreBuildEvent = "call RunCodeGeneration.bat";
	}

	if (project_settings._AddStaticBuilds)
	{
		{
			GenerateVisualStudioSolution::Parameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

			configuration._Name = "Debug Static";
			configuration._Type = GenerateVisualStudioSolution::Parameters::Configuration::Type::STATIC_LIBRARY;
			configuration._TargetFileExtension = ".lib";
			configuration._UseDebugLibraries = true;
			configuration._RuntimeLibrary = "MultiThreadedDebug";
			configuration._SubSystem = "Windows";
			AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
			AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
			configuration._PreBuildEvent = "call RunCodeGeneration.bat";
		}

		{
			GenerateVisualStudioSolution::Parameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

			configuration._Name = "Profile Static";
			configuration._Type = GenerateVisualStudioSolution::Parameters::Configuration::Type::STATIC_LIBRARY;
			configuration._TargetFileExtension = ".lib";
			configuration._UseDebugLibraries = false;
			configuration._RuntimeLibrary = "MultiThreaded";
			configuration._SubSystem = "Windows";
			AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
			AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
			configuration._PreBuildEvent = "call RunCodeGeneration.bat";
		}

		{
			GenerateVisualStudioSolution::Parameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

			configuration._Name = "Final Static";
			configuration._Type = GenerateVisualStudioSolution::Parameters::Configuration::Type::STATIC_LIBRARY;
			configuration._TargetFileExtension = ".lib";
			configuration._UseDebugLibraries = false;
			configuration._RuntimeLibrary = "MultiThreaded";
			configuration._SubSystem = "Windows";
			AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
			AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
			configuration._PreBuildEvent = "call RunCodeGeneration.bat";
		}
	}

	{
		GenerateVisualStudioSolution::Parameters::Platform &platform{ parameters._Platforms.emplace_back() };

		platform._Name = "x64";
	}

	//Add common paramaters.
	AddCommonParameters(project_settings, &parameters);

	//Generate the code generation file.
	GenerateRunCodeGenerationBatchFile(DIRECTORY, project_settings, false);

	//Generate!
	GenerateVisualStudioSolution::Generate(parameters);

	//Remember the error code for filesystem functions.
	std::error_code error_code;

	//Create the directories for the different configurations.
	for (const GenerateVisualStudioSolution::Parameters::Configuration &configuration : parameters._Configurations)
	{
		std::filesystem::create_directories(std::string(parameters._DirectoryPath + std::string("\\") + configuration._Name).c_str(), error_code); CHECK_ERROR_CODE();
	}

	//Copy assimp-vc143-mt.dll.
	std::filesystem::copy("..\\..\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\assimp-vc143-mtd.dll", std::string(DIRECTORY + std::string("\\Debug")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
	std::filesystem::copy("..\\..\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\assimp-vc143-mtd.dll", std::string(DIRECTORY + std::string("\\Debug Editor")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
	std::filesystem::copy("..\\..\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\assimp-vc143-mt.dll", std::string(DIRECTORY + std::string("\\Profile")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
	std::filesystem::copy("..\\..\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\assimp-vc143-mt.dll", std::string(DIRECTORY + std::string("\\Profile Editor")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
	std::filesystem::copy("..\\..\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\assimp-vc143-mt.dll", std::string(DIRECTORY + std::string("\\Final")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();

	if (project_settings._AddStaticBuilds)
	{
		std::filesystem::copy("..\\..\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\assimp-vc143-mtd.dll", std::string(DIRECTORY + std::string("\\Debug Static")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("..\\..\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\assimp-vc143-mt.dll", std::string(DIRECTORY + std::string("\\Profile Static")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
	}

	//Copy onnxruntime.dll.
	if (project_settings._IncludeONNXRuntimeLibrary)
	{
		for (const GenerateVisualStudioSolution::Parameters::Configuration &configuration : parameters._Configurations)
		{
			std::filesystem::copy("..\\..\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\onnxruntime.dll", std::string(parameters._DirectoryPath + std::string("\\") + configuration._Name).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		}
	}
}

/*
*	Generates an audio plugin solution with the given parameters.
*/
void GenerateAudioPluginSolution(const ProjectSettings &project_settings, const ProjectSettings::AudioPlugin &audio_plugin)
{
	//Figure out the project name.
	char project_name[256];
	sprintf_s(project_name, "%s_Win64", audio_plugin._Name.c_str());

	//Set up the parameters.
	GenerateVisualStudioSolution::Parameters parameters;

	parameters._DirectoryPath = AudioPluginDirectory(audio_plugin);
	parameters._ProjectName = project_name;

	{
		GenerateVisualStudioSolution::Parameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "Standalone Debug";
		configuration._Type = GenerateVisualStudioSolution::Parameters::Configuration::Type::APPLICATION;
		configuration._TargetFileExtension = ".exe";
		configuration._UseDebugLibraries = true;
		configuration._RuntimeLibrary = "MultiThreadedDebug";
		configuration._SubSystem = "Windows";
		AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
		configuration._PreprocessorDefinitions.emplace_back("AUDIO_PLUGIN");
		configuration._PreprocessorDefinitions.emplace_back("AUDIO_PLUGIN_STANDALONE");
		AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
		configuration._PreBuildEvent = "call RunCodeGeneration.bat";
	}

	{
		GenerateVisualStudioSolution::Parameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "Standalone Profile";
		configuration._Type = GenerateVisualStudioSolution::Parameters::Configuration::Type::APPLICATION;
		configuration._TargetFileExtension = ".exe";
		configuration._UseDebugLibraries = false;
		configuration._RuntimeLibrary = "MultiThreaded";
		configuration._SubSystem = "Windows";
		AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
		configuration._PreprocessorDefinitions.emplace_back("AUDIO_PLUGIN");
		configuration._PreprocessorDefinitions.emplace_back("AUDIO_PLUGIN_STANDALONE");
		AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
		configuration._PreBuildEvent = "call RunCodeGeneration.bat";
	}

	{
		GenerateVisualStudioSolution::Parameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "Standalone Final";
		configuration._Type = GenerateVisualStudioSolution::Parameters::Configuration::Type::APPLICATION;
		configuration._TargetFileExtension = ".exe";
		configuration._UseDebugLibraries = false;
		configuration._RuntimeLibrary = "MultiThreaded";
		configuration._SubSystem = "Windows";
		AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
		configuration._PreprocessorDefinitions.emplace_back("AUDIO_PLUGIN");
		configuration._PreprocessorDefinitions.emplace_back("AUDIO_PLUGIN_STANDALONE");
		AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
		configuration._PreBuildEvent = "call RunCodeGeneration.bat";
	}

	{
		GenerateVisualStudioSolution::Parameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "CLAP Debug";
		configuration._Type = GenerateVisualStudioSolution::Parameters::Configuration::Type::DYNAMIC_LIBRARY;
		configuration._TargetFileExtension = ".clap";
		configuration._UseDebugLibraries = true;
		configuration._RuntimeLibrary = "MultiThreadedDebug";
		configuration._SubSystem = "Windows";
		AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
		configuration._PreprocessorDefinitions.emplace_back("AUDIO_PLUGIN");
		configuration._PreprocessorDefinitions.emplace_back("AUDIO_PLUGIN_CLAP");
		AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
		configuration._PreBuildEvent = "call RunCodeGeneration.bat";
	}

	{
		GenerateVisualStudioSolution::Parameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "CLAP Final";
		configuration._Type = GenerateVisualStudioSolution::Parameters::Configuration::Type::DYNAMIC_LIBRARY;
		configuration._TargetFileExtension = ".clap";
		configuration._UseDebugLibraries = false;
		configuration._RuntimeLibrary = "MultiThreaded";
		configuration._SubSystem = "Windows";
		AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
		configuration._PreprocessorDefinitions.emplace_back("AUDIO_PLUGIN");
		configuration._PreprocessorDefinitions.emplace_back("AUDIO_PLUGIN_CLAP");
		AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
		configuration._PreBuildEvent = "call RunCodeGeneration.bat";
	}

	{
		GenerateVisualStudioSolution::Parameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "VST3 Debug";
		configuration._Type = GenerateVisualStudioSolution::Parameters::Configuration::Type::DYNAMIC_LIBRARY;
		configuration._TargetFileExtension = ".vst3";
		configuration._UseDebugLibraries = true;
		configuration._RuntimeLibrary = "MultiThreadedDebug";
		configuration._SubSystem = "Windows";
		AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
		configuration._PreprocessorDefinitions.emplace_back("AUDIO_PLUGIN");
		configuration._PreprocessorDefinitions.emplace_back("AUDIO_PLUGIN_VST3");
		AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
		configuration._PreBuildEvent = "call RunCodeGeneration.bat";
	}

	{
		GenerateVisualStudioSolution::Parameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "VST3 Final";
		configuration._Type = GenerateVisualStudioSolution::Parameters::Configuration::Type::DYNAMIC_LIBRARY;
		configuration._TargetFileExtension = ".vst3";
		configuration._UseDebugLibraries = false;
		configuration._RuntimeLibrary = "MultiThreaded";
		configuration._SubSystem = "Windows";
		AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
		configuration._PreprocessorDefinitions.emplace_back("AUDIO_PLUGIN");
		configuration._PreprocessorDefinitions.emplace_back("AUDIO_PLUGIN_VST3");
		AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
		configuration._PreBuildEvent = "call RunCodeGeneration.bat";
	}

	{
		GenerateVisualStudioSolution::Parameters::Platform &platform{ parameters._Platforms.emplace_back() };

		platform._Name = "x64";
	}

	//Generate the audio plugin files.
	{
		//Generate the "Main.cpp" file.
		{
			char buffer[256];
			sprintf_s(buffer, "%s\\StandaloneMain.cpp", std::string(std::string("..\\") + audio_plugin._DirectoryPath).c_str());

			std::ofstream file{ buffer };

			file << "#if defined(AUDIO_PLUGIN_STANDALONE)" << std::endl;

			file << "//Core." << std::endl;
			file << "#include <Core/Essential/CatalystEssential.h>" << std::endl;
			file << std::endl;

			file << "//Platform." << std::endl;
			file << "#include <Platform/Windows/CatalystPlatformWindows.h>" << std::endl;
			file << std::endl;

			file << "MAIN_FUNCTION" << std::endl;
			file << "(" << std::endl;
			file << ")" << std::endl;

			file << "#endif";

			file.close();
		}

		//Generate the "AudioPluginGlue.h" file.
		{
			char buffer[256];
			sprintf_s(buffer, "%s\\AudioPluginGlue.h", std::string(std::string("..\\") + audio_plugin._DirectoryPath).c_str());

			std::ofstream file{ buffer };

			file << "#pragma once" << std::endl;
			file << std::endl;

			file << "//Core." << std::endl;
			file << "#include <Core/Essential/CatalystEssential.h>" << std::endl;
			file << std::endl;

			file << "//Audio plugin." << std::endl;
			file << "#include <" << audio_plugin._ClassName.c_str() << ".h>" << std::endl;
			file << std::endl;

			file << "namespace AudioPluginGlue" << std::endl;
			file << "{" << std::endl;
			file << std::endl;

			file << "\t/*" << std::endl;
			file << "\t*\tReturns the identifier of the plugin." << std::endl;
			file << "\t*/" << std::endl;
			file << "\tFORCE_INLINE constexpr const char *const RESTRICT Identifier() NOEXCEPT" << std::endl;
			file << "\t{" << std::endl;
			file << "\t\treturn \"com." << project_settings._DeveloperNameNoSpaces.c_str() << "." << audio_plugin._NameNoSpaces.c_str() << "\";" << std::endl;
			file << "\t}" << std::endl;
			file << std::endl;

			file << "\t/*" << std::endl;
			file << "\t*\tReturns the name of the plugin." << std::endl;
			file << "\t*/" << std::endl;
			file << "\tFORCE_INLINE constexpr const char *const RESTRICT Name() NOEXCEPT" << std::endl;
			file << "\t{" << std::endl;
			file << "\t\treturn \"" << audio_plugin._Name.c_str() << "\";" << std::endl;
			file << "\t}" << std::endl;
			file << std::endl;

			file << "\t/*" << std::endl;
			file << "\t*\tReturns the vendor of the plugin." << std::endl;
			file << "\t*/" << std::endl;
			file << "\tFORCE_INLINE constexpr const char *const RESTRICT Vendor() NOEXCEPT" << std::endl;
			file << "\t{" << std::endl;
			file << "\t\treturn \"" << project_settings._DeveloperNameNoSpaces.c_str() << "\";" << std::endl;
			file << "\t}" << std::endl;
			file << std::endl;

			file << "\t/*" << std::endl;
			file << "\t*\tCreates an instance of the plugin." << std::endl;
			file << "\t*/" << std::endl;
			file << "\tFORCE_INLINE Audio::Plugin *const RESTRICT Create() NOEXCEPT" << std::endl;
			file << "\t{" << std::endl;
			file << "\t\treturn new " << audio_plugin._ClassName.c_str() << "();" << std::endl;
			file << "\t}" << std::endl;
			file << std::endl;

			file << "\t/*" << std::endl;
			file << "\t*\tDestroys an instance of the plugin." << std::endl;
			file << "\t*/" << std::endl;
			file << "\tFORCE_INLINE void Destroy(Audio::Plugin *const RESTRICT plugin) NOEXCEPT" << std::endl;
			file << "\t{" << std::endl;
			file << "\t\tdelete plugin;" << std::endl;
			file << "\t}" << std::endl;
			file << std::endl;

			file << "}";

			file.close();
		}

		//Generate the audio plugin header file.
		{
			char buffer[256];
			sprintf_s(buffer, "%s\\%s.h", std::string(std::string("..\\") + audio_plugin._DirectoryPath).c_str(), audio_plugin._ClassName.c_str());

			if (!std::filesystem::exists(buffer))
			{
				std::ofstream file{ buffer };

				file << "#pragma once" << std::endl;
				file << std::endl;

				file << "//Core." << std::endl;
				file << "#include <Core/Essential/CatalystEssential.h>" << std::endl;
				file << std::endl;

				file << "//Audio." << std::endl;
				file << "#include <Audio/Plugins/Core/Plugin.h>" << std::endl;
				file << std::endl;

				file << "class " << audio_plugin._ClassName.c_str() << " final : public Audio::Plugin" << std::endl;
				file << "{" << std::endl;

				file << std::endl;
				file << "public:" << std::endl;
				file << std::endl;

				file << "\t/*" << std::endl;
				file << "\t*\tCallback for this plugin to process the given buffer." << std::endl;
				file << "\t*/" << std::endl;
				file << "\tvoid Process" << std::endl;
				file << "\t(" << std::endl;
				file << "\t\tconst AudioProcessContext &context," << std::endl;
				file << "\t\tconst DynamicArray<DynamicArray<float32>> &inputs," << std::endl;
				file << "\t\tDynamicArray<DynamicArray<float32>> *const RESTRICT outputs," << std::endl;
				file << "\t\tconst uint8 number_of_channels," << std::endl;
				file << "\t\tconst uint32 number_of_samples" << std::endl;
				file << "\t) NOEXCEPT override;" << std::endl;
				file << std::endl;

				file << "};";

				file.close();
			}
		}

		//Generate the audio plugin CPP file.
		{
			char buffer[256];
			sprintf_s(buffer, "%s\\%s.cpp", std::string(std::string("..\\") + audio_plugin._DirectoryPath).c_str(), audio_plugin._ClassName.c_str());

			if (!std::filesystem::exists(buffer))
			{
				std::ofstream file{ buffer };

				file << "//Header file." << std::endl;
				file << "#include <" << audio_plugin._ClassName.c_str() <<".h>" << std::endl;
				file << std::endl;

				file << "/*" << std::endl;
				file << "*\tCallback for this plugin to process the given buffer." << std::endl;
				file << "*/" << std::endl;
				file << "void " << audio_plugin._ClassName.c_str() << "::Process" << std::endl;
				file << "(" << std::endl;
				file << "\tconst AudioProcessContext &context," << std::endl;
				file << "\tconst DynamicArray<DynamicArray<float32>> &inputs," << std::endl;
				file << "\tDynamicArray<DynamicArray<float32>> *const RESTRICT outputs," << std::endl;
				file << "\tconst uint8 number_of_channels," << std::endl;
				file << "\tconst uint32 number_of_samples" << std::endl;
				file << ") NOEXCEPT" << std::endl;
				file << "{" << std::endl;
				file << std::endl;
				file << "}";

				file.close();
			}
		}
	}

	//Add the code include path.
	parameters._CodeIncludePaths.emplace_back(std::string(std::string("..\\..\\..\\") + audio_plugin._DirectoryPath));

	//Add the audio plugin directory.
	{
		GenerateVisualStudioSolution::Parameters::Directory &directory{ parameters._Directories.emplace_back() };
		directory._Name = audio_plugin._NameNoSpaces;
		ScanDirectory(&directory, std::string(std::string("..\\") + audio_plugin._DirectoryPath).c_str());
	}

	//Add common paramaters.
	AddCommonParameters(project_settings, &parameters);

	//Generate the code generation file.
	GenerateRunCodeGenerationBatchFile(AudioPluginDirectory(audio_plugin), project_settings, false);

	//Generate!
	GenerateVisualStudioSolution::Generate(parameters);

	//Remember the error code for filesystem functions.
	std::error_code error_code;

	//Create the directories for the different configurations.
	for (const GenerateVisualStudioSolution::Parameters::Configuration &configuration : parameters._Configurations)
	{
		std::filesystem::create_directories(std::string(parameters._DirectoryPath + std::string("\\") + configuration._Name).c_str(), error_code); CHECK_ERROR_CODE();
	}

	//Copy assimp-vc143-mt.dll.
	std::filesystem::copy("..\\..\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\assimp-vc143-mtd.dll", std::string(AudioPluginDirectory(audio_plugin) + std::string("\\Standalone Debug")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
	std::filesystem::copy("..\\..\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\assimp-vc143-mtd.dll", std::string(AudioPluginDirectory(audio_plugin) + std::string("\\Standalone Profile")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
	std::filesystem::copy("..\\..\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\assimp-vc143-mt.dll", std::string(AudioPluginDirectory(audio_plugin) + std::string("\\Standalone Final")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();

	//Copy onnxruntime.dll.
	if (project_settings._IncludeONNXRuntimeLibrary)
	{
		for (const GenerateVisualStudioSolution::Parameters::Configuration &configuration : parameters._Configurations)
		{
			std::filesystem::copy("..\\..\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\onnxruntime.dll", std::string(parameters._DirectoryPath + std::string("\\") + configuration._Name).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		}
	}
}

/*
*	Generates a solution with the given project settings.
*/
void GenerateSolution::Run(const ProjectSettings &project_settings)
{
	//Clean, first of all. (:
	Clean(project_settings);

	//Ensure the directories exists.
	std::filesystem::create_directories(DIRECTORY);

	for (const ProjectSettings::AudioPlugin &audio_plugin : project_settings._AudioPlugins)
	{
		std::filesystem::create_directories(AudioPluginDirectory(audio_plugin));
	}

	//Generate the "RunCodeGeneration.bat" file.
	GenerateRunCodeGenerationBatchFile(DIRECTORY, project_settings, true);

	//Run the code generation, to make sure that generated files gets picked up.
	system(DIRECTORY "\\RunCodeGeneration.bat");

	//Generate the default solution.
	GenerateDefaultSolution(project_settings);

	//Generate solutions for each audio plugin.
	for (const ProjectSettings::AudioPlugin &audio_plugin : project_settings._AudioPlugins)
	{
		//Generate the solution!
		GenerateAudioPluginSolution(project_settings, audio_plugin);
	}
}

/*
*	Cleans.
*/
void GenerateSolution::Clean(const ProjectSettings &project_settings)
{
	//Define constants.
	constexpr const char *CODE_GENERATION_DIRECTORY_PATH{ "..\\Code\\CodeGeneration" };

	//Remember the error code for filesystem functions.
	std::error_code error_code;

	//Remove (almost) all files in the directory.
	if (std::filesystem::exists("Win64"))
	{
		std::filesystem::remove_all("Win64", error_code); CHECK_ERROR_CODE()
	}

	for (const ProjectSettings::AudioPlugin &audio_plugin : project_settings._AudioPlugins)
	{
		if (std::filesystem::exists(audio_plugin._Name.c_str()))
		{
			std::filesystem::remove_all(audio_plugin._Name.c_str(), error_code); CHECK_ERROR_CODE()
		}
	}

	//Remove all the code generation caches.
	if (std::filesystem::exists(CODE_GENERATION_DIRECTORY_PATH))
	{
		for (const auto &entry : std::filesystem::directory_iterator(std::string(CODE_GENERATION_DIRECTORY_PATH)))
		{
			const std::string file_path{ entry.path().string() };

			if (file_path != std::string(CODE_GENERATION_DIRECTORY_PATH) + std::string("\\.gitignore"))
			{
				std::filesystem::remove(file_path, error_code); CHECK_ERROR_CODE()
			}
		}
	}
}