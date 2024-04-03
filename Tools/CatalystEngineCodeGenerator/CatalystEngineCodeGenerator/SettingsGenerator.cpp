//Header file.
#include "SettingsGenerator.h"

//Constants.
#define CONTENT_DIRECTORY "..\\..\\..\\Content\\"
#define ASSETS_DIRECTORY CONTENT_DIRECTORY "\\Assets"

#define SOURCE_FILE_PATH "..\\..\\..\\Code\\Source\\Generated\\Settings.Generated.cpp"

/*
*	Enumeration class definition.
*/
class Enumeration final
{

public:

	/*
	*	Value class definition.
	*/
	class Value final
	{

	public:

		//The name.
		std::string _Name;

		//The value.
		std::string _Value;

	};

	//The name.
	std::string _Name;

	//The type.
	std::string _Type;

	//The values.
	std::vector<Value> _Values;

};

/*
*	Variable definition.
*/
class Variable final
{

public:

	//The name.
	std::string _Name;

	//The type.
	std::string _Type;

	//The default.
	std::string _Default;

	//Denotes whether or not this variable is an array.
	bool _IsArray;

};

/*
*	Class definition.
*/
class Class final
{

public:

	//The name.
	std::string _Name;

	//The variables.
	std::vector<Variable> _Variables;

};

/*
*	Generates serialize to stream archive for the given type.
*/
inline void GenerateSerializeToStreamArchiveForType
(
	const std::string &indent,
	const std::string &JSON,
	const std::string &type,
	const std::vector<Enumeration> &enumerations,
	const std::vector<Class> &classes,
	std::ofstream &file)
{
	//Check if the type is an enumeration.
	for (const Enumeration &enumeration : enumerations)
	{
		if (type == enumeration._Name)
		{
			file << indent << "const " << enumeration._Name.c_str() << " _value{ StringToEnumerationValue<" << enumeration._Name.c_str() << ">(" << JSON << ".get<std::string>()) };" << std::endl;
			file << indent << "stream_archive->Write(&_value, sizeof(" << enumeration._Name.c_str() << "));" << std::endl;

			return;
		}
	}

	//Check if the type is an object.
	for (const Class &_class : classes)
	{
		if (type == _class._Name)
		{
			file << indent << "Serialize" << _class._Name.c_str() << "ToStreamArchive(" << JSON << ", stream_archive);" << std::endl;

			return;
		}
	}

	//Check for special cases.
	if (type == "String")
	{
		file << indent << "const std::string _string{ " << JSON << ".get<std::string>() };" << std::endl;
		file << indent << "const uint64 _string_length{ _string.length() + 1 };" << std::endl;
		file << indent << "stream_archive->Write(&_string_length, sizeof(uint64));" << std::endl;
		file << indent << "stream_archive->Write(_string.data(), _string.length());" << std::endl;
		file << indent << "constexpr char NUL_TERMINATOR{ '\\0' };" << std::endl;
		file << indent << "stream_archive->Write(&NUL_TERMINATOR, sizeof(char));" << std::endl;

		return;
	}

	if (type == "HashString")
	{
		file << indent << "const HashString _hash_string{ " << JSON << ".get<std::string>().c_str() };" << std::endl;
		file << indent << "stream_archive->Write(&_hash_string, sizeof(HashString));" << std::endl;

		return;
	}

	//Otherwise it's a "builtin type".
	file << indent << "const " << type << " _value{ " << JSON << ".get<" << type << ">() };" << std::endl;
	file << indent << "stream_archive->Write(&_value, sizeof(" << type << "));" << std::endl;
}

