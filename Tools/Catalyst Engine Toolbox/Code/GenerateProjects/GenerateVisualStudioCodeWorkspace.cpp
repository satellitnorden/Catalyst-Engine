//Header file.
#include <GenerateProjects/GenerateVisualStudioCodeWorkspace.h>

//Core.
#include <Core/Utilities.h>

#if VISUAL_STUDIO_CODE_WORKSPACE_SUPPORTED
//Generate projects.
#include <GenerateProjects/GenerateCMake.h>
#include <GenerateProjects/GenerateCopyDynamicLibraries.h>

//Third party.
#include <ThirdParty/json.hpp>

//STD.
#include <filesystem>
#include <fstream>

/*
*	Generates!
*/
void GenerateVisualStudioCodeWorkspace::Generate(const ProjectSettings &project_settings, const GenerateProjectParameters &parameters)
{
    //Ensure the directories exists.
    std::filesystem::create_directories(parameters._DirectoryPath);
    std::filesystem::create_directories(parameters._DirectoryPath + std::string("/.vscode"));

    //Generate CMake first, as Visual Studio Code will use that.
    GenerateCMake::Generate(parameters);

    //Generate the '.code-workspace' file.
    GenerateCodeWorkspaceFile(parameters);

    //Generate the 'CMakePresets.json' file.
    GenerateCMakePresetsFile(parameters);

    //Generate the 'settings.json' file.
    GenerateSettingsFile(parameters);

    //Generate the 'extensions.json' file.
    GenerateExtensionsFile(parameters);

    //Generate the 'tasks.json' file.
    GenerateTasksFile(parameters);

    //Generate the 'launch.json' file.
    GenerateLaunchFile(parameters);

    //Copy the dynamic libraries.
    GenerateCopyDynamicLibraries::Generate(project_settings, parameters, false, "Build");
}

/*
*	Generates the '.code-workspace' file.
*/
void GenerateVisualStudioCodeWorkspace::GenerateCodeWorkspaceFile(const GenerateProjectParameters &parameters)
{
    //Set up the JSON.
    nlohmann::ordered_json JSON;

    //Add the folders entry.
    {
        nlohmann::ordered_json &folders_entry{ JSON["folders"] };

        //Add the "Workspace" folder.
        {
            nlohmann::ordered_json &folder_entry{ folders_entry.emplace_back() };

            folder_entry["name"] = "Workspace";
            folder_entry["path"] = ".";
        }

        //Add the directories.
        for (const GenerateProjectParameters::Directory &directory : parameters._Directories)
        {
            nlohmann::ordered_json &folder_entry{ folders_entry.emplace_back() };

            folder_entry["name"] = directory._Name;
            folder_entry["path"] = directory._Path;
        }
    }

    //Add the settings entry.
    {
        nlohmann::ordered_json &settings_entry{ JSON["settings"] };

        //Add the configuration provider.
    #if defined(PLATFORM_Win64)
        settings_entry["C_Cpp.default.configurationProvider"] = "ms-vscode.cmake-tools";
    #elif defined(PLATFORM_LINUX)
        settings_entry["clangd.arguments"].emplace_back("--compile-commands-dir=${workspaceFolder}/Build");
    #else
        #errror "Unknown platform!";
    #endif

        //Set debug options.
        settings_entry["debug.allowBreakpointsEverywhere"] = true;

        //Set CMake options.
        settings_entry["cmake.options.statusBarVisibility"] = "visible";
        settings_entry["cmake.sourceDirectory"] = "${workspaceFolder}";
        settings_entry["cmake.autoSelectActiveFolder"] = false;
        settings_entry["cmake.setBuildTargetSameAsLaunchTarget"] = true;
    }

    //Figure out the file path.
    char file_path[256];
    PRINT_BUFFER(file_path, "%s/%s.code-workspace", parameters._DirectoryPath.c_str(), parameters._ProjectName);

    //Open the file.
    std::ofstream file{ file_path };

    //Write the JSON.
    file << std::setw(4) << JSON;

    //Close the file.
    file.close();
}

/*
*	Generates the 'CMakePresets.json' file.
*/
void GenerateVisualStudioCodeWorkspace::GenerateCMakePresetsFile(const GenerateProjectParameters &parameters)
{
    //Set up the JSON.
    nlohmann::ordered_json JSON;

    //Add the version.
    JSON["version"] = 8;

    //Add the configure presets.
    {
        nlohmann::ordered_json &configure_presets_entry{ JSON["configurePresets"] };

        nlohmann::ordered_json &configure_preset_entry{ configure_presets_entry.emplace_back() };

        configure_preset_entry["name"] = "Default";
        configure_preset_entry["displayName"] = "Default";
        configure_preset_entry["binaryDir"] = "${sourceDir}/Build";

        {
            nlohmann::ordered_json &cache_variables_entry{ configure_preset_entry["cacheVariables"] };

            cache_variables_entry["CMAKE_INSTALL_PREFIX"] = "${sourceDir}/Install";
        }
    }

    //Add the build presets.
    {
        nlohmann::ordered_json &build_presets_entry{ JSON["buildPresets"] };

        nlohmann::ordered_json &build_preset_entry{ build_presets_entry.emplace_back() };

        build_preset_entry["name"] = "Default";
        build_preset_entry["configurePreset"] = "Default";
    }

    //Figure out the file path.
    char file_path[256];
    PRINT_BUFFER(file_path, "%s/CMakePresets.json", parameters._DirectoryPath.c_str());

    //Open the file.
    std::ofstream file{ file_path };

    //Write the JSON.
    file << std::setw(4) << JSON;

    //Close the file.
    file.close();
}

