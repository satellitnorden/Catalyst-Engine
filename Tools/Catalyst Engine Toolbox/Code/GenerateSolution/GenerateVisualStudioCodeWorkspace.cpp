//Header file.
#include <GenerateSolution/GenerateVisualStudioCodeWorkspace.h>

//Third party.
#include <ThirdParty/json.hpp>

//STD.
#include <filesystem>
#include <fstream>
#include <iostream>

/*
*	Generates!
*/
void GenerateVisualStudioCodeWorkspace::Generate(const GenerateSolutionParameters &parameters)
{
    //Ensure the directory exists.
    std::filesystem::create_directories(parameters._DirectoryPath);

    //Also ensure the '.vscode' directory exists.
    //std::filesystem::create_directories(parameters._DirectoryPath + std::string("\\.vscode"));

    //Generate the '.code-workspace' file.
    GenerateCodeWorkspaceFile(parameters);

    //Generate the '.vscode\c_cpp_properties.json' file.
    //GenerateCCPPPropertiesFile(parameters);
}

/*
*	Generates the '.code-workspace' file.
*/
void GenerateVisualStudioCodeWorkspace::GenerateCodeWorkspaceFile(const GenerateSolutionParameters &parameters)
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
        for (const GenerateSolutionParameters::Directory &directory : parameters._Directories)
        {
            nlohmann::ordered_json &folder_entry{ folders_entry.emplace_back() };

            folder_entry["name"] = directory._Name;
            folder_entry["path"] = directory._Path;
        }
    }

    //Add the settings entry.
    {
        nlohmann::ordered_json &settings_entry{ JSON["settings"] };

        //Add the include paths.
        {
            nlohmann::ordered_json &include_paths_entry{ settings_entry["C_Cpp.default.includePath"] };

            for (const std::string &code_include_path : parameters._CodeIncludePaths)
            {
                include_paths_entry.emplace_back(code_include_path);
            }
        }
    }

    //Figure out the file path.
    char file_path[256];
    sprintf_s(file_path, "%s\\%s.code-workspace", parameters._DirectoryPath.c_str(), parameters._ProjectName);

    //Open the file.
    std::ofstream file{ file_path };

    //Write the JSON.
    file << std::setw(4) << JSON;

    //Close the file.
    file.close();
}

/*
*	Generates the '.vscode\c_cpp_properties.json' file.
*/
void GenerateVisualStudioCodeWorkspace::GenerateCCPPPropertiesFile(const GenerateSolutionParameters &parameters)
{
    //Set up the JSON.
    nlohmann::ordered_json JSON;

    //Add the 'configurations' entry.
    {
        nlohmann::ordered_json &configurations_entry{ JSON["configurations"] };

        //Add each configuration.
        for (const GenerateSolutionParameters::Configuration &configuration : parameters._Configurations)
        {
            //Add the configuration entry.
            nlohmann::ordered_json &configuration_entry{ configurations_entry.emplace_back() };

            //Set the name.
            configuration_entry["name"] = configuration._Name;

            //Add the include paths.
            {
                nlohmann::ordered_json &include_path_entry{ configuration_entry["includePath"] };

                for (const std::string &code_include_path : parameters._CodeIncludePaths)
                {
                    include_path_entry.emplace_back(code_include_path);
                }
            }

            //Add the defines.
            {
                nlohmann::ordered_json &defines_entry{ configuration_entry["defines"] };

                for (const std::string &preprocessor_definition : configuration._PreprocessorDefinitions)
                {
                    defines_entry.emplace_back(preprocessor_definition);
                }
            }

            //Set the C++ standard.
            configuration_entry["cppStandard"] = "c++20";
        }
    }

    //Set the 'enableConfigurationSquiggles' entry.
    JSON["enableConfigurationSquiggles"] = true;

    //Figure out the file path.
    char file_path[256];
    sprintf_s(file_path, "%s\\.vscode\\c_cpp_properties.json", parameters._DirectoryPath.c_str());

    //Open the file.
    std::ofstream file{ file_path };

    //Write the JSON.
    file << std::setw(4) << JSON;

    //Close the file.
    file.close();
}