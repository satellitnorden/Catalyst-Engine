//Standard library.
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#undef NULL

//Macros.
#define CHECK_ERROR_CODE() if (error_code) { std::cout << "Error at " << __LINE__ << ": " << error_code.message() << std::endl; }

//Enumeration covering all projet types.
enum class ProjectType
{
	ANDROID,
	OCULUS_QUEST,
	WIN64,

	UNKNOWN
};

//Enumeration covering all Win64 distributions.
enum class Win64Distribution
{
	NULL,
	STEAM
};

//Enumeration covering all physics.
enum class Physics
{
	NULL,
	PHYSX,
	JOLT
};

/*
*	General parameters class definition.
*/
class GeneralParameters final
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

	//The physics.
	Physics _Physics{ Physics::NULL };

	//Denotes whether or not to use the ONNX Runtime library.
	bool _UseONNXRuntimeLibrary{ false };

	//Denotes whether or not to include environment resource collection.
	bool _IncludeEnvironmentResourceCollection{ false };

	//Denotes whether or not to include extra resource collection.
	bool _IncludeExtraResourceCollection{ false };

	//Denotes whether or not to enable code generation.
	bool _EnableCodeGeneration{ false };

	//The script generator game include.
	std::string _ScriptGeneratorGameInclude;

};

/*
*	Android parameters class definition.
*/
class AndroidParameters final
{

public:

	//Denotes whether or not to force landscape mode.
	bool _ForceLandscapeMode{ false };

	//Denotes whether or not to force portrait mode.
	bool _ForcePortraitMode{ false };

	//The included resource collections.
	std::vector<std::string> _IncludedResourceCollections;

	//The keystore file path.
	std::string _KeystoreFilePath;

	//The keystore password.
	std::string _KeystorePassword;

	//The keystore alias.
	std::string _KeystoreAlias;

	//Denotes whether or not to use asset pack.
	bool _UseAssetPack{ false };

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


	//The distribution.
	Win64Distribution _Distribution{ Win64Distribution::NULL };

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
	std::cout << "\t\tINCLUDE_ENVIRONMENT_RESOURCE_COLLECTION: Defines that the project should include the environment resource collection." << std::endl;
	std::cout << "\t\tINCLUDE_EXTRA_RESOURCE_COLLECTION: Defines that the project should include the extra resource collection." << std::endl;
	std::cout << "\t\tDISTRIBUTION_STEAM: Defines that the project should use STEAM as it's distribution system." << std::endl;
	std::cout << "\t\tPHYSICS_PHYSX: Defines that the project should use PHYSX as it's physics system." << std::endl;
	std::cout << "\t\tANDROID_FORCE_LANDSCAPE_MODE: Defines that the Android build of this project should force landscape mode." << std::endl;
	std::cout << std::endl;
}

