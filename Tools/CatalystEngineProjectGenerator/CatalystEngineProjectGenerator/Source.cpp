//Standard library.
#include <filesystem>
#include <fstream>
#include <iostream>

#undef NULL

//Enumeration covering all distributions.
enum class Distribution
{
	NULL,
	STEAM
};

//Enumeration covering all physics.
enum class Physics
{
	NULL,
	PHYSX
};

/*
*	Generates Android.
*/
void GenerateAndroid(const char *const developer_name, const char *const project_name, const Distribution distribution, const Physics physics)
{
	//Cache the lower case versions of developer_name and project_name.
	std::string lower_developer_name{ developer_name };
	std::transform(lower_developer_name.begin(), lower_developer_name.end(), lower_developer_name.begin(), [](unsigned char character) { return std::tolower(character); });

	std::string lower_project_name{ project_name };
	std::transform(lower_project_name.begin(), lower_project_name.end(), lower_project_name.begin(), [](unsigned char character) { return std::tolower(character); });

	//Remember the error code for filesystem functions.
	std::error_code error_code;

	//Read the template CMakeLists.txt file and output a new file.
	std::ifstream cmake_lists_input_file{ "C:\\Github\\Catalyst-Engine\\Templates\\CMakeLists_Android_Template.txt" };
	std::ofstream cmake_lists_output_file{ "CMakeLists_Android.txt" };

	std::string cmake_lists_line;

	while (std::getline(cmake_lists_input_file, cmake_lists_line))
	{
		{
			const size_t project_name_position{ cmake_lists_line.find("[PROJECT_NAME]") };

			if (project_name_position != std::string::npos)
			{
				cmake_lists_line.replace(project_name_position, strlen("[PROJECT_NAME]"), project_name);
			}
		}

		cmake_lists_output_file << cmake_lists_line << std::endl;
	}

	//Close the files.
	cmake_lists_input_file.close();
	cmake_lists_output_file.close();

	//Remove all the contents of the Android folder.
	std::filesystem::remove_all("Android", error_code);

	if (error_code)
	{
		std::cout << error_code.message() << std::endl;
	}

	//Copy the Android project template.
	std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Templates\\Android_Project_Template", "Android", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive, error_code);

	if (error_code)
	{
		std::cout << error_code.message() << std::endl;
	}

	//Copy the CMakeLists.txt file.
	std::filesystem::copy("CMakeLists_Android.txt", "Android\\app\\src\\main\\cpp\\CMakeLists.txt", std::filesystem::copy_options::overwrite_existing, error_code);

	if (error_code)
	{
		std::cout << error_code.message() << std::endl;
	}

	//Modify the AndroidManifest.xml file
	{
		std::ifstream input_file{ "Android\\app\\src\\main\\AndroidManifest.xml" };
		std::ofstream output_file{ "Android\\app\\src\\main\\AndroidManifest_Temporary.xml" };

		std::string input_line;

		while (std::getline(input_file, input_line))
		{
			{
				const size_t developer_name_position{ input_line.find("[DEVELOPER_NAME]") };

				if (developer_name_position != std::string::npos)
				{
					input_line.replace(developer_name_position, strlen("[DEVELOPER_NAME]"), lower_developer_name);
				}
			}

			{
				const size_t project_name_position{ input_line.find("[PROJECT_NAME]") };

				if (project_name_position != std::string::npos)
				{
					input_line.replace(project_name_position, strlen("[PROJECT_NAME]"), lower_project_name);
				}
			}

			{
				const size_t library_name_position{ input_line.find("[LIBRARY_NAME]") };

				if (library_name_position != std::string::npos)
				{
					char buffer[128];
					sprintf_s(buffer, "%s_Android-lib", project_name);

					input_line.replace(library_name_position, strlen("[LIBRARY_NAME]"), buffer);
				}
			}

			output_file << input_line << std::endl;
		}

		//Close the files.
		input_file.close();
		output_file.close();

		//Replace the file.
		std::filesystem::copy("Android\\app\\src\\main\\AndroidManifest_Temporary.xml", "Android\\app\\src\\main\\AndroidManifest.xml", std::filesystem::copy_options::overwrite_existing, error_code);

		if (error_code)
		{
			std::cout << error_code.message() << std::endl;
		}

		std::filesystem::remove("Android\\app\\src\\main\\AndroidManifest_Temporary.xml", error_code);

		if (error_code)
		{
			std::cout << error_code.message() << std::endl;
		}
	}

	//Modify the strings.xml file
	{
		std::ifstream input_file{ "Android\\app\\src\\main\\res\\values\\strings.xml" };
		std::ofstream output_file{ "Android\\app\\src\\main\\res\\values\\strings_Temporary.xml" };

		std::string input_line;

		while (std::getline(input_file, input_line))
		{
			{
				const size_t position{ input_line.find("[PROJECT_NAME]") };

				if (position != std::string::npos)
				{
					input_line.replace(position, strlen("[PROJECT_NAME]"), project_name);
				}
			}

			output_file << input_line << std::endl;
		}

		//Close the files.
		input_file.close();
		output_file.close();

		//Replace the file.
		std::filesystem::copy("Android\\app\\src\\main\\res\\values\\strings_Temporary.xml", "Android\\app\\src\\main\\res\\values\\strings.xml", std::filesystem::copy_options::overwrite_existing, error_code);

		if (error_code)
		{
			std::cout << error_code.message() << std::endl;
		}

		std::filesystem::remove("Android\\app\\src\\main\\res\\values\\strings_Temporary.xml", error_code);

		if (error_code)
		{
			std::cout << error_code.message() << std::endl;
		}
	}

	//Modify the build.gradle file
	{
		std::ifstream input_file{ "Android\\app\\build.gradle" };
		std::ofstream output_file{ "Android\\app\\build_Temporary.gradle" };

		std::string input_line;

		while (std::getline(input_file, input_line))
		{
			{
				const size_t position{ input_line.find("[DEVELOPER_NAME]") };

				if (position != std::string::npos)
				{
					input_line.replace(position, strlen("[DEVELOPER_NAME]"), lower_developer_name);
				}
			}

			{
				const size_t position{ input_line.find("[PROJECT_NAME]") };

				if (position != std::string::npos)
				{
					input_line.replace(position, strlen("[PROJECT_NAME]"), lower_project_name);
				}
			}

			output_file << input_line << std::endl;
		}

		//Close the files.
		input_file.close();
		output_file.close();

		//Replace the file.
		std::filesystem::copy("Android\\app\\build_Temporary.gradle", "Android\\app\\build.gradle", std::filesystem::copy_options::overwrite_existing, error_code);

		if (error_code)
		{
			std::cout << error_code.message() << std::endl;
		}

		std::filesystem::remove("Android\\app\\build_Temporary.gradle", error_code);

		if (error_code)
		{
			std::cout << error_code.message() << std::endl;
		}
	}

	//Copy over the relevant resource collections.
	{
		std::filesystem::create_directory("Android\\app\\src\\main\\assets");
		
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Resources\\Final\\CatalystEngineResourceCollection_0.crc", "Android\\app\\src\\main\\assets\\", std::filesystem::copy_options::overwrite_existing, error_code);
	
		if (error_code)
		{
			std::cout << error_code.message() << std::endl;
		}

		for (const auto &entry : std::filesystem::directory_iterator(std::string("..\\Resources\\Final")))
		{
			const std::string file_path{ entry.path().string() };

			if (file_path != ".gitignore")
			{
				std::filesystem::copy(file_path, "Android\\app\\src\\main\\assets\\", std::filesystem::copy_options::overwrite_existing, error_code);

				if (error_code)
				{
					std::cout << error_code.message() << std::endl;
				}
			}
		}
	}
}

