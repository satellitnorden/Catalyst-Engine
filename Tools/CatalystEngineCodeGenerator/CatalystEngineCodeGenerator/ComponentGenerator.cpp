//Header file.
#include "ComponentGenerator.h"

//Catalyst code generator.
#include "TextParsing.h"

//Constants.
#define ENGINE_CODE_INCLUDE_DIRECTORY "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Include"
#define ENGINE_CODE_SOURCE_DIRECTORY "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Source"
#define GAME_CODE_INCLUDE_DIRECTORY "..\\..\\..\\Code\\Include"
#define GAME_CODE_SOURCE_DIRECTORY "..\\..\\..\\Code\\Source"

/*
*	Component data class definition.
*/
class ComponentData final
{

public:

	//The name.
	std::string _Name;

};

/*
*	Runs this component generator.
*/
void ComponentGenerator::Run()
{
	//Create the "CodeGeneration" directory, if it doesn't exist.
	std::filesystem::create_directory("..\\..\\..\\Code\\CodeGeneration");

	//Set up the JSON object.
	nlohmann::json JSON;

	//Read the cache, if it exists.
#if defined(NDEBUG)
	if (std::filesystem::exists("..\\..\\..\\Code\\CodeGeneration\\ComponentCache.json"))
	{
		std::ifstream input_file{ "..\\..\\..\\Code\\CodeGeneration\\ComponentCache.json" };
		input_file >> JSON;
		input_file.close();
	}
#endif

	//Gather components!
	bool new_files_parsed{ false };

	new_files_parsed |= GatherComponents(ENGINE_CODE_INCLUDE_DIRECTORY, &JSON);
	new_files_parsed |= GatherComponents(ENGINE_CODE_SOURCE_DIRECTORY, &JSON);
	new_files_parsed |= GatherComponents(GAME_CODE_INCLUDE_DIRECTORY, &JSON);
	new_files_parsed |= GatherComponents(GAME_CODE_SOURCE_DIRECTORY, &JSON);

	//Generate the header file.
	if (new_files_parsed)
	{
		GenerateSourceFile(JSON);
	}

	//Write the JSON to file.
#if defined(NDEBUG)
	std::ofstream file{ "..\\..\\..\\Code\\CodeGeneration\\ComponentCache.json" };
	file << std::setw(4) << JSON;
	file.close();
#endif
}

/*
*	Gathers components in the given directory.
*/
bool ComponentGenerator::GatherComponents(const char *const directory_path, nlohmann::json *const JSON)
{
	//Remember if new files were parsed.
	bool new_files_parsed{ false };

	//Iterate over all files.
	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		//Gather recursively if this is a directory.
		if (entry.is_directory())
		{
			new_files_parsed |= GatherComponents(entry.path().string().c_str(), JSON);

			continue;
		}

		//Cache the file path string.
		const std::string file_path{ entry.path().string() };

		//Figure out the extension.
		std::string extension;

		{
			const size_t last_dot_position{ file_path.find_last_of(".") };
			extension = file_path.substr(last_dot_position + 1, std::string::npos);
		}

		//Check if this is a C++ file.
		if (extension != "h" && extension != "cpp")
		{
			continue;
		}

		//Cache the entry.
		nlohmann::json &entry{ (*JSON)[file_path] };

		//Calculate the last write time.
		std::time_t last_write_time;

		{
			const std::filesystem::file_time_type _last_write_time{ std::filesystem::last_write_time(file_path) };
			const std::chrono::time_point<std::chrono::system_clock> time_point{ std::chrono::utc_clock::to_sys(std::chrono::file_clock::to_utc(_last_write_time)) };
			last_write_time = std::chrono::system_clock::to_time_t(time_point);
		}

		//Check if this entry should be parse.
		bool parse_entry{ true };

		if (!entry.is_null())
		{
			nlohmann::json &last_write_time_entry{ entry["LastWriteTime"] };

			if (!last_write_time_entry.is_null())
			{
				parse_entry = last_write_time != last_write_time_entry.get<std::time_t>();
			}
		}

		if (!parse_entry)
		{
			continue;
		}

		//Cache the previous entry, to be able to check if it actually changed.
		const nlohmann::json previous_entry{ entry };

		//Clear the entry before writing to it.
		entry.clear();

		//Write the last write time.
		entry["LastWriteTime"] = last_write_time;

		//Parse the file.
		{
			std::ifstream file{ file_path.c_str() };

			std::string current_line;

			while (std::getline(file, current_line))
			{
				//Parse components.
				{
					const size_t define_position{ current_line.find("#define") };
					const size_t macro_position{ current_line.find("DECLARE_COMPONENT") };

					if (define_position == std::string::npos
						&& macro_position != std::string::npos
						&& !OnlyComment(current_line.c_str(), current_line.length()))
					{
						nlohmann::json &components_entry{ entry["Components"] };

						ParseComponent(file, current_line, &components_entry);

						//Let's just say, if it contains enumerations, then new files were parsed.
						new_files_parsed = true;
					}
				}
			}
		}
	}

	return new_files_parsed;
}

