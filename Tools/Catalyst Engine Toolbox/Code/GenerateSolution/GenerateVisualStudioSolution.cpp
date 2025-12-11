//Header file.
#include <GenerateSolution/GenerateVisualStudioSolution.h>

//Windows.
#include <objbase.h>

//STD.
#include <fstream>
#include <iostream>

/*
*	Generates a GUID.
*/
std::string GenerateGUID()
{
    GUID guid;
    CoCreateGuid(&guid);

    char buffer[64];
    snprintf(
        buffer,
        sizeof(buffer),
        "{%08lX-%04hX-%04hX-%04hX-%012llX}",
        guid.Data1,
        guid.Data2,
        guid.Data3,
        (guid.Data4[0] << 8) | guid.Data4[1],
        ((unsigned long long)guid.Data4[2] << 40) |
        ((unsigned long long)guid.Data4[3] << 32) |
        ((unsigned long long)guid.Data4[4] << 24) |
        ((unsigned long long)guid.Data4[5] << 16) |
        ((unsigned long long)guid.Data4[6] << 8) |
        ((unsigned long long)guid.Data4[7])
    );

    return buffer;
}

/*
*   Returns a configuration/platform combination condition string.
*/
std::string GetConfigurationPlatformConditionString(const std::string &configuration_name, const std::string &platform_name)
{
    return std::string("\"'$(Configuration)|$(Platform)'=='") + configuration_name + std::string("|") + platform_name + std::string("'\"");
}

/*
*	Generates!
*/
void GenerateVisualStudioSolution::Generate(const Parameters &parameters)
{
    //Generate the GUID's.
    const std::string solution_guid{ GenerateGUID() };
    const std::string project_guid{ GenerateGUID() };

    //Generate the project file.
    GenerateProjectFile(parameters, project_guid);

    //Generate the filters file.
    GenerateFiltersFile(parameters);

    //Generate the solution file.
    GenerateSolutionFile(parameters, solution_guid, project_guid);
}