/*
*	Generates Android.
*/
void GenerateAndroid(const GeneralParameters& general_parameters, const AndroidParameters& platform_parameters)
{
	//Cache the lower case versions of developer_name and project_name.
	std::string lower_developer_name{ general_parameters._DeveloperNameNoSpaces };
	std::transform(lower_developer_name.begin(), lower_developer_name.end(), lower_developer_name.begin(), [](unsigned char character) { return std::tolower(character); });

	std::string lower_project_name{ general_parameters._ProjectNameNoSpaces };
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
				cmake_lists_line.replace(project_name_position, strlen("[PROJECT_NAME]"), general_parameters._ProjectNameNoSpaces);
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
					sprintf_s(buffer, "%s_Android-lib", general_parameters._ProjectNameNoSpaces.c_str());

					input_line.replace(library_name_position, strlen("[LIBRARY_NAME]"), buffer);
				}
			}

			{
				const size_t position{ input_line.find("[ANDROID_FORCE_ORIENTATION_MODE]") };

				if (position != std::string::npos)
				{
					if (platform_parameters._ForceLandscapeMode)
					{
						input_line.replace(position, strlen("[ANDROID_FORCE_ORIENTATION_MODE]"), "android:screenOrientation=\"landscape\"");
					}

					if (platform_parameters._ForcePortraitMode)
					{
						input_line.replace(position, strlen("[ANDROID_FORCE_ORIENTATION_MODE]"), "android:screenOrientation=\"portrait\"");
					}

					else
					{
						input_line.replace(position, strlen("[ANDROID_FORCE_ORIENTATION_MODE]"), "");
					}
				}
			}

			output_file << input_line << std::endl;
		}

		//Add the CATALYST_INCLUDE_ENVIROMENT_RESOURCE_COLLECTION define.
		if (general_parameters._IncludeEnvironmentResourceCollection)
		{
			output_file << std::endl;
			output_file << "#Include the enviroment resource collection." << std::endl;
			output_file << "add_compile_definitions(CATALYST_INCLUDE_ENVIROMENT_RESOURCE_COLLECTION)" << std::endl;
		}

		//Add the CATALYST_INCLUDE_EXTRA_RESOURCE_COLLECTION define.
		if (general_parameters._IncludeExtraResourceCollection)
		{
			output_file << std::endl;
			output_file << "#Include the extra resource collection." << std::endl;
			output_file << "add_compile_definitions(CATALYST_INCLUDE_EXTRA_RESOURCE_COLLECTION)" << std::endl;
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
			bool include_line{ true };

			{
				const size_t position{ input_line.find("[SIGNING_CONFIGS]") };

				if (position != std::string::npos)
				{
					if (!platform_parameters._KeystoreFilePath.empty()
						&& !platform_parameters._KeystorePassword.empty()
						&& !platform_parameters._KeystoreAlias.empty())
					{
						input_line.replace(position, strlen("[SIGNING_CONFIGS]"), "signingConfigs {\n");
						input_line += "\t\trelease {\n";

						input_line += "\t\t\tstoreFile file('";
						input_line += platform_parameters._KeystoreFilePath;
						input_line += "')\n";

						input_line += "\t\t\tstorePassword '";
						input_line += platform_parameters._KeystorePassword;
						input_line += "'\n";

						input_line += "\t\t\tkeyPassword '";
						input_line += platform_parameters._KeystorePassword;
						input_line += "'\n";

						input_line += "\t\t\tkeyAlias '";
						input_line += platform_parameters._KeystoreAlias;
						input_line += "'\n";

						input_line += "\t\t}\n";
						input_line += "\t}\n";
					}

					else
					{
						include_line = false;
					}
				}
			}

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

			{
				const size_t position{ input_line.find("[DEFAULT_SIGNING_CONFIG]") };

				if (position != std::string::npos)
				{
					if (!platform_parameters._KeystoreFilePath.empty()
						&& !platform_parameters._KeystorePassword.empty()
						&& !platform_parameters._KeystoreAlias.empty())
					{
						input_line.replace(position, strlen("[DEFAULT_SIGNING_CONFIG]"), "signingConfig signingConfigs.release");
					}

					else
					{
						include_line = false;
					}
				}
			}

			{
				const size_t position{ input_line.find("[ASSET_PACKS]") };

				if (position != std::string::npos)
				{
					if (platform_parameters._UseAssetPack)
					{
						input_line.replace(position, strlen("[ASSET_PACKS]"), "assetPacks = [\":asset_pack\"]");
					}

					else
					{
						include_line = false;
					}
				}
			}

			if (include_line)
			{
				output_file << input_line << std::endl;
			}
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

	//Add the settings.gradle file.
	{
		std::ofstream file{ "Android\\settings.gradle" };

		file << "include 'app'";

		if (platform_parameters._UseAssetPack)
		{
			file << std::endl;
			file << "include ':asset_pack'";
		}

		file.close();
	}

	//Copy over the relevant resource collections.
	if (platform_parameters._UseAssetPack)
	{
		//Create the directory.
		std::filesystem::create_directory("Android\\asset_pack");

		//Create the build.gradle file.
		{
			std::ofstream file{ "Android\\asset_pack\\build.gradle" };

			file << "plugins {" << std::endl;
			file << "\tid 'com.android.asset-pack'" << std::endl;
			file << "}" << std::endl;

			file << std::endl;

			file << "assetPack {" << std::endl;
			file << "\tpackName = \"asset_pack\"" << std::endl;
			file << "\tdynamicDelivery {" << std::endl;
			file << "\t\tdeliveryType = \"install-time\"" << std::endl;
			file << "\t}" << std::endl;
			file << "}";

			file.close();
		}

		//Create the asset_pack/src/main/assets directory.
		std::filesystem::create_directories("Android\\asset_pack\\src\\main\\assets");

		//Copy over the resource collections.
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Resources\\Final\\CatalystEngineBaseResourceCollection_0.crc", "Android\\asset_pack\\src\\main\\assets\\", std::filesystem::copy_options::overwrite_existing, error_code);

		if (general_parameters._IncludeEnvironmentResourceCollection)
		{
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Resources\\Final\\CatalystEngineEnvironmentResourceCollection_0.crc", "Android\\asset_pack\\src\\main\\assets\\", std::filesystem::copy_options::overwrite_existing, error_code);
		}

		if (general_parameters._IncludeExtraResourceCollection)
		{
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Resources\\Final\\CatalystEngineExtraResourceCollection_0.crc", "Android\\asset_pack\\src\\main\\assets\\", std::filesystem::copy_options::overwrite_existing, error_code);
		}

		if (error_code)
		{
			std::cout << error_code.message() << std::endl;
		}

		for (const std::string& included_resource_collection : platform_parameters._IncludedResourceCollections)
		{
			char buffer[256];
			sprintf_s(buffer, "..\\Resources\\Final\\%s", included_resource_collection.c_str());

			std::filesystem::copy(buffer, "Android\\asset_pack\\src\\main\\assets\\", std::filesystem::copy_options::overwrite_existing, error_code);

			if (error_code)
			{
				std::cout << error_code.message() << std::endl;
			}
		}
	}

	else
	{
		std::filesystem::create_directory("Android\\app\\src\\main\\assets");

		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Resources\\Final\\CatalystEngineBaseResourceCollection_0.crc", "Android\\app\\src\\main\\assets\\", std::filesystem::copy_options::overwrite_existing, error_code);

		if (general_parameters._IncludeEnvironmentResourceCollection)
		{
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Resources\\Final\\CatalystEngineEnvironmentResourceCollection_0.crc", "Android\\app\\src\\main\\assets\\", std::filesystem::copy_options::overwrite_existing, error_code);
		}

		if (general_parameters._IncludeExtraResourceCollection)
		{
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Resources\\Final\\CatalystEngineExtraResourceCollection_0.crc", "Android\\app\\src\\main\\assets\\", std::filesystem::copy_options::overwrite_existing, error_code);
		}

		if (error_code)
		{
			std::cout << error_code.message() << std::endl;
		}

		for (const std::string& included_resource_collection : platform_parameters._IncludedResourceCollections)
		{
			char buffer[256];
			sprintf_s(buffer, "..\\Resources\\Final\\%s", included_resource_collection.c_str());

			std::filesystem::copy(buffer, "Android\\app\\src\\main\\assets\\", std::filesystem::copy_options::overwrite_existing, error_code);

			if (error_code)
			{
				std::cout << error_code.message() << std::endl;
			}
		}
	}
}

