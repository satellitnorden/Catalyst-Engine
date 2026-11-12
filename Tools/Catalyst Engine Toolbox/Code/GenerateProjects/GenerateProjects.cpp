//Header file.
#include <GenerateProjects/GenerateProjects.h>

//Core.
#include <Core/Utilities.h>

//Generate code.
#include <GenerateCode/GenerateCode.h>

//Generate projects.
#include <GenerateProjects/GenerateAudioPlugin.h>
#include <GenerateProjects/GenerateCMake.h>
#include <GenerateProjects/GenerateVisualStudioCodeWorkspace.h>
#include <GenerateProjects/GenerateVisualStudioSolution.h>

//STD.
#include <cstring>
#include <filesystem>
#include <fstream>

//Macros.
#define CHECK_ERROR_CODE() if (error_code) { std::cerr << "Error at " << __LINE__ << ": " << error_code.message() << std::endl; }

/*
*	Returns the CMake directory.
*/
std::string CMakeDirectory()
{
#if defined(PLATFORM_WIN64)
	return "Win64/CMake";
#elif defined(PLATFORM_LINUX)
	return "Linux/CMake";
#else
	#error "Unknown platform!"
#endif
}

/*
*	Returns the visual studio solution directory.
*/
std::string VisualStudioSolutionDirectory()
{
#if defined(PLATFORM_WIN64)
	return "Win64/Visual Studio";
#elif defined(PLATFORM_LINUX)
	return "Linux/Visual Studio";
#else
	#error "Unknown platform!"
#endif
}

/*
*	Returns the visual studio code workspace directory.
*/
std::string VisualStudioCodeWorkspaceDirectory()
{
#if defined(PLATFORM_WIN64)
	return "Win64/Visual Studio Code";
#elif defined(PLATFORM_LINUX)
	return "Linux/Visual Studio Code";
#else
	#error "Unknown platform!"
#endif
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
		preprocessor_definitions->emplace_back("CATALYST_HAS_VULKAN");
		preprocessor_definitions->emplace_back("CATALYST_HAS_ASSIMP");
		preprocessor_definitions->emplace_back("TRACY_ENABLE");
		preprocessor_definitions->emplace_back("TRACY_ON_DEMAND");
	}

	else if (strcmp(configuration_name, "Debug Editor") == 0)
	{
		preprocessor_definitions->emplace_back("_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_HAS_VULKAN");
		preprocessor_definitions->emplace_back("CATALYST_HAS_ASSIMP");
		preprocessor_definitions->emplace_back("CATALYST_EDITOR");
		preprocessor_definitions->emplace_back("TRACY_ENABLE");
		preprocessor_definitions->emplace_back("TRACY_ON_DEMAND");
	}

	else if (strcmp(configuration_name, "Profile") == 0)
	{
		preprocessor_definitions->emplace_back("NDEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_PROFILE");
		preprocessor_definitions->emplace_back("CATALYST_HAS_VULKAN");
		preprocessor_definitions->emplace_back("CATALYST_HAS_ASSIMP");
		preprocessor_definitions->emplace_back("TRACY_ENABLE");
		preprocessor_definitions->emplace_back("TRACY_ON_DEMAND");
	}

	else if (strcmp(configuration_name, "Profile Editor") == 0)
	{
		preprocessor_definitions->emplace_back("NDEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_PROFILE");
		preprocessor_definitions->emplace_back("CATALYST_HAS_VULKAN");
		preprocessor_definitions->emplace_back("CATALYST_HAS_ASSIMP");
		preprocessor_definitions->emplace_back("CATALYST_EDITOR");
		preprocessor_definitions->emplace_back("TRACY_ENABLE");
		preprocessor_definitions->emplace_back("TRACY_ON_DEMAND");
	}

	else if (strcmp(configuration_name, "Final") == 0)
	{
		preprocessor_definitions->emplace_back("NDEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_FINAL");
		preprocessor_definitions->emplace_back("CATALYST_HAS_VULKAN");
	}

	else if (strcmp(configuration_name, "Debug Static") == 0)
	{
		preprocessor_definitions->emplace_back("_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_HAS_VULKAN");
		preprocessor_definitions->emplace_back("CATALYST_HAS_ASSIMP");
		preprocessor_definitions->emplace_back("TRACY_ENABLE");
		preprocessor_definitions->emplace_back("TRACY_ON_DEMAND");
	}

	else if (strcmp(configuration_name, "Profile Static") == 0)
	{
		preprocessor_definitions->emplace_back("NDEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_PROFILE");
		preprocessor_definitions->emplace_back("CATALYST_HAS_VULKAN");
		preprocessor_definitions->emplace_back("CATALYST_HAS_ASSIMP");
		preprocessor_definitions->emplace_back("TRACY_ENABLE");
		preprocessor_definitions->emplace_back("TRACY_ON_DEMAND");
	}

	else if (strcmp(configuration_name, "Final Static") == 0)
	{
		preprocessor_definitions->emplace_back("NDEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_FINAL");
		preprocessor_definitions->emplace_back("CATALYST_HAS_VULKAN");
	}

	else if (strcmp(configuration_name, "Standalone Debug") == 0)
	{
		preprocessor_definitions->emplace_back("_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_HAS_VULKAN");
		preprocessor_definitions->emplace_back("CATALYST_HAS_OPEN_GL");
		preprocessor_definitions->emplace_back("CATALYST_HAS_ASSIMP");
		preprocessor_definitions->emplace_back("TRACY_ENABLE");
		preprocessor_definitions->emplace_back("TRACY_ON_DEMAND");
	}

	else if (strcmp(configuration_name, "Standalone Profile") == 0)
	{
		preprocessor_definitions->emplace_back("NDEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_PROFILE");
		preprocessor_definitions->emplace_back("CATALYST_HAS_VULKAN");
		preprocessor_definitions->emplace_back("CATALYST_HAS_OPEN_GL");
		preprocessor_definitions->emplace_back("CATALYST_HAS_ASSIMP");
		preprocessor_definitions->emplace_back("TRACY_ENABLE");
		preprocessor_definitions->emplace_back("TRACY_ON_DEMAND");
	}

	else if (strcmp(configuration_name, "Standalone Final") == 0)
	{
		preprocessor_definitions->emplace_back("NDEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_FINAL");
		preprocessor_definitions->emplace_back("CATALYST_HAS_VULKAN");
		preprocessor_definitions->emplace_back("CATALYST_HAS_OPEN_GL");
	}

	else if (strcmp(configuration_name, "CLAP Debug") == 0)
	{
		preprocessor_definitions->emplace_back("_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_HAS_OPEN_GL");
	}

	else if (strcmp(configuration_name, "CLAP Final") == 0)
	{
		preprocessor_definitions->emplace_back("NDEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_FINAL");
		preprocessor_definitions->emplace_back("CATALYST_HAS_OPEN_GL");
	}

	else if (strcmp(configuration_name, "VST3 Debug") == 0)
	{
		preprocessor_definitions->emplace_back("_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_DEBUG");
		preprocessor_definitions->emplace_back("CATALYST_HAS_OPEN_GL");
	}

	else if (strcmp(configuration_name, "VST3 Final") == 0)
	{
		preprocessor_definitions->emplace_back("NDEBUG");
		preprocessor_definitions->emplace_back("CATALYST_CONFIGURATION_FINAL");
		preprocessor_definitions->emplace_back("CATALYST_HAS_OPEN_GL");
	}

	else
	{
		std::cerr << "Unknown configuration type " << configuration_name << "!" << std::endl;
	}

	//Add platform-specific preprocessor definitions.
