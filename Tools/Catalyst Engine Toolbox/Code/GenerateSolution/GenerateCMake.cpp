//Header file.
#include <GenerateSolution/GenerateCMake.h>

//STD.
#include <filesystem>
#include <fstream>

/*
*	Generates!
*/
void GenerateCMake::Generate(const GenerateSolutionParameters &parameters)
{
    //Ensure the directory exists.
    std::filesystem::create_directories(parameters._DirectoryPath);

    //Figure out the file path.
    char file_path[256];
    sprintf_s(file_path, "%s\\CMakeLists.txt", parameters._DirectoryPath.c_str());

    //Open file.
    std::ofstream file{ file_path };

    //Add the minimum required version.
    file << "# Set the minimum required version." << std::endl;
    file << "cmake_minimum_required(VERSION 4.4.5)" << std::endl;
    file << std::endl;

    //Add the the project.
    file << "# Add the the project." << std::endl;
    file << "project(\"" << parameters._ProjectName << "\")" << std::endl;
    file << std::endl;

    //Set the build types.
    file << "# Set the build types." << std::endl;
    file << "set(CMAKE_BUILD_TYPE" << std::endl;;

    for (size_t i{ 0 }; i < parameters._Configurations.size(); ++i)
    {
        file << "\t\"" << parameters._Configurations[i]._Name << "\"" << std::endl;
    }

    file << ")" << std::endl;
    file << std::endl;

    //Set the configuration types.
    file << "# Set the configuration types." << std::endl;
    file << "set(CMAKE_CONFIGURATION_TYPES" << std::endl;;

    for (size_t i{ 0 }; i < parameters._Configurations.size(); ++i)
    {
        file << "\t\"" << parameters._Configurations[i]._Name << "\"" << std::endl;
    }

    file << ")";

    //Close the file.
    file.close();
}