/*
*	Generates Oculus quest.
*/
void GenerateOculusQuest(const GeneralParameters& general_parameters, const OculusQuestParameters& platform_parameters)
{
	//Cache the lower case versions of developer_name and project_name.
	std::string lower_developer_name{ general_parameters._DeveloperNameNoSpaces };
	std::transform(lower_developer_name.begin(), lower_developer_name.end(), lower_developer_name.begin(), [](unsigned char character) { return std::tolower(character); });

	std::string lower_project_name{ general_parameters._ProjectNameNoSpaces };
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
				cmake_lists_line.replace(project_name_position, strlen("[PROJECT_NAME]"), general_parameters._ProjectNameNoSpaces);
			}
		}

		cmake_lists_output_file << cmake_lists_line << std::endl;
	}

	//Add the CATALYST_INCLUDE_ENVIROMENT_RESOURCE_COLLECTION define.
	if (general_parameters._IncludeEnvironmentResourceCollection)
	{
		cmake_lists_output_file << std::endl;
		cmake_lists_output_file << "#Include the enviroment resource collection." << std::endl;
		cmake_lists_output_file << "add_compile_definitions(CATALYST_INCLUDE_ENVIRONMENT_RESOURCE_COLLECTION)" << std::endl;
	}

	//Add the CATALYST_INCLUDE_EXTRA_RESOURCE_COLLECTION define.
	if (general_parameters._IncludeExtraResourceCollection)
	{
		cmake_lists_output_file << std::endl;
		cmake_lists_output_file << "#Include the extra resource collection." << std::endl;
		cmake_lists_output_file << "add_compile_definitions(CATALYST_INCLUDE_EXTRA_RESOURCE_COLLECTION)" << std::endl;
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
					sprintf_s(buffer, "%s_Oculus_Quest-lib", general_parameters._ProjectNameNoSpaces.c_str());

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

		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Resources\\Final\\CatalystEngineBaseResourceCollection_0.crc", "Oculus_Quest\\app\\src\\main\\assets\\", std::filesystem::copy_options::overwrite_existing, error_code);

		if (general_parameters._IncludeEnvironmentResourceCollection)
		{
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Resources\\Final\\CatalystEngineEnvironmentResourceCollection_0.crc", "Android\\app\\src\\main\\assets\\", std::filesystem::copy_options::overwrite_existing, error_code);
		}

		if (general_parameters._IncludeExtraResourceCollection)
		{
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Resources\\Final\\CatalystEngineExtraResourceCollection_0.crc", "Android\\app\\src\\main\\assets\\", std::filesystem::copy_options::overwrite_existing, error_code);
		}

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

	//Remove everything (but the .gitignore file) in the code generation folder.
	if (std::filesystem::exists("..\\Code\\CodeGeneration"))
	{
		for (const auto &entry : std::filesystem::directory_iterator(std::string("..\\Code\\CodeGeneration")))
		{
			const std::string file_path{ entry.path().string() };

			if (file_path.find(".gitignore") != std::string::npos)
			{
				std::filesystem::remove(file_path, error_code); CHECK_ERROR_CODE();
			}
		}
	}

	//Open the "CMakeLists.txt" file.
	std::ofstream cmake_lists_file{ "CMakeLists.txt" };

	cmake_lists_file << "#Set the CMake minimum required version." << std::endl;
	cmake_lists_file << "cmake_minimum_required(VERSION 3.10)" << std::endl;
	cmake_lists_file << std::endl;

	cmake_lists_file << "#Set the project name." << std::endl;
	cmake_lists_file << "set(PROJECT_NAME " << general_parameters._ProjectNameNoSpaces.c_str() << "_Win64 C CXX ASM_MASM)" << std::endl;
	cmake_lists_file << std::endl;

	cmake_lists_file << "#Set the project." << std::endl;
	cmake_lists_file << "project(${PROJECT_NAME})" << std::endl;
	cmake_lists_file << std::endl;

	cmake_lists_file << "#Set the C++ standard." << std::endl;
	cmake_lists_file << "set(CMAKE_CXX_STANDARD 20)" << std::endl;
	cmake_lists_file << "set(CMAKE_CXX_STANDARD_REQUIRED True)" << std::endl;
	cmake_lists_file << std::endl;

	cmake_lists_file << "#Set the configuration types." << std::endl;
	cmake_lists_file << "set(CMAKE_CONFIGURATION_TYPES \"Debug;DebugEditor;Profile;ProfileEditor;Final\")" << std::endl;
	cmake_lists_file << "set(CMAKE_CONFIGURATION_TYPES \"${CMAKE_CONFIGURATION_TYPES}\" CACHE STRING \"Reset config to what we need\" FORCE)" << std::endl;
	cmake_lists_file << std::endl;

	cmake_lists_file << "set(CMAKE_EXE_LINKER_FLAGS_DEBUG \"/DEBUG /LTCG:incremental\")" << std::endl;
	cmake_lists_file << "set(CMAKE_EXE_LINKER_FLAGS_DEBUGEDITOR \"/DEBUG /LTCG:incremental\")" << std::endl;
	cmake_lists_file << "set(CMAKE_EXE_LINKER_FLAGS_PROFILE \"/DEBUG /LTCG:incremental\")" << std::endl;
	cmake_lists_file << "set(CMAKE_EXE_LINKER_FLAGS_PROFILEEDITOR \"/DEBUG /LTCG:incremental\")" << std::endl;
	cmake_lists_file << "set(CMAKE_EXE_LINKER_FLAGS_FINAL \"/LTCG:incremental\")" << std::endl;
	cmake_lists_file << std::endl;

	cmake_lists_file << "#Add defines." << std::endl;
	cmake_lists_file << "add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t$<$<CONFIG:DebugEditor>:_DEBUG>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t$<$<CONFIG:Profile>:NDEBUG>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t$<$<CONFIG:ProfileEditor>:NDEBUG>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t$<$<CONFIG:Final>:NDEBUG>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t)" << std::endl;
	cmake_lists_file << "add_compile_definitions(__WINDOWS_WASAPI__)" << std::endl;
	cmake_lists_file << "add_compile_definitions(__WINDOWS_ASIO__)" << std::endl;
	cmake_lists_file << "add_compile_definitions(__WINDOWS_MM__)" << std::endl;
	cmake_lists_file << "add_compile_definitions($<$<CONFIG:Debug>:CATALYST_CONFIGURATION_DEBUG>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t$<$<CONFIG:DebugEditor>:CATALYST_CONFIGURATION_DEBUG>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t$<$<CONFIG:Profile>:CATALYST_CONFIGURATION_PROFILE>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t$<$<CONFIG:ProfileEditor>:CATALYST_CONFIGURATION_PROFILE>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t$<$<CONFIG:Final>:CATALYST_CONFIGURATION_FINAL>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t)" << std::endl;

	{
		const char *distribution_string{ "CATALYST_DISTRIBUTION_NULL" };

		switch (platform_parameters._Distribution)
		{
			case Win64Distribution::STEAM:
			{
				distribution_string = "CATALYST_DISTRIBUTION_STEAM";

				break;
			}
		}

		cmake_lists_file << "add_compile_definitions(" << distribution_string << ")" << std::endl;
	}

	cmake_lists_file << "add_compile_definitions($<$<CONFIG:Debug>:CATALYST_ENABLE_RESOURCE_BUILDING>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t$<$<CONFIG:DebugEditor>:CATALYST_ENABLE_RESOURCE_BUILDING>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t$<$<CONFIG:Profile>:CATALYST_ENABLE_RESOURCE_BUILDING>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t$<$<CONFIG:ProfileEditor>:CATALYST_ENABLE_RESOURCE_BUILDING>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t)" << std::endl;
	cmake_lists_file << "add_compile_definitions($<$<CONFIG:DebugEditor>:CATALYST_EDITOR>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t$<$<CONFIG:ProfileEditor>:CATALYST_EDITOR>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t)" << std::endl;

	{
		const char *physics_string{ "CATALYST_PHYSICS_NULL" };

		switch (general_parameters._Physics)
		{
			case Physics::PHYSX:
			{
				physics_string = "CATALYST_PHYSICS_PHYSX";

				break;
			}

			case Physics::JOLT:
			{
				physics_string = "CATALYST_PHYSICS_JOLT";

				break;
			}
		}

		cmake_lists_file << "add_compile_definitions(" << physics_string << ")" << std::endl;
	}

	cmake_lists_file << "add_compile_definitions(CATALYST_PLATFORM_WINDOWS)" << std::endl;
	cmake_lists_file << "add_compile_definitions(CATALYST_RENDERING_VULKAN)" << std::endl;
	cmake_lists_file << "add_compile_definitions($<$<CONFIG:Debug>:TRACY_ENABLE>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t$<$<CONFIG:DebugEditor>:TRACY_ENABLE>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t$<$<CONFIG:Profile>:TRACY_ENABLE>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t$<$<CONFIG:ProfileEditor>:TRACY_ENABLE>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t)" << std::endl;
	cmake_lists_file << "add_compile_definitions($<$<CONFIG:Debug>:TRACY_ON_DEMAND>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t$<$<CONFIG:DebugEditor>:TRACY_ON_DEMAND>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t$<$<CONFIG:Profile>:TRACY_ON_DEMAND>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t$<$<CONFIG:ProfileEditor>:TRACY_ON_DEMAND>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t)" << std::endl;

	if (general_parameters._UseONNXRuntimeLibrary)
	{
		cmake_lists_file << "add_compile_definitions(CATALYST_HAS_ONNX>" << std::endl;
	}

	cmake_lists_file << std::endl;

	cmake_lists_file << "#Add the include/source files." << std::endl;
	cmake_lists_file << "file(GLOB_RECURSE ENGINE_INCLUDE_FILES ../../Catalyst-Engine/Engine/Include/*.h ../../Catalyst-Engine/Engine/Include/*.hpp)" << std::endl;
	cmake_lists_file << "file(GLOB_RECURSE ENGINE_INLINE_FILES ../../Catalyst-Engine/Engine/Include/*.inl)" << std::endl;
	cmake_lists_file << "file(GLOB_RECURSE ENGINE_SOURCE_FILES ../../Catalyst-Engine/Engine/Source/*.c ../../Catalyst-Engine/Engine/Source/*.cpp  ../../Catalyst-Engine/Engine/Source/*.asm)" << std::endl;
	cmake_lists_file << "file(GLOB_RECURSE ENGINE_RENDERING_FILES ../../Catalyst-Engine/Engine/Rendering/*.common_shader ../../Catalyst-Engine/Engine/Rendering/*.global_render_data ../../Catalyst-Engine/Engine/Rendering/*.render_pipeline ../../Catalyst-Engine/Engine/Rendering/*.shader_function_library ../../Catalyst-Engine/Engine/Rendering/*.storage_buffer_definition ../../Catalyst-Engine/Engine/Rendering/*.uniform_buffer_definition)" << std::endl;
	cmake_lists_file << "file(GLOB_RECURSE GAME_INCLUDE_FILES ../Code/Include/*.h ../Code/Include/*.hpp)" << std::endl;
	cmake_lists_file << "file(GLOB_RECURSE GAME_RENDERING_FILES ../Rendering/*.common_shader ../Rendering/*.global_render_data ../Rendering/*.render_pipeline ../Rendering/*.shader_function_library ../Rendering/*.storage_buffer_definition ../Rendering/*.uniform_buffer_definition)" << std::endl;
	cmake_lists_file << "file(GLOB_RECURSE GAME_SCRIPT_FILES ../Content/Assets/*.Script)" << std::endl;
	cmake_lists_file << "file(GLOB_RECURSE GAME_SOURCE_FILES ../Code/Source/*.c ../Code/Source/*.cpp  ../Code/Source/*.asm)" << std::endl;

	if (general_parameters._Physics == Physics::JOLT)
	{
		cmake_lists_file << "file(GLOB_RECURSE THIRD_PARTY_JOLT_INCLUDE_FILES \"../../Catalyst-Engine/Engine/Third Party/Jolt/*.h\")" << std::endl;
		cmake_lists_file << "file(GLOB_RECURSE THIRD_PARTY_JOLT_SOURCE_FILES \"../../Catalyst-Engine/Engine/Third Party/Jolt/*.cpp\")" << std::endl;
	}

	cmake_lists_file << std::endl;

	cmake_lists_file << "#Add the third party include files." << std::endl;
	cmake_lists_file << "file(GLOB_RECURSE ASSIMP_INCLUDE_FILES ../../Catalyst-Engine/Engine/Include/ThirdParty/assimp/*.h ../../Catalyst-Engine/Engine/Include/ThirdParty/assimp/*.hpp)" << std::endl;
	cmake_lists_file << "file(GLOB_RECURSE BC7ENC_INCLUDE_FILES ../../Catalyst-Engine/Engine/Include/ThirdParty/bc7enc/*.h)" << std::endl;
	cmake_lists_file << "file(GLOB_RECURSE IMGUI_INCLUDE_FILES ../../Catalyst-Engine/Engine/Include/ThirdParty/ImGui/*.h)" << std::endl;
	cmake_lists_file << "file(GLOB_RECURSE IMGUINODEEDITOR_INCLUDE_FILES ../../Catalyst-Engine/Engine/Include/ThirdParty/imguinodeeditor/*.h ../../Catalyst-Engine/Engine/Include/ThirdParty/imguinodeeditor/*.inl)" << std::endl;
	cmake_lists_file << std::endl;

	cmake_lists_file << "#Add the executable." << std::endl;
	cmake_lists_file << "add_executable(${PROJECT_NAME} WIN32 ${ENGINE_INCLUDE_FILES} ${ENGINE_INLINE_FILES} ${ENGINE_SOURCE_FILES} ${ENGINE_RENDERING_FILES} ${GAME_INCLUDE_FILES} ${GAME_RENDERING_FILES} ${GAME_SCRIPT_FILES} ${GAME_SOURCE_FILES} ${ASSIMP_INCLUDE_FILES} ${BC7ENC_INCLUDE_FILES} ${IMGUI_INCLUDE_FILES} ${IMGUINODEEDITOR_INCLUDE_FILES}";
	
	if (general_parameters._Physics == Physics::JOLT)
	{
		cmake_lists_file << " ${THIRD_PARTY_JOLT_INCLUDE_FILES} ${THIRD_PARTY_JOLT_SOURCE_FILES}";
	}
	
	cmake_lists_file << ")" << std::endl;

	cmake_lists_file << std::endl;

	cmake_lists_file << "#Set the core include paths." << std::endl;
	cmake_lists_file << "target_include_directories(${PROJECT_NAME} PUBLIC ../../Catalyst-Engine/Engine/Include)" << std::endl;
	cmake_lists_file << "target_include_directories(${PROJECT_NAME} PUBLIC ../Code/Include)" << std::endl;
	cmake_lists_file << std::endl;

	cmake_lists_file << "#Set the third party include paths." << std::endl;
	cmake_lists_file << "target_include_directories(${PROJECT_NAME} PUBLIC ../../Catalyst-Engine/Engine/Include/ThirdParty/assimp)" << std::endl;
	cmake_lists_file << "target_include_directories(${PROJECT_NAME} PUBLIC ../../Catalyst-Engine/Engine/Include/ThirdParty/bc7enc)" << std::endl;
	cmake_lists_file << "target_include_directories(${PROJECT_NAME} PUBLIC ../../Catalyst-Engine/Engine/Include/ThirdParty/ImGui)" << std::endl;
	cmake_lists_file << "target_include_directories(${PROJECT_NAME} PUBLIC ../../Catalyst-Engine/Engine/Include/ThirdParty/imguinodeeditor)" << std::endl;

	if (general_parameters._Physics == Physics::JOLT)
	{
		cmake_lists_file << "target_include_directories(${PROJECT_NAME} PUBLIC \"../../Catalyst-Engine/Engine/Third Party/Jolt\")" << std::endl;
	}

	cmake_lists_file << std::endl;

	cmake_lists_file << "#Set the library paths." << std::endl;
	cmake_lists_file << "target_link_directories(${PROJECT_NAME} PUBLIC ../../Catalyst-Engine/Engine/Libraries/Static)" << std::endl;
	cmake_lists_file << std::endl;

	cmake_lists_file << "#Link the libraries." << std::endl;

	if (general_parameters._Physics == Physics::PHYSX)
	{
		cmake_lists_file << "target_link_libraries(${PROJECT_NAME} $<$<CONFIG:Debug>:Debug/PhysX_64> $<$<CONFIG:DebugEditor>:Debug/PhysX_64> $<$<CONFIG:Profile>:Profile/PhysX_64> $<$<CONFIG:ProfileEditor>:Profile/PhysX_64> $<$<CONFIG:Final>:Final/PhysX_64>)" << std::endl;
		cmake_lists_file << "target_link_libraries(${PROJECT_NAME} $<$<CONFIG:Debug>:Debug/PhysXCharacterKinematic_static_64> $<$<CONFIG:DebugEditor>:Debug/PhysXCharacterKinematic_static_64> $<$<CONFIG:Profile>:Profile/PhysXCharacterKinematic_static_64> $<$<CONFIG:ProfileEditor>:Profile/PhysXCharacterKinematic_static_64> $<$<CONFIG:Final>:Final/PhysXCharacterKinematic_static_64>)" << std::endl;
		cmake_lists_file << "target_link_libraries(${PROJECT_NAME} $<$<CONFIG:Debug>:Debug/PhysXCommon_64> $<$<CONFIG:DebugEditor>:Debug/PhysXCommon_64> $<$<CONFIG:Profile>:Profile/PhysXCommon_64> $<$<CONFIG:ProfileEditor>:Profile/PhysXCommon_64> $<$<CONFIG:Final>:Final/PhysXCommon_64>)" << std::endl;
		cmake_lists_file << "target_link_libraries(${PROJECT_NAME} $<$<CONFIG:Debug>:Debug/PhysXCooking_64> $<$<CONFIG:DebugEditor>:Debug/PhysXCooking_64> $<$<CONFIG:Profile>:Profile/PhysXCooking_64> $<$<CONFIG:ProfileEditor>:Profile/PhysXCooking_64> $<$<CONFIG:Final>:Final/PhysXCooking_64>)" << std::endl;
		cmake_lists_file << "target_link_libraries(${PROJECT_NAME} $<$<CONFIG:Debug>:Debug/PhysXExtensions_static_64> $<$<CONFIG:DebugEditor>:Debug/PhysXExtensions_static_64> $<$<CONFIG:Profile>:Profile/PhysXExtensions_static_64> $<$<CONFIG:ProfileEditor>:Profile/PhysXExtensions_static_64> $<$<CONFIG:Final>:Final/PhysXExtensions_static_64>)" << std::endl;
		cmake_lists_file << "target_link_libraries(${PROJECT_NAME} $<$<CONFIG:Debug>:Debug/PhysXFoundation_64> $<$<CONFIG:DebugEditor>:Debug/PhysXFoundation_64> $<$<CONFIG:Profile>:Profile/PhysXFoundation_64> $<$<CONFIG:ProfileEditor>:Profile/PhysXFoundation_64> $<$<CONFIG:Final>:Final/PhysXFoundation_64>)" << std::endl;
		cmake_lists_file << "target_link_libraries(${PROJECT_NAME} $<$<CONFIG:Debug>:Debug/PhysXPvdSDK_static_64> $<$<CONFIG:DebugEditor>:Debug/PhysXPvdSDK_static_64> $<$<CONFIG:Profile>:Profile/PhysXPvdSDK_static_64> $<$<CONFIG:ProfileEditor>:Profile/PhysXPvdSDK_static_64>)" << std::endl;
	}

	if (platform_parameters._Distribution == Win64Distribution::STEAM)
	{
		cmake_lists_file << "target_link_libraries(${PROJECT_NAME} steam_api64)" << std::endl;
	}

	cmake_lists_file << "target_link_libraries(${PROJECT_NAME} vulkan-1)" << std::endl;
	cmake_lists_file << "target_link_libraries(${PROJECT_NAME}\t$<$<CONFIG:Debug>:Debug/shaderc_shared>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t\t\t\t\t$<$<CONFIG:DebugEditor>:Debug/shaderc_shared>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t\t\t\t\t$<$<CONFIG:Profile>:Profile/shaderc_shared>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t\t\t\t\t$<$<CONFIG:ProfileEditor>:Profile/shaderc_shared>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t\t\t\t\t)" << std::endl;
	cmake_lists_file << "target_link_libraries(${PROJECT_NAME} Winmm)" << std::endl;
	cmake_lists_file << "target_link_libraries(${PROJECT_NAME} xinput)" << std::endl;
	cmake_lists_file << "target_link_libraries(${PROJECT_NAME}\t$<$<CONFIG:Debug>:Debug/libsoundwave>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t\t\t\t\t$<$<CONFIG:DebugEditor>:Debug/libsoundwave>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t\t\t\t\t$<$<CONFIG:Profile>:Profile/libsoundwave>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t\t\t\t\t$<$<CONFIG:ProfileEditor>:Profile/libsoundwave>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t\t\t\t\t$<$<CONFIG:Final>:Final/libsoundwave>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t\t\t\t\t)" << std::endl;
	cmake_lists_file << "target_link_libraries(${PROJECT_NAME}\t$<$<CONFIG:Debug>:Debug/assimp-vc143-mtd>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t\t\t\t\t$<$<CONFIG:DebugEditor>:Debug/assimp-vc143-mtd>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t\t\t\t\t$<$<CONFIG:Profile>:Profile/assimp-vc143-mt>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t\t\t\t\t$<$<CONFIG:ProfileEditor>:Profile/assimp-vc143-mt>" << std::endl;
	cmake_lists_file << "\t\t\t\t\t\t\t\t\t\t)" << std::endl;

	if (general_parameters._UseONNXRuntimeLibrary)
	{
		cmake_lists_file << "target_link_libraries(${PROJECT_NAME} onnxruntime)" << std::endl;
	}

	cmake_lists_file << std::endl;

	cmake_lists_file << "#Set the startup project." << std::endl;
	cmake_lists_file << "set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})" << std::endl;
	cmake_lists_file << std::endl;

	cmake_lists_file << "#Add the Visual Studio filters." << std::endl;
	cmake_lists_file << "set(ENGINE_INCLUDE_FILES_ROOT \"${CMAKE_SOURCE_DIR}/../../Catalyst-Engine/Engine/Include\")" << std::endl;
	cmake_lists_file << "source_group(TREE ${ENGINE_INCLUDE_FILES_ROOT} PREFIX \"Catalyst Engine Include\" FILES ${ENGINE_INCLUDE_FILES} ${ENGINE_INLINE_FILES})" << std::endl;
	cmake_lists_file << std::endl;
	cmake_lists_file << "set(ENGINE_SOURCE_FILES_ROOT \"${CMAKE_SOURCE_DIR}/../../Catalyst-Engine/Engine/Source\")" << std::endl;
	cmake_lists_file << "source_group(TREE ${ENGINE_SOURCE_FILES_ROOT} PREFIX \"Catalyst Engine Source\" FILES ${ENGINE_SOURCE_FILES})" << std::endl;
	cmake_lists_file << std::endl;
	cmake_lists_file << "set(ENGINE_RENDERING_FILES_ROOT \"${CMAKE_SOURCE_DIR}/../../Catalyst-Engine/Engine/Rendering\")" << std::endl;
	cmake_lists_file << "source_group(TREE ${ENGINE_RENDERING_FILES_ROOT} PREFIX \"Catalyst Engine Rendering\" FILES ${ENGINE_RENDERING_FILES})" << std::endl;
	cmake_lists_file << std::endl;

	cmake_lists_file << "set(ENGINE_THIRD_PARTY_FILES_ROOT \"${CMAKE_SOURCE_DIR}/../../Catalyst-Engine/Engine/Third Party\")" << std::endl;
	cmake_lists_file << "source_group(TREE ${ENGINE_THIRD_PARTY_FILES_ROOT} PREFIX \"Catalyst Engine Third Party\" FILES ";

	if (general_parameters._Physics == Physics::JOLT)
	{
		cmake_lists_file << "${THIRD_PARTY_JOLT_INCLUDE_FILES} ${THIRD_PARTY_JOLT_SOURCE_FILES}";
	}

	cmake_lists_file << ")" << std::endl;

	cmake_lists_file << std::endl;

	cmake_lists_file << "set(GAME_INCLUDE_FILES_ROOT \"${CMAKE_SOURCE_DIR}/../Code/Include\")" << std::endl;
	cmake_lists_file << "source_group(TREE ${GAME_INCLUDE_FILES_ROOT} PREFIX \"${PROJECT_NAME} Include\" FILES ${GAME_INCLUDE_FILES})" << std::endl;
	cmake_lists_file << std::endl;
	cmake_lists_file << "set(GAME_RENDERING_FILES_ROOT \"${CMAKE_SOURCE_DIR}/../Rendering\")" << std::endl;
	cmake_lists_file << "source_group(TREE ${GAME_RENDERING_FILES_ROOT} PREFIX \"${PROJECT_NAME} Rendering\" FILES ${GAME_RENDERING_FILES})" << std::endl;
	cmake_lists_file << std::endl;
	cmake_lists_file << "set(GAME_SCRIPT_FILES_ROOT \"${CMAKE_SOURCE_DIR}/../Content/Assets\")" << std::endl;
	cmake_lists_file << "source_group(TREE ${GAME_SCRIPT_FILES_ROOT} PREFIX \"${PROJECT_NAME} Script\" FILES ${GAME_SCRIPT_FILES})" << std::endl;
	cmake_lists_file << std::endl;
	cmake_lists_file << "set(GAME_SOURCE_FILES_ROOT \"${CMAKE_SOURCE_DIR}/../Code/Source\")" << std::endl;
	cmake_lists_file << "source_group(TREE ${GAME_SOURCE_FILES_ROOT} PREFIX \"${PROJECT_NAME} Source\" FILES ${GAME_SOURCE_FILES})" << std::endl;
	cmake_lists_file << std::endl;

	cmake_lists_file << "#Set optimization levels." << std::endl;
	cmake_lists_file << "set(CMAKE_CXX_FLAGS_DEBUG \"/Od /Oi /MTd /sdl- /GL /Zi\")" << std::endl;
	cmake_lists_file << "set(CMAKE_CXX_FLAGS_DEBUGEDITOR \"/Od /Oi /MTd /sdl- /GL /Zi\")" << std::endl;
	cmake_lists_file << "set(CMAKE_CXX_FLAGS_PROFILE \"/O2 /Oi /MT /sdl- /GL /Zi\")" << std::endl;
	cmake_lists_file << "set(CMAKE_CXX_FLAGS_PROFILEEDITOR \"/O2 /Oi /MT /sdl- /GL /Zi\")" << std::endl;
	cmake_lists_file << "set(CMAKE_CXX_FLAGS_FINAL \"/O2 /Oi /MT /sdl- /GL\")" << std::endl;
	cmake_lists_file << std::endl;

	cmake_lists_file << "#Remove the ZERO_CHECK project." << std::endl;
	cmake_lists_file << "set(CMAKE_SUPPRESS_REGENERATION true)" << std::endl;
	cmake_lists_file << std::endl;

	cmake_lists_file << "#Add build generation for the Final config." << std::endl;
	cmake_lists_file << "add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND if $(Configuration) == Final call \"RunBuildGeneration.bat\")" << std::endl;
	cmake_lists_file << std::endl;

	//Add the CATALYST_INCLUDE_ENVIROMENT_RESOURCE_COLLECTION define.
	if (general_parameters._IncludeEnvironmentResourceCollection)
	{
		cmake_lists_file << std::endl;
		cmake_lists_file << "#Include the enviroment resource collection." << std::endl;
		cmake_lists_file << "add_compile_definitions(CATALYST_INCLUDE_ENVIROMENT_RESOURCE_COLLECTION)" << std::endl;
	}

	//Add the CATALYST_INCLUDE_EXTRA_RESOURCE_COLLECTION define.
	if (general_parameters._IncludeExtraResourceCollection)
	{
		cmake_lists_file << std::endl;
		cmake_lists_file << "#Include the extra resource collection." << std::endl;
		cmake_lists_file << "add_compile_definitions(CATALYST_INCLUDE_EXTRA_RESOURCE_COLLECTION)" << std::endl;
	}

	//Add the code generation pre-build step, if requested.
	if (general_parameters._EnableCodeGeneration)
	{
		cmake_lists_file << std::endl;
		cmake_lists_file << "#Add the code generation pre-build step." << std::endl;
		cmake_lists_file << "add_custom_command(" << std::endl;
		cmake_lists_file << "TARGET ${PROJECT_NAME}" << std::endl;
		cmake_lists_file << "PRE_BUILD" << std::endl;
		cmake_lists_file << "COMMAND" << std::endl;
		cmake_lists_file << "call \"RunCodeGeneration.bat\"";
		cmake_lists_file << ")" << std::endl;
	}

	//Close the file.
	cmake_lists_file.close();

	//Remove all the contents of the Win64 folder.
	std::filesystem::remove_all("Win64", error_code); CHECK_ERROR_CODE();

	/*
	*	Generate the code generation .bat file, run it, and delete it.
	*	Important that we do this before generating the solution, so the generated files gets included.
	*/
	{
		std::filesystem::create_directories("Win64\\Win64", error_code); CHECK_ERROR_CODE();

		std::ofstream run_code_generation_file{ "Win64\\Win64\\RunCodeGeneration.bat" };

		run_code_generation_file << "cd Win64\\Win64\\" << std::endl;
		run_code_generation_file << "\"../../../../Catalyst-Engine/Tools/Binaries/CatalystEngineCodeGenerator.exe\"";

		if (!general_parameters._ScriptGeneratorGameInclude.empty())
		{
			run_code_generation_file << " -script_generator_game_include " << general_parameters._ScriptGeneratorGameInclude.c_str();
		}

		run_code_generation_file.close();

		system("Win64\\Win64\\RunCodeGeneration.bat");

		std::filesystem::remove("Win64\\Win64\\RunCodeGeneration.bat");
	}

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

		//Copy assimp-vc143-mt.dll.
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\assimp-vc143-mtd.dll", "Win64\\Win64\\Debug", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Debug\\assimp-vc143-mtd.dll", "Win64\\Win64\\DebugEditor", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\assimp-vc143-mt.dll", "Win64\\Win64\\Profile", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\Profile\\assimp-vc143-mt.dll", "Win64\\Win64\\ProfileEditor", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();

		//Copy steam_api64.dll.
		if (platform_parameters._Distribution == Win64Distribution::STEAM)
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

		//Copy onnxruntime.dll.
		if (general_parameters._UseONNXRuntimeLibrary)
		{
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\onnxruntime.dll", "Win64\\Win64\\Debug", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\onnxruntime.dll", "Win64\\Win64\\DebugEditor", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\onnxruntime.dll", "Win64\\Win64\\Profile", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\onnxruntime.dll", "Win64\\Win64\\ProfileEditor", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
			std::filesystem::copy("C:\\Github\\Catalyst-Engine\\Engine\\Libraries\\Dynamic\\onnxruntime.dll", "Win64\\Win64\\Final", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
		}
	}

	//Copy the CMakeLists.txt file to CMakeLists_Windows, and remove CMakeLists.txt. Nice to save the CMakeLists.txt file, to look at it later. (:
	std::filesystem::copy("CMakeLists.txt", "CMakeLists_Windows.txt", std::filesystem::copy_options::overwrite_existing, error_code); CHECK_ERROR_CODE();
	std::filesystem::remove("CMakeLists.txt", error_code); CHECK_ERROR_CODE();

	//A bit ugly, but I guess the "RunCodeGeneration.bat" file gets removed in a previous step, so uh, just re-add it!.
	if (general_parameters._EnableCodeGeneration)
	{
		//Add the "RunCodeGeneration.bat" file.
		std::ofstream run_code_generation_file{ "Win64\\Win64\\RunCodeGeneration.bat" };

		run_code_generation_file << "\"../../../../Catalyst-Engine/Tools/Binaries/CatalystEngineCodeGenerator.exe\"";

		if (!general_parameters._ScriptGeneratorGameInclude.empty())
		{
			run_code_generation_file << " -script_generator_game_include " << general_parameters._ScriptGeneratorGameInclude.c_str();
		}

		run_code_generation_file.close();
	}

	//Add the "RunBuildGeneration.bat" file.
	{
		std::ofstream run_code_generation_file{ "Win64\\Win64\\RunBuildGeneration.bat" };

		run_code_generation_file << "\"../../../../Catalyst-Engine/Tools/Binaries/CatalystEngineBuildGenerator.exe\"";

		run_code_generation_file.close();
	}
}