#if defined(PLATFORM_WIN64)
	preprocessor_definitions->emplace_back("__WINDOWS_WASAPI__");
	preprocessor_definitions->emplace_back("__WINDOWS_ASIO__");
	preprocessor_definitions->emplace_back("__WINDOWS_ASIO__");
	preprocessor_definitions->emplace_back("__WINDOWS_MM__");
	preprocessor_definitions->emplace_back("CATALYST_PLATFORM_WINDOWS");
#endif

#if defined(PLATFORM_LINUX)
	preprocessor_definitions->emplace_back("CATALYST_PLATFORM_LINUX");
#endif

	//Add global preprocessor definitions.
	preprocessor_definitions->emplace_back("CATALYST_DISTRIBUTION_NULL");
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
		library_includes->emplace_back("vulkan-1.lib");
		library_includes->emplace_back("Debug/shaderc_shared.lib");
		library_includes->emplace_back("Debug/libsoundwave.lib");
		library_includes->emplace_back("Debug/assimp.lib");
	}

	else if (strcmp(configuration_name, "Debug Editor") == 0)
	{
		library_includes->emplace_back("vulkan-1.lib");
		library_includes->emplace_back("Debug/shaderc_shared.lib");
		library_includes->emplace_back("Debug/libsoundwave.lib");
		library_includes->emplace_back("Debug/assimp.lib");
	}

	else if (strcmp(configuration_name, "Profile") == 0)
	{
		library_includes->emplace_back("vulkan-1.lib");
		library_includes->emplace_back("Profile/shaderc_shared.lib");
		library_includes->emplace_back("Profile/libsoundwave.lib");
		library_includes->emplace_back("Profile/assimp.lib");
	}

	else if (strcmp(configuration_name, "Profile Editor") == 0)
	{
		library_includes->emplace_back("vulkan-1.lib");
		library_includes->emplace_back("Profile/shaderc_shared.lib");
		library_includes->emplace_back("Profile/libsoundwave.lib");
		library_includes->emplace_back("Profile/assimp.lib");
	}

	else if (strcmp(configuration_name, "Final") == 0)
	{
		library_includes->emplace_back("vulkan-1.lib");
		library_includes->emplace_back("Final/libsoundwave.lib");
	}

	else if (strcmp(configuration_name, "Debug Static") == 0)
	{
		library_includes->emplace_back("vulkan-1.lib");
		library_includes->emplace_back("Debug/shaderc_shared.lib");
		library_includes->emplace_back("Debug/libsoundwave.lib");
		library_includes->emplace_back("Debug/assimp.lib");
	}

	else if (strcmp(configuration_name, "Profile Static") == 0)
	{
		library_includes->emplace_back("vulkan-1.lib");
		library_includes->emplace_back("Profile/shaderc_shared.lib");
		library_includes->emplace_back("Profile/libsoundwave.lib");
		library_includes->emplace_back("Profile/assimp.lib");
	}

	else if (strcmp(configuration_name, "Final Static") == 0)
	{
		library_includes->emplace_back("vulkan-1.lib");
		library_includes->emplace_back("Final/libsoundwave.lib");
	}

	else if (strcmp(configuration_name, "Standalone Debug") == 0)
	{
		library_includes->emplace_back("vulkan-1.lib");
		library_includes->emplace_back("opengl32.lib");
		library_includes->emplace_back("Debug/shaderc_shared.lib");
		library_includes->emplace_back("Debug/libsoundwave.lib");
		library_includes->emplace_back("Debug/assimp.lib");
	}

	else if (strcmp(configuration_name, "Standalone Profile") == 0)
	{
		library_includes->emplace_back("vulkan-1.lib");
		library_includes->emplace_back("opengl32.lib");
		library_includes->emplace_back("Profile/shaderc_shared.lib");
		library_includes->emplace_back("Profile/libsoundwave.lib");
		library_includes->emplace_back("Profile/assimp.lib");
	}

	else if (strcmp(configuration_name, "Standalone Final") == 0)
	{
		library_includes->emplace_back("vulkan-1.lib");
		library_includes->emplace_back("opengl32.lib");
		library_includes->emplace_back("Final/libsoundwave.lib");
	}

	else if (strcmp(configuration_name, "CLAP Debug") == 0)
	{
		library_includes->emplace_back("opengl32.lib");
		library_includes->emplace_back("Debug/shaderc_shared.lib");
		library_includes->emplace_back("Debug/libsoundwave.lib");
	}

	else if (strcmp(configuration_name, "CLAP Final") == 0)
	{
		library_includes->emplace_back("opengl32.lib");
		library_includes->emplace_back("Final/libsoundwave.lib");
	}

	else if (strcmp(configuration_name, "VST3 Debug") == 0)
	{
		library_includes->emplace_back("opengl32.lib");
		library_includes->emplace_back("Debug/shaderc_shared.lib");
		library_includes->emplace_back("Debug/libsoundwave.lib");
	}

	else if (strcmp(configuration_name, "VST3 Final") == 0)
	{
		library_includes->emplace_back("opengl32.lib");
		library_includes->emplace_back("Final/libsoundwave.lib");
	}

	else
	{
		std::cerr << "Unknown configuration type " << configuration_name << "!" << std::endl;
	}

	//Add global library includes.
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
void ScanDirectory(GenerateProjectParameters::Directory *directory, const char *path)
{
	if (!std::filesystem::exists(path))
	{
		return;
	}

	//Set the path.
	directory->_Path = std::string("../../") + path;

	for (auto &entry : std::filesystem::directory_iterator(path))
	{
		const std::string entry_file_path{ entry.path().generic_string() };

		if (entry.is_directory())
		{
			GenerateProjectParameters::Directory &new_directory{ directory->_Directories.emplace_back() };
			new_directory._Name = entry_file_path.substr(entry_file_path.find_last_of('/') + 1);
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
				GenerateProjectParameters::File &new_file{ directory->_Files.emplace_back() };

				new_file._Path = std::string("../../") + entry_file_path;
				new_file._EnableWarnings = true;
			}
		}
	}
}