/*
*	Generates the project file.
*/
void GenerateVisualStudioSolution::GenerateProjectFile(const Parameters &parameters, const std::string &project_guid)
{
    //Figure out the project file path.
    char file_path_buffer[256];
    sprintf_s(file_path_buffer, "%s\\%s.vcxproj", parameters._DirectoryPath, parameters._ProjectName);

    //Open the file.
    std::ofstream file{ file_path_buffer };

    //Write the header.
    file << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;

    //Begin the project item.
    file << "<Project DefaultTargets=\"Build\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">" << std::endl;

    //Write the project configuration item group.
    {
        //Begin the item group.
        file << "\t<ItemGroup Label=\"ProjectConfigurations\">" << std::endl;

        for (const Parameters::Configuration &configuration : parameters._Configurations)
        {
            for (const Parameters::Platform &platform : parameters._Platforms)
            {
                file << "\t\t<ProjectConfiguration Include=\"" << configuration._Name << "|" << platform._Name << "\">" << std::endl;
                file << "\t\t\t<Configuration>" << configuration._Name << "</Configuration>" << std::endl;
                file << "\t\t\t<Platform>" << platform._Name << "</Platform>" << std::endl;
                file << "\t\t</ProjectConfiguration>" << std::endl;
            }
        }

        //End the item group.
        file << "\t</ItemGroup>" << std::endl;
    }

    //Write the globals proprty group.
    {
        //Begin the property group.
        file << "\t<PropertyGroup Label=\"Globals\">" << std::endl;

        //Add the properties.
        file << "\t\t<VCProjectVersion>17.0</VCProjectVersion>" << std::endl;
        file << "\t\t<Keyword>Win32Proj</Keyword>" << std::endl;
        file << "\t\t<ProjectGuid>" << project_guid.c_str() << "</ProjectGuid>" << std::endl;
        file << "\t\t<RootNamespace>" << parameters._ProjectName << "</RootNamespace>" << std::endl;
        file << "\t\t<WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>" << std::endl;

        //End the property group.
        file << "\t</PropertyGroup>" << std::endl;
    }

    //Write the import for "Microsoft.Cpp.Default.props".
    file << "\t<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\"/>" << std::endl;

    //Write the property group for all configuration/platform combinations.
    for (const Parameters::Configuration &configuration : parameters._Configurations)
    {
        for (const Parameters::Platform &platform : parameters._Platforms)
        {
            //Begin the property group.
            file << "\t<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='" << configuration._Name << "|" << platform._Name << "'\" Label=\"Configuration\">" << std::endl;

            //Add the properties.
            file << "\t\t<ConfigurationType>Application</ConfigurationType>" << std::endl;

            switch (configuration._Type)
            {
                case Parameters::Configuration::Type::APPLICATION:
                {
                    file << "\t\t<ConfigurationType>Application</ConfigurationType>" << std::endl;

                    break;
                }

                case Parameters::Configuration::Type::STATIC_LIBRARY:
                {
                    file << "\t\t<ConfigurationType>StaticLibrary</ConfigurationType>" << std::endl;

                    break;
                }

                case Parameters::Configuration::Type::DYNAMIC_LIBRARY:
                {
                    file << "\t\t<ConfigurationType>DynamicLibrary</ConfigurationType>" << std::endl;

                    break;
                }

                default:
                {
                    std::cerr << "Unknown configuration type!" << std::endl;

                    break;
                }
            }

            file << "\t\t<UseDebugLibraries>" << (configuration._UseDebugLibraries ? "true" : "false") << "</UseDebugLibraries>" << std::endl;
            file << "\t\t<PlatformToolset>v143</PlatformToolset>" << std::endl;
            file << "\t\t<CharacterSet>MultiByte</CharacterSet>" << std::endl;
            file << "\t\t<WholeProgramOptimization>true</WholeProgramOptimization>" << std::endl;
            file << "\t\t<PreferredToolArchitecture>x64</PreferredToolArchitecture>" << std::endl;

            //End the property group.
            file << "\t</PropertyGroup>" << std::endl;
        }
    }

    //Write the import for "Microsoft.Cpp.props".
    file << "\t<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\"/>" << std::endl;

    //Write the empty extension settings import group.
    file << "\t<ImportGroup Label=\"ExtensionSettings\">" << std::endl;
    file << "\t</ImportGroup>" << std::endl;

    //Write the empty shared property group.
    file << "\t<ImportGroup Label=\"Shared\">" << std::endl;
    file << "\t</ImportGroup>" << std::endl;

    //Write the property sheets import groups.
    for (const Parameters::Configuration &configuration : parameters._Configurations)
    {
        for (const Parameters::Platform &platform : parameters._Platforms)
        {
            //Begin the import group.
            file << "\t<ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='" << configuration._Name << "|" << platform._Name << "'\">" << std::endl;

            //Add the properties.
            file << "\t\t<Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\"/>" << std::endl;

            //End the import group.
            file << "\t</ImportGroup>" << std::endl;
        }
    }

    //Start the property group.
    file << "\t<PropertyGroup>" << std::endl;

    for (const Parameters::Configuration &configuration : parameters._Configurations)
    {
        for (const Parameters::Platform &platform : parameters._Platforms)
        {
            //Set the outpit directory.
            file << "\t\t<OutDir Condition=" << GetConfigurationPlatformConditionString(configuration._Name, platform._Name) << ">" << configuration._Name << "\\</OutDir>" << std::endl;

            //Set the intermediate directory.
            file << "\t\t<IntDir Condition=" << GetConfigurationPlatformConditionString(configuration._Name, platform._Name) << ">" << parameters._ProjectName << ".dir\\" << configuration._Name << "\\</IntDir>" << std::endl;

            //Set the target name.
            file << "\t\t<TargetName Condition=" << GetConfigurationPlatformConditionString(configuration._Name, platform._Name) << ">" << parameters._ProjectName << "</TargetName>" << std::endl;

            //Set the target extension.
            file << "\t\t<TargetExt>" << configuration._TargetFileExtension << "</TargetExt>" << std::endl;
        }
    }

    //End the property group.
    file << "\t</PropertyGroup>" << std::endl;

    //Add the empty user macros property group.
    file << "\t<PropertyGroup Label=\"UserMacros\"/>" << std::endl;

    //Write the item defition groups for all configuration/platform combinations.
    for (const Parameters::Configuration &configuration : parameters._Configurations)
    {
        for (const Parameters::Platform &platform : parameters._Platforms)
        {
            //Begin the item definition group.
            file << "\t<ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='" << configuration._Name << "|" << platform._Name << "'\">" << std::endl;

            //Write the compile settings.
            {
                //Begin the compile item.
                file << "\t\t<ClCompile>" << std::endl;

                //Write the properties.
                file << "\t\t\t<WarningLevel>Level3</WarningLevel>" << std::endl;
                file << "\t\t\t<SDLCheck>false</SDLCheck>" << std::endl;

                file << "\t\t\t<PreprocessorDefinitions>WIN32;";
                
                for (const std::string &preprocessor_definition : configuration._PreprocessorDefinitions)
                {
                    file << preprocessor_definition.c_str() << ";";
                }
                
                file << "%(PreprocessorDefinitions)</PreprocessorDefinitions>" << std::endl;

                file << "\t\t\t<ConformanceMode>true</ConformanceMode>" << std::endl;
                file << "\t\t\t<LanguageStandard>stdcpp20</LanguageStandard>" << std::endl;

                file << "\t\t\t<AdditionalIncludeDirectories>";

                for (const std::string &code_include_path : parameters._CodeIncludePaths)
                {
                    file << code_include_path.c_str() << ";";
                }

                file << "%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>" << std::endl;

                file << "\t\t\t<DebugInformationFormat>ProgramDatabase</DebugInformationFormat>" << std::endl;
                file << "\t\t\t<RuntimeLibrary>" << configuration._RuntimeLibrary << "</RuntimeLibrary>" << std::endl;
                file << "\t\t\t<ExceptionHandling>false</ExceptionHandling>" << std::endl;

                //end the compile item.
                file << "\t\t</ClCompile>" << std::endl;
            }

            //Write the link settings.
            {
                //Begin the link item.
                file << "\t\t<Link>" << std::endl;

                //Write the properties.
                file << "\t\t\t<SubSystem>" << configuration._SubSystem << "</SubSystem>" << std::endl;
                file << "\t\t\t<GenerateDebugInformation>true</GenerateDebugInformation>" << std::endl;

                file << "\t\t\t<AdditionalLibraryDirectories>";

                for (const std::string &library_include_path : parameters._LibraryIncludePaths)
                {
                    file << library_include_path.c_str() << ";";
                }

                file << "%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>" << std::endl;

                file << "\t\t\t<AdditionalDependencies>";

                for (const std::string &library_include : configuration._LibraryIncludes)
                {
                    file << library_include.c_str() << ";";
                }

                file << "%(AdditionalDependencies)</AdditionalDependencies>" << std::endl;

                //end the link item.
                file << "\t\t</Link>" << std::endl;
            }

            //Add the pre build event.
            if (!configuration._PreBuildEvent.empty())
            {
                file << "\t\t<PreBuildEvent>" << std::endl;
                file << "\t\t\t<Command>" << configuration._PreBuildEvent.c_str() << "</Command>" << std::endl;
                file << "\t\t</PreBuildEvent>" << std::endl;
            }

            //End the item definition group.
            file << "\t</ItemDefinitionGroup>" << std::endl;
        }
    }

    //Add the item group for all files.
    file << "\t<ItemGroup>" << std::endl;

    for (const Parameters::Directory &directory : parameters._Directories)
    {
        AddCompileFiles(file, directory);
    }

    file << "\t</ItemGroup>" << std::endl;

    //Add the project import.
    file << "\t<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\"/>" << std::endl;

    //Add the extension targets import group.
    file << "\t<ImportGroup Label=\"ExtensionTargets\"></ImportGroup>" << std::endl;

    //End the project item.
    file << "</Project>";

    //Close the file.
    file.close();
}

