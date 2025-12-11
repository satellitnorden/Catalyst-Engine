//Header file.
#include <ProjectSettings/ProjectSettings.h>

//Third party.
#include <ThirdParty/json.hpp>

//STD.
#include <fstream>
#include <iostream>

/*
*	Utility function for reading a value from a JSON file with a default value.
*/
template <typename TYPE>
TYPE ReadValue(const nlohmann::json &JSON, const char *identifier, const TYPE default_value)
{
	return JSON.contains(identifier) ? JSON[identifier].get<TYPE>() : default_value;
}

/*
*	Reads the project settings.
*/
void ProjectSettings::Read(const char *file_path)
{
	//Read the JSON.
	nlohmann::json JSON;

	{
		//Open the file.
		std::ifstream file{ file_path };

		//Read into the JSON.
		file >> JSON;

		//Close the file.
		file.close();
	}

	//Read the developer name.
	_DeveloperName = ReadValue(JSON, "DeveloperName", std::string("UNKNOWN DEVELOPER NAME"));

	//Read the project name.
	_ProjectName = ReadValue(JSON, "ProjectName", std::string("UNKNOWN PROJECT NAME"));

	//Read the physics.
	if (JSON.contains("Physics"))
	{
		const std::string physics{ JSON["Physics"].get<std::string>() };

		if (physics == "NONE")
		{
			_Physics = Physics::NONE;
		}

		else if (physics == "JOLT")
		{
			_Physics = Physics::JOLT;
		}

		else
		{
			std::cerr << "Couldn't read physics " << physics.c_str() << "!" << std::endl;
		}
	}

	else
	{
		_Physics = Physics::NONE;
	}

	//Read whether or not to include the ONNX Runtime library.
	_IncludeONNXRuntimeLibrary = ReadValue(JSON, "IncludeONNXRuntimeLibrary", false);

	//Read whether or not to add static builds.
	_AddStaticBuilds = ReadValue(JSON, "AddStaticBuilds", false);

	//Read the script game include.
	if (JSON.contains("ScriptGameInclude"))
	{
		_ScriptGameInclude = JSON["ScriptGameInclude"].get<std::string>();
	}

	//Calculate the developer name with no spaces.
	_DeveloperNameNoSpaces = _DeveloperName;
	_DeveloperNameNoSpaces.erase(std::remove_if(_DeveloperNameNoSpaces.begin(), _DeveloperNameNoSpaces.end(), [](unsigned char x) { return std::isspace(x); }), _DeveloperNameNoSpaces.end());

	//Calculate the project name with no spaces.
	_ProjectNameNoSpaces = _ProjectName;
	_ProjectNameNoSpaces.erase(std::remove_if(_ProjectNameNoSpaces.begin(), _ProjectNameNoSpaces.end(), [](unsigned char x) { return std::isspace(x); }), _ProjectNameNoSpaces.end());

}