/*
*	Returns whether or not the given third party directory should be included.
*/
bool ShouldIncludeThirdPartyDirectory(const ProjectSettings& project_settings, const std::string &path)
{
	if (path == "../../Catalyst-Engine/Engine/Third Party/Jolt")
	{
		return project_settings._Physics == ProjectSettings::Physics::JOLT;
	}

	return true;
}

/*
*	Returns whether or not the given third party file should be included.
*/
bool ShouldIncludeThirdPartyFile(const ProjectSettings &project_settings, const std::string &path)
{
#if defined(PLATFORM_LINUX)
	if (path == "../../Catalyst-Engine/Engine/Third Party/minizip/minizip/iowin32.h")
	{
		return false;
	}

	if (path == "../../Catalyst-Engine/Engine/Third Party/minizip/iowin32.c")
	{
		return false;
	}
#endif

	return true;
}

/*
*	Scans the given third party directory.
*/
void ScanThirdPartyDirectory(const ProjectSettings &project_settings, GenerateProjectParameters *parameters, GenerateProjectParameters::Directory *directory, const char *path, const bool is_top_level)
{
	if (!std::filesystem::exists(path))
	{
		return;
	}

	//Set the path.
	directory->_Path = std::string("../../") + path;

	for (auto &entry : std::filesystem::directory_iterator(path))
	{
		const std::string entry_file_path{ entry.path().generic_string() };

		if (entry.is_directory())
		{
			if (ShouldIncludeThirdPartyDirectory(project_settings, entry_file_path))
			{
				if (is_top_level)
				{
					parameters->_CodeIncludePaths.emplace_back(std::string("../../") + entry_file_path);
				}

				GenerateProjectParameters::Directory &new_directory{ directory->_Directories.emplace_back() };
				new_directory._Name = entry_file_path.substr(entry_file_path.find_last_of('/') + 1);
				ScanThirdPartyDirectory(project_settings, parameters, &new_directory, entry_file_path.c_str(), false);
			}
		}

		else if (ShouldIncludeThirdPartyFile(project_settings, entry_file_path))
		{
			const std::string extension{ entry_file_path.substr(entry_file_path.find_last_of('.')) };

			if (extension == ".h"
				|| extension == ".inl"
				|| extension == ".cpp"
				|| extension == ".c")
			{
				GenerateProjectParameters::File &new_file{ directory->_Files.emplace_back() };

				new_file._Path = std::string("../../") + entry_file_path;
				new_file._EnableWarnings = false;
			}
		}
	}
}