/*
*	Generates the filters file.
*/
void GenerateVisualStudioSolution::GenerateFiltersFile(const Parameters &parameters)
{
    //Figure out the project file path.
    char file_path_buffer[256];
    sprintf_s(file_path_buffer, "%s\\%s.vcxproj.filters", parameters._DirectoryPath, parameters._ProjectName);

    //Open the file.
    std::ofstream file{ file_path_buffer };

    //Write the header.
    file << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;

    //Start the project.
    file << "<Project ToolsVersion=\"17.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">" << std::endl;

    //Add item groups for files in each top level directory.
    for (const Parameters::Directory &directory : parameters._Directories)
    {
        //Start the item group.
        file << "\t<ItemGroup>" << std::endl;

        //Set up the filter string.
        const std::string filter_string{ directory._Name };

        //Add the filter files.
        AddFilterFiles(file, directory, filter_string);

        //End the item group.
        file << "\t</ItemGroup>" << std::endl;
    }

    //Add item groups for identifiers in each top level directory.
    for (const Parameters::Directory &directory : parameters._Directories)
    {
        //Start the item group.
        file << "\t<ItemGroup>" << std::endl;

        //Set up the filter string.
        const std::string filter_string{ directory._Name };

        //Add the filter identifiers.
        AddFilterIdentifiers(file, directory, filter_string);

        //End the item group.
        file << "\t</ItemGroup>" << std::endl;
    }

    //End the project.
    file << "</Project>";

    //Close the file.
    file.close();
}

