#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>

#define CHECK_ERROR_CODE() if (error_code) { std::cout << "Error at " << __LINE__ << ": " << error_code.message() << std::endl; return 1; }

enum class Physics
{
	NONE,
	PHYSX
};

class Parameters final
{

public:

	//The project name.
	std::string _ProjectName;

	//The physics.
	Physics _Physics{ Physics::NONE };

};

int main(int argument_count, char *arguments[])
{
	//Read the parameters.
	Parameters parameters;

	{
		//Open the config file.
		std::ifstream config_file{ "..\\..\\Solution_Generation.ini" };

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

			//Is this the project name?
			if (identifier == "PROJECT_NAME")
			{
				parameters._ProjectName = argument;
			}

			//Is this the physics?
			else if (identifier == "PHYSICS")
			{
				if (argument == "PHYSX")
				{
					parameters._Physics = Physics::PHYSX;
				}
			}

			/*
			//Should the mlpack library be used?
			else if (identifier == "USE_MLPACK_LIBRARY")
			{
				general_parameters._UseMlpackLibrary = true;
			}

			//Should the environment resource collection be included?
			else if (identifier == "INCLUDE_ENVIRONMENT_RESOURCE_COLLECTION")
			{
				general_parameters._IncludeEnvironmentResourceCollection = true;
			}

			//Should the extra resource collection be included?
			else if (identifier == "INCLUDE_EXTRA_RESOURCE_COLLECTION")
			{
				general_parameters._IncludeExtraResourceCollection = true;
			}

			//Should code generation be enabled?
			else if (identifier == "ENABLE_CODE_GENERATION")
			{
				general_parameters._EnableCodeGeneration = true;
			}

			//is this the script generator game include
			else if (identifier == "SCRIPT_GENERATOR_GAME_INCLUDE")
			{
				general_parameters._ScriptGeneratorGameInclude = argument;
			}

			//Should android force landscape mode?
			else if (identifier == "ANDROID_FORCE_LANDSCAPE_MODE")
			{
				android_parameters._ForceLandscapeMode = true;
			}

			//Should android force portrait mode?
			else if (identifier == "ANDROID_FORCE_PORTRAIT_MODE")
			{
				android_parameters._ForcePortraitMode = true;
			}

			//Should a resource collection be included for Android?
			else if (identifier == "ANDROID_INCLUDE_RESOURCE_COLLECTION")
			{
				android_parameters._IncludedResourceCollections.emplace_back(argument);
			}

			//Is this the Android keystore file path?
			else if (identifier == "ANDROID_KEYSTORE_FILE_PATH")
			{
				android_parameters._KeystoreFilePath = argument;
			}

			//Is this the Android keystore password?
			else if (identifier == "ANDROID_KEYSTORE_PASSWORD")
			{
				android_parameters._KeystorePassword = argument;
			}

			//Is this the Android keystore alias?
			else if (identifier == "ANDROID_KEYSTORE_ALIAS")
			{
				android_parameters._KeystoreAlias = argument;
			}

			//Should Android use asset pack?
			else if (identifier == "ANDROID_USE_ASSET_PACK")
			{
				android_parameters._UseAssetPack = true;
			}

			//Is this the Win64 distribution?
			else if (identifier == "WIN64_DISTRIBUTION")
			{
				if (argument == "STEAM")
				{
					win64_parameters._Distribution = Win64Distribution::STEAM;
				}
			}
			*/
		}

		//Close the config file.
		config_file.close();
	}

	//The error code.
	std::error_code error_code;

	//Cache the builds directory.
	char builds_directory[256];
	sprintf_s(builds_directory, "..\\..\\..\\Builds");

	//Create the directory, if it doesn't exist.
	std::filesystem::create_directory(builds_directory, error_code); CHECK_ERROR_CODE();

	//Retrieve the current date and time.
	std::string current_date_time;

	{
		const time_t time{ std::time(nullptr) };
		const tm *const time_info{ std::localtime(&time) };
		std::ostringstream string_stream;
		string_stream << std::put_time(time_info, "%Y-%m-%d %H.%M.%S");

		current_date_time = string_stream.str().c_str();
	}

	//Cache the build directory.
	char build_directory[256];
	sprintf_s(build_directory, "%s\\%s", builds_directory, current_date_time.c_str());

	//Create the directory, if it doesn't exist.
	std::filesystem::create_directory(build_directory, error_code); CHECK_ERROR_CODE();

	//Copy over the .exe file.
	{
		char origin[256];
		sprintf_s(origin, "Final\\%s_Win64.exe", parameters._ProjectName.c_str());

		char destination[256];
		sprintf_s(destination, "%s\\%s.exe", build_directory, parameters._ProjectName.c_str());

		std::filesystem::copy_file(origin, destination, error_code); CHECK_ERROR_CODE();
	}

	//Set up the engine content.
	{
		char directory[256];
		sprintf_s(directory, "%s\\EngineContent", build_directory);

		std::filesystem::create_directory(directory, error_code); CHECK_ERROR_CODE();

		//Copy over the base asset collection.
		{
			char origin[256];
			sprintf_s(origin, "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Collections\\Base.cac");

			char destination[256];
			sprintf_s(destination, "%s\\Base.cac", directory);

			std::filesystem::copy_file(origin, destination, error_code); CHECK_ERROR_CODE();
		}

		//*sigh* need to copy over the old resource collection as well.
		char old_directory[256];
		sprintf_s(old_directory, "%s\\Old", directory); CHECK_ERROR_CODE();

		std::filesystem::create_directory(old_directory);

		for (const auto &entry : std::filesystem::directory_iterator(std::string("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Intermediate\\Base")))
		{
			//Cache the file path.
			const std::string file_path{ entry.path().string() };

			//Figure out the name.
			std::string name;

			{
				const size_t last_slash_position{ file_path.find_last_of("\\") };
				name = file_path.substr(last_slash_position + 1, file_path.length() - last_slash_position);
			}

			//Copy!
			char destination[256];
			sprintf_s(destination, "%s\\Old\\%s", directory, name.c_str());

			std::filesystem::copy(file_path, destination, error_code); CHECK_ERROR_CODE();
		}
	}

	//Set up the game content.
	{
		char directory[256];
		sprintf_s(directory, "%s\\GameContent", build_directory);

		std::filesystem::create_directory(directory, error_code); CHECK_ERROR_CODE();

		//Copy over the asset collections.
		for (const auto &entry : std::filesystem::directory_iterator(std::string("..\\..\\..\\Content\\Collections")))
		{
			//Cache the file path.
			const std::string file_path{ entry.path().string() };

			//Ignore ".gitignore".
			if (file_path.find(".gitignore") != std::string::npos)
			{
				continue;
			}

			//Figure out the name.
			std::string name;

			{
				const size_t last_slash_position{ file_path.find_last_of("\\") };
				name = file_path.substr(last_slash_position + 1, file_path.length() - last_slash_position);
			}

			//Copy!
			char destination[256];
			sprintf_s(destination, "%s\\%s", directory, name.c_str());

			std::filesystem::copy_file(file_path, destination, error_code); CHECK_ERROR_CODE();
		}
	}

	//Copy over physics libraries.
	if (parameters._Physics == Physics::PHYSX)
	{
		std::filesystem::copy("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Final\\PhysX_64.dll", build_directory, std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Final\\PhysXCommon_64.dll", build_directory, std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Final\\PhysXCooking_64.dll", build_directory, std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Final\\PhysXFoundation_64.dll", build_directory, std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
	}

	//Return. (:
	return 0;
}