//Header file.
#include <GenerateProjects/GenerateEmbeddedResources.h>

//Core.
#include <Core/Utilities.h>

//STD.
#include <filesystem>
#include <fstream>

/*
*	Generates!
*/
void GenerateEmbeddedResources::Generate(const std::vector<std::string> &embedded_resources, const char *const directory_path)
{
    //Create the directory.
    std::filesystem::create_directories(std::string(directory_path));

   //Generate!
#if defined(PLATFORM_WIN64)
    GenerateWin64(embedded_resources, directory_path);
#elif defined(PLATFORM_LINUX)
    GenerateLinux(embedded_resources, directory_path);
#else
    #error "Unknown platform!"
#endif
}

#if defined(PLATFORM_WIN64)
/*
*	Generates for Win64.
*/
void GenerateEmbeddedResources::GenerateWin64(const std::vector<std::string> &embedded_resources, const char *const directory_path)
{
    //Create the "resource.h" file.
    {
        //Figure out the project file path.
        char file_path_buffer[256];
        PRINT_BUFFER(file_path_buffer, "%s\\resource.h", directory_path);

        //Open the file.
        std::ofstream file{ file_path_buffer };

        //Just a bunch of boilerplate...
        file << "//{{NO_DEPENDENCIES}}" << std::endl;
        file << "// Microsoft Visual C++ generated include file." << std::endl;
        file << "// Used by Resources.rc" << std::endl;
        file << "//" << std::endl;
        file << std::endl;

        file << "// Next default values for new objects" << std::endl;
        file << "//" << std::endl;
        file << "#ifdef APSTUDIO_INVOKED" << std::endl;
        file << "#ifndef APSTUDIO_READONLY_SYMBOLS" << std::endl;
        file << "#define _APS_NEXT_RESOURCE_VALUE        102" << std::endl;
        file << "#define _APS_NEXT_COMMAND_VALUE         40001" << std::endl;
        file << "#define _APS_NEXT_SYMED_VALUE           101" << std::endl;
        file << "#endif" << std::endl;
        file << "#endif" << std::endl;

        //Close the file.
        file.close();
    }

    //Generate the "Resources.rc" file.
    {
        //Figure out the project file path.
        char file_path_buffer[256];
        PRINT_BUFFER(file_path_buffer, "%s\\Resources.rc", directory_path);

        //Open the file.
        std::ofstream file{ file_path_buffer };

        //Just a bunch of boilerplate...
        file << "// Microsoft Visual C++ generated resource script." << std::endl;
        file << "//" << std::endl;
        file << "#include \"resource.h\"" << std::endl;
        file << std::endl;

        file << "#define APSTUDIO_READONLY_SYMBOLS" << std::endl;
        file << "/////////////////////////////////////////////////////////////////////////////" << std::endl;
        file << "//" << std::endl;
        file << "// Generated from the TEXTINCLUDE 2 resource." << std::endl;
        file << "//" << std::endl;
        file << "#include \"winres.h\"" << std::endl;
        file << std::endl;

        file << "/////////////////////////////////////////////////////////////////////////////" << std::endl;
        file << "#undef APSTUDIO_READONLY_SYMBOLS" << std::endl;
        file << std::endl;

        file << "/////////////////////////////////////////////////////////////////////////////" << std::endl;
        file << "// English (United States) resources" << std::endl;
        file << std::endl;

        file << "#if !defined(AFX_RESOURCE_DLL) || defined(AFX_TARG_ENU)" << std::endl;
        file << "LANGUAGE LANG_ENGLISH, SUBLANG_ENGLISH_US" << std::endl;
        file << std::endl;

        file << "#ifdef APSTUDIO_INVOKED" << std::endl;
        file << "/////////////////////////////////////////////////////////////////////////////" << std::endl;
        file << "//" << std::endl;
        file << "// TEXTINCLUDE" << std::endl;
        file << "//" << std::endl;
        file << std::endl;

        file << "1 TEXTINCLUDE " << std::endl;
        file << "BEGIN" << std::endl;
        file << "    \"resource.h\\0\"" << std::endl;
        file << "END" << std::endl;
        file << std::endl;

        file << "2 TEXTINCLUDE " << std::endl;
        file << "BEGIN" << std::endl;
        file << "    \"#include \"\"winres.h\"\"\\r\\n\"" << std::endl;
        file << "    \"\\0\"" << std::endl;
        file << "END" << std::endl;
        file << std::endl;

        file << "3 TEXTINCLUDE " << std::endl;
        file << "BEGIN" << std::endl;
        file << "    \"\\r\\n\"" << std::endl;
        file << "    \"\\0\"" << std::endl;
        file << "END" << std::endl;
        file << std::endl;

        file << "#endif    // APSTUDIO_INVOKED" << std::endl;
        file << std::endl;

        file << "/////////////////////////////////////////////////////////////////////////////" << std::endl;
        file << "//" << std::endl;
        file << "// RCDATA" << std::endl;
        file << "//" << std::endl;
        file << std::endl;

        file << "#endif    // English (United States) resources" << std::endl;
        file << "/////////////////////////////////////////////////////////////////////////////" << std::endl;
        file << std::endl;

        file << "#ifndef APSTUDIO_INVOKED" << std::endl;
        file << "/////////////////////////////////////////////////////////////////////////////" << std::endl;
        file << "//" << std::endl;
        file << "// Generated from the TEXTINCLUDE 3 resource." << std::endl;
        file << "//" << std::endl;
        file << std::endl;

        //Now we finally get to the actually important stuff - Adding the resources. (:
        for (size_t embedded_resource_index{ 0 }; embedded_resource_index < embedded_resources.size(); ++embedded_resource_index)
        {
            const std::string &embedded_resource{ embedded_resources[embedded_resource_index] };

            const char *identifier{ &embedded_resource[embedded_resource.length() - 1] };

            while ((*identifier) != '\\' && (*identifier) != '/')
            {
                --identifier;
            }

            ++identifier;

            std::string file_path;

            for (const char character : embedded_resource)
            {
                if (character == '\\')
                {
                    file_path += '\\';
                    file_path += '\\';
                }

                else
                {
                    file_path += character;
                }
            }

            file << identifier << " RCDATA \"" << file_path.c_str() << "\"" << std::endl;
        }

        file << "/////////////////////////////////////////////////////////////////////////////" << std::endl;
        file << "#endif    // not APSTUDIO_INVOKED" << std::endl;

        //Close the file.
        file.close();
    }
}
#endif

#if defined(PLATFORM_LINUX)
/*
*	Generates for Linux.
*/
void GenerateEmbeddedResources::GenerateLinux(const std::vector<std::string> &embedded_resources, const char *const directory_path)
{

}
#endif