/*
*	Parses a component.
*/
void ComponentGenerator::ParseComponent(std::ifstream &file, std::string &current_line, nlohmann::json *const JSON)
{
	//Retrieve the name.
	std::string name;

	while (name.empty())
	{
		//Retrieve the line.
		std::getline(file, current_line);

		//Ignore the opening parantheses.
		if (current_line == "(")
		{
			continue;
		}

		//Otherwise, this should be the name.
		size_t start_position{ 0 };

		while (current_line[start_position] == '\t')
		{
			++start_position;
		}

		name = &current_line[start_position];

		name[name.length() - 1] = '\0';
	}

	//Add the entry for this component.
	nlohmann::json &component_entry{ (*JSON)[name.c_str()] };
}

/*
*	Generates the source file.
*/
void ComponentGenerator::GenerateSourceFile(const nlohmann::json &JSON)
{
	//Figure out the directory path.
	char directory_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(directory_path, "..\\..\\..\\Code\\Source\\Generated");

	//Create the directory, if it doesn't exist.
	std::filesystem::create_directory(directory_path);

	//Figure out the file path.
	char file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(file_path, "%s\\Component.Generated.cpp", directory_path);

	//Open the file.
	std::ofstream file{ file_path };

	//Add header stuff.
	file << "//Components." << std::endl;
	file << "#include <Components/Core/Component.h>" << std::endl;
	file << std::endl;

	//Add all component data.
	std::vector<ComponentData> component_data;

	for (auto file_iterator{ JSON.begin() }; file_iterator != JSON.end(); ++file_iterator)
	{
		const nlohmann::json &file_entry{ *file_iterator };

		if (!file_entry.contains("Components"))
		{
			continue;
		}

		const nlohmann::json &components_entry{ file_entry["Components"] };

		//Add the include.
		file << "#include \"" << file_iterator.key().c_str() << "\"" << std::endl;
		file << std::endl;

		for (auto component_iterator{ components_entry.begin() }; component_iterator != components_entry.end(); ++component_iterator)
		{
			//Cache the component entry.
			const nlohmann::json &component_entry{ *component_iterator };

			//Add the singleton definition.
			file << "DEFINE_SINGLETON(" << component_iterator.key().c_str() << ");" << std::endl;

			//Add the component data.
			{
				ComponentData &new_component_data{ component_data.emplace_back() };

				//Set the name.
				new_component_data._Name = component_iterator.key().c_str();
			}
		}

		file << std::endl;
	}

	//Set up the "Components::Size" function.
	file << "NO_DISCARD uint64 Components::Size() NOEXCEPT" << std::endl;
	file << "{" << std::endl;
	file << "\treturn " << component_data.size() << ";" << std::endl;
	file << "}" << std::endl;
	file << std::endl;

	//Set up the "Components::At()" function.
	file << "NO_DISCARD Component *const RESTRICT Components::At(const uint64 index) NOEXCEPT" << std::endl;
	file << "{" << std::endl;
	file << "\tswitch (index)" << std::endl;
	file << "\t{" << std::endl;

	for (uint64 i{ 0 }; i < component_data.size(); ++i)
	{
		file << "\t\tcase " << i << ":" << std::endl;
		file << "\t\t{" << std::endl;
		file << "\t\t\treturn " << component_data[i]._Name.c_str() << "::Instance.Get();" << std::endl;
		file << "\t\t}" << std::endl;
	}

	file << "\t\tdefault:" << std::endl;
	file << "\t\t{" << std::endl;
	file << "\t\t\tASSERT(false, \"Invalid case!\");" << std::endl;
	file << "\t\t\treturn nullptr;" << std::endl;
	file << "\t\t}" << std::endl;

	file << "\t}" << std::endl;

	file << "}";
}