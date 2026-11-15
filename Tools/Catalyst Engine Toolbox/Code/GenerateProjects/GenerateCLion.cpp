//Header file.
#include <GenerateProjects/GenerateCLion.h>

//Generate projects.
#include <GenerateProjects/GenerateCMake.h>

//STD.
#include <filesystem>

/*
*	Generates!
*/
void GenerateCLion::Generate(const GenerateProjectParameters &parameters)
{
    //Ensure the directories exists.
    std::filesystem::create_directories(parameters._DirectoryPath);

    //CLion essentially only uses CMake, so just generate that.
    GenerateCMake::Generate(parameters);
}