/*
*	Generates the "RunCodeGeneration.bat" file.
*/
void GenerateRunCodeGenerationBatchFile(const std::string &directory, const ProjectSettings &project_settings)
{
	//Add the "RunCodeGeneration.bat" file.
	std::ofstream run_code_generation_file{ directory + "\\RunCodeGeneration.bat" };

	run_code_generation_file << "\"..\\..\\..\\..\\Catalyst-Engine\\Tools\\Binaries\\Catalyst Engine Toolbox.exe\" generate_code";

	run_code_generation_file.close();
}

/*
*	Adds common parameters.
*/
void AddCommonParameters(const ProjectSettings &project_settings, GenerateProjectParameters *const parameters)
{
	//Fill in the core code include paths.
	parameters->_CodeIncludePaths.emplace_back("../../../../Catalyst-Engine/Engine/Include");
	parameters->_CodeIncludePaths.emplace_back("../../../../Catalyst-Engine/Engine/Third Party");
	parameters->_CodeIncludePaths.emplace_back("../../../Code/Include");

	//Fill in the third party code include paths.
	parameters->_CodeIncludePaths.emplace_back("../../../../Catalyst-Engine/Engine/Include/ThirdParty/imguinodeeditor");

	//Fill in the library include paths.
#if defined(PLATFORM_WIN64)
	parameters->_LibraryIncludePaths.emplace_back("../../../../Catalyst-Engine/Engine/Libraries/Win64/Static");
#endif

#if defined(PLATFORM_LINUX)
	parameters->_LibraryIncludePaths.emplace_back("../../../../Catalyst-Engine/Engine/Libraries/Linux/Static");
#endif

	//Fill in the directories.
	{
		GenerateProjectParameters::Directory &directory{ parameters->_Directories.emplace_back() };
		directory._Name = "Catalyst Engine Include";
		ScanDirectory(&directory, "../../Catalyst-Engine/Engine/Include");
	}

	{
		GenerateProjectParameters::Directory &directory{ parameters->_Directories.emplace_back() };
		directory._Name = "Catalyst Engine Third Party";
		ScanThirdPartyDirectory(project_settings, parameters, &directory, "../../Catalyst-Engine/Engine/Third Party", true);
	}

	{
		GenerateProjectParameters::Directory &directory{ parameters->_Directories.emplace_back() };
		directory._Name = "Catalyst Engine Source";
		ScanDirectory(&directory, "../../Catalyst-Engine/Engine/Source");
	}

	{
		GenerateProjectParameters::Directory &directory{ parameters->_Directories.emplace_back() };
		directory._Name = "Catalyst Engine Rendering";
		ScanDirectory(&directory, "../../Catalyst-Engine/Engine/Rendering");
	}

	{
		GenerateProjectParameters::Directory &directory{ parameters->_Directories.emplace_back() };
		directory._Name = project_settings._ProjectName + std::string(" Include");
		ScanDirectory(&directory, "../Code/Include");
	}

	{
		GenerateProjectParameters::Directory &directory{ parameters->_Directories.emplace_back() };
		directory._Name = project_settings._ProjectName + std::string(" Source");
		ScanDirectory(&directory, "../Code/Source");
	}

	{
		GenerateProjectParameters::Directory &directory{ parameters->_Directories.emplace_back() };
		directory._Name = project_settings._ProjectName + std::string(" Rendering");
		ScanDirectory(&directory, "../Rendering");
	}
}