/*
*	Generates the solution file.
*/
void GenerateVisualStudioSolution::GenerateSolutionFile(const Parameters &parameters, const std::string &solution_guid, const std::string &project_guid)
{
    //Define constants.
    constexpr const char *GPP_PROJECT_GUID{ "{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}" };

    //Figure out the project file path.
    char file_path_buffer[256];
    sprintf_s(file_path_buffer, "%s\\%s.sln", parameters._DirectoryPath, parameters._ProjectName);

    //Open the file.
    std::ofstream file{ file_path_buffer };

    //Add the header.
    file << "Microsoft Visual Studio Solution File, Format Version 12.00" << std::endl;
    file << "# Visual Studio Version 17" << std::endl;
    file << "VisualStudioVersion = 17.14.36717.8 d17.14" << std::endl; //TODO: Maybe query this in some way?
    file << "MinimumVisualStudioVersion = 10.0.40219.1" << std::endl; //TODO: Maybe query this in some way?

    //Add the project.
    file << "Project(\"" << GPP_PROJECT_GUID << "\") = \"" << parameters._ProjectName << "\", \"" << parameters._ProjectName << ".vcxproj\", \"" << project_guid.c_str() << "\"" << std::endl;
    file << "EndProject" << std::endl;

    //Add the global section.
    {
        //Start the global section.
        file << "Global" << std::endl;

        //Add the pre solution configurations/platforms.
        file << "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution" << std::endl;

        for (const Parameters::Configuration &configuration : parameters._Configurations)
        {
            for (const Parameters::Platform &platform : parameters._Platforms)
            {
                file << "\t\t" << configuration._Name << "|" << platform._Name << " = " << configuration._Name << "|" << platform._Name << std::endl;
            }
        }

        file << "\tEndGlobalSection" << std::endl;

        //Add the post solution configurations/platforms.
        file << "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution" << std::endl;

        for (const Parameters::Configuration &configuration : parameters._Configurations)
        {
            for (const Parameters::Platform &platform : parameters._Platforms)
            {
                file << "\t\t" << project_guid.c_str() << "." << configuration._Name << "|" << platform._Name << ".ActiveCfg = " << configuration._Name << "|" << platform._Name << std::endl;
                file << "\t\t" << project_guid.c_str() << "." << configuration._Name << "|" << platform._Name << ".Build.0 = " << configuration._Name << "|" << platform._Name << std::endl;
            }
        }

        file << "\tEndGlobalSection" << std::endl;

        //Add the solution properties global section.
        file << "\tGlobalSection(SolutionProperties) = preSolution" << std::endl;
        file << "\t\tHideSolutionNode = FALSE" << std::endl;
        file << "\tEndGlobalSection" << std::endl;

        //Add the extensibility globals global section.
        file << "\tGlobalSection(ExtensibilityGlobals) = postSolution" << std::endl;
        file << "\t\tSolutionGuid = " << solution_guid.c_str() << std::endl;
        file << "\tEndGlobalSection" << std::endl;

        //End the global section.
        file << "EndGlobal" << std::endl;
    }

    //Close the file.
    file.close();
}

