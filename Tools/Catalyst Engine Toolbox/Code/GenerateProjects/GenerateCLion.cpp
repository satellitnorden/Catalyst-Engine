//Header file.
#include <GenerateProjects/GenerateCLion.h>

//Core.
#include <Core/Utilities.h>

//Generate projects.
#include <GenerateProjects/GenerateCMake.h>
#include <GenerateProjects/GenerateCopyDynamicLibraries.h>

//STD.
#include <filesystem>
#include <fstream>

#if CLION_GENERATE_IDEA
/*
*   Returns the '.idea' directory path.
*/
std::string GetIdeaDirectoryPath(const GenerateProjectParameters &parameters)
{
    return std::string(parameters._DirectoryPath) + std::string("/.idea");
}
#endif

/*
*	Generates!
*/
void GenerateCLion::Generate(const ProjectSettings &project_settings, const GenerateProjectParameters &parameters)
{
    //Ensure the directories exists.
    std::filesystem::create_directories(parameters._DirectoryPath);

#if CLION_GENERATE_IDEA
    std::filesystem::create_directories(GetIdeaDirectoryPath(parameters));
#endif

    //CLion essentially only uses CMake, so just generate that.
    GenerateCMake::Generate(parameters);

#if CLION_GENERATE_IDEA
    //Generate the run configuration files.
    GenerateRunConfigurationFiles(parameters);
#endif

    //Copy the dynamic libraries.
    GenerateCopyDynamicLibraries::Generate(project_settings, parameters, false, "Build");
}

#if CLION_GENERATE_IDEA
/*
*	Generates the run configuration files. file.
*/
void GenerateCLion::GenerateRunConfigurationFiles(const GenerateProjectParameters &parameters)
{
    //Figure out the directory path.
    const std::string directory_path{ GetIdeaDirectoryPath(parameters) + "/runConfigurations" };

    //Make sure it exists.
    std::filesystem::create_directories(directory_path);

    for (const GenerateProjectParameters::Configuration &configuration : parameters._Configurations)
    {
        //Figure out the file path.
        const std::string file_path{ directory_path + "/" + Utilities::RemoveSpaces(configuration._Name) + ".xml" };

        //Open the file.
        std::ofstream file{ file_path };

        //Write the stuff.
        file << "<component name=\"ProjectRunConfigurationManager\">" << std::endl;

        file << "\t<configuration";
        file << " default=\"false\"";
        file << " name=\"" << Utilities::RemoveSpaces(configuration._Name) << "\"";
        file << " type=\"CMakeRunConfiguration\"";
        file << " factoryName=\"factoryName\"";
        file << " REDIRECT_INPUT=\"false\"";
        file << " ELEVATE=\"false\"";
        file << " USE_EXTERNAL_CONSOLE=\"false\"";
        file << " EMULATE_TERMINAL=\"false\"";
        file << " WORKING_DIR=\"file://$PROJECT_DIR$\"";
        file << " PASS_PARENT_ENVS_2=\"true\"";
        file << " PROJECT_NAME=\"" << parameters._ProjectName << "\"";
        file << " TARGET_NAME=\"" << Utilities::RemoveSpaces(configuration._Name) << "\"";
        file << " CONFIG_NAME=\"Default\"";
        file << " RUN_TARGET_PROJECT_NAME=\"" << parameters._ProjectName << "\"";
        file << " RUN_TARGET_NAME=\"" << Utilities::RemoveSpaces(configuration._Name) << "\"";
        file << ">" << std::endl;

        file << "\t\t<method v=\"2\">" << std::endl;
        file << "\t\t\t<option name=\"com.jetbrains.cidr.execution.CidrBuildBeforeRunTaskProvider$BuildBeforeRunTask\" enabled=\"true\" />" << std::endl;
        file << "\t\t</method>" << std::endl;
        file << "\t</configuration>" << std::endl;
        file << "</component>" << std::endl;

        //Close the file.
        file.close();
    }
}

/*
*	Generates the 'workspace.xml' file.
*/
void GenerateCLion::GenerateWorkspaceFile(const GenerateProjectParameters &parameters)
{

}
#endif