int main(int argument_count, char* arguments[])
{
	//Print the options.
	PrintOptions();

	//Construct the parameters.
	GeneralParameters general_parameters;
	AndroidParameters android_parameters;
	OculusQuestParameters oculus_quest_parameters;
	Win64Parameters win64_parameters;

	//Figure out which project type to generate.
	ProjectType project_type_to_generate{ ProjectType::UNKNOWN };

	if (strcmp(arguments[1], "ANDROID") == 0)
	{
		project_type_to_generate = ProjectType::ANDROID;
	}

	else if (strcmp(arguments[1], "OCULUS_QUEST") == 0)
	{
		project_type_to_generate = ProjectType::OCULUS_QUEST;
	}

	else if (strcmp(arguments[1], "WIN64") == 0)
	{
		project_type_to_generate = ProjectType::WIN64;
	}

	//Read the parameters.
	{
		//Open the config file.
		std::ifstream config_file{ "..\\Project.ini" };

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
				general_parameters._DeveloperName = argument;
			}

			//Is this the project name?
			else if (identifier == "PROJECT_NAME")
			{
				general_parameters._ProjectName = argument;
			}

			//Is this the physics?
			else if (identifier == "PHYSICS")
			{
				if (argument == "PHYSX")
				{
					general_parameters._Physics = Physics::PHYSX;
				}

				else if (argument == "JOLT")
				{
					general_parameters._Physics = Physics::JOLT;
				}
			}

			//Should the ONNX Runtime library be used?
			else if (identifier == "USE_ONNX_RUNTIME_LIBRARY")
			{
				general_parameters._UseONNXRuntimeLibrary = true;
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
		}

		//Close the config file.
		config_file.close();
	}

	//Ensure that the proper parameters has been set.
	if (general_parameters._DeveloperName.empty())
	{
		std::cout << "Need to set developer name!" << std::endl;

		return 0;
	}

	if (general_parameters._ProjectName.empty())
	{
		std::cout << "Need to set project name!" << std::endl;

		return 0;
	}

	//Calculate the developer name with no spaces.
	general_parameters._DeveloperNameNoSpaces = general_parameters._DeveloperName;
	general_parameters._DeveloperNameNoSpaces.erase(std::remove_if(general_parameters._DeveloperNameNoSpaces.begin(), general_parameters._DeveloperNameNoSpaces.end(), [](unsigned char x) { return std::isspace(x); }), general_parameters._DeveloperNameNoSpaces.end());

	//Calculate the project name with no spaces.
	general_parameters._ProjectNameNoSpaces = general_parameters._ProjectName;
	general_parameters._ProjectNameNoSpaces.erase(std::remove_if(general_parameters._ProjectNameNoSpaces.begin(), general_parameters._ProjectNameNoSpaces.end(), [](unsigned char x) { return std::isspace(x); }), general_parameters._ProjectNameNoSpaces.end());

	//Generate Android.
	if (project_type_to_generate == ProjectType::ANDROID
		|| project_type_to_generate == ProjectType::UNKNOWN)
	{
		GenerateAndroid(general_parameters, android_parameters);
	}

	//Generate Oculus Quest.
	if (project_type_to_generate == ProjectType::OCULUS_QUEST
		|| project_type_to_generate == ProjectType::UNKNOWN)
	{
		GenerateOculusQuest(general_parameters, oculus_quest_parameters);
	}

	//Generate Win64.
	if (project_type_to_generate == ProjectType::WIN64
		|| project_type_to_generate == ProjectType::UNKNOWN)
	{
		GenerateWin64(general_parameters, win64_parameters);
	}

	return 0;
}