/*
*	Generates a serialization of a default value.
*/
inline void GenerateSerializeDefaultValue(const std::string &indent, const Variable &variable, std::ofstream &file)
{
	//Arrays can't really have default values - They are just empty.
	if (variable._IsArray)
	{
		file << indent << "constexpr uint64 DEFAULT{ 0 };" << std::endl;
		file << indent << "stream_archive->Write(&DEFAULT, sizeof(uint64));" << std::endl;
	}

	else
	{
		//If this variable has a default value, try to use that.
		if (!variable._Default.empty())
		{
			//Special cases.
			if (variable._Type == "HashString")
			{
				file << indent << "const HashString DEFAULT{ \"" << variable._Default.c_str() << "\" }; " << std::endl;
				file << indent << "stream_archive->Write(&DEFAULT, sizeof(HashString));" << std::endl;
			}

			else if (variable._Type == "String")
			{
				file << indent << "constexpr const char *const RESTRICT DEFAULT{ \"" << variable._Default.c_str() << "\" }; " << std::endl;
				file << indent << "constexpr uint64 DEFAULT_LENGTH{ StringUtilities::StringLength(DEFAULT) + 1 }; " << std::endl;
				file << indent << "stream_archive->Write(&DEFAULT_LENGTH, sizeof(uint64));" << std::endl;
				file << indent << "stream_archive->Write(RESTRICT, DEFAULT_LENGTH);" << std::endl;
			}

			else
			{
				file << indent << "const " << variable._Type.c_str() << " DEFAULT{ " << variable._Default.c_str() << " }; " << std::endl;
				file << indent << "stream_archive->Write(&DEFAULT, sizeof(" << variable._Type.c_str() << "));" << std::endl;
			}
		}

		else
		{
			//Special cases.
			if (variable._Type == "String")
			{
				file << indent << "constexpr uint64 DEFAULT{ 0 };" << std::endl;
				file << indent << "stream_archive->Write(&DEFAULT, sizeof(uint64));" << std::endl;
			}

			else
			{
				file << indent << "const " << variable._Type.c_str() << " DEFAULT{ }; " << std::endl;
				file << indent << "stream_archive->Write(&DEFAULT, sizeof(" << variable._Type.c_str() << "));" << std::endl;
			}
		}
		
	}
}

/*
*	Generates deserialize from stream archive for the given type.
*/
inline void GenerateDeserializeFromStreamArchiveForType
(
	const std::string &indent,
	const std::string &name,
	const std::string &type,
	const std::vector<Class> &classes,
	std::ofstream &file)
{
	//First check if the type is an object.
	for (const Class& _class : classes)
	{
		if (type == _class._Name)
		{
			file << indent << "Deserialize" << _class._Name.c_str() << "FromStreamArchive(stream_archive, &" << name << ", stream_archive_position);" << std::endl;

			return;
		}
	}

	//Check for special cases.
	if (type == "String")
	{
		file << indent << "uint64 _string_length;" << std::endl;
		file << indent << "stream_archive.Read(&_string_length, sizeof(uint64), stream_archive_position);" << std::endl;
		file << indent << "if (_string_length > 0)" << std::endl;
		file << indent << "{" << std::endl;
		file << indent << "\t" << name << " = reinterpret_cast<const char *RESTRICT>(&stream_archive.Data()[*stream_archive_position]);" << std::endl;
		file << indent << "\t*stream_archive_position += _string_length;" << std::endl;
		file << indent << "}" << std::endl;

		return;
	}

	//Otherwise it's a "builtin type".
	file << indent << "stream_archive.Read(&" << name << ", sizeof(" << type << "), stream_archive_position);" << std::endl;
}

/*
*	Runs this data library generator.
*/
void SettingsGenerator::Run()
{
	//Cache the error code.
	std::error_code error_code;

	//Create the "CodeGeneration" directory, if it doesn't exist.
	std::filesystem::create_directory("..\\..\\..\\Code\\CodeGeneration", error_code);

	//Set up the cache.
	nlohmann::ordered_json cache;

#if defined(NDEBUG)
	if (std::filesystem::exists("..\\..\\..\\Code\\CodeGeneration\\SettingsCache.json"))
	{
		std::ifstream input_file{ "..\\..\\..\\Code\\CodeGeneration\\SettingsCache.json" };
		input_file >> cache;
		input_file.close();
	}
#endif

	//Gather settings definitions!
	GatherSettingsDefinitions(ASSETS_DIRECTORY, cache);

	//Compile settings, if needed.
	if (_CompileSettings || !std::filesystem::exists(SOURCE_FILE_PATH))
	{
		GenerateCode();
	}

	//Write the cache.
	{
		std::ofstream file{ "..\\..\\..\\Code\\CodeGeneration\\SettingsCache.json" };
		file << std::setw(4) << cache;
		file.close();
	}
}

