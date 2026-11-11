//Header file.
#include <GenerateProjects/GenerateCMake.h>

//Core.
#include <Core/Utilities.h>

//STD.
#include <cstdio>
#include <filesystem>
#include <fstream>

/*
*   Adds the given directory.
*/
void AddDirectory(const GenerateProjectParameters::Directory &directory, std::ofstream &file)
{
    for (const GenerateProjectParameters::Directory &sub_directory : directory._Directories)
    {
        AddDirectory(sub_directory, file);
    }

    for (const GenerateProjectParameters::File &_file : directory._Files)
    {
        file << "\t\"" << Utilities::BackwardToForwardSlashes(_file._Path).c_str() << "\"" << std::endl;
    }
}

/*
*	Generates!
*/
void GenerateCMake::Generate(const GenerateProjectParameters &parameters)
{
    //Ensure the directory exists.
    std::filesystem::create_directories(parameters._DirectoryPath);

    //Figure out the file path.
    char file_path[256];
    PRINT_BUFFER(file_path, "%s/CMakeLists.txt", parameters._DirectoryPath.c_str());

    //Open file.
    std::ofstream file{ file_path };

    //Add the minimum required version.
    file << "# Set the minimum required version." << std::endl;
    file << "cmake_minimum_required(VERSION 4.4.4)" << std::endl;
    file << std::endl;

    //Add the the project.
    file << "# Add the the project." << std::endl;
    file << "project(\"" << parameters._ProjectName << "\" LANGUAGES C CXX)" << std::endl;
    file << std::endl;

    //Set the C++ standard.
    file << "# Set the C++ standard." << std::endl;
    file << "set(CMAKE_CXX_STANDARD 20)" << std::endl;
    file << "set(CMAKE_CXX_STANDARD_REQUIRED ON)" << std::endl;
    file << std::endl;

    //Set the configuration types.
    file << "# Set the configuration types." << std::endl;
    file << "set(CMAKE_CONFIGURATION_TYPES" << std::endl;;

    for (size_t i{ 0 }; i < parameters._Configurations.size(); ++i)
    {
        file << "\t" << Utilities::RemoveSpaces(parameters._Configurations[i]._Name).c_str() << "" << std::endl;
    }

    file << ")" << std::endl;
    file << std::endl;

    //Collect the source files.
    file << "# Collect the source files" << std::endl;
    file << "set(SOURCE_FILES" << std::endl;

    for (const GenerateProjectParameters::Directory &directory : parameters._Directories)
    {
        AddDirectory(directory, file);
    }

    file << ")" << std::endl;
    file << std::endl;

    //Add the the executable.
    file << "# Add the the executable." << std::endl;
    file << "add_executable(" << Utilities::RemoveSpaces(parameters._ProjectName).c_str() << " ${SOURCE_FILES})" << std::endl;
    file << std::endl;

    //Set the startup project.
    file << "# Set the startup project." << std::endl;
    file << "set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT " << Utilities::RemoveSpaces(parameters._ProjectName).c_str() << ")" << std::endl;
    file << std::endl;

    //Add the include paths.
    file << "# Add the include paths." << std::endl;

    for (const std::string &code_include_path : parameters._CodeIncludePaths)
    {
        file << "target_include_directories(" << Utilities::RemoveSpaces(parameters._ProjectName).c_str() << " PRIVATE \"" << Utilities::BackwardToForwardSlashes(code_include_path).c_str() << "\")" << std::endl;
    }

    file << std::endl;

    //Add the compile definitions.
    file << "# Add the compile definitions." << std::endl;
    file << "target_compile_definitions(" << Utilities::RemoveSpaces(parameters._ProjectName).c_str() << " PRIVATE" << std::endl;

    for (const GenerateProjectParameters::Configuration &configuration : parameters._Configurations)
    {
        file << "\t$<$<CONFIG:" << Utilities::RemoveSpaces(configuration._Name).c_str() << ">:";

        for (size_t preprocessor_definition_index{ 0 }; preprocessor_definition_index < configuration._PreprocessorDefinitions.size(); ++preprocessor_definition_index)
        {
            file << configuration._PreprocessorDefinitions[preprocessor_definition_index].c_str();

            if (preprocessor_definition_index < (configuration._PreprocessorDefinitions.size() - 1))
            {
                file << ";";
            }
        }

        file << ">" << std::endl;
    }

    file << ")" << std::endl;
    file << std::endl;

    //Add the library include paths.
    file << "# Add the library include paths." << std::endl;
    file << "target_link_directories(" << Utilities::RemoveSpaces(parameters._ProjectName).c_str() << " PRIVATE" << std::endl;

    for (const std::string &library_include_path : parameters._LibraryIncludePaths)
    {
        file << "\t\"" << Utilities::BackwardToForwardSlashes(library_include_path).c_str() << "\"" << std::endl;
    }

    file << ")" << std::endl;
    file << std::endl;

    //Add the library includes.
    file << "# Add the library includes." << std::endl;
    file << "target_link_libraries(" << Utilities::RemoveSpaces(parameters._ProjectName).c_str() << " PRIVATE" << std::endl;

    for (const GenerateProjectParameters::Configuration &configuration : parameters._Configurations)
    {
        file << "\t$<$<CONFIG:" << Utilities::RemoveSpaces(configuration._Name).c_str() << ">:";

        for (size_t library_include_index{ 0 }; library_include_index < configuration._LibraryIncludes.size(); ++library_include_index)
        {
            file << Utilities::StripExtension(Utilities::BackwardToForwardSlashes(configuration._LibraryIncludes[library_include_index])).c_str();

            if (library_include_index < (configuration._LibraryIncludes.size() - 1))
            {
                file << ";";
            }
        }

        file << ">" << std::endl;
    }

    file << ")" << std::endl;
    file << std::endl;

    //Set the runtime library.
    file << "# Set the runtime library." << std::endl;
    file << "set_property(TARGET " << Utilities::RemoveSpaces(parameters._ProjectName).c_str() << " PROPERTY MSVC_RUNTIME_LIBRARY" << std::endl;
    file << "\t";

    for (const GenerateProjectParameters::Configuration &configuration : parameters._Configurations)
    {
        file << "$<$<CONFIG:" << Utilities::RemoveSpaces(configuration._Name).c_str() << ">:" << configuration._RuntimeLibrary << ">";
    }

    file << std::endl;
    file << ")" << std::endl;
    file << std::endl;

    //Set the sub system.
    file << "# Set the sub system." << std::endl;
    file << "set_property(TARGET " << Utilities::RemoveSpaces(parameters._ProjectName).c_str() << " PROPERTY WIN32_EXECUTABLE TRUE)" << std::endl;
    file << std::endl;

    //Set up the Visual Studio filters.
    file << "# Set up the Visual Studio filters." << std::endl;

    for (const GenerateProjectParameters::Directory &directory : parameters._Directories)
    {
        if (directory._Path.empty())
        {
            continue;
        }

        file << "file(GLOB_RECURSE " << Utilities::RemoveSpaces(directory._Name).c_str() << "_FILES" << std::endl;
        file << "\t\"${CMAKE_CURRENT_SOURCE_DIR}/" << Utilities::BackwardToForwardSlashes(directory._Path).c_str() << "/*.h\"" << std::endl;
        file << "\t\"${CMAKE_CURRENT_SOURCE_DIR}/" << Utilities::BackwardToForwardSlashes(directory._Path).c_str() << "/*.hpp\"" << std::endl;
        file << "\t\"${CMAKE_CURRENT_SOURCE_DIR}/" << Utilities::BackwardToForwardSlashes(directory._Path).c_str() << "/*.c\"" << std::endl;
        file << "\t\"${CMAKE_CURRENT_SOURCE_DIR}/" << Utilities::BackwardToForwardSlashes(directory._Path).c_str() << "/*.cpp\"" << std::endl;
        file << "\t\"${CMAKE_CURRENT_SOURCE_DIR}/" << Utilities::BackwardToForwardSlashes(directory._Path).c_str() << "/*.common_shader\"" << std::endl;
        file << "\t\"${CMAKE_CURRENT_SOURCE_DIR}/" << Utilities::BackwardToForwardSlashes(directory._Path).c_str() << "/*.global_render_data\"" << std::endl;
        file << "\t\"${CMAKE_CURRENT_SOURCE_DIR}/" << Utilities::BackwardToForwardSlashes(directory._Path).c_str() << "/*.render_pipeline\"" << std::endl;
        file << "\t\"${CMAKE_CURRENT_SOURCE_DIR}/" << Utilities::BackwardToForwardSlashes(directory._Path).c_str() << "/*.shader_function_library\"" << std::endl;
        file << "\t\"${CMAKE_CURRENT_SOURCE_DIR}/" << Utilities::BackwardToForwardSlashes(directory._Path).c_str() << "/*.storage_buffer_definition\"" << std::endl;
        file << "\t\"${CMAKE_CURRENT_SOURCE_DIR}/" << Utilities::BackwardToForwardSlashes(directory._Path).c_str() << "/*.uniform_buffer_definition\"" << std::endl;
        file << ")" << std::endl;
        file << "source_group(TREE \"${CMAKE_CURRENT_SOURCE_DIR}/" << Utilities::BackwardToForwardSlashes(directory._Path).c_str() << "\" PREFIX \"" << directory._Name.c_str() << "\" FILES ${" << Utilities::RemoveSpaces(directory._Name).c_str() << "_FILES})" << std::endl;
        file << std::endl;
    }

    //Set MSVC-specific flags.
    file << "# Set MSVC-specific flags." << std::endl;
    file << "if (CMAKE_CXX_COMPILER_ID STREQUAL \"MSVC\")" << std::endl;

    for (size_t configuration_index{ 0 }; configuration_index < parameters._Configurations.size(); ++configuration_index)
    {
        //Cache the configuration.
        const GenerateProjectParameters::Configuration &configuration{ parameters._Configurations[configuration_index] };

        //Figure out the names.
        const std::string compiler_flags_name{ std::string("CMAKE_CXX_FLAGS_") + Utilities::ToUpperCase(Utilities::RemoveSpaces(configuration._Name)) };
        const std::string linker_flags_name{ std::string("CMAKE_EXE_LINKER_FLAGS_") + Utilities::ToUpperCase(Utilities::RemoveSpaces(configuration._Name)) };

        //Add the compiler flags.
        file << "\t# Set compiler flags." << std::endl;
        file << "\tset(" << compiler_flags_name << " \"";

        //Always generate debug information.
        file << "/Zi";

        //Set optimizations.
        if (configuration._ApplyOptimizations)
        {
            file << " /O2";
            file << " /GL";
        }

        else
        {
            file << " /Od";
        }

        file << "\")" << std::endl;

        //Add the linker flags.
        file << "\t# Set linker flags." << std::endl;
        file << "\tset(" << linker_flags_name << " \"/DEBUG";

        if (configuration._ApplyOptimizations)
        {
            file << " /LTCG:incremental";
            file << " /INCREMENTAL:NO";
        }

        else
        {
            file << " /INCREMENTAL";
        }

        file << "\")" << std::endl;

        if (configuration_index < (parameters._Configurations.size() - 1))
        {
            file << std::endl;
        }
    }

    file << "endif()" << std::endl;
    file << std::endl;

    //Set GCC-specific flags.
    file << "# Set GCC-specific flags." << std::endl;
    file << "if (CMAKE_CXX_COMPILER_ID STREQUAL \"GNU\")" << std::endl;

    for (size_t configuration_index{ 0 }; configuration_index < parameters._Configurations.size(); ++configuration_index)
    {
        //Cache the configuration.
        const GenerateProjectParameters::Configuration &configuration{ parameters._Configurations[configuration_index] };

        //Figure out the names.
        const std::string flags_name{ std::string("CMAKE_CXX_FLAGS_") + Utilities::ToUpperCase(Utilities::RemoveSpaces(configuration._Name)) };
        //Add the compiler flags.
        file << "\t# Set flags." << std::endl;
        file << "\tset(" << flags_name << " \"";

        //Always generate debug information.
        file << "-g";

        file << "\")" << std::endl;

        if (configuration_index < (parameters._Configurations.size() - 1))
        {
            file << std::endl;
        }
    }

    file << "endif()" << std::endl;

    //Close the file.
    file.close();
}