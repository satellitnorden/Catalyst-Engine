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
		preprocessor_definitions->emplace_back("TRACY_ENABLE");
		preprocessor_definitions->emplace_back("TRACY_ON_DEMAND");
	}

	else if (strcmp(configuration_name, "Debug Editor") == 0)
	{
		preprocessor_definitions->emplace_back("_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_ENABLE_RESOURCE_BUILDING");
		preprocessor_definitions->emplace_back("CATALYST_EDITOR");
		preprocessor_definitions->emplace_back("TRACY_ENABLE");
		preprocessor_definitions->emplace_back("TRACY_ON_DEMAND");
	}

	else if (strcmp(configuration_name, "Profile") == 0)
	{
		preprocessor_definitions->emplace_back("NDEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_PROFILE");
		preprocessor_definitions->emplace_back("CATALYST_ENABLE_RESOURCE_BUILDING");
		preprocessor_definitions->emplace_back("TRACY_ENABLE");
		preprocessor_definitions->emplace_back("TRACY_ON_DEMAND");
	}

	else if (strcmp(configuration_name, "Profile Editor") == 0)
	{
		preprocessor_definitions->emplace_back("NDEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_PROFILE");
		preprocessor_definitions->emplace_back("CATALYST_ENABLE_RESOURCE_BUILDING");
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
*	Generates a solution with the given project settings.
*/
void GenerateSolution::Run(const ProjectSettings &project_settings)
{
	//Clean, first of all. (:
	Clean();

	//Ensure the directory exists.
	std::filesystem::create_directories(DIRECTORY);

	//Generate the "RunCodeGeneration.bat" file.
	GenerateRunCodeGenerationBatchFile(project_settings, true);

	//Run the code generation, to make sure that generated files gets picked up.
	system(DIRECTORY "\\RunCodeGeneration.bat");

	//Generate the "RunCodeGeneration.bat" file again, now without the pre-generation stuff..
	GenerateRunCodeGenerationBatchFile(project_settings, false);

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

	//Fill in the core code include paths.
	parameters._CodeIncludePaths.emplace_back("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Include");
	parameters._CodeIncludePaths.emplace_back("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Third Party");
	parameters._CodeIncludePaths.emplace_back("..\\..\\..\\Code\\Include");

	//Fill in the third party code include paths.
	parameters._CodeIncludePaths.emplace_back("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Include\\ThirdParty\\assimp");
	parameters._CodeIncludePaths.emplace_back("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Include\\ThirdParty\\imguinodeeditor");

	//Fill in the library include paths.
	parameters._LibraryIncludePaths.emplace_back("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Libraries\\Static");

	//Fill in the directories.
	{
		GenerateVisualStudioSolution::Parameters::Directory &directory{ parameters._Directories.emplace_back() };
		directory._Name = "Catalyst Engine Include";
		ScanDirectory(&directory, "..\\..\\Catalyst-Engine\\Engine\\Include");
	}

	{
		GenerateVisualStudioSolution::Parameters::Directory &directory{ parameters._Directories.emplace_back() };
		directory._Name = "Catalyst Engine Third Party";
		ScanThirdPartyDirectory(project_settings, &parameters, &directory, "..\\..\\Catalyst-Engine\\Engine\\Third Party", true);
	}

	{
		GenerateVisualStudioSolution::Parameters::Directory &directory{ parameters._Directories.emplace_back() };
		directory._Name = "Catalyst Engine Source";
		ScanDirectory(&directory, "..\\..\\Catalyst-Engine\\Engine\\Source");
	}

	{
		GenerateVisualStudioSolution::Parameters::Directory &directory{ parameters._Directories.emplace_back() };
		directory._Name = "Catalyst Engine Rendering";
		ScanDirectory(&directory, "..\\..\\Catalyst-Engine\\Engine\\Rendering");
	}

	{
		GenerateVisualStudioSolution::Parameters::Directory &directory{ parameters._Directories.emplace_back() };
		directory._Name = project_settings._ProjectName + std::string(" Include");
		ScanDirectory(&directory, "..\\Code\\Include");
	}

	{
		GenerateVisualStudioSolution::Parameters::Directory &directory{ parameters._Directories.emplace_back() };
		directory._Name = project_settings._ProjectName + std::string(" Source");
		ScanDirectory(&directory, "..\\Code\\Source");
	}

	{
		GenerateVisualStudioSolution::Parameters::Directory& directory{ parameters._Directories.emplace_back() };
		directory._Name = project_settings._ProjectName + std::string(" Rendering");
		ScanDirectory(&directory, "..\\Rendering");
	}

	//Generate!
	GenerateVisualStudioSolution::Generate(parameters);

	//Remember the error code for filesystem functions.
	std::error_code error_code;

	//Create the directories for the different configurations.
	std::filesystem::create_directories(DIRECTORY "\\Debug", error_code); CHECK_ERROR_CODE();
	std::filesystem::create_directories(DIRECTORY "\\Debug Editor", error_code); CHECK_ERROR_CODE();
	std::filesystem::create_directories(DIRECTORY "\\Profile", error_code); CHECK_ERROR_CODE();
	std::filesystem::create_directories(DIRECTORY "\\Profile Editor", error_code); CHECK_ERROR_CODE();
	std::filesystem::create_directories(DIRECTORY "\\Final", error_code); CHECK_ERROR_CODE();

	if (project_settings._AddStaticBuilds)
	{
		std::filesystem::create_directories(DIRECTORY "\\Debug Static", error_code); CHECK_ERROR_CODE();
		std::filesystem::create_directories(DIRECTORY "\\Profile Static", error_code); CHECK_ERROR_CODE();
		std::filesystem::create_directories(DIRECTORY "\\Final Static", error_code); CHECK_ERROR_CODE();
	}

	//Copy assimp-vc143-mt.dll.
	std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\assimp-vc143-mtd.dll", DIRECTORY "\\Debug", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
	std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\assimp-vc143-mtd.dll", DIRECTORY "\\Debug Editor", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
	std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\assimp-vc143-mt.dll", DIRECTORY "\\Profile", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
	std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\assimp-vc143-mt.dll", DIRECTORY "\\Profile Editor", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();

	if (project_settings._AddStaticBuilds)
	{
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\assimp-vc143-mtd.dll", DIRECTORY "\\Debug Static", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\assimp-vc143-mtd.dll", DIRECTORY "\\Profile Static", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
	}

	//Copy onnxruntime.dll.
	if (project_settings._IncludeONNXRuntimeLibrary)
	{
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\onnxruntime.dll", DIRECTORY "\\Debug", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\onnxruntime.dll", DIRECTORY "\\Debug Editor", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\onnxruntime.dll", DIRECTORY "\\Profile", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\onnxruntime.dll", DIRECTORY "\\Profile Editor", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\onnxruntime.dll", DIRECTORY "\\Final", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
	
		if (project_settings._AddStaticBuilds)
		{
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\onnxruntime.dll", DIRECTORY "\\Debug Static", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\onnxruntime.dll", DIRECTORY "\\Profile Static", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\onnxruntime.dll", DIRECTORY "\\Final Static", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		}
	}
}

/*
*	Cleans.
*/
void GenerateSolution::Clean()
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

/*
*	Generates the "RunCodeGeneration.bat" file.
*/
void GenerateSolution::GenerateRunCodeGenerationBatchFile(const ProjectSettings &project_settings, const bool for_pre_generation)
{
	//Add the "RunCodeGeneration.bat" file.
	std::ofstream run_code_generation_file{ DIRECTORY "\\RunCodeGeneration.bat" };

	if (for_pre_generation)
	{
		run_code_generation_file << "cd " DIRECTORY "\\" << std::endl;
	}

	run_code_generation_file << "\"../../../../Catalyst-Engine/Tools/Binaries/CatalystEngineCodeGenerator.exe\"";

	if (!project_settings._ScriptGameInclude.empty())
	{
		run_code_generation_file << " -script_generator_game_include " << project_settings._ScriptGameInclude.c_str();
	}

	run_code_generation_file.close();
}