/*
*	Gathers type libraries in the given directory.
*/
void SettingsGenerator::GatherSettingsDefinitions(const char *const directory_path, nlohmann::ordered_json&cache)
{
	//If the directory doesn't exist, just exit.
	if (!std::filesystem::exists(directory_path))
	{
		return;
	}

	//Iterate over all type libraries.
	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		//Gather recursively if this is a directory.
		if (entry.is_directory())
		{
			GatherSettingsDefinitions(entry.path().string().c_str(), cache);

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

		//Check if this is a settings definitions.
		if (extension != "SettingsDefinitions")
		{
			continue;
		}

		//Add the file path.
		_FilePaths.emplace_back(file_path);

		//Cache the entry.
		nlohmann::ordered_json&entry{ cache[file_path] };

		//Calculate the last write time.
		std::time_t last_write_time;

		{
			const std::filesystem::file_time_type _last_write_time{ std::filesystem::last_write_time(file_path) };
			const std::chrono::time_point<std::chrono::system_clock> time_point{ std::chrono::utc_clock::to_sys(std::chrono::file_clock::to_utc(_last_write_time)) };
			last_write_time = std::chrono::system_clock::to_time_t(time_point);
		}

		//Check if we should parse this entry.
		bool parse_entry{ true };

		if (!entry.is_null())
		{
			nlohmann::ordered_json &last_write_time_entry{ entry["LastWriteTime"] };

			if (!last_write_time_entry.is_null())
			{
				parse_entry = last_write_time != last_write_time_entry.get<std::time_t>();
			}
		}

		if (!parse_entry)
		{
			continue;
		}

		//Set that we need to compile settings.
		_CompileSettings = true;

		//Clear the entry before writing to it.
		entry.clear();

		//Write the last write time.
		entry["LastWriteTime"] = last_write_time;
	}
}