/*
*	Generates Win64.
*/
void GenerateWin64(const char *const developer_name, const char *const project_name, const Distribution distribution, const Physics physics)
{
	//Remember the error code for filesystem functions.
	std::error_code error_code;

	//Read the template CMakeLists.txt file and output a new file.
	std::ifstream cmake_lists_input_file{ "C:\\Github\\Catalyst-Engine\\Templates\\CMakeLists_Win64_Template.txt" };
	std::ofstream cmake_lists_output_file{ "CMakeLists.txt" };

	std::string cmake_lists_line;

	while (std::getline(cmake_lists_input_file, cmake_lists_line))
	{
		{
			const size_t project_name_position{ cmake_lists_line.find("[PROJECT_NAME]") };

			if (project_name_position != std::string::npos)
			{
				cmake_lists_line.replace(project_name_position, strlen("[PROJECT_NAME]"), project_name);
			}
		}

		{
			const size_t catalyst_distribution_position{ cmake_lists_line.find("[CATALYST_DISTRIBUTION]") };

			if (catalyst_distribution_position != std::string::npos)
			{
				const char* distribution_string{ "CATALYST_DISTRIBUTION_NULL" };

				switch (distribution)
				{
					case Distribution::STEAM:
					{
						distribution_string = "CATALYST_DISTRIBUTION_STEAM";

						break;
					}
				}

				cmake_lists_line.replace(catalyst_distribution_position, strlen("[CATALYST_DISTRIBUTION]"), distribution_string);
			}
		}

		{
			const size_t catalyst_physics_position{ cmake_lists_line.find("[CATALYST_PHYSICS]") };

			if (catalyst_physics_position != std::string::npos)
			{
				const char *physics_string{ "CATALYST_PHYSICS_NULL" };

				switch (physics)
				{
					case Physics::PHYSX:
					{
						physics_string = "CATALYST_PHYSICS_PHYSX";

						break;
					}
				}

				cmake_lists_line.replace(catalyst_physics_position, strlen("[CATALYST_PHYSICS]"), physics_string);
			}
		}

		cmake_lists_output_file << cmake_lists_line << std::endl;
	}

	//Close the files.
	cmake_lists_input_file.close();
	cmake_lists_output_file.close();

	//Remove all the contents of the Win64 folder.
	std::filesystem::remove_all("Win64", error_code);

	if (error_code)
	{
		std::cout << error_code.message() << std::endl;
	}

	//Win64 stuff.
	{
		//Generate the Win64 project.
		std::filesystem::create_directories("Win64\\Win64", error_code);

		if (error_code)
		{
			std::cout << error_code.message() << std::endl;
		}

		//Run CMake.
		system("cmake -B Win64\\Win64");

		//Copy the relevant .dll files to the correct folders.
		std::filesystem::create_directory("Win64\\Win64\\Debug");
		std::filesystem::create_directory("Win64\\Win64\\DebugEditor");
		std::filesystem::create_directory("Win64\\Win64\\Profile");
		std::filesystem::create_directory("Win64\\Win64\\ProfileEditor");
		std::filesystem::create_directory("Win64\\Win64\\Final");

		//Copy assimp.dll.
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\assimp.dll", "Win64\\Win64\\Debug", std::filesystem::copy_options::overwrite_existing);
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\assimp.dll", "Win64\\Win64\\DebugEditor", std::filesystem::copy_options::overwrite_existing);
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\assimp.dll", "Win64\\Win64\\Profile", std::filesystem::copy_options::overwrite_existing);
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\assimp.dll", "Win64\\Win64\\ProfileEditor", std::filesystem::copy_options::overwrite_existing);

		//Copy steam_api64.dll.
		if (distribution == Distribution::STEAM)
		{
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\steam_api64.dll", "Win64\\Win64\\Debug", std::filesystem::copy_options::overwrite_existing);
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\steam_api64.dll", "Win64\\Win64\\DebugEditor", std::filesystem::copy_options::overwrite_existing);
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\steam_api64.dll", "Win64\\Win64\\Profile", std::filesystem::copy_options::overwrite_existing);
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\steam_api64.dll", "Win64\\Win64\\ProfileEditor", std::filesystem::copy_options::overwrite_existing);
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\steam_api64.dll", "Win64\\Win64\\Final", std::filesystem::copy_options::overwrite_existing);
		}

		//Copy PhysX dll's.
		if (physics == Physics::PHYSX)
		{
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\PhysX_64.dll", "Win64\\Win64\\Debug", std::filesystem::copy_options::overwrite_existing);
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\PhysXCommon_64.dll", "Win64\\Win64\\Debug", std::filesystem::copy_options::overwrite_existing);
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\PhysXCooking_64.dll", "Win64\\Win64\\Debug", std::filesystem::copy_options::overwrite_existing);
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\PhysXFoundation_64.dll", "Win64\\Win64\\Debug", std::filesystem::copy_options::overwrite_existing);

			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\PhysX_64.dll", "Win64\\Win64\\DebugEditor", std::filesystem::copy_options::overwrite_existing);
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\PhysXCommon_64.dll", "Win64\\Win64\\DebugEditor", std::filesystem::copy_options::overwrite_existing);
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\PhysXCooking_64.dll", "Win64\\Win64\\DebugEditor", std::filesystem::copy_options::overwrite_existing);
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\PhysXFoundation_64.dll", "Win64\\Win64\\DebugEditor", std::filesystem::copy_options::overwrite_existing);

			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\steam_api64.dll", "Win64\\Win64\\Profile", std::filesystem::copy_options::overwrite_existing);
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\PhysXCommon_64.dll", "Win64\\Win64\\Profile", std::filesystem::copy_options::overwrite_existing);
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\PhysXCooking_64.dll", "Win64\\Win64\\Profile", std::filesystem::copy_options::overwrite_existing);
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\PhysXFoundation_64.dll", "Win64\\Win64\\Profile", std::filesystem::copy_options::overwrite_existing);

			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\steam_api64.dll", "Win64\\Win64\\ProfileEditor", std::filesystem::copy_options::overwrite_existing);
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\PhysXCommon_64.dll", "Win64\\Win64\\ProfileEditor", std::filesystem::copy_options::overwrite_existing);
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\PhysXCooking_64.dll", "Win64\\Win64\\ProfileEditor", std::filesystem::copy_options::overwrite_existing);
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\PhysXFoundation_64.dll", "Win64\\Win64\\ProfileEditor", std::filesystem::copy_options::overwrite_existing);

			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Final\\steam_api64.dll", "Win64\\Win64\\Final", std::filesystem::copy_options::overwrite_existing);
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Final\\PhysXCommon_64.dll", "Win64\\Win64\\Final", std::filesystem::copy_options::overwrite_existing);
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Final\\PhysXCooking_64.dll", "Win64\\Win64\\Final", std::filesystem::copy_options::overwrite_existing);
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Final\\PhysXFoundation_64.dll", "Win64\\Win64\\Final", std::filesystem::copy_options::overwrite_existing);
		}
	}

	//Copy the CMakeLists.txt file to CMakeLists_Windows, and remove CMakeLists.txt. Nice to save the CMakeLists.txt file, to look at it later. (:
	std::filesystem::copy("CMakeLists.txt", "CMakeLists_Windows.txt", std::filesystem::copy_options::overwrite_existing, error_code);

	if (error_code)
	{
		std::cout << error_code.message() << std::endl;
	}

	std::filesystem::remove("CMakeLists.txt", error_code);

	if (error_code)
	{
		std::cout << error_code.message() << std::endl;
	}
}

int main(int argc, char *argv[])
{
	//Retrieve the developer name.
	const char *const developer_name{ argv[1] };

	//Retrieve the project name.
	const char *const project_name{ argv[2] };

	//Process remaining arguments.
	Distribution distribution{ Distribution::NULL };
	Physics physics{ Physics::NULL };

	for (int i{ 3 }; i < argc; ++i)
	{
		if (strcmp(argv[i], "DISTRIBUTION_STEAM") == 0)
		{
			distribution = Distribution::STEAM;
		}

		if (strcmp(argv[i], "PHYSICS_PHYSX") == 0)
		{
			physics = Physics::PHYSX;
		}
	}

	//Generate Android.
	GenerateAndroid(developer_name, project_name, distribution, physics);

	//Generate Win64.
	GenerateWin64(developer_name, project_name, distribution, physics);

	return 0;
}