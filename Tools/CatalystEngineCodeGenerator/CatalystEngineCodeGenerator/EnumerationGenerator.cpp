//Header file.
#include "EnumerationGenerator.h"

//Catalyst code generator.
#include "TextParsing.h"

//Constants.
#define ENGINE_CODE_INCLUDE_DIRECTORY "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Include"
#define ENGINE_CODE_SOURCE_DIRECTORY "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Source"
#define GAME_CODE_INCLUDE_DIRECTORY "..\\..\\..\\Code\\Include"
#define GAME_CODE_SOURCE_DIRECTORY "..\\..\\..\\Code\\Source"

/*
*	Enumeration value class definition.
*/
class EnumerationValue final
{

public:

	//The index.
	uint64 _Index;

	//The name.
	std::string _Name;

	//The value.
	std::string _Value;

};

/*
*	Enumeration data class definition.
*/
class EnumerationData final
{

public:

	//The file path.
	std::string _FilePath;

	//The name.
	std::string _Name;

	//The type.
	std::string _Type;

	//The values.
	std::vector<EnumerationValue> _Values;

};

/*
*	Runs this enumeration generator.
*/
void EnumerationGenerator::Run()
{
	//Create the "CodeGeneration" directory, if it doesn't exist.
	std::filesystem::create_directory("..\\..\\..\\Code\\CodeGeneration");

	//Set up the JSON object.
	nlohmann::json JSON;

	//Read the cache, if it exists.
//#if defined(NDEBUG)
	if (std::filesystem::exists("..\\..\\..\\Code\\CodeGeneration\\EnumerationCache.json"))
	{
		std::ifstream input_file{ "..\\..\\..\\Code\\CodeGeneration\\EnumerationCache.json" };
		input_file >> JSON;
		input_file.close();
	}
//#endif

	//Gather enumerations!
	bool new_files_parsed{ false };

	new_files_parsed |= GatherEnumerations(ENGINE_CODE_INCLUDE_DIRECTORY, &JSON);
	new_files_parsed |= GatherEnumerations(ENGINE_CODE_SOURCE_DIRECTORY, &JSON);
	new_files_parsed |= GatherEnumerations(GAME_CODE_INCLUDE_DIRECTORY, &JSON);
	new_files_parsed |= GatherEnumerations(GAME_CODE_SOURCE_DIRECTORY, &JSON);

	//Generate the header file.
	if (new_files_parsed)
	{
		GenerateSourceFile(JSON);
	}

	//Write the JSON to file.
	std::ofstream file{ "..\\..\\..\\Code\\CodeGeneration\\EnumerationCache.json" };
	file << std::setw(4) << JSON;
	file.close();
}