/*
*	Generates the 'settings.json' file.
*/
void GenerateVisualStudioCodeWorkspace::GenerateSettingsFile(const GenerateProjectParameters &parameters)
{
    //Set up the JSON.
    nlohmann::ordered_json JSON;

    //Add the stuff.
    JSON["cmake.configureOnOpen"] = true;

    //Figure out the file path.
    char file_path[256];
    PRINT_BUFFER(file_path, "%s/.vscode/settings.json", parameters._DirectoryPath.c_str());

    //Open the file.
    std::ofstream file{ file_path };

    //Write the JSON.
    file << std::setw(4) << JSON;

    //Close the file.
    file.close();
}

/*
*	Generates the 'extensions.json' file.
*/
void GenerateVisualStudioCodeWorkspace::GenerateExtensionsFile(const GenerateProjectParameters &parameters)
{
    //Set up the JSON.
    nlohmann::ordered_json JSON;

    //Add the recommendations entry.
    {
        nlohmann::ordered_json &recommendations_entry{ JSON["recommendations"] };

        recommendations_entry.emplace_back("ms-vscode.cmake-tools");
        recommendations_entry.emplace_back("ms-vscode.cpptools");

    #if defined(PLATFORM_LINUX)
        recommendations_entry.emplace_back("vadimcn.vscode-lldb");
        recommendations_entry.emplace_back("llvm-vs-code-extensions.vscode-clangd");
    #endif
    }
    
    //Figure out the file path.
    char file_path[256];
    PRINT_BUFFER(file_path, "%s/.vscode/extensions.json", parameters._DirectoryPath.c_str());

    //Open the file.
    std::ofstream file{ file_path };

    //Write the JSON.
    file << std::setw(4) << JSON;

    //Close the file.
    file.close();
}

/*
*	Generates the 'tasks.json' file.
*/
void GenerateVisualStudioCodeWorkspace::GenerateTasksFile(const GenerateProjectParameters &parameters)
{
    //Set up the JSON.
    nlohmann::ordered_json JSON;

    //Add the version.
    JSON["version"] = "2.0.0";

    //Add the tasks.
    {
        nlohmann::ordered_json &tasks_entry{ JSON["tasks"] };

        //Add the build task.
        {
            nlohmann::ordered_json &build_task{ tasks_entry.emplace_back() };

            build_task["type"] = "cmake";
            build_task["label"] = "CMake: Build";
            build_task["command"] = "build";
            build_task["targets"].emplace_back("${command:cmake.buildTargetName}");

            {
                nlohmann::ordered_json &group_entry{ build_task["group"] };

                group_entry["kind"] = "build";
                group_entry["isDefault"] = true;
            }

            build_task["detail"] = "Builds the current CMake target.";
        }
    }

    //Figure out the file path.
    char file_path[256];
    PRINT_BUFFER(file_path, "%s/.vscode/tasks.json", parameters._DirectoryPath.c_str());

    //Open the file.
    std::ofstream file{ file_path };

    //Write the JSON.
    file << std::setw(4) << JSON;

    //Close the file.
    file.close();
}

/*
*	Generates the 'launch.json' file.
*/
void GenerateVisualStudioCodeWorkspace::GenerateLaunchFile(const GenerateProjectParameters &parameters)
{
    //Set up the JSON.
    nlohmann::ordered_json JSON;

    //Add the version.
    JSON["version"] = "2.0.0";

    //Add the configurations.
    {
        nlohmann::ordered_json &configurations_entry{ JSON["configurations"] };

        //Add the launch configuration.
        {
            nlohmann::ordered_json &launch_configuration{ configurations_entry.emplace_back() };

        #if defined(PLATFORM_WIN64)
            launch_configuration["name"] = "Visual Studio Debugger";
            launch_configuration["type"] = "cppvsdbg";
        #elif defined(PLATFORM_LINUX)
            launch_configuration["name"] = "Debug C/C++ (CodeLLDB)";
            launch_configuration["type"] = "lldb";
        #else
            #error "Unknown platform!"
        #endif
            launch_configuration["request"] = "launch";
            launch_configuration["program"] = "${command:cmake.launchTargetPath}";
            launch_configuration["cwd"] = "${workspaceFolder}";
            launch_configuration["preLaunchTask"] = "CMake: Build";
        }
    }

    //Figure out the file path.
    char file_path[256];
    PRINT_BUFFER(file_path, "%s/.vscode/launch.json", parameters._DirectoryPath.c_str());

    //Open the file.
    std::ofstream file{ file_path };

    //Write the JSON.
    file << std::setw(4) << JSON;

    //Close the file.
    file.close();
}
#endif