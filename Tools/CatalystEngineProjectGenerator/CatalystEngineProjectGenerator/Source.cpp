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
void GenerateAndroid(const char* const project_name, const Distribution distribution, const Physics physics)
{
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
}

/*
*	Generates Win64.
*/
void GenerateWin64(const char *const project_name, const Distribution distribution, const Physics physics)
{
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

	//Win64 stuff.
	{
		//Generate the Win64 project.
		std::filesystem::create_directories("Win64\\Win64");

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
}

int main(int argc, char *argv[])
{
	//Retrieve the project name.
	const char *const project_name{ argv[1] };

	//Process remaining arguments.
	Distribution distribution{ Distribution::NULL };
	Physics physics{ Physics::NULL };

	for (int i{ 2 }; i < argc; ++i)
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
	GenerateAndroid(project_name, distribution, physics);

	//Generate Win64.
	GenerateWin64(project_name, distribution, physics);

	return 0;
}