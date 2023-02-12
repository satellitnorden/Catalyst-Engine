//Standard library
#include <filesystem>
#include <fstream>
#include <iostream>

//Macros.
#define ASSERT(CONDITION, MESSAGE) if (!(CONDITION)) { std::cout << MESSAGE << std::endl; }
#define ASSERT_RETURN(CONDITION, MESSAGE) if (!(CONDITION)) { std::cout << MESSAGE << std::endl; return 0; }

int main(int argument_count, char *arguments[])
{
	ASSERT_RETURN(argument_count > 1, "Needs to pass project name as the first argument!");
	
	//Set up the project names buffers.
	char project_name_with_spaces_buffer[260];
	unsigned with_spaces_index{ 0 };
	char project_name_without_spaces_buffer[260];
	unsigned without_spaces_index{ 0 };

	for (int i{ 1 }; i < argument_count; ++i)
	{
		if (i != 1)
		{
			project_name_with_spaces_buffer[with_spaces_index++] = ' ';
		}

		with_spaces_index += sprintf_s(&(project_name_with_spaces_buffer[with_spaces_index]), 260 - with_spaces_index, "%s", arguments[i]);
		without_spaces_index += sprintf_s(&project_name_without_spaces_buffer[without_spaces_index], 260 - with_spaces_index, "%s", arguments[i]);
	}

	//Cache the directory buffer.
	char directory_buffer[260];
	sprintf_s(directory_buffer, "..\\..\\..\\..\\%s", project_name_without_spaces_buffer);

	//Create the ".gitignore" file.
	{
		char file_buffer[260];
		sprintf_s(file_buffer, "%s\\.gitignore", directory_buffer);

		std::ofstream file{ file_buffer };

		file << "*.crc" << std::endl;
		file << "*.exe" << std::endl;
		file << "*.ilk" << std::endl;
		file << "*.iobj" << std::endl;
		file << "*.iobj" << std::endl;
		file << "*.pdb" << std::endl;
		file << "*.pch" << std::endl;
		file << "*.iobj";

		file.close();
	}

	//Create the "Code" directory.
	{
		char code_directory_buffer[260];
		sprintf_s(code_directory_buffer, "%s\\Code", directory_buffer);

		std::filesystem::create_directory(code_directory_buffer);

		//Create the "Code/Include" directory.
		{
			char code_include_directory_buffer[260];
			sprintf_s(code_include_directory_buffer, "%s\\Include", code_directory_buffer);

			std::filesystem::create_directory(code_include_directory_buffer);

			//Create the Code/Include/Main directory.
			{
				char code_include_main_directory_buffer[260];
				sprintf_s(code_include_main_directory_buffer, "%s\\Main", code_include_directory_buffer);

				std::filesystem::create_directory(code_include_main_directory_buffer);

				//Create the game system header file.
				{
					char game_system_header_file_buffer[260];
					sprintf_s(game_system_header_file_buffer, "%s\\%sGameSystem.h", code_include_main_directory_buffer, project_name_without_spaces_buffer);

					std::ofstream file{ game_system_header_file_buffer };

					file << "#pragma once" << std::endl;

					file << std::endl;

					file << "//Core." << std::endl;
					file << "#include <Core/Essential/CatalystEssential.h>" << std::endl;

					file << std::endl;

					file << "class " << project_name_without_spaces_buffer << "GameSystem final" << std::endl;
					file << "{" << std::endl;

					file << std::endl;

					file << "public:" << std::endl;

					file << std::endl;

					file << "\t//Singleton declaration." << std::endl;
					file << "\tDECLARE_SINGLETON(" << project_name_without_spaces_buffer << "GameSystem); " << std::endl;

					file << std::endl;

					file << "\t/*" << std::endl;
					file << "\t*\tDefault constructor." << std::endl;
					file << "\t*/" << std::endl;
					file << "\t" << project_name_without_spaces_buffer << "GameSystem() NOEXCEPT;" << std::endl;

					file << std::endl;

					file << "\t/*" << std::endl;
					file << "\t*\tInitializes the " << project_name_with_spaces_buffer << " game system." << std::endl;
					file << "\t*/" << std::endl;
					file << "\tvoid Initialize() NOEXCEPT;" << std::endl;

					file << std::endl;

					file << "\t/*" << std::endl;
					file << "\t*\tPost-initializes the " << project_name_with_spaces_buffer << " game system." << std::endl;
					file << "\t*/" << std::endl;
					file << "\tvoid PostInitialize() NOEXCEPT;" << std::endl;

					file << std::endl;

					file << "\t/*" << std::endl;
					file << "\t*\tStarts the " << project_name_with_spaces_buffer << " game." << std::endl;
					file << "\t*/" << std::endl;
					file << "\tvoid StartGame() NOEXCEPT;" << std::endl;

					file << std::endl;
					file << "\t/*" << std::endl;
					file << "\t*\tEnds the " << project_name_with_spaces_buffer << " game." << std::endl;
					file << "\t*/" << std::endl;
					file << "\tvoid StartGame() NOEXCEPT;" << std::endl;

					file << std::endl;

					file << "\t/*" << std::endl;
					file << "\t*\tTerminates the " << project_name_with_spaces_buffer << " game system." << std::endl;
					file << "\t*/" << std::endl;
					file << "\tvoid Terminate() NOEXCEPT;" << std::endl;

					file << std::endl;

					file << "};";

					file.close();
				}
			}
		}

		//Create the "Code/Source" directory.
		{
			char code_source_directory_buffer[260];
			sprintf_s(code_source_directory_buffer, "%s\\Source", code_directory_buffer);

			std::filesystem::create_directory(code_source_directory_buffer);

			//Create the Code/Source/Main directory.
			{
				char code_source_main_directory_buffer[260];
				sprintf_s(code_source_main_directory_buffer, "%s\\Main", code_source_directory_buffer);

				std::filesystem::create_directory(code_source_main_directory_buffer);

				//Create the game system source file.
				{
					char game_system_source_file_buffer[260];
					sprintf_s(game_system_source_file_buffer, "%s\\%sGameSystem.cpp", code_source_main_directory_buffer, project_name_without_spaces_buffer);

					std::ofstream file{ game_system_source_file_buffer };

					file << "//Header file." << std::endl;
					file << "#include <Main/" << project_name_without_spaces_buffer << ".h>" << std::endl;

					file << std::endl;

					file << "//Singleton definition." << std::endl;
					file << "DEFINE_SINGLETON(" << project_name_without_spaces_buffer << "GameSystem); " << std::endl;

					file << std::endl;

					file << "/*" << std::endl;
					file << "*\tDefault constructor." << std::endl;
					file << "*/" << std::endl;
					file << project_name_without_spaces_buffer << "GameSystem::" << project_name_without_spaces_buffer << "GameSystem() NOEXCEPT" << std::endl;
					file << "{" << std::endl;
					file << std::endl;
					file << "}" << std::endl;

					file << std::endl;

					file << "/*" << std::endl;
					file << "*\tInitializes the " << project_name_with_spaces_buffer << " game system." << std::endl;
					file << "*/" << std::endl;
					file << project_name_without_spaces_buffer << "GameSystem::Initialize() NOEXCEPT" << std::endl;
					file << "{" << std::endl;
					file << std::endl;
					file << "}" << std::endl;

					file << std::endl;

					file << "/*" << std::endl;
					file << "*\tPost-initializes the " << project_name_with_spaces_buffer << " game system." << std::endl;
					file << "*/" << std::endl;
					file << project_name_without_spaces_buffer << "GameSystem::PostInitialize() NOEXCEPT" << std::endl;
					file << "{" << std::endl;
					file << std::endl;
					file << "}" << std::endl;

					file << std::endl;

					file << "/*" << std::endl;
					file << "*\tStarts the " << project_name_with_spaces_buffer << " game." << std::endl;
					file << "*/" << std::endl;
					file << project_name_without_spaces_buffer << "GameSystem::StartGame() NOEXCEPT" << std::endl;
					file << "{" << std::endl;
					file << std::endl;
					file << "}" << std::endl;

					file << std::endl;

					file << "/*" << std::endl;
					file << "*\tEnds the " << project_name_with_spaces_buffer << " game." << std::endl;
					file << "*/" << std::endl;
					file << project_name_without_spaces_buffer << "GameSystem::EndGame() NOEXCEPT" << std::endl;
					file << "{" << std::endl;
					file << std::endl;
					file << "}" << std::endl;

					file << std::endl;

					file << "/*" << std::endl;
					file << "*\tTerminates the " << project_name_with_spaces_buffer << " game system." << std::endl;
					file << "*/" << std::endl;
					file << project_name_without_spaces_buffer << "GameSystem::" << "Terminate() NOEXCEPT" << std::endl;
					file << "{" << std::endl;
					file << std::endl;
					file << "}";

					file.close();
				}

				//Create the main source file.
				{
					char main_source_file_buffer[260];
					sprintf_s(main_source_file_buffer, "%s\\Main.cpp", code_source_main_directory_buffer);

					std::ofstream file{ main_source_file_buffer };

					file << "//Core." << std::endl;
					file << "#include <Core/Essential/CatalystEssential.h>" << std::endl;

					file << std::endl;

					file << "//Systems." << std::endl;
					file << "#include <Systems/CatalystEngineSystem.h>" << std::endl;

					file << std::endl;

					file << "/*" << std::endl;
					file << "*\tCreates the Catalyst project configuration." << std::endl;
					file << "*/" << std::endl;
					file << "void CreateCatalystProjectConfiguration(CatalystProjectConfiguration *const RESTRICT configuration) NOEXCEPT" << std::endl;
					file << "{" << std::endl;
					file << "\tconfiguration->_GeneratlConfiguration._ProjectName = \"" << project_name_with_spaces_buffer << "\"" << std::endl;

					file << "\tconfiguration->_GeneratlConfiguration._InitializationFunction = []()" << std::endl;
					file << "\t{" << std::endl;
					file << "\t\t" << project_name_without_spaces_buffer << "GameSystem::Instance->Initialize();" << std::endl;
					file << "\t};" << std::endl;

					file << "\tconfiguration->_GeneratlConfiguration._PostInitializationFunction = []()" << std::endl;
					file << "\t{" << std::endl;
					file << "\t\t" << project_name_without_spaces_buffer << "GameSystem::Instance->PostInitialize();" << std::endl;
					file << "\t};" << std::endl;

					file << "\tconfiguration->_GeneratlConfiguration._StartGameFunction = []()" << std::endl;
					file << "\t{" << std::endl;
					file << "\t\t" << project_name_without_spaces_buffer << "GameSystem::Instance->StartGame();" << std::endl;
					file << "\t};" << std::endl;

					file << "\tconfiguration->_GeneratlConfiguration._EndGameFunction = []()" << std::endl;
					file << "\t{" << std::endl;
					file << "\t\t" << project_name_without_spaces_buffer << "GameSystem::Instance->EndGame();" << std::endl;
					file << "\t};" << std::endl;

					file << "\tconfiguration->_GeneratlConfiguration._TerminationFunction = []()" << std::endl;
					file << "\t{" << std::endl;
					file << "\t\t" << project_name_without_spaces_buffer << "GameSystem::Instance->Terminate();" << std::endl;
					file << "\t};" << std::endl;

					file << "}" << std::endl;

					file << std::endl;

					file << "MAIN_FUNCTION" << std::endl;
					file << "{" << std::endl;
					file << "\t//Create the Catalyst project configuration." << std::endl;
					file << "\tCatalystProjectConfiguration configuration;" << std::endl;
					file << "\tCreateCatalystProjectConfiguration(&configuration);" << std::endl;
					file << std::endl;
					file << "\t//Initialize the engine system." << std::endl;
					file << "\tCatalystEngineSystem::Instance->Initialize(configuration);" << std::endl;
					file << std::endl;
					file << "\t//Main game loop." << std::endl;
					file << "\twhile (CatalystEngineSystem::Instance->Update());" << std::endl;
					file << std::endl;
					file << "\t//Terminate the engine system." << std::endl;
					file << "\tCatalystEngineSystem::Instance->Terminate();" << std::endl;
					file << "}";

					file.close();
				}
			}
		}
	}

	return 0;
}