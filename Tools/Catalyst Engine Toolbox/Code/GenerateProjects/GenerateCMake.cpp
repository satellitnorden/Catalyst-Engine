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

    //Set the minimum required version.
    file << "# Set the minimum required version." << std::endl;
    file << "cmake_minimum_required(VERSION 4.4.2)" << std::endl;
    file << std::endl;

    //Set the CMake policy.
    file << "# Set the CMake policy." << std::endl;
    file << "cmake_policy(SET CMP0184 NEW)" << std::endl;
    file << std::endl;

    //Disable runtime checks.
    file << "# Disable runtime checks." << std::endl;
    file << "set(CMAKE_MSVC_RUNTIME_CHECKS \"\")" << std::endl;
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

    //Clear compiler flags (we will set our own).
    file << "# Clear compiler flags (we will set our own)." << std::endl;
    file << "set(CMAKE_CXX_FLAGS_DEBUG \"\")" << std::endl;
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

    /*
    *   Add a different target per configuration - The incoming configurations work perfectly in Visual Studio, but for CMake we have to do things a bit differently.
    */
    for (uint64 configuration_index{ 0 }; configuration_index < parameters._Configurations.size(); ++configuration_index)
    {
        //Cache the configuration.
        const GenerateProjectParameters::Configuration &configuration{ parameters._Configurations[configuration_index] };

        //Add the the target.
        file << "# Add the the target." << std::endl;

        switch (configuration._Type)
        {
            case GenerateProjectParameters::Configuration::Type::APPLICATION:
            {
                file << "add_executable(" << Utilities::RemoveSpaces(configuration._Name).c_str() << " ${SOURCE_FILES})" << std::endl;
                file << std::endl;

                break;
            }

            case GenerateProjectParameters::Configuration::Type::STATIC_LIBRARY:
            {
                file << "add_library(" << Utilities::RemoveSpaces(configuration._Name).c_str() << " STATIC ${SOURCE_FILES})" << std::endl;
                file << std::endl;

                break;
            }

            case GenerateProjectParameters::Configuration::Type::DYNAMIC_LIBRARY:
            {
                file << "add_library(" << Utilities::RemoveSpaces(configuration._Name).c_str() << " SHARED ${SOURCE_FILES})" << std::endl;
                file << std::endl;

                break;
            }

            default:
            {
                ASSERT(false, "Invalid case!");

                break;
            }
        }

        //Set the startup project.
        if (configuration_index == 0)
        {
            file << "# Set the startup project." << std::endl;
            file << "set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT " << Utilities::RemoveSpaces(configuration._Name).c_str() << ")" << std::endl;
            file << std::endl;
        }

        //Add the include paths.
        file << "# Add the include paths." << std::endl;

        for (const std::string &code_include_path : parameters._CodeIncludePaths)
        {
            file << "target_include_directories(" << Utilities::RemoveSpaces(configuration._Name).c_str() << " PRIVATE \"" << Utilities::BackwardToForwardSlashes(code_include_path).c_str() << "\")" << std::endl;
        }

        file << std::endl;

        //Add the compile definitions.
        file << "# Add the compile definitions." << std::endl;
        file << "target_compile_definitions(" << Utilities::RemoveSpaces(configuration._Name).c_str() << " PRIVATE" << std::endl;

        for (size_t preprocessor_definition_index{ 0 }; preprocessor_definition_index < configuration._PreprocessorDefinitions.size(); ++preprocessor_definition_index)
        {
            file << configuration._PreprocessorDefinitions[preprocessor_definition_index].c_str();

            if (preprocessor_definition_index < (configuration._PreprocessorDefinitions.size() - 1))
            {
                file << ";";
            }
        }
         
        file << ")" << std::endl;
        file << std::endl;

        //Add the library include paths.
        file << "# Add the library include paths." << std::endl;
        file << "target_link_directories(" << Utilities::RemoveSpaces(configuration._Name).c_str() << " PRIVATE" << std::endl;

        for (const std::string &library_include_path : parameters._LibraryIncludePaths)
        {
            file << "\t\"" << Utilities::BackwardToForwardSlashes(library_include_path).c_str() << "\"" << std::endl;
        }

        file << ")" << std::endl;
        file << std::endl;

        //Add the library includes.
        file << "# Add the library includes." << std::endl;
        file << "target_link_libraries(" << Utilities::RemoveSpaces(configuration._Name).c_str() << " PRIVATE" << std::endl;

        for (size_t library_include_index{ 0 }; library_include_index < configuration._LibraryIncludes.size(); ++library_include_index)
        {
            file << Utilities::StripExtension(Utilities::BackwardToForwardSlashes(configuration._LibraryIncludes[library_include_index])).c_str();

            if (library_include_index < (configuration._LibraryIncludes.size() - 1))
            {
                file << ";";
            }
        }

        file << ")" << std::endl;
        file << std::endl;

        //Set the runtime library.
        file << "# Set the runtime library." << std::endl;
        file << "set_property(TARGET " << Utilities::RemoveSpaces(configuration._Name).c_str() << " PROPERTY MSVC_RUNTIME_LIBRARY " << configuration._RuntimeLibrary << ")" << std::endl;
        file << std::endl;
    
        //Set the sub system.
        file << "# Set the sub system." << std::endl;
        file << "set_property(TARGET " << Utilities::RemoveSpaces(configuration._Name).c_str() << " PROPERTY WIN32_EXECUTABLE TRUE)" << std::endl;
        file << std::endl;

        //Set the output directory.
        file << "# Set the output directory." << std::endl;
        file << "set_target_properties(" << Utilities::RemoveSpaces(configuration._Name).c_str() << " PROPERTIES" << std::endl;
        file << "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG \"${CMAKE_BINARY_DIR}/" << Utilities::RemoveSpaces(configuration._Name).c_str() << "\"" << std::endl;
        file << "\tLIBRARY_OUTPUT_DIRECTORY_DEBUG \"${CMAKE_BINARY_DIR}/" << Utilities::RemoveSpaces(configuration._Name).c_str() << "\"" << std::endl;
        file << "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG \"${CMAKE_BINARY_DIR}/" << Utilities::RemoveSpaces(configuration._Name).c_str() << "\"" << std::endl;
        file << "\tOUTPUT_NAME_DEBUG \"" << parameters._ProjectName << "\"" << std::endl;
        file << "\tSUFFIX \"" << configuration._TargetFileExtension << "\"" << std::endl;
        file << ")" << std::endl;
        file << std::endl;

        //Set MSVC-specific flags.
        file << "# Set MSVC-specific flags." << std::endl;
        file << "if (CMAKE_CXX_COMPILER_ID STREQUAL \"MSVC\")" << std::endl;

        //Add the compiler flags.
        file << "\t# Set compiler flags." << std::endl;
        file << "\ttarget_compile_options(" << Utilities::RemoveSpaces(configuration._Name) << " PRIVATE" << std::endl;

        //Always generate debug information.
        file << "\t\t/Zi" << std::endl;

        //Set optimizations.
        if (configuration._ApplyOptimizations)
        {
            file << "\t\t/O2" << std::endl;
            file << "\t\t/GL" << std::endl;
        }

        else
        {
            file << "\t\t/Od" << std::endl;
        }

        file << "\t)" << std::endl;
        file << std::endl;

        //Add the linker flags.
        file << "\t# Set linker flags." << std::endl;
        file << "\ttarget_link_options(" << Utilities::RemoveSpaces(configuration._Name) << " PRIVATE" << std::endl;

        if (configuration._ApplyOptimizations)
        {
            file << "\t\t/LTCG:incremental" << std::endl;
            file << "\t\t/INCREMENTAL:NO" << std::endl;
        }

        else
        {
            file << "\t\t/INCREMENTAL" << std::endl;
        }

        file << "\t)" << std::endl;

        file << "endif()" << std::endl;
        file << std::endl;

        //Set GCC-specific flags.
        file << "# Set GCC-specific flags." << std::endl;
        file << "if (CMAKE_CXX_COMPILER_ID STREQUAL \"GNU\")" << std::endl;

        //Add the compiler flags.
        file << "\t# Set flags." << std::endl;
        file << "\ttarget_compile_options(" << Utilities::RemoveSpaces(configuration._Name) << " PRIVATE" << std::endl;

        //Always generate debug information.
        file << "\t\t-g" << std::endl;

        //Always support AVX2 instructions.
        file << "\t\t-mavx2" << std::endl;

        file << "\t)" << std::endl;

        file << "endif()" << std::endl;
        file << std::endl;
    }

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

    //Close the file.
    file.close();
}