/*
*	Generates the default projects with the given parameters.
*/
void GenerateDefaultProjects(const ProjectSettings &project_settings)
{
	//Figure out the project name.
	char project_name[256];
	PRINT_BUFFER(project_name, "%s_Win64", project_settings._ProjectNameNoSpaces.c_str());

	//Set up the parameters.
	GenerateProjectParameters parameters;

	parameters._DirectoryPath = VisualStudioSolutionDirectory();
	parameters._ProjectName = project_name;

	{
		GenerateProjectParameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "Debug";
		configuration._Type = GenerateProjectParameters::Configuration::Type::APPLICATION;
		configuration._TargetFileExtension = ".exe";
		configuration._ApplyOptimizations = false;
		configuration._UseDebugLibraries = true;
		configuration._RuntimeLibrary = "MultiThreadedDebug";
		configuration._SubSystem = "Windows";
		AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
		AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
		configuration._PreBuildEvent = "call RunCodeGeneration.bat";
	}

	{
		GenerateProjectParameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "Debug Editor";
		configuration._Type = GenerateProjectParameters::Configuration::Type::APPLICATION;
		configuration._TargetFileExtension = ".exe";
		configuration._ApplyOptimizations = false;
		configuration._UseDebugLibraries = true;
		configuration._RuntimeLibrary = "MultiThreadedDebug";
		configuration._SubSystem = "Windows";
		AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
		AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
		configuration._PreBuildEvent = "call RunCodeGeneration.bat";
	}

	{
		GenerateProjectParameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "Profile";
		configuration._Type = GenerateProjectParameters::Configuration::Type::APPLICATION;
		configuration._TargetFileExtension = ".exe";
		configuration._ApplyOptimizations = true;
		configuration._UseDebugLibraries = false;
		configuration._RuntimeLibrary = "MultiThreaded";
		configuration._SubSystem = "Windows";
		AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
		AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
		configuration._PreBuildEvent = "call RunCodeGeneration.bat";
	}

	{
		GenerateProjectParameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "Profile Editor";
		configuration._Type = GenerateProjectParameters::Configuration::Type::APPLICATION;
		configuration._TargetFileExtension = ".exe";
		configuration._ApplyOptimizations = true;
		configuration._UseDebugLibraries = false;
		configuration._RuntimeLibrary = "MultiThreaded";
		configuration._SubSystem = "Windows";
		AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
		AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
		configuration._PreBuildEvent = "call RunCodeGeneration.bat";
	}

	{
		GenerateProjectParameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "Final";
		configuration._Type = GenerateProjectParameters::Configuration::Type::APPLICATION;
		configuration._TargetFileExtension = ".exe";
		configuration._ApplyOptimizations = true;
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
			GenerateProjectParameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

			configuration._Name = "Debug Static";
			configuration._Type = GenerateProjectParameters::Configuration::Type::STATIC_LIBRARY;
			configuration._TargetFileExtension = ".lib";
			configuration._ApplyOptimizations = false;
			configuration._UseDebugLibraries = true;
			configuration._RuntimeLibrary = "MultiThreadedDebug";
			configuration._SubSystem = "Windows";
			AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
			AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
			configuration._PreBuildEvent = "call RunCodeGeneration.bat";
		}

		{
			GenerateProjectParameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

			configuration._Name = "Profile Static";
			configuration._Type = GenerateProjectParameters::Configuration::Type::STATIC_LIBRARY;
			configuration._TargetFileExtension = ".lib";
			configuration._ApplyOptimizations = true;
			configuration._UseDebugLibraries = false;
			configuration._RuntimeLibrary = "MultiThreaded";
			configuration._SubSystem = "Windows";
			AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
			AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
			configuration._PreBuildEvent = "call RunCodeGeneration.bat";
		}

		{
			GenerateProjectParameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

			configuration._Name = "Final Static";
			configuration._Type = GenerateProjectParameters::Configuration::Type::STATIC_LIBRARY;
			configuration._TargetFileExtension = ".lib";
			configuration._ApplyOptimizations = true;
			configuration._UseDebugLibraries = false;
			configuration._RuntimeLibrary = "MultiThreaded";
			configuration._SubSystem = "Windows";
			AddPreprocessorDefinitions(project_settings, configuration._Name, &configuration._PreprocessorDefinitions);
			AddLibraryIncludes(project_settings, configuration._Name, &configuration._LibraryIncludes);
			configuration._PreBuildEvent = "call RunCodeGeneration.bat";
		}
	}

	{
		GenerateProjectParameters::Platform &platform{ parameters._Platforms.emplace_back() };

		platform._Name = "x64";
	}

	//Add common paramaters.
	AddCommonParameters(project_settings, &parameters);

	//Generate the code generation file.
	GenerateRunCodeGenerationBatchFile(VisualStudioSolutionDirectory(), project_settings);

	//Generate the CMake file.
	{
		parameters._DirectoryPath = CMakeDirectory();
		GenerateCMake::Generate(parameters);
	}

	//Generate the visual studio code workspace.
	{
		parameters._DirectoryPath = VisualStudioCodeWorkspaceDirectory();
		GenerateVisualStudioCodeWorkspace::Generate(parameters);

		//Remember the error code for filesystem functions.
		std::error_code error_code;

		//Create the directories for the different configurations.
		for (const GenerateProjectParameters::Configuration &configuration : parameters._Configurations)
		{
			std::filesystem::create_directories(std::string(parameters._DirectoryPath + std::string("/Build/") + Utilities::RemoveSpaces(configuration._Name)).c_str(), error_code); CHECK_ERROR_CODE();
		}

		//Copy assimp-vc143-mt.dll.
		std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Debug/assimp.dll", std::string(parameters._DirectoryPath + std::string("/Build/Debug")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Debug/assimp.dll", std::string(parameters._DirectoryPath + std::string("/Build/DebugEditor")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Profile/assimp.dll", std::string(parameters._DirectoryPath + std::string("/Build/Profile")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Profile/assimp.dll", std::string(parameters._DirectoryPath + std::string("/Build/ProfileEditor")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Final/assimp.dll", std::string(parameters._DirectoryPath + std::string("/Build/Final")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();

		if (project_settings._AddStaticBuilds)
		{
			std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Debug/assimp.dll", std::string(parameters._DirectoryPath + std::string("/Build/DebugStatic")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Profile/assimp.dll", std::string(parameters._DirectoryPath + std::string("/Build/ProfileStatic")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		}

		//Copy onnxruntime.dll.
		if (project_settings._IncludeONNXRuntimeLibrary)
		{
			for (const GenerateProjectParameters::Configuration &configuration : parameters._Configurations)
			{
				std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/onnxruntime.dll", std::string(parameters._DirectoryPath + std::string("/Build/") + Utilities::RemoveSpaces(configuration._Name)).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			}
		}
	}

	//Generate the visual studio solution.
#if defined(VISUAL_STUDIO_SOLUTION_SUPPORTED)
	{
		parameters._DirectoryPath = VisualStudioSolutionDirectory();
		GenerateVisualStudioSolution::Generate(parameters);

		//Remember the error code for filesystem functions.
		std::error_code error_code;

		//Create the directories for the different configurations.
		for (const GenerateProjectParameters::Configuration &configuration : parameters._Configurations)
		{
			std::filesystem::create_directories(std::string(parameters._DirectoryPath + std::string("/") + configuration._Name).c_str(), error_code); CHECK_ERROR_CODE();
		}

		//Copy assimp-vc143-mt.dll.
		std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Debug/assimp.dll", std::string(parameters._DirectoryPath + std::string("/Debug")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Debug/assimp.dll", std::string(parameters._DirectoryPath + std::string("/Debug Editor")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Profile/assimp.dll", std::string(parameters._DirectoryPath + std::string("/Profile")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Profile/assimp.dll", std::string(parameters._DirectoryPath + std::string("/Profile Editor")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Final/assimp.dll", std::string(parameters._DirectoryPath + std::string("/Final")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();

		if (project_settings._AddStaticBuilds)
		{
			std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Debug/assimp.dll", std::string(parameters._DirectoryPath + std::string("/Debug Static")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Profile/assimp.dll", std::string(parameters._DirectoryPath + std::string("/Profile Static")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		}

		//Copy onnxruntime.dll.
		if (project_settings._IncludeONNXRuntimeLibrary)
		{
			for (const GenerateProjectParameters::Configuration &configuration : parameters._Configurations)
			{
				std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/onnxruntime.dll", std::string(parameters._DirectoryPath + std::string("/") + configuration._Name).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			}
		}
	}
#endif
}

/*
*	Generates an audio plugin projects with the given parameters.
*/
void GenerateAudioPluginProjects(const ProjectSettings &project_settings, const ProjectSettings::AudioPlugin &audio_plugin)
{
	//Set up the parameters.
	GenerateProjectParameters parameters;

	parameters._ProjectName = audio_plugin._Name.c_str();

	{
		GenerateProjectParameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "Standalone Debug";
		configuration._Type = GenerateProjectParameters::Configuration::Type::APPLICATION;
		configuration._TargetFileExtension = ".exe";
		configuration._ApplyOptimizations = false;
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
		GenerateProjectParameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "Standalone Profile";
		configuration._Type = GenerateProjectParameters::Configuration::Type::APPLICATION;
		configuration._TargetFileExtension = ".exe";
		configuration._ApplyOptimizations = true;
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
		GenerateProjectParameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "Standalone Final";
		configuration._Type = GenerateProjectParameters::Configuration::Type::APPLICATION;
		configuration._TargetFileExtension = ".exe";
		configuration._ApplyOptimizations = true;
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
		GenerateProjectParameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "CLAP Debug";
		configuration._Type = GenerateProjectParameters::Configuration::Type::DYNAMIC_LIBRARY;
		configuration._TargetFileExtension = ".clap";
		configuration._ApplyOptimizations = false;
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
		GenerateProjectParameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "CLAP Final";
		configuration._Type = GenerateProjectParameters::Configuration::Type::DYNAMIC_LIBRARY;
		configuration._TargetFileExtension = ".clap";
		configuration._ApplyOptimizations = true;
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
		GenerateProjectParameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "VST3 Debug";
		configuration._Type = GenerateProjectParameters::Configuration::Type::DYNAMIC_LIBRARY;
		configuration._TargetFileExtension = ".vst3";
		configuration._ApplyOptimizations = false;
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
		GenerateProjectParameters::Configuration &configuration{ parameters._Configurations.emplace_back() };

		configuration._Name = "VST3 Final";
		configuration._Type = GenerateProjectParameters::Configuration::Type::DYNAMIC_LIBRARY;
		configuration._TargetFileExtension = ".vst3";
		configuration._ApplyOptimizations = true;
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
		GenerateProjectParameters::Platform &platform{ parameters._Platforms.emplace_back() };

		platform._Name = "x64";
	}

	//Generate the audio plugin files.
	GenerateAudioPlugin::GenerateFiles(project_settings, audio_plugin);

	//Add the code include path.
	parameters._CodeIncludePaths.emplace_back(std::string("../../") + GenerateAudioPlugin::CodeDirectory(audio_plugin));

	//Add the audio plugin directory.
	{
		GenerateProjectParameters::Directory &directory{ parameters._Directories.emplace_back() };
		directory._Name = audio_plugin._NameNoSpaces;
		ScanDirectory(&directory, GenerateAudioPlugin::CodeDirectory(audio_plugin).c_str());
	}

	//Add common paramaters.
	AddCommonParameters(project_settings, &parameters);

	//Generate the code generation file.
	GenerateRunCodeGenerationBatchFile(GenerateAudioPlugin::VisualStudioSolutionDirectory(audio_plugin), project_settings);

	//Embed some resources.
	parameters._EmbeddedResources.emplace_back("../../../../Catalyst-Engine/Engine/Content/Collections/Base.cac");

	{
		const std::string collections_directory{ GenerateAudioPlugin::ContentDirectory(audio_plugin) + "/Collections" };

		if (std::filesystem::exists(collections_directory))
		{
			for (const auto &entry : std::filesystem::directory_iterator(collections_directory))
			{
				const std::string file_path{ entry.path().generic_string() };

				if (file_path.ends_with(".cac"))
				{
					parameters._EmbeddedResources.emplace_back("../../" + file_path);
				}
			}
		}
	}

	//Define that we have embedded resource.
	for (GenerateProjectParameters::Configuration &configuration : parameters._Configurations)
	{
		configuration._PreprocessorDefinitions.emplace_back("CATALYST_HAS_EMBEDDED_RESOURCES");
	}

	//Generate the CMake file.
	{
		parameters._DirectoryPath = GenerateAudioPlugin::CMakeDirectory(audio_plugin);
		GenerateCMake::Generate(parameters);

		//Remember the error code for filesystem functions.
		std::error_code error_code;

		//Create the directories for the different configurations.
		for (const GenerateProjectParameters::Configuration &configuration : parameters._Configurations)
		{
			std::filesystem::create_directories(std::string(parameters._DirectoryPath + std::string("/") + Utilities::RemoveSpaces(configuration._Name)).c_str(), error_code); CHECK_ERROR_CODE();
		}

		//Copy assimp-vc143-mt.dll.
		std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Debug/assimp.dll", std::string(parameters._DirectoryPath + std::string("/StandaloneDebug")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Profile/assimp.dll", std::string(parameters._DirectoryPath + std::string("/StandaloneProfile")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Final/assimp.dll", std::string(parameters._DirectoryPath + std::string("/StandaloneFinal")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
	}

#if VISUAL_STUDIO_CODE_WORKSPACE_SUPPORTED
	//Generate the visual studio code workspace.
	{
		parameters._DirectoryPath = GenerateAudioPlugin::VisualStudioCodeWorkspaceDirectory(audio_plugin);
		GenerateVisualStudioCodeWorkspace::Generate(parameters);

		//Remember the error code for filesystem functions.
		std::error_code error_code;

		//Create the directories for the different configurations.
		for (const GenerateProjectParameters::Configuration &configuration : parameters._Configurations)
		{
			std::filesystem::create_directories(std::string(parameters._DirectoryPath + std::string("/Build/") + Utilities::RemoveSpaces(configuration._Name)).c_str(), error_code); CHECK_ERROR_CODE();
		}

		//Copy assimp-vc143-mt.dll.
		std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Debug/assimp.dll", std::string(parameters._DirectoryPath + std::string("/Build/StandaloneDebug")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Profile/assimp.dll", std::string(parameters._DirectoryPath + std::string("/Build/StandaloneProfile")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Final/assimp.dll", std::string(parameters._DirectoryPath + std::string("/Build/StandaloneFinal")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
	}
#endif

	//Generate the visual studio solution.
#if defined(VISUAL_STUDIO_SOLUTION_SUPPORTED)
	{
		parameters._DirectoryPath = GenerateAudioPlugin::VisualStudioSolutionDirectory(audio_plugin);
		GenerateVisualStudioSolution::Generate(parameters);

		//Remember the error code for filesystem functions.
		std::error_code error_code;

		//Create the directories for the different configurations.
		for (const GenerateProjectParameters::Configuration &configuration : parameters._Configurations)
		{
			std::filesystem::create_directories(std::string(parameters._DirectoryPath + std::string("/") + configuration._Name).c_str(), error_code); CHECK_ERROR_CODE();
		}

		//Copy assimp-vc143-mt.dll.
		std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Debug/assimp.dll", std::string(GenerateAudioPlugin::VisualStudioSolutionDirectory(audio_plugin) + std::string("/Standalone Debug")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Profile/assimp.dll", std::string(GenerateAudioPlugin::VisualStudioSolutionDirectory(audio_plugin) + std::string("/Standalone Profile")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Win64/Dynamic/Final/assimp.dll", std::string(GenerateAudioPlugin::VisualStudioSolutionDirectory(audio_plugin) + std::string("/Standalone Final")).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();

		//Copy onnxruntime.dll.
		if (project_settings._IncludeONNXRuntimeLibrary)
		{
			for (const GenerateProjectParameters::Configuration &configuration : parameters._Configurations)
			{
				std::filesystem::copy("../../Catalyst-Engine/Engine/Libraries/Dynamic/onnxruntime.dll", std::string(parameters._DirectoryPath + std::string("/") + configuration._Name).c_str(), std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			}
		}
	}
#endif
}

/*
*	Generates the projects with the given project settings.
*/
void GenerateProjects::Run(const ProjectSettings &project_settings)
{
	//Clean, first of all. (:
	Clean(project_settings);

	//Ensure the directories exists.
	std::filesystem::create_directories(VisualStudioSolutionDirectory());

	for (const ProjectSettings::AudioPlugin &audio_plugin : project_settings._AudioPlugins)
	{
		std::filesystem::create_directories(GenerateAudioPlugin::VisualStudioSolutionDirectory(audio_plugin));
	}

	//Run code generation once.
	{
		//Remember the current working directory.
		const std::filesystem::path current_working_directory{ std::filesystem::current_path() };

		//Set the temporary working directory.
		std::filesystem::current_path(VisualStudioSolutionDirectory());
		
		//Run code generation!
		GenerateCode::Run(project_settings);

		//Restore the current working directory.
		std::filesystem::current_path(current_working_directory);
	}

	//Generate the default projects.
	GenerateDefaultProjects(project_settings);

	//Generate projects for each audio plugin.
	for (const ProjectSettings::AudioPlugin &audio_plugin : project_settings._AudioPlugins)
	{
		//Generate the projects!
		GenerateAudioPluginProjects(project_settings, audio_plugin);
	}
}

/*
*	Cleans.
*/
void GenerateProjects::Clean(const ProjectSettings &project_settings)
{
	//Define constants.
	constexpr const char *CODE_GENERATION_DIRECTORY_PATH{ "../Code/CodeGeneration" };

	//Remember the error code for filesystem functions.
	std::error_code error_code;

	//Remove (almost) all files in the directory.
	if (std::filesystem::exists("Win64"))
	{
		std::filesystem::remove_all("Win64", error_code); CHECK_ERROR_CODE()
	}

	for (const ProjectSettings::AudioPlugin &audio_plugin : project_settings._AudioPlugins)
	{
		if (std::filesystem::exists(GenerateAudioPlugin::CMakeDirectory(audio_plugin)))
		{
			std::filesystem::remove_all(GenerateAudioPlugin::CMakeDirectory(audio_plugin), error_code); CHECK_ERROR_CODE()
		}

		if (std::filesystem::exists(GenerateAudioPlugin::VisualStudioSolutionDirectory(audio_plugin)))
		{
			std::filesystem::remove_all(GenerateAudioPlugin::VisualStudioSolutionDirectory(audio_plugin), error_code); CHECK_ERROR_CODE()
		}

		if (std::filesystem::exists(GenerateAudioPlugin::VisualStudioCodeWorkspaceDirectory(audio_plugin)))
		{
			std::filesystem::remove_all(GenerateAudioPlugin::VisualStudioCodeWorkspaceDirectory(audio_plugin), error_code); CHECK_ERROR_CODE()
		}
	}

	//Remove all the code generation caches.
	if (std::filesystem::exists(CODE_GENERATION_DIRECTORY_PATH))
	{
		for (const auto &entry : std::filesystem::directory_iterator(std::string(CODE_GENERATION_DIRECTORY_PATH)))
		{
			const std::string file_path{ entry.path().generic_string() };

			if (file_path != (CODE_GENERATION_DIRECTORY_PATH + std::string("/.gitignore")))
			{
				std::filesystem::remove(file_path, error_code); CHECK_ERROR_CODE()
			}
		}
	}
}