/*
*   Adds compile files for the given directory.
*/
void GenerateVisualStudioSolution::AddCompileFiles(std::ofstream &file, const Parameters::Directory &directory)
{
    for (const Parameters::Directory &_directory : directory._Directories)
    {
        AddCompileFiles(file, _directory);
    }

    for (const Parameters::File &compile_file : directory._Files)
    {
        const std::string extension{ compile_file._Path.substr(compile_file._Path.find_last_of('.')) };

        const bool is_compile_file{ extension == ".cpp" || extension == ".c" };

        if (is_compile_file)
        {
            file << "\t\t<ClCompile Include=\"" << compile_file._Path.c_str() << "\">" << std::endl;
        }

        else
        {
            file << "\t\t<ClInclude Include=\"" << compile_file._Path.c_str() << "\">" << std::endl;
        }

        if (!compile_file._EnableWarnings)
        {
            file << "\t\t\t<WarningLevel>TurnOffAllWarnings</WarningLevel>" << std::endl;
        }

        if (is_compile_file)
        {
            file << "\t\t</ClCompile>" << std::endl;
        }

        else
        {
            file << "\t\t</ClInclude>" << std::endl;
        }
    }
}

/*
*	Adds filter files.
*/
void GenerateVisualStudioSolution::AddFilterFiles(std::ofstream &file, const Parameters::Directory &directory, const std::string &filter_string)
{
    for (const Parameters::Directory &_directory : directory._Directories)
    {
        const std::string _filter_string{ filter_string + "\\" + std::string(_directory._Name) };
        AddFilterFiles(file, _directory, _filter_string);
    }

    for (const Parameters::File &compile_file : directory._Files)
    {
        const std::string extension{ compile_file._Path.substr(compile_file._Path.find_last_of('.')) };

        if (extension == ".cpp" || extension == ".c")
        {
            file << "\t\t<ClCompile Include=\"" << compile_file._Path.c_str() << "\">" << std::endl;
            file << "\t\t\t<Filter>" << filter_string.c_str() << "</Filter>" << std::endl;
            file << "\t\t</ClCompile>" << std::endl;
        }

        else
        {
            file << "\t\t<ClInclude Include=\"" << compile_file._Path.c_str() << "\">" << std::endl;
            file << "\t\t\t<Filter>" << filter_string.c_str() << "</Filter>" << std::endl;
            file << "\t\t</ClInclude>" << std::endl;
        }
    }
}

/*
*	Adds filter identifiers.
*/
void GenerateVisualStudioSolution::AddFilterIdentifiers(std::ofstream &file, const Parameters::Directory &directory, const std::string &filter_string)
{
    for (const Parameters::Directory &_directory : directory._Directories)
    {
        const std::string _filter_string{ filter_string + "\\" + std::string(_directory._Name) };
        AddFilterIdentifiers(file, _directory, _filter_string);
    }

    file << "\t\t<Filter Include=\"" << filter_string.c_str() << "\">" << std::endl;
    file << "\t\t\t<UniqueIdentifier>" << GenerateGUID().c_str() << "</UniqueIdentifier>" << std::endl;
    file << "\t\t</Filter>" << std::endl;
}