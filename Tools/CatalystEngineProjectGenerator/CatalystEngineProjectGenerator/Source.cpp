//Standard library.
#include <filesystem>
#include <fstream>
#include <iostream>

#undef NULL

//Macros.
#define CHECK_ERROR_CODE() if (error_code) { std::cout << "Error at " << __LINE__ << ": " << error_code.message() << std::endl; }

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
*	General parameters class definition.
*/
class GeneralParameters final
{

public:

	//The developer name.
	const char *_DeveloperName{ nullptr };

	//The projet name.
	const char *_ProjectName{ nullptr };

	//The distribution.
	Distribution _Distribution{ Distribution::NULL };

	//The physics.
	Physics _Physics{ Physics::NULL };

};

/*
*	Android parameters class definition.
*/
class AndroidParameters final
{

public:

	//Denotes whether or not to force landscape mode.
	bool _ForceLandscapeMode{ false };

};

/*
*	Oculus quest parameters class definition.
*/
class OculusQuestParameters final
{

public:

};

/*
*	Win64 parameters class definition.
*/
class Win64Parameters final
{

public:


};

/*
*	Prints the options.
*/
void PrintOptions()
{
	std::cout << "Catalyst Engine Project Generator" << std::endl;
	std::cout << std::endl;
	std::cout << "\tOptions:" << std::endl;
	std::cout << "\tFirst argument to this program is always the developer name." << std::endl;
	std::cout << "\tSecond argument to this program is always the project name." << std::endl;
	std::cout << std::endl;
	std::cout << "\tOptional options:" << std::endl;
	std::cout << "\t\tDISTRIBUTION_STEAM: Defines that the project should use STEAM as it's distribution system." << std::endl;
	std::cout << "\t\tPHYSICS_PHYSX: Defines that the project should use PHYSX as it's physics system." << std::endl;
	std::cout << "\t\tANDROID_FORCE_LANDSCAPE_MODE: Defines that the Android build of this project should force landscape mode." << std::endl;
	std::cout << std::endl;
}