/*
*	Gathers enumerations in the given directory.
*/
bool EnumerationGenerator::GatherEnumerations(const char *const directory_path, nlohmann::json *const JSON)
{
	//Remember if new files were parsed.
	bool new_files_parsed{ false };

	//Iterate over all scripts.
	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		//Scan recursively if this is a directory.
		if (entry.is_directory())
		{
			new_files_parsed |= GatherEnumerations(entry.path().string().c_str(), JSON);

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
		nlohmann::json& entry{ (*JSON)[file_path] };

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
				//Parse enumerations.
				{
					const size_t define_position{ current_line.find("#define") };
					const size_t macro_position{ current_line.find("CATALYST_ENUMERATION") };

					if (define_position == std::string::npos
						&& macro_position != std::string::npos)
					{
						nlohmann::json &enumerations_entry{ entry["Enumerations"] };

						ParseEnumeration(file, current_line, &enumerations_entry);

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
*	Parses an enumeration.
*/
void EnumerationGenerator::ParseEnumeration(std::ifstream &file, std::string &current_line, nlohmann::json *const JSON)
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

	//Retrieve the type.
	std::string type;

	while (type.empty())
	{
		//Retrieve the line.
		std::getline(file, current_line);

		//This should be the type.
		size_t start_position{ 0 };

		while (current_line[start_position] == '\t')
		{
			++start_position;
		}

		type = &current_line[start_position];

		type[type.length() - 1] = '\0';
	}

	//Set up the arguments.
	std::array<std::string, 8> arguments;

	//Add the entry.
	nlohmann::json &enumeration_entry{ (*JSON)[name] };

	//Add the type.
	enumeration_entry["Type"] = type;

	//Walk through the rest of the lines.
	std::string previous_value;
	uint64 value_index{ 0 };

	while (std::getline(file, current_line))
	{
		//Break on the closing parantheses.
		if (current_line == ");")
		{
			break;
		}

		//Parse the arguments.
		const uint64 arguments_parsed
		{
			ParseSpaceSeparatedArguments
			(
				current_line.c_str(),
				current_line.length(),
				arguments.data()
			)
		};

		//Figure out the name, and an optional value.
		const char *value_name{ nullptr };
		const char *value_value{ nullptr };

		for (uint64 i{ 0 }; i < arguments_parsed; ++i)
		{
			//Disregard equal signs.
			if (arguments[i] == "=")
			{
				continue;
			}

			//Remove commas.
			for (uint64 character_index{ 0 }; character_index < arguments[i].length(); ++character_index)
			{
				if (arguments[i][character_index] == ',')
				{
					arguments[i][character_index] = '\0';
				}
			}

			//If we haven't filled in the name yet, this is it - Otherwise, it's the value.
			if (!value_name)
			{
				value_name = arguments[i].c_str();
			}

			else
			{
				value_value = arguments[i].c_str();
			}
		}

		//Add the value entry.
		nlohmann::json &value_entry{ enumeration_entry[value_name] };

		//Write the index.
		value_entry["Index"] = value_index++;

		//Write the value.
		if (value_value)
		{
			value_entry["Value"] = value_value;
		}

		else if (!previous_value.empty())
		{
			value_entry["Value"] = previous_value + std::string(" + 1");
		}

		else
		{
			value_entry["Value"] = "0";
		}

		previous_value = value_entry["Value"].get<std::string>();
	}
}

/*
*	Generates the source file.
*/
void EnumerationGenerator::GenerateSourceFile(const nlohmann::json &JSON)
{
	//Figure out the directory path.
	char directory_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(directory_path, "..\\..\\..\\Code\\Source\\Generated");

	//Create the directory, if it doesn't exist.
	std::filesystem::create_directory(directory_path);

	//Figure out the file path.
	char file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(file_path, "%s\\Enumeration.Generated.cpp", directory_path);

	//Open the file.
	std::ofstream file{ file_path };

	//Add header stuff.
	file << "//Core." << std::endl;
	file << "#include <Core/General/Enumeration.h>" << std::endl;
	file << std::endl;

	//Add all enumerations.
	for (auto file_iterator{ JSON.begin() }; file_iterator != JSON.end(); ++file_iterator)
	{
		const nlohmann::json &file_entry{ *file_iterator };

		if (!file_entry.contains("Enumerations"))
		{
			continue;
		}

		const nlohmann::json &enumerations_entry{ file_entry["Enumerations"] };

		for (auto enumeration_iterator{ enumerations_entry.begin() }; enumeration_iterator != enumerations_entry.end(); ++enumeration_iterator)
		{
			//Cache the enumeration entry.
			const nlohmann::json &enumeration_entry{ *enumeration_iterator };

			//Gather the enumeration data.
			EnumerationData enumeration_data;

			//Set the file path.
			enumeration_data._FilePath = file_iterator.key().c_str();

			//Set the name.
			enumeration_data._Name = enumeration_iterator.key().c_str();

			//Set the type.
			if (enumeration_entry.contains("Type"))
			{
				enumeration_data._Type = enumeration_entry["Type"].get<std::string>().c_str();
			}

			else
			{
				enumeration_data._Type = "int32";
			}

			//Gather the enumeration values.
			for (auto enumeration_value_iterator{ enumeration_entry.begin() }; enumeration_value_iterator != enumeration_entry.end(); ++enumeration_value_iterator)
			{
				//Disregard the type.
				if (enumeration_value_iterator.key() == "Type")
				{
					continue;
				}

				//Cache the enumeration value entry.
				const nlohmann::json &enumeration_value_entry{ *enumeration_value_iterator };

				//Add the new enumeration value.
				EnumerationValue &new_enumeration_value{ enumeration_data._Values.emplace_back() };

				//Set the index.
				new_enumeration_value._Index = enumeration_value_entry["Index"].get<uint64>();

				//Set the name.
				new_enumeration_value._Name = enumeration_value_iterator.key().c_str();

				//Set the value.
				new_enumeration_value._Value = enumeration_value_entry["Value"].get<std::string>().c_str();
			}

			//Sort the enumeration values.
			std::sort
			(
				enumeration_data._Values.begin(),
				enumeration_data._Values.end(),
				[](const EnumerationValue &first, const EnumerationValue &second)
				{
					return first._Index < second._Index;
				}
			);

			//Write the include.
			file << "#include \"" << enumeration_data._FilePath.c_str() << "\"" << std::endl;
			file << std::endl;

			//Write the default constructor.
			file << enumeration_data._Name.c_str() << "::" << enumeration_data._Name.c_str() << "() NOEXCEPT" << std::endl;
			file << "{" << std::endl;
			file << "\t_VALUE = " << enumeration_data._Values[0]._Value.c_str() << ";" << std::endl;
			file << "}" << std::endl;

			//Write the constructor taking the value as an argument.
			file << enumeration_data._Name.c_str() << "::" << enumeration_data._Name.c_str() << "(const " << enumeration_data._Type.c_str() << " value) NOEXCEPT" << std::endl;
			file << "{" << std::endl;
			file << "\t_VALUE = value;" << std::endl;
			file << "}" << std::endl;

			//Write the type operator.
			file << enumeration_data._Name.c_str() << "::" << "operator " << enumeration_data._Type.c_str() << "() const NOEXCEPT" << std::endl;
			file << "{" << std::endl;
			file << "\treturn _VALUE;" << std::endl;
			file << "}" << std::endl;

			//Write the Size() function.
			file << "NO_DISCARD uint64 " << enumeration_data._Name.c_str() << "::" << "Size() const NOEXCEPT" << std::endl;
			file << "{" << std::endl;
			file << "\treturn " << enumeration_data._Values.size() << ";" << std::endl;
			file << "}" << std::endl;

			//Write the SetFromIndex() function.
			file << "void " << enumeration_data._Name.c_str() << "::" << "SetFromIndex(const uint64 index) NOEXCEPT" << std::endl;
			file << "{" << std::endl;
	
			file << "\tswitch (index)" << std::endl;
			file << "\t{" << std::endl;
			
			for (const EnumerationValue &enumeration_value : enumeration_data._Values)
			{
				file << "\t\tcase " << enumeration_value._Index << ": { _VALUE = " << enumeration_value._Value.c_str() << "; break; }" << std::endl;
			}

			file << "\t\tdefault: { ASSERT(false, \"Invalid case!\"); break; }" << std::endl;

			file << "\t}" << std::endl;
			
			file << "}" << std::endl;

			//Write the SetFromIndex() function.
			file << "void " << enumeration_data._Name.c_str() << "::" << "SetFromHash(const uint64 hash) NOEXCEPT" << std::endl;
			file << "{" << std::endl;

			file << "\tswitch (hash)" << std::endl;
			file << "\t{" << std::endl;

			for (const EnumerationValue& enumeration_value : enumeration_data._Values)
			{
				const uint64 hash{ MurmurHash64(enumeration_value._Name.c_str(), enumeration_value._Name.length()) };

				file << "\t\tcase " << hash << ": /*" << enumeration_value._Name.c_str() << "*/ { _VALUE = " << enumeration_value._Value.c_str() << "; break; }" << std::endl;
			}

			file << "\t\tdefault: { ASSERT(false, \"Invalid case!\"); break; }" << std::endl;

			file << "\t}" << std::endl;

			file << "}" << std::endl;

			//Write the ToString() function.
			file << "NO_DISCARD const char *const RESTRICT " << enumeration_data._Name.c_str() << "::" << "ToString() const NOEXCEPT" << std::endl;
			file << "{" << std::endl;

			file << "\tswitch (_VALUE)" << std::endl;
			file << "\t{" << std::endl;

			for (const EnumerationValue &enumeration_value : enumeration_data._Values)
			{
				file << "\t\tcase (" << enumeration_value._Value.c_str() << "): /*" << enumeration_value._Name.c_str() << "*/ { return \"" << enumeration_value._Name.c_str() << "\"; }" << std::endl;
			}

			file << "\t\tdefault: { ASSERT(false, \"Invalid case!\"); return \"\"; }" << std::endl;

			file << "\t}" << std::endl;

			file << "}" << std::endl;

			//Write the IndexToString() function.
			file << "NO_DISCARD const char *const RESTRICT " << enumeration_data._Name.c_str() << "::" << "IndexToString(const uint64 index) const NOEXCEPT" << std::endl;
			file << "{" << std::endl;

			file << "\tswitch (index)" << std::endl;
			file << "\t{" << std::endl;

			for (const EnumerationValue &enumeration_value : enumeration_data._Values)
			{
				file << "\t\tcase " << enumeration_value._Index << ": /*" << enumeration_value._Name.c_str() << "*/ { return \"" << enumeration_value._Name.c_str() << "\"; }" << std::endl;
			}

			file << "\t\tdefault: { ASSERT(false, \"Invalid case!\"); return \"\"; }" << std::endl;

			file << "\t}" << std::endl;

			file << "}" << std::endl;

			//Add some space until the next enumeration.
			file << std::endl;
		}
	}
}