/*
*	Generates code.
*/
void SettingsGenerator::GenerateCode()
{
	//Parse all settings definitions files.
	std::vector<Enumeration> enumerations;
	std::vector<Class> classes;

	for (const std::string &file_path : _FilePaths)
	{
		//Load the JSON object.
		nlohmann::ordered_json JSON;

		{
			std::ifstream input_file{ file_path.c_str() };

			try
			{
				input_file >> JSON;
			}
			catch (const std::exception &exception)
			{
				ASSERT(false, file_path.c_str() << " - " << exception.what());
			}

			input_file.close();
		}

		//Parse enumerations.
		if (JSON.contains("Enumerations"))
		{
			const nlohmann::ordered_json &enumerations_entry{ JSON["Enumerations"] };

			for (auto enumeration_iterator{ enumerations_entry.begin() }; enumeration_iterator != enumerations_entry.end(); ++enumeration_iterator)
			{
				//Cache the enumeration entry.
				const nlohmann::ordered_json &enumeration_entry{ *enumeration_iterator };

				//Add the new enumeration.
				Enumeration &new_enumeration{ enumerations.emplace_back() };

				//Parse the name.
				new_enumeration._Name = enumeration_iterator.key();

				//Parse the type.
				new_enumeration._Type = enumeration_entry["Type"].get<std::string>();

				//Parse the values.
				const nlohmann::ordered_json &values_entry{ enumeration_entry["Values"] };

				for (auto value_iterator{ values_entry.begin() }; value_iterator != values_entry.end(); ++value_iterator)
				{
					//Cache the value entry.
					const nlohmann::ordered_json &value_entry{ *value_iterator };

					//Add the new value.
					Enumeration::Value &new_value{ new_enumeration._Values.emplace_back() };

					if (value_entry.is_object())
					{
						new_value._Name = value_iterator.key();
						new_value._Value = value_entry.get<std::string>();
					}

					else
					{
						new_value._Name = value_entry.get<std::string>().c_str();
					}
				}
			}
		}

		//Parse classes.
		if (JSON.contains("Classes"))
		{
			const nlohmann::ordered_json &classes_entry{ JSON["Classes"] };

			for (auto class_iterator{ classes_entry.begin() }; class_iterator != classes_entry.end(); ++class_iterator)
			{
				//Cache the class entry.
				const nlohmann::ordered_json &class_entry{ *class_iterator };

				//Add the new class.
				Class &new_class{ classes.emplace_back() };

				//Parse the name.
				new_class._Name = class_iterator.key();

				//Parse the variables.
				if (class_entry.contains("Variables"))
				{
					const nlohmann::ordered_json &variables_entry{ class_entry["Variables"] };

					for (auto variable_iterator{ variables_entry.begin() }; variable_iterator != variables_entry.end(); ++variable_iterator)
					{
						//Cache the variable entry.
						const nlohmann::ordered_json &variable_entry{ *variable_iterator };

						//Add the new variable.
						Variable &new_variable{ new_class._Variables.emplace_back() };

						//Parse the name.
						new_variable._Name = variable_iterator.key();

						//Parse the type.
						new_variable._Type = variable_entry["Type"].get<std::string>();

						//Parse the default.
						if (variable_entry.contains("Default"))
						{
							const nlohmann::json &default_entry{ variable_entry["Default"] };

							if (new_variable._Type == "float32")
							{
								char buffer[16];
								sprintf_s(buffer, "%f", variable_entry["Default"].get<float32>());

								new_variable._Default = buffer;

								if (new_variable._Default[new_variable._Default.length() - 1] != 'f')
								{
									new_variable._Default += "f";
								}
							}

							else if (new_variable._Type == "String")
							{
								new_variable._Default = variable_entry["Default"].get<std::string>();
							}
							
							else
							{
								ASSERT(false, "Unknown default type!");
							}
						}

						//Parse whether or not this variable is an array.
						if (variable_entry.contains("Array") && variable_entry["Array"].get<bool>())
						{
							new_variable._IsArray = true;
						}
					}
				}
			}
		}
	}

	//Generate a header file for each enumeration.
	{
		//Figure out the header directory path.
		char header_directory_path[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(header_directory_path, "..\\..\\..\\Code\\Include\\Generated\\Settings");

		//Create the directory, if it doesn't exist.
		std::filesystem::create_directory(header_directory_path);

		//Write all enumerations.
		for (const Enumeration &enumeration : enumerations)
		{
			//Figure out the file path.
			char file_path[MAXIMUM_FILE_PATH_LENGTH];
			sprintf_s(file_path, "%s\\%s.Generated.h", header_directory_path, enumeration._Name.c_str());

			//Open the file.
			std::ofstream file{ file_path };

			//Pragma once.
			file << "#pragma once" << std::endl;
			file << std::endl;

			//Add core includes.
			file << "//Core." << std::endl;
			file << "#include <Core/Essential/CatalystEssential.h>" << std::endl;
			file << std::endl;
			
			//Add the enumeration.
			file << "enum class " << enumeration._Name << " : " << enumeration._Type << std::endl;
			file << "{" << std::endl;

			for (const Enumeration::Value &value : enumeration._Values)
			{
				if (value._Value.empty())
				{
					file << "\t" << value._Name.c_str() << "," << std::endl;
				}

				else
				{
					file << "\t" << value._Name.c_str() << " = " << value._Value.c_str() << "," << std::endl;
				}
			}

			file << "};";

			//Close the file.
			file.close();
		}
	}

	//Generate a header file for each class.
	{
		//Figure out the header directory path.
		char header_directory_path[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(header_directory_path, "..\\..\\..\\Code\\Include\\Generated\\Settings");

		//Create the directory, if it doesn't exist.
		std::filesystem::create_directory(header_directory_path);

		//Write all classes.
		for (const Class &_class : classes)
		{
			//Figure out the file path.
			char file_path[MAXIMUM_FILE_PATH_LENGTH];
			sprintf_s(file_path, "%s\\%s.Generated.h", header_directory_path, _class._Name.c_str());

			//Open the file.
			std::ofstream file{ file_path };

			//Pragma once.
			file << "#pragma once" << std::endl;
			file << std::endl;

			//Add core includes.
			{
				bool has_added_any_core_include{ false };

				//Check if any variable in this class is an array - If so, include <DynamicArray.h>.
				{
					bool any_variable_is_array{ false };

					for (const Variable& variable : _class._Variables)
					{
						if (variable._IsArray)
						{
							any_variable_is_array = true;

							break;
						}
					}

					if (any_variable_is_array)
					{
						if (!has_added_any_core_include)
						{
							file << "//Core." << std::endl;

							has_added_any_core_include = true;
						}

						file << "#include <Core/Containers/DynamicArray.h>" << std::endl;
					}
				}

				//Check if any variable in this class is a HashString - If so, include <HashString.h>.
				{
					bool any_variable_is_hashstring{ false };

					for (const Variable &variable : _class._Variables)
					{
						if (variable._Type == "HashString")
						{
							any_variable_is_hashstring = true;

							break;
						}
					}

					if (any_variable_is_hashstring)
					{
						if (!has_added_any_core_include)
						{
							file << "//Core." << std::endl;

							has_added_any_core_include = true;
						}

						file << "#include <Core/General/HashString.h>" << std::endl;
					}
				}

				if (has_added_any_core_include)
				{
					file << std::endl;
				}
			}

			//Check if this class references any enumerations that should be included.
			bool has_referenced_any_enumerations{ false };

			{
				for (const Variable &variable : _class._Variables)
				{
					for (const Enumeration &enumeration : enumerations)
					{
						if (variable._Type == enumeration._Name)
						{
							if (!has_referenced_any_enumerations)
							{
								file << "//Settings." << std::endl;

								has_referenced_any_enumerations = true;
							}

							file << "#include <Generated/Settings/" << enumeration._Name.c_str() << ".Generated.h>" << std::endl;
						}
					}
				}

				if (has_referenced_any_enumerations)
				{
					file << std::endl;
				}
			}

			//Check if this class references any other classes that should be included.
			{
				bool has_referenced_any_classes{ false };

				for (const Variable &variable : _class._Variables)
				{
					for (const Class &other_class : classes)
					{
						if (variable._Type == other_class._Name)
						{
							if (!has_referenced_any_classes)
							{
								if (!has_referenced_any_enumerations)
								{
									file << "//Settings." << std::endl;
								}

								has_referenced_any_classes = true;
							}

							file << "#include <Generated/Settings/" << other_class._Name.c_str() << ".Generated.h>" << std::endl;
						}
					}
				}

				if (has_referenced_any_classes)
				{
					file << std::endl;
				}
			}

			//Write the class!
			file << "class " << _class._Name.c_str() << " final" << std::endl;
			file << "{" << std::endl;
			file << std::endl;
			file << "public:" << std::endl;
			file << std::endl;

			{
				const uint64 class_hash{ MurmurHash64(_class._Name.c_str(), _class._Name.length()) };

				file << "\tconstexpr static uint64 CLASS_IDENTIFIER{ " << class_hash << " };" << std::endl;
				file << std::endl;
			}

			for (const Variable &variable : _class._Variables)
			{
				//Figure out the type.
				std::string type{ variable._Type };

				//Special case for "String".
				if (type == "String")
				{
					type = "const char *RESTRICT";
				}

				if (variable._IsArray)
				{
					file << "\tDynamicArray<" << type.c_str() << "> _" << variable._Name.c_str() << ";" << std::endl;
				}
				
				else
				{
					file << "\t" << type.c_str() << " _" << variable._Name.c_str() << ";" << std::endl;
				}
			}

			file << std::endl;
			file << "};";

			//Close the file.
			file.close();
		}
	}

	//Figure out the directory path.
	char directory_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(directory_path, "..\\..\\..\\Code\\Source\\Generated");

	//Create the directory, if it doesn't exist.
	std::filesystem::create_directory(directory_path);

	//Figure out the file path.
	char file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(file_path, "%s\\Settings.Generated.cpp", directory_path);

	//Open the file.
	std::ofstream file{ file_path };

	//Add the includes.
	file << "//Header file." << std::endl;
	file << "#include <Settings/Settings.h>" << std::endl;
	file << std::endl;

	file << "//Core." << std::endl;
	file << "#include <Core/Algorithms/HashAlgorithms.h>" << std::endl;
	file << std::endl;

	file << "//Settings." << std::endl;

	for (const Class &_class : classes)
	{
		file << "#include <Generated/Settings/" << _class._Name.c_str() << ".Generated.h>" << std::endl;
	}

	file << std::endl;

	//Add forward declarations for all class specific functions.
	file << "//Forward declarations." << std::endl;

	file << "template <typename TYPE>" << std::endl;
	file << "FORCE_INLINE NO_DISCARD TYPE StringToEnumerationValue(const std::string &string) NOEXCEPT;" << std::endl;
	file << std::endl;

	for (const Class &_class : classes)
	{
		file << "FORCE_INLINE void Serialize" << _class._Name.c_str() << "ToStreamArchive(const nlohmann::json &JSON, StreamArchive *const RESTRICT stream_archive) NOEXCEPT;" << std::endl;
	}

	file << std::endl;

	for (const Class &_class : classes)
	{
		file << "FORCE_INLINE void Deserialize" << _class._Name.c_str() << "FromStreamArchive(const StreamArchive &stream_archive, " << _class._Name.c_str() << " *const RESTRICT object, uint64 *const RESTRICT stream_archive_position) NOEXCEPT;" << std::endl;
	}

	file << std::endl;

	//Add the "SerializeToStreamArchive" function.
	file << "/*" << std::endl;
	file << "*\tSerializes settings from JSON into a stream archive." << std::endl;
	file << "*/" << std::endl;
	file << "void Settings::SerializeToStreamArchive(const nlohmann::json &JSON, StreamArchive *const RESTRICT stream_archive) NOEXCEPT" << std::endl;
	file << "{" << std::endl;
	file << "\tfor (auto entry_iterator{ JSON.begin() }; entry_iterator != JSON.end(); ++entry_iterator)" << std::endl;
	file << "\t{" << std::endl;
	file << "\t\tconst uint64 hash{ HashAlgorithms::MurmurHash64(entry_iterator.key().c_str(), entry_iterator.key().length()) };" << std::endl;
	file << "\t\tswitch (hash)" << std::endl;
	file << "\t\t{" << std::endl;

	for (const Class &_class : classes)
	{
		file << "\t\t\tcase " << _class._Name.c_str() << "::CLASS_IDENTIFIER:" << std::endl;
		file << "\t\t\t{" << std::endl;
		file << "\t\t\t\tSerialize" << _class._Name.c_str() << "ToStreamArchive(*entry_iterator, stream_archive);" << std::endl;
		file << "\t\t\t\tbreak;" << std::endl;
		file << "\t\t\t}" << std::endl;
	}

	file << "\t\t\tdefault:" << std::endl;
	file << "\t\t\t{" << std::endl;
	file << "\t\t\t\tASSERT(false, \"Invalid case!\");" << std::endl;
	file << "\t\t\t\tbreak;" << std::endl;
	file << "\t\t\t}" << std::endl;
	file << "\t\t}" << std::endl;
	file << "\t}" << std::endl;
	file << "}" << std::endl;
	file << std::endl;

	//Add the "DeserializeFromStreamArchiveInternal" function.
	file << "/*" << std::endl;
	file << "*\tDeserializes a settings object from the given stream archive internally." << std::endl;
	file << "*/" << std::endl;
	file << "void Settings::DeserializeFromStreamArchiveInternal(const uint64 class_identifier, const StreamArchive &stream_archive, void *const RESTRICT object) NOEXCEPT" << std::endl;
	file << "{" << std::endl;
	file << "\tuint64 stream_archive_position{ 0 };" << std::endl;
	file << "\tswitch (class_identifier)" << std::endl;
	file << "\t{" << std::endl;

	for (const Class &_class : classes)
	{
		file << "\t\tcase " << _class._Name.c_str() << "::CLASS_IDENTIFIER:" << std::endl;
		file << "\t\t{" << std::endl;
		file << "\t\t\tDeserialize" << _class._Name.c_str() << "FromStreamArchive(stream_archive, static_cast<" << _class._Name.c_str() << " *const RESTRICT>(object), &stream_archive_position);" << std::endl;
		file << "\t\t\tbreak;" << std::endl;
		file << "\t\t}" << std::endl;
	}

	file << "\t\tdefault:" << std::endl;
	file << "\t\t{" << std::endl;
	file << "\t\t\tASSERT(false, \"Invalid case!\");" << std::endl;
	file << "\t\t\tbreak;" << std::endl;
	file << "\t\t}" << std::endl;
	file << "\t}" << std::endl;
	file << "}" << std::endl;
	file << std::endl;

	//Add the "StringToEnumerationValue" functions for all enumerations.
	for (const Enumeration &enumeration : enumerations)
	{
		file << "template <>" << std::endl;
		file << "FORCE_INLINE NO_DISCARD " << enumeration._Name.c_str() << " StringToEnumerationValue<" << enumeration._Name.c_str() << ">(const std::string &string) NOEXCEPT" << std::endl;
		file << "{" << std::endl;
		file << "\tconst uint64 hash{ HashAlgorithms::MurmurHash64(string.data(), string.length()) };" << std::endl;
		file << "\tswitch (hash)" << std::endl;
		file << "\t{" << std::endl;

		for (const Enumeration::Value &value : enumeration._Values)
		{
			const uint64 hash{ MurmurHash64(value._Name.data(), value._Name.length()) };

			file << "\t\tcase " << hash << ":" << std::endl;
			file << "\t\t{" << std::endl;
			file << "\t\t\treturn " << enumeration._Name.c_str() << "::" << value._Name.c_str() << ";" << std::endl;
			file << "\t\t}" << std::endl;
		}

		file << "\t\tdefault:" << std::endl;
		file << "\t\t{" << std::endl;
		file << "\t\t\tASSERT(false, \"Invalid case!\");" << std::endl;
		file << "\t\t\treturn static_cast<" << enumeration._Name.c_str() << ">(0);" << std::endl;
		file << "\t\t}" << std::endl;

		file << "\t}" << std::endl;
		file << "}" << std::endl;
	}

	if (!enumerations.empty())
	{
		file << std::endl;
	}

	//Add "SerializeToStreamArchive" functions for all classes.
	for (const Class &_class : classes)
	{
		file << "/*" << std::endl;
		file << "*\tSerializes " << _class._Name.c_str() << " to a stream archive." << std::endl;
		file << "*/" << std::endl;
		file << "FORCE_INLINE void Serialize" << _class._Name.c_str() << "ToStreamArchive(const nlohmann::json &JSON, StreamArchive *const RESTRICT stream_archive) NOEXCEPT" << std::endl;
		file << "{" << std::endl;

		//Serialize all variables.
		for (const Variable &variable : _class._Variables)
		{
			file << "\tif (JSON.contains(\"" << variable._Name.c_str() << "\"))" << std::endl;
			file << "\t{" << std::endl;
			file << "\t\tconst nlohmann::json &variable_entry{ JSON[\"" << variable._Name.c_str() << "\"] };" << std::endl;

			if (variable._IsArray)
			{
				file << "\t\tconst uint64 size{ variable_entry.size() };" << std::endl;
				file << "\t\tstream_archive->Write(&size, sizeof(uint64));" << std::endl;
				file << "\t\tfor (const nlohmann::json &array_entry : variable_entry)" << std::endl;
				file << "\t\t{" << std::endl;

				GenerateSerializeToStreamArchiveForType
				(
					"\t\t\t",
					"array_entry",
					variable._Type,
					enumerations,
					classes,
					file
				);

				file << "\t\t}" << std::endl;
			}

			else
			{
				GenerateSerializeToStreamArchiveForType
				(
					"\t\t",
					"variable_entry",
					variable._Type,
					enumerations,
					classes,
					file
				);
			}

			file << "\t}" << std::endl;
			file << "\telse" << std::endl;
			file << "\t{" << std::endl;

			GenerateSerializeDefaultValue("\t\t", variable, file);

			file << "\t}" << std::endl;
		}

		file << "}" << std::endl;
		file << std::endl;
	}

	//Add "DeserializeFromStreamArchive" functions for all classes.
	for (const Class &_class : classes)
	{
		file << "/*" << std::endl;
		file << "*\tDeserializes " << _class._Name.c_str() << " from a stream archive." << std::endl;
		file << "*/" << std::endl;
		file << "FORCE_INLINE void Deserialize" << _class._Name.c_str() << "FromStreamArchive(const StreamArchive &stream_archive, " << _class._Name.c_str() <<" *const RESTRICT object, uint64 *const RESTRICT stream_archive_position) NOEXCEPT" << std::endl;
		file << "{" << std::endl;

		for (const Variable &variable : _class._Variables)
		{
			file << "\t{" << std::endl;

			if (variable._IsArray)
			{
				file << "\t\tuint64 size;" << std::endl;
				file << "\t\tstream_archive.Read(&size, sizeof(uint64), stream_archive_position);" << std::endl;
				file << "\t\tif (size > 0)" << std::endl;
				file << "\t\t{" << std::endl;
				file << "\t\t\tobject->_" << variable._Name.c_str() << ".Upsize<true>(size);" << std::endl;
				file << "\t\t\tfor (uint64 i{ 0 }; i < size; ++i)" << std::endl;
				file << "\t\t\t{" << std::endl;

				GenerateDeserializeFromStreamArchiveForType
				(
					"\t\t\t\t",
					std::string("object->_") + variable._Name.c_str() + std::string("[i]"),
					variable._Type,
					classes,
					file
				);

				file << "\t\t\t}" << std::endl;
				file << "\t\t}" << std::endl;
			}

			else
			{
				GenerateDeserializeFromStreamArchiveForType
				(
					"\t\t",
					std::string("object->_") + variable._Name.c_str(),
					variable._Type,
					classes,
					file
				);
			}

			file << "\t}" << std::endl;
		}

		file << "}" << std::endl;
		file << std::endl;
	}

	//Close the file.
	file.close();
}