/*
*	Generates Android.
*/
void GenerateAndroid(const GeneralParameters &general_parameters, const AndroidParameters &platform_parameters)
{
	//Cache the lower case versions of developer_name and project_name.
	std::string lower_developer_name{ general_parameters._DeveloperName };
	std::transform(lower_developer_name.begin(), lower_developer_name.end(), lower_developer_name.begin(), [](unsigned char character) { return std::tolower(character); });

	std::string lower_project_name{ general_parameters._ProjectName };
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
				cmake_lists_line.replace(project_name_position, strlen("[PROJECT_NAME]"), general_parameters._ProjectName);
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
					sprintf_s(buffer, "%s_Android-lib", general_parameters._ProjectName);

					input_line.replace(library_name_position, strlen("[LIBRARY_NAME]"), buffer);
				}
			}

			{
				const size_t position{ input_line.find("[ANDROID_FORCE_LANDSCAPE_MODE]") };

				if (position != std::string::npos)
				{
					if (platform_parameters._ForceLandscapeMode)
					{
						input_line.replace(position, strlen("[ANDROID_FORCE_LANDSCAPE_MODE]"), "android:screenOrientation=\"landscape\"");
					}

					else
					{
						input_line.replace(position, strlen("[ANDROID_FORCE_LANDSCAPE_MODE]"), "");
					}
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
					input_line.replace(position, strlen("[PROJECT_NAME]"), general_parameters._ProjectName);
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
*	Generates Oculus quest.
*/
void GenerateOculusQuest(const GeneralParameters &general_parameters, const OculusQuestParameters &platform_parameters)
{
	//Cache the lower case versions of developer_name and project_name.
	std::string lower_developer_name{ general_parameters._DeveloperName };
	std::transform(lower_developer_name.begin(), lower_developer_name.end(), lower_developer_name.begin(), [](unsigned char character) { return std::tolower(character); });

	std::string lower_project_name{ general_parameters._ProjectName };
	std::transform(lower_project_name.begin(), lower_project_name.end(), lower_project_name.begin(), [](unsigned char character) { return std::tolower(character); });

	//Remember the error code for filesystem functions.
	std::error_code error_code;

	//Read the template CMakeLists.txt file and output a new file.
	std::ifstream cmake_lists_input_file{ "C:\\Github\\Catalyst-Engine\\Templates\\CMakeLists_Oculus_Quest_Template.txt" };
	std::ofstream cmake_lists_output_file{ "CMakeLists_Oculus_Quest.txt" };

	std::string cmake_lists_line;

	while (std::getline(cmake_lists_input_file, cmake_lists_line))
	{
		{
			const size_t project_name_position{ cmake_lists_line.find("[PROJECT_NAME]") };

			if (project_name_position != std::string::npos)
			{
				cmake_lists_line.replace(project_name_position, strlen("[PROJECT_NAME]"), general_parameters._ProjectName);
			}
		}

		cmake_lists_output_file << cmake_lists_line << std::endl;
	}

	//Close the files.
	cmake_lists_input_file.close();
	cmake_lists_output_file.close();

	//Remove all the contents of the Oculus_Quest folder.
	std::filesystem::remove_all("Oculus_Quest", error_code);

	if (error_code)
	{
		std::cout << error_code.message() << std::endl;
	}

	//Copy the Oculus_Quest project template.
	std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Templates\\Oculus_Quest_Project_Template", "Oculus_Quest", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive, error_code);

	if (error_code)
	{
		std::cout << error_code.message() << std::endl;
	}

	//Copy the CMakeLists.txt file.
	std::filesystem::copy("CMakeLists_Oculus_Quest.txt", "Oculus_Quest\\app\\src\\main\\cpp\\CMakeLists.txt", std::filesystem::copy_options::overwrite_existing, error_code);

	if (error_code)
	{
		std::cout << error_code.message() << std::endl;
	}

	//Modify the AndroidManifest.xml file
	{
		std::ifstream input_file{ "Oculus_Quest\\app\\src\\main\\AndroidManifest.xml" };
		std::ofstream output_file{ "Oculus_Quest\\app\\src\\main\\AndroidManifest_Temporary.xml" };

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
					sprintf_s(buffer, "%s_Oculus_Quest-lib", general_parameters._ProjectName);

					input_line.replace(library_name_position, strlen("[LIBRARY_NAME]"), buffer);
				}
			}

			output_file << input_line << std::endl;
		}

		//Close the files.
		input_file.close();
		output_file.close();

		//Replace the file.
		std::filesystem::copy("Oculus_Quest\\app\\src\\main\\AndroidManifest_Temporary.xml", "Oculus_Quest\\app\\src\\main\\AndroidManifest.xml", std::filesystem::copy_options::overwrite_existing, error_code);

		if (error_code)
		{
			std::cout << error_code.message() << std::endl;
		}

		std::filesystem::remove("Oculus_Quest\\app\\src\\main\\AndroidManifest_Temporary.xml", error_code);

		if (error_code)
		{
			std::cout << error_code.message() << std::endl;
		}
	}

	//Modify the strings.xml file
	{
		std::ifstream input_file{ "Oculus_Quest\\app\\src\\main\\res\\values\\strings.xml" };
		std::ofstream output_file{ "Oculus_Quest\\app\\src\\main\\res\\values\\strings_Temporary.xml" };

		std::string input_line;

		while (std::getline(input_file, input_line))
		{
			{
				const size_t position{ input_line.find("[PROJECT_NAME]") };

				if (position != std::string::npos)
				{
					input_line.replace(position, strlen("[PROJECT_NAME]"), general_parameters._ProjectName);
				}
			}

			output_file << input_line << std::endl;
		}

		//Close the files.
		input_file.close();
		output_file.close();

		//Replace the file.
		std::filesystem::copy("Oculus_Quest\\app\\src\\main\\res\\values\\strings_Temporary.xml", "Oculus_Quest\\app\\src\\main\\res\\values\\strings.xml", std::filesystem::copy_options::overwrite_existing, error_code);

		if (error_code)
		{
			std::cout << error_code.message() << std::endl;
		}

		std::filesystem::remove("Oculus_Quest\\app\\src\\main\\res\\values\\strings_Temporary.xml", error_code);

		if (error_code)
		{
			std::cout << error_code.message() << std::endl;
		}
	}

	//Modify the build.gradle file
	{
		std::ifstream input_file{ "Oculus_Quest\\app\\build.gradle" };
		std::ofstream output_file{ "Oculus_Quest\\app\\build_Temporary.gradle" };

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
		std::filesystem::copy("Oculus_Quest\\app\\build_Temporary.gradle", "Oculus_Quest\\app\\build.gradle", std::filesystem::copy_options::overwrite_existing, error_code);

		if (error_code)
		{
			std::cout << error_code.message() << std::endl;
		}

		std::filesystem::remove("Oculus_Quest\\app\\build_Temporary.gradle", error_code);

		if (error_code)
		{
			std::cout << error_code.message() << std::endl;
		}
	}

	//Copy over the relevant resource collections.
	{
		std::filesystem::create_directory("Oculus_Quest\\app\\src\\main\\assets");

		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Resources\\Final\\CatalystEngineResourceCollection_0.crc", "Oculus_Quest\\app\\src\\main\\assets\\", std::filesystem::copy_options::overwrite_existing, error_code);

		if (error_code)
		{
			std::cout << error_code.message() << std::endl;
		}

		for (const auto &entry : std::filesystem::directory_iterator(std::string("..\\Resources\\Final")))
		{
			const std::string file_path{ entry.path().string() };

			if (file_path != ".gitignore")
			{
				std::filesystem::copy(file_path, "Oculus_Quest\\app\\src\\main\\assets\\", std::filesystem::copy_options::overwrite_existing, error_code);

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
void GenerateWin64(const GeneralParameters &general_parameters, const Win64Parameters &platform_parameters)
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
				cmake_lists_line.replace(project_name_position, strlen("[PROJECT_NAME]"), general_parameters._ProjectName);
			}
		}

		{
			const size_t catalyst_distribution_position{ cmake_lists_line.find("[CATALYST_DISTRIBUTION]") };

			if (catalyst_distribution_position != std::string::npos)
			{
				const char* distribution_string{ "CATALYST_DISTRIBUTION_NULL" };

				switch (general_parameters._Distribution)
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

				switch (general_parameters._Physics)
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

		{
			const size_t position{ cmake_lists_line.find("[LINK_PHYSX_LIBRARIES]") };

			if (position != std::string::npos)
			{
				switch (general_parameters._Physics)
				{
					case Physics::NULL:
					{
						cmake_lists_line.replace(position, strlen("[LINK_PHYSX_LIBRARIES]"), "");

						break;
					}

					case Physics::PHYSX:
					{
						cmake_lists_line = "target_link_libraries(${PROJECT_NAME} $<$<CONFIG:Debug>:Debug/PhysX_64> $<$<CONFIG:DebugEditor>:Debug/PhysX_64> $<$<CONFIG:Profile>:Profile/PhysX_64> $<$<CONFIG:ProfileEditor>:Profile/PhysX_64> $<$<CONFIG:Final>:Final/PhysX_64>)\n";
						cmake_lists_line += "target_link_libraries(${PROJECT_NAME} $<$<CONFIG:Debug>:Debug/PhysXCharacterKinematic_static_64> $<$<CONFIG:DebugEditor>:Debug/PhysXCharacterKinematic_static_64> $<$<CONFIG:Profile>:Profile/PhysXCharacterKinematic_static_64> $<$<CONFIG:ProfileEditor>:Profile/PhysXCharacterKinematic_static_64> $<$<CONFIG:Final>:Final/PhysXCharacterKinematic_static_64>)\n";
						cmake_lists_line += "target_link_libraries(${PROJECT_NAME} $<$<CONFIG:Debug>:Debug/PhysXCommon_64> $<$<CONFIG:DebugEditor>:Debug/PhysXCommon_64> $<$<CONFIG:Profile>:Profile/PhysXCommon_64> $<$<CONFIG:ProfileEditor>:Profile/PhysXCommon_64> $<$<CONFIG:Final>:Final/PhysXCommon_64>)\n";
						cmake_lists_line += "target_link_libraries(${PROJECT_NAME} $<$<CONFIG:Debug>:Debug/PhysXCooking_64> $<$<CONFIG:DebugEditor>:Debug/PhysXCooking_64> $<$<CONFIG:Profile>:Profile/PhysXCooking_64> $<$<CONFIG:ProfileEditor>:Profile/PhysXCooking_64> $<$<CONFIG:Final>:Final/PhysXCooking_64>)\n";
						cmake_lists_line += "target_link_libraries(${PROJECT_NAME} $<$<CONFIG:Debug>:Debug/PhysXExtensions_static_64> $<$<CONFIG:DebugEditor>:Debug/PhysXExtensions_static_64> $<$<CONFIG:Profile>:Profile/PhysXExtensions_static_64> $<$<CONFIG:ProfileEditor>:Profile/PhysXExtensions_static_64> $<$<CONFIG:Final>:Final/PhysXExtensions_static_64>)\n";
						cmake_lists_line += "target_link_libraries(${PROJECT_NAME} $<$<CONFIG:Debug>:Debug/PhysXFoundation_64> $<$<CONFIG:DebugEditor>:Debug/PhysXFoundation_64> $<$<CONFIG:Profile>:Profile/PhysXFoundation_64> $<$<CONFIG:ProfileEditor>:Profile/PhysXFoundation_64> $<$<CONFIG:Final>:Final/PhysXFoundation_64>)\n";
						cmake_lists_line += "target_link_libraries(${PROJECT_NAME} $<$<CONFIG:Debug>:Debug/PhysXPvdSDK_static_64> $<$<CONFIG:DebugEditor>:Debug/PhysXPvdSDK_static_64> $<$<CONFIG:Profile>:Profile/PhysXPvdSDK_static_64> $<$<CONFIG:ProfileEditor>:Profile/PhysXPvdSDK_static_64>)\n";

						break;
					}
				}
			}
		}

		{
			const size_t position{ cmake_lists_line.find("[LINK_STEAM_LIBRARIES]") };

			if (position != std::string::npos)
			{
				switch (general_parameters._Distribution)
				{
					case Distribution::NULL:
					{
						cmake_lists_line.replace(position, strlen("[LINK_STEAM_LIBRARIES]"), "");

						break;
					}

					case Distribution::STEAM:
					{
						cmake_lists_line.replace(position, strlen("[LINK_STEAM_LIBRARIES]"), "target_link_libraries(${PROJECT_NAME} steam_api64)");

						break;
					}
				}
			}
		}

		cmake_lists_output_file << cmake_lists_line << std::endl;
	}

	//Close the files.
	cmake_lists_input_file.close();
	cmake_lists_output_file.close();

	//Remove all the contents of the Win64 folder.
	std::filesystem::remove_all("Win64", error_code); CHECK_ERROR_CODE();

	//Win64 stuff.
	{
		//Generate the Win64 project.
		std::filesystem::create_directories("Win64\\Win64", error_code); CHECK_ERROR_CODE();

		//Run CMake.
		system("cmake -B Win64\\Win64");

		//Copy the relevant .dll files to the correct folders.
		std::filesystem::create_directory("Win64\\Win64\\Debug", error_code); CHECK_ERROR_CODE();
		std::filesystem::create_directory("Win64\\Win64\\DebugEditor", error_code); CHECK_ERROR_CODE();
		std::filesystem::create_directory("Win64\\Win64\\Profile", error_code); CHECK_ERROR_CODE();
		std::filesystem::create_directory("Win64\\Win64\\ProfileEditor", error_code); CHECK_ERROR_CODE();
		std::filesystem::create_directory("Win64\\Win64\\Final", error_code); CHECK_ERROR_CODE();

		//Copy assimp.dll.
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\assimp.dll", "Win64\\Win64\\Debug", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\assimp.dll", "Win64\\Win64\\DebugEditor", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\assimp.dll", "Win64\\Win64\\Profile", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\assimp.dll", "Win64\\Win64\\ProfileEditor", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();

		//Copy steam_api64.dll.
		if (general_parameters._Distribution == Distribution::STEAM)
		{
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\steam_api64.dll", "Win64\\Win64\\Debug", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\steam_api64.dll", "Win64\\Win64\\DebugEditor", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\steam_api64.dll", "Win64\\Win64\\Profile", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\steam_api64.dll", "Win64\\Win64\\ProfileEditor", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\steam_api64.dll", "Win64\\Win64\\Final", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		}

		//Copy PhysX dll's.
		if (general_parameters._Physics == Physics::PHYSX)
		{
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\PhysX_64.dll", "Win64\\Win64\\Debug", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\PhysXCommon_64.dll", "Win64\\Win64\\Debug", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\PhysXCooking_64.dll", "Win64\\Win64\\Debug", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\PhysXFoundation_64.dll", "Win64\\Win64\\Debug", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();

			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\PhysX_64.dll", "Win64\\Win64\\DebugEditor", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\PhysXCommon_64.dll", "Win64\\Win64\\DebugEditor", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\PhysXCooking_64.dll", "Win64\\Win64\\DebugEditor", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\PhysXFoundation_64.dll", "Win64\\Win64\\DebugEditor", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();

			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\PhysX_64.dll", "Win64\\Win64\\Profile", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\PhysXCommon_64.dll", "Win64\\Win64\\Profile", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\PhysXCooking_64.dll", "Win64\\Win64\\Profile", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\PhysXFoundation_64.dll", "Win64\\Win64\\Profile", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();

			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\PhysX_64.dll", "Win64\\Win64\\ProfileEditor", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\PhysXCommon_64.dll", "Win64\\Win64\\ProfileEditor", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\PhysXCooking_64.dll", "Win64\\Win64\\ProfileEditor", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\PhysXFoundation_64.dll", "Win64\\Win64\\ProfileEditor", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();

			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Final\\PhysX_64.dll", "Win64\\Win64\\Final", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Final\\PhysXCommon_64.dll", "Win64\\Win64\\Final", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Final\\PhysXCooking_64.dll", "Win64\\Win64\\Final", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Final\\PhysXFoundation_64.dll", "Win64\\Win64\\Final", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		}
	}

	//Copy the CMakeLists.txt file to CMakeLists_Windows, and remove CMakeLists.txt. Nice to save the CMakeLists.txt file, to look at it later. (:
	std::filesystem::copy("CMakeLists.txt", "CMakeLists_Windows.txt", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
	std::filesystem::remove("CMakeLists.txt", error_code); CHECK_ERROR_CODE();
}

int main(int argc, char *argv[])
{
	//Print the options.
	PrintOptions();

	//Construct the parameters.
	GeneralParameters general_parameters;
	AndroidParameters android_parameters;
	OculusQuestParameters oculus_quest_parameters;
	Win64Parameters win64_parameters;

	//Retrieve the developer name.
	general_parameters._DeveloperName = argv[1];

	//Retrieve the project name.
	general_parameters._ProjectName = argv[2];

	//Process remaining arguments.
	for (int i{ 3 }; i < argc; ++i)
	{
		if (strcmp(argv[i], "DISTRIBUTION_STEAM") == 0)
		{
			general_parameters._Distribution = Distribution::STEAM;
		}

		if (strcmp(argv[i], "PHYSICS_PHYSX") == 0)
		{
			general_parameters._Physics = Physics::PHYSX;
		}

		if (strcmp(argv[i], "ANDROID_FORCE_LANDSCAPE_MODE") == 0)
		{
			android_parameters._ForceLandscapeMode = true;
		}
	}

	//Generate Android.
	GenerateAndroid(general_parameters, android_parameters);

	//Generate Oculus Quest.
	GenerateOculusQuest(general_parameters, oculus_quest_parameters);

	//Generate Win64.
	GenerateWin64(general_parameters, win64_parameters);

	return 0;
}