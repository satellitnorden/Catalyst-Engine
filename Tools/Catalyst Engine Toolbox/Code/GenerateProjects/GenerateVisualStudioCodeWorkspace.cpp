//Header file.
#include <GenerateProjects/GenerateVisualStudioCodeWorkspace.h>

//Core.
#include <Core/Utilities.h>

#if VISUAL_STUDIO_CODE_WORKSPACE_SUPPORTED
//Generate projects.
#include <GenerateProjects/GenerateCMake.h>

//Third party.
#include <ThirdParty/json.hpp>

//STD.
#include <filesystem>
#include <fstream>
#include <iostream>

/*
*	Generates!
*/
void GenerateVisualStudioCodeWorkspace::Generate(const GenerateProjectParameters &parameters)
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
        settings_entry["C_Cpp.default.configurationProvider"] = "ms-vscode.cmake-tools";

        //Set CMake options.
        settings_entry["cmake.options.statusBarVisibility"] = "visible";
        settings_entry["cmake.sourceDirectory"] = "${workspaceFolder}";
        settings_entry["cmake.autoSelectActiveFolder"] = false;
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

        //Add the first (and only) entry.
        {
            nlohmann::ordered_json &configure_preset_entry{ configure_presets_entry.emplace_back() };

            configure_preset_entry["name"] = "Win64";
            configure_preset_entry["displayName"] = "Win64";
            configure_preset_entry["generator"] = "Visual Studio 17 2022";
            configure_preset_entry["architecture"] = "x64";
            configure_preset_entry["binaryDir"] = "${sourceDir}/Build";

            {
                nlohmann::ordered_json &cache_variables_entry{ configure_preset_entry["cacheVariables"] };

                cache_variables_entry["CMAKE_INSTALL_PREFIX"] = "${sourceDir}/Install";
            }
        }
    }

    //Add the build presets.
    {
        nlohmann::ordered_json &build_presets_entry{ JSON["buildPresets"] };

        for (const GenerateProjectParameters::Configuration &configuration : parameters._Configurations)
        {
            nlohmann::ordered_json &build_preset_entry{ build_presets_entry.emplace_back() };

            build_preset_entry["name"] = Utilities::RemoveSpaces(configuration._Name).c_str();
            build_preset_entry["configurePreset"] = "Win64";
            build_preset_entry["configuration"] = Utilities::RemoveSpaces(configuration._Name).c_str();
        }
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
            build_task["label"] = "CMake: build";
            build_task["command"] = "build";
            build_task["targets"].emplace_back(Utilities::RemoveSpaces(parameters._ProjectName).c_str());

            {
                nlohmann::ordered_json &group_entry{ build_task["group"] };

                group_entry["kind"] = "build";
                group_entry["isDefault"] = true;
            }

            build_task["detail"] = "CMake template build task";
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

            launch_configuration["type"] = "cppvsdbg";
            launch_configuration["name"] = parameters._ProjectName;
            launch_configuration["request"] = "launch";
            launch_configuration["program"] = "${command:cmake.launchTargetPath}";
            launch_configuration["stopAtEntry"] = false;
            launch_configuration["cwd"] = "${workspaceFolder}";
            launch_configuration["console"] = "internalConsole";
            launch_configuration["preLaunchTask"] = "CMake: build";
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