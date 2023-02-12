//Standard library
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

//Macros.
#define ASSERT(CONDITION, MESSAGE) if (!(CONDITION)) { std::cout << MESSAGE << std::endl; }
#define ASSERT_RETURN(CONDITION, MESSAGE) if (!(CONDITION)) { std::cout << MESSAGE << std::endl; return 0; }

/*
*	Parameters class definition.
*/
class Parameters final
{

public:

	//The developer name.
	std::string _DeveloperName;

	//The developer name, with no spaces.
	std::string _DeveloperNameNoSpaces;

	//The projet name.
	std::string _ProjectName;

	//The projet name, with no spaces.
	std::string _ProjectNameNoSpaces;

};

int main(int argument_count, char *arguments[])
{
	//Read the parameters.
	Parameters parameters;

	{
		//Open the config file.
		std::ifstream config_file{ "Project_Generation.ini" };

		//Cache some commonly used variables.
		std::string current_line;
		std::string identifier;
		std::string argument;

		//Go over all the lines.
		while (std::getline(config_file, current_line))
		{
			//Ignore comments.
			if (current_line[0] == '#')
			{
				continue;
			}

			//Ignore empty lines.
			if (current_line.empty())
			{
				continue;
			}

			//Split the current line into identifier and argument.
			const size_t space_position{ current_line.find_first_of(" ") };

			if (space_position != std::string::npos)
			{
				identifier = current_line.substr(0, space_position);
				argument = current_line.substr(space_position + 1, std::string::npos);
			}

			else
			{
				identifier = current_line;
			}

			//Is this the developer name?
			if (identifier == "DEVELOPER_NAME")
			{
				parameters._DeveloperName = argument;

				parameters._DeveloperNameNoSpaces = parameters._DeveloperName;
				parameters._DeveloperNameNoSpaces.erase(std::remove_if(parameters._DeveloperNameNoSpaces.begin(), parameters._DeveloperNameNoSpaces.end(), [](unsigned char x) { return std::isspace(x); }), parameters._DeveloperNameNoSpaces.end());

			}

			//Is this the project name?
			else if (identifier == "PROJECT_NAME")
			{
				parameters._ProjectName = argument;

				parameters._ProjectNameNoSpaces = parameters._ProjectName;
				parameters._ProjectNameNoSpaces.erase(std::remove_if(parameters._ProjectNameNoSpaces.begin(), parameters._ProjectNameNoSpaces.end(), [](unsigned char x) { return std::isspace(x); }), parameters._ProjectNameNoSpaces.end());
			}
		}

		//Close the config file.
		config_file.close();
	}


	ASSERT_RETURN(!parameters._DeveloperName.empty(), "Needs to set deveoloper name!");
	ASSERT_RETURN(!parameters._ProjectName.empty(), "Needs to set project name!");
	
	//Create the ".gitignore" file.
	{
		std::ofstream file{ ".gitignore" };

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
		std::filesystem::create_directory("Code");

		//Create the "Code/Include" directory.
		{
			std::filesystem::create_directory("Code\\Include");

			//Create the Code/Include/Main directory.
			{
				std::filesystem::create_directory("Code\\Include\\Main");

				//Create the game system header file.
				{
					char game_system_header_file_buffer[260];
					sprintf_s(game_system_header_file_buffer, "Code\\Include\\Main\\%sGameSystem.h", parameters._ProjectNameNoSpaces.c_str());

					std::ofstream file{ game_system_header_file_buffer };

					file << "#pragma once" << std::endl;

					file << std::endl;

					file << "//Core." << std::endl;
					file << "#include <Core/Essential/CatalystEssential.h>" << std::endl;

					file << std::endl;

					file << "class ALIGN(8) " << parameters._ProjectNameNoSpaces.c_str() << "GameSystem final" << std::endl;
					file << "{" << std::endl;

					file << std::endl;

					file << "public:" << std::endl;

					file << std::endl;

					file << "\t//Singleton declaration." << std::endl;
					file << "\tDECLARE_SINGLETON(" << parameters._ProjectNameNoSpaces.c_str() << "GameSystem); " << std::endl;

					file << std::endl;

					file << "\t/*" << std::endl;
					file << "\t*\tDefault constructor." << std::endl;
					file << "\t*/" << std::endl;
					file << "\t" << parameters._ProjectNameNoSpaces.c_str() << "GameSystem() NOEXCEPT;" << std::endl;

					file << std::endl;

					file << "\t/*" << std::endl;
					file << "\t*\tInitializes the " << parameters._ProjectName.c_str() << " game system." << std::endl;
					file << "\t*/" << std::endl;
					file << "\tvoid Initialize() NOEXCEPT;" << std::endl;

					file << std::endl;

					file << "\t/*" << std::endl;
					file << "\t*\tPost-initializes the " << parameters._ProjectName.c_str() << " game system." << std::endl;
					file << "\t*/" << std::endl;
					file << "\tvoid PostInitialize() NOEXCEPT;" << std::endl;

					file << std::endl;

					file << "\t/*" << std::endl;
					file << "\t*\tStarts the " << parameters._ProjectName.c_str() << " game." << std::endl;
					file << "\t*/" << std::endl;
					file << "\tvoid StartGame() NOEXCEPT;" << std::endl;

					file << std::endl;
					file << "\t/*" << std::endl;
					file << "\t*\tEnds the " << parameters._ProjectName.c_str() << " game." << std::endl;
					file << "\t*/" << std::endl;
					file << "\tvoid EndGame() NOEXCEPT;" << std::endl;

					file << std::endl;

					file << "\t/*" << std::endl;
					file << "\t*\tTerminates the " << parameters._ProjectName.c_str() << " game system." << std::endl;
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
			std::filesystem::create_directory("Code\\Source");

			//Create the Code/Source/Main directory.
			{
				std::filesystem::create_directory("Code\\Source\\Main");

				//Create the game system source file.
				{
					char game_system_source_file_buffer[260];
					sprintf_s(game_system_source_file_buffer, "Code\\Source\\Main\\%sGameSystem.cpp", parameters._ProjectNameNoSpaces.c_str());

					std::ofstream file{ game_system_source_file_buffer };

					file << "//Header file." << std::endl;
					file << "#include <Main/" << parameters._ProjectNameNoSpaces.c_str() << "GameSystem.h>" << std::endl;

					file << std::endl;

					file << "//Singleton definition." << std::endl;
					file << "DEFINE_SINGLETON(" << parameters._ProjectNameNoSpaces.c_str() << "GameSystem); " << std::endl;

					file << std::endl;

					file << "/*" << std::endl;
					file << "*\tDefault constructor." << std::endl;
					file << "*/" << std::endl;
					file << parameters._ProjectNameNoSpaces.c_str() << "GameSystem::" << parameters._ProjectNameNoSpaces.c_str() << "GameSystem() NOEXCEPT" << std::endl;
					file << "{" << std::endl;
					file << std::endl;
					file << "}" << std::endl;

					file << std::endl;

					file << "/*" << std::endl;
					file << "*\tInitializes the " << parameters._ProjectName.c_str() << " game system." << std::endl;
					file << "*/" << std::endl;
					file << parameters._ProjectNameNoSpaces.c_str() << "GameSystem::Initialize() NOEXCEPT" << std::endl;
					file << "{" << std::endl;
					file << std::endl;
					file << "}" << std::endl;

					file << std::endl;

					file << "/*" << std::endl;
					file << "*\tPost-initializes the " << parameters._ProjectName.c_str() << " game system." << std::endl;
					file << "*/" << std::endl;
					file << parameters._ProjectNameNoSpaces.c_str() << "GameSystem::PostInitialize() NOEXCEPT" << std::endl;
					file << "{" << std::endl;
					file << std::endl;
					file << "}" << std::endl;

					file << std::endl;

					file << "/*" << std::endl;
					file << "*\tStarts the " << parameters._ProjectName.c_str() << " game." << std::endl;
					file << "*/" << std::endl;
					file << parameters._ProjectNameNoSpaces.c_str() << "GameSystem::StartGame() NOEXCEPT" << std::endl;
					file << "{" << std::endl;
					file << std::endl;
					file << "}" << std::endl;

					file << std::endl;

					file << "/*" << std::endl;
					file << "*\tEnds the " << parameters._ProjectName.c_str() << " game." << std::endl;
					file << "*/" << std::endl;
					file << parameters._ProjectNameNoSpaces.c_str() << "GameSystem::EndGame() NOEXCEPT" << std::endl;
					file << "{" << std::endl;
					file << std::endl;
					file << "}" << std::endl;

					file << std::endl;

					file << "/*" << std::endl;
					file << "*\tTerminates the " << parameters._ProjectName.c_str() << " game system." << std::endl;
					file << "*/" << std::endl;
					file << parameters._ProjectNameNoSpaces.c_str() << "GameSystem::" << "Terminate() NOEXCEPT" << std::endl;
					file << "{" << std::endl;
					file << std::endl;
					file << "}";

					file.close();
				}

				//Create the main source file.
				{
					std::ofstream file{ "Code\\Source\\Main\\Main.cpp" };

					file << "//Core." << std::endl;
					file << "#include <Core/Essential/CatalystEssential.h>" << std::endl;

					file << std::endl;

					file << "//Main." << std::endl;
					file << "#include <Main/" << parameters._ProjectNameNoSpaces.c_str() << "GameSystem.h>" << std::endl;

					file << std::endl;

					file << "//Systems." << std::endl;
					file << "#include <Systems/CatalystEngineSystem.h>" << std::endl;

					file << std::endl;

					file << "/*" << std::endl;
					file << "*\tCreates the Catalyst project configuration." << std::endl;
					file << "*/" << std::endl;
					file << "void CreateCatalystProjectConfiguration(CatalystProjectConfiguration *const RESTRICT configuration) NOEXCEPT" << std::endl;
					file << "{" << std::endl;
					file << "\tconfiguration->_GeneralConfiguration._ProjectName = \"" << parameters._ProjectName.c_str() << "\";" << std::endl;

					file << "\tconfiguration->_GeneralConfiguration._InitializationFunction = []()" << std::endl;
					file << "\t{" << std::endl;
					file << "\t\t" << parameters._ProjectNameNoSpaces.c_str() << "GameSystem::Instance->Initialize();" << std::endl;
					file << "\t};" << std::endl;

					file << "\tconfiguration->_GeneralConfiguration._PostInitializationFunction = []()" << std::endl;
					file << "\t{" << std::endl;
					file << "\t\t" << parameters._ProjectNameNoSpaces.c_str() << "GameSystem::Instance->PostInitialize();" << std::endl;
					file << "\t};" << std::endl;

					file << "\tconfiguration->_GeneralConfiguration._StartGameFunction = []()" << std::endl;
					file << "\t{" << std::endl;
					file << "\t\t" << parameters._ProjectNameNoSpaces.c_str() << "GameSystem::Instance->StartGame();" << std::endl;
					file << "\t};" << std::endl;

					file << "\tconfiguration->_GeneralConfiguration._EndGameFunction = []()" << std::endl;
					file << "\t{" << std::endl;
					file << "\t\t" << parameters._ProjectNameNoSpaces.c_str() << "GameSystem::Instance->EndGame();" << std::endl;
					file << "\t};" << std::endl;

					file << "\tconfiguration->_GeneralConfiguration._TerminationFunction = []()" << std::endl;
					file << "\t{" << std::endl;
					file << "\t\t" << parameters._ProjectNameNoSpaces.c_str() << "GameSystem::Instance->Terminate();" << std::endl;
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

	//Create the "Solutions" directory.
	{
		std::filesystem::create_directory("Solutions");

		//Create the ".gitignore" file.
		{
			std::ofstream file{ "Solutions\\.gitignore" };

			file << "#Ignore everything in this directory." << std::endl;
			file << "*" << std::endl;

			file << std::endl;

			file << "#Except this file!" << std::endl;
			file << "!.gitignore" << std::endl;

			file << std::endl;

			file << "#And the project generation files!" << std::endl;
			file << "!Generate_All_Projects.bat" << std::endl;
			file << "!Generate_Android_Project.bat" << std::endl;
			file << "!Generate_Oculus_Quest_Project.bat" << std::endl;
			file << "!Generate_Win64_Project.bat" << std::endl;
			file << "!Project_Generation.ini";

			file.close();
		}

		//Create the "Generate_All_Solutions.bat" file.
		{
			std::ofstream file{ "Solutions\\Generate_All_Solutions.bat" };

			file << "..\\..\\Catalyst-Engine\\Tools\\Binaries\\CatalystEngineSolutionGenerator.exe" << std::endl;
			file << std::endl;

			file << "pause" << std::endl;

			file.close();
		}

		//Create the "Generate_Android_Solution.bat" file.
		{
			std::ofstream file{ "Solutions\\Generate_Android_Solution.bat" };

			file << "..\\..\\Catalyst-Engine\\Tools\\Binaries\\CatalystEngineSolutionGenerator.exe ANDROID" << std::endl;
			file << std::endl;

			file << "pause" << std::endl;

			file.close();
		}

		//Create the "Generate_Oculus_Quest_Solution.bat" file.
		{
			std::ofstream file{ "Solutions\\Generate_Oculus_Quest_Solution.bat" };

			file << "..\\..\\Catalyst-Engine\\Tools\\Binaries\\CatalystEngineSolutionGenerator.exe OCULUS_QUEST" << std::endl;
			file << std::endl;

			file << "pause" << std::endl;

			file.close();
		}

		//Create the "Generate_Win64_Solution.bat" file.
		{
			std::ofstream file{ "Solutions\\Generate_Win64_Solution.bat" };

			file << "..\\..\\Catalyst-Engine\\Tools\\Binaries\\CatalystEngineSolutionGenerator.exe WIN64" << std::endl;
			file << std::endl;

			file << "pause" << std::endl;

			file.close();
		}

		//Create the "Solution_Generation.ini" file.
		{
			std::ofstream file{ "Solutions\\Solution_Generation.ini" };

			file << "#The developer namee." << std::endl;
			file << "DEVELOPER_NAME " << parameters._DeveloperName << std::endl;

			file << std::endl;

			file << "#The project namee." << std::endl;
			file << "PROJECT_NAME " << parameters._ProjectName << std::endl;

			file.close();
		}
	}

	return 0;
}