//Header file.
#include "SystemGenerator.h"

//Catalyst code generator.
#include "TextParsing.h"

//Constants.
#define ENGINE_CODE_INCLUDE_DIRECTORY "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Include"
#define ENGINE_CODE_SOURCE_DIRECTORY "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Source"
#define GAME_CODE_INCLUDE_DIRECTORY "..\\..\\..\\Code\\Include"
#define GAME_CODE_SOURCE_DIRECTORY "..\\..\\..\\Code\\Source"

/*
*	System data class definition.
*/
class SystemData final
{

public:

	//The name.
	std::string _Name;

	//Denotes whether or not this systems wants an 'Initialize()' call.
	bool _Initialize;

	//Denotes whether or not this systems wants a 'PostInitialize()' call.
	bool _PostInitialize;

	//Denotes whether or not this systems wants a 'Terminate()' call.
	bool _Terminate;

	//Denotes whether or not this systems wants a 'PostTerminate()' call.
	bool _PostTerminate;

	//The not defined requirements.
	std::vector<std::string> _NotDefinedRequirements;

	//The defined requirements.
	std::vector<std::string> _DefinedRequirements;

};

/*
*	Runs this component generator.
*/
void SystemGenerator::Run()
{
	//Create the "CodeGeneration" directory, if it doesn't exist.
	std::filesystem::create_directory("..\\..\\..\\Code\\CodeGeneration");

	//Set up the JSON object.
	nlohmann::json JSON;

	//Read the cache, if it exists.
#if defined(NDEBUG)
	if (std::filesystem::exists("..\\..\\..\\Code\\CodeGeneration\\SystemCache.json"))
	{
		std::ifstream input_file{ "..\\..\\..\\Code\\CodeGeneration\\SystemCache.json" };
		input_file >> JSON;
		input_file.close();
	}
#endif

	//Gather systems!
	bool new_files_parsed{ false };

	new_files_parsed |= GatherSystems(ENGINE_CODE_INCLUDE_DIRECTORY, &JSON);
	new_files_parsed |= GatherSystems(ENGINE_CODE_SOURCE_DIRECTORY, &JSON);
	new_files_parsed |= GatherSystems(GAME_CODE_INCLUDE_DIRECTORY, &JSON);
	new_files_parsed |= GatherSystems(GAME_CODE_SOURCE_DIRECTORY, &JSON);

	//Generate the header file.
	if (new_files_parsed)
	{
		GenerateSourceFile(JSON);
	}

	//Write the JSON to file.
	std::ofstream file{ "..\\..\\..\\Code\\CodeGeneration\\SystemCache.json" };
	file << std::setw(4) << JSON;
	file.close();
}

/*
*	Gathers systems in the given directory.
*/
bool SystemGenerator::GatherSystems(const char *const directory_path, nlohmann::json *const JSON)
{
	//Remember if new files were parsed.
	bool new_files_parsed{ false };

	//Iterate over all files.
	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		//Gather recursively if this is a directory.
		if (entry.is_directory())
		{
			new_files_parsed |= GatherSystems(entry.path().string().c_str(), JSON);

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
				//Parse systems.
				{
					const size_t define_position{ current_line.find("#define") };
					const size_t macro_position{ current_line.find("CATALYST_SYSTEM") };

					if (define_position == std::string::npos
						&& macro_position != std::string::npos
						&& !OnlyComment(current_line.c_str(), current_line.length()))
					{
						nlohmann::json &systems_entry{ entry["Systems"] };

						ParseSystem(file, current_line, &systems_entry);

						//Let's just say, if it contains systems, then new files were parsed.
						new_files_parsed = true;
					}
				}
			}
		}
	}

	return new_files_parsed;
}

/*
*	Parses a system.
*/
void SystemGenerator::ParseSystem(std::ifstream &file, std::string &current_line, nlohmann::json *const JSON)
{
	//Retrieve the name.
	std::string name;

	while (name.empty())
	{
		//Retrieve the line.
		std::getline(file, current_line);

		char character;
		size_t position;
		TextParsing::GetNextNonWhitespaceCharacter(current_line.c_str(), 0, &character, &position);

		const std::string _current_line{ &current_line[position] };

		//Ignore the opening parantheses.
		if (_current_line == "(")
		{
			continue;
		}

		//Otherwise, this should be the name.
		name = _current_line;

		if (name[name.length() - 1] == ',')
		{
			name.pop_back();
		}
	}

	//Add the entry for this system.
	nlohmann::json &system_entry{ (*JSON)[name.c_str()] };

	//Set the name.
	system_entry["Name"] = name.c_str();

	//Set to initial state.
	system_entry["Initialize"] = false;
	system_entry["PostInitialize"] = false;
	system_entry["Terminate"] = false;
	system_entry["PostTerminate"] = false;

	//Set up the arguments.
	std::array<std::string, 8> arguments;

	//Walk through the rest of the lines.
	std::string previous_value;
	uint64 value_index{ 0 };

	while (std::getline(file, current_line))
	{
		//Stop if we've reached the end.
		if (current_line == ");")
		{
			break;
		}

		//Check if this system has a not defined requirement.
		{
			const size_t position{ current_line.find("SYSTEM_NOT_DEFINED_REQUIREMENT(") };

			if (position != std::string::npos)
			{
				std::array<std::string, 8> arguments;

				const uint64 number_of_arguments
				{
					TextParsing::ParseFunctionArguments
					(
						current_line.c_str(),
						current_line.length(),
						arguments.data()
					)
				};

				system_entry["NotDefinedRequirements"].emplace_back() = arguments[0].c_str();

				continue;
			}
		}

		//Check if this system has a defined requirement.
		{
			const size_t position{ current_line.find("SYSTEM_DEFINED_REQUIREMENT(") };

			if (position != std::string::npos)
			{
				std::array<std::string, 8> arguments;

				const uint64 number_of_arguments
				{
					TextParsing::ParseFunctionArguments
					(
						current_line.c_str(),
						current_line.length(),
						arguments.data()
					)
				};

				system_entry["DefinedRequirements"].emplace_back() = arguments[0].c_str();

				continue;
			}
		}

		//Check if this system wants an 'Initialize()' call.
		{
			const size_t position{ current_line.find("SYSTEM_INITIALIZE()") };

			if (position != std::string::npos)
			{
				system_entry["Initialize"] = true;

				continue;
			}
		}

		//Check if this system wants a 'PostInitialize()' call.
		{
			const size_t position{ current_line.find("SYSTEM_POST_INITIALIZE()") };

			if (position != std::string::npos)
			{
				system_entry["PostInitialize"] = true;

				continue;
			}
		}

		//Check if this system wants a 'Terminate()' call.
		{
			const size_t position{ current_line.find("SYSTEM_TERMINATE()") };

			if (position != std::string::npos)
			{
				system_entry["Terminate"] = true;

				continue;
			}
		}

		//Check if this system wants a 'PostTerminate()' call.
		{
			const size_t position{ current_line.find("SYSTEM_POST_TERMINATE()") };

			if (position != std::string::npos)
			{
				system_entry["PostTerminate"] = true;

				continue;
			}
		}
	}
}

/*
*	Generates the source file.
*/
void SystemGenerator::GenerateSourceFile(const nlohmann::json &JSON)
{
	//Figure out the directory path.
	char directory_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(directory_path, "..\\..\\..\\Code\\Source\\Generated");

	//Create the directory, if it doesn't exist.
	std::filesystem::create_directory(directory_path);

	//Figure out the file path.
	char file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(file_path, "%s\\System.Generated.cpp", directory_path);

	//Open the file.
	std::ofstream file{ file_path };

	//Add header stuff.
	file << "//Header file." << std::endl;
	file << "#include <Systems/System.h>" << std::endl;
	file << std::endl;

	//Gather all systems data - Add includes as we go.
	file << "//Systems." << std::endl;

	std::vector<SystemData> system_data;

	for (auto file_iterator{ JSON.begin() }; file_iterator != JSON.end(); ++file_iterator)
	{
		const nlohmann::json &file_entry{ *file_iterator };

		if (!file_entry.contains("Systems"))
		{
			continue;
		}

		const nlohmann::json &systems_entry{ file_entry["Systems"] };

		//Add the include.
		file << "#include \"" << file_iterator.key().c_str() << "\"" << std::endl;

		for (auto system_iterator{ systems_entry.begin() }; system_iterator != systems_entry.end(); ++system_iterator)
		{
			//Cache the system entry.
			const nlohmann::json &system_entry{ *system_iterator };

			//Add the system data.
			{
				SystemData &new_system_data{ system_data.emplace_back() };

				//Set the name.
				new_system_data._Name = system_iterator.key().c_str();

				//Set whether or not this systems wants an 'Initialize()' call.
				new_system_data._Initialize = system_entry.contains("Initialize") && system_entry["Initialize"].get<bool>();

				//Set whether or not this systems wants a 'PostInitialize()' call.
				new_system_data._PostInitialize = system_entry.contains("PostInitialize") && system_entry["PostInitialize"].get<bool>();

				//Set whether or not this systems wants a 'Terminate()' call.
				new_system_data._Terminate = system_entry.contains("Terminate") && system_entry["Terminate"].get<bool>();

				//Set whether or not this systems wants a 'PostTerminate()' call.
				new_system_data._PostTerminate = system_entry.contains("PostTerminate") && system_entry["PostTerminate"].get<bool>();

				//Add the not defined requiremnets.
				if (system_entry.contains("NotDefinedRequirements"))
				{
					for (const nlohmann::json &not_defined_requirement : system_entry["NotDefinedRequirements"])
					{
						new_system_data._NotDefinedRequirements.emplace_back(not_defined_requirement.get<std::string>());
					}
				}

				//Add the defined requiremnets.
				if (system_entry.contains("DefinedRequirements"))
				{
					for (const nlohmann::json &defined_requirement : system_entry["DefinedRequirements"])
					{
						new_system_data._DefinedRequirements.emplace_back(defined_requirement.get<std::string>());
					}
				}
			}
		}
	}

	file << std::endl;

	//Add the macros.
	file << "//Macros." << std::endl;

	{
		constexpr uint64 ALIGNMENT{ 8 };

		file << "#define ALIGNED_SIZE_OF(X) ((sizeof(X) + " << (ALIGNMENT - 1) << ") & ~(" << (ALIGNMENT - 1) << "))" << std::endl;
	}

	file << std::endl;

	//Add the static variable definitions for all systems.
	file << "//Static variable definitions." << std::endl;

	for (const SystemData &_system_data : system_data)
	{
		for (const std::string &not_defined_requirement : _system_data._NotDefinedRequirements)
		{
			file << "#if !defined(" << not_defined_requirement.c_str() << ")" << std::endl;
		}

		for (const std::string &defined_requirement : _system_data._DefinedRequirements)
		{
			file << "#if defined(" << defined_requirement.c_str() << ")" << std::endl;
		}

		file << _system_data._Name.c_str() << " *RESTRICT " << _system_data._Name.c_str() << "::Instance = nullptr;" << std::endl;

		for (size_t i{ 0 }; i < (_system_data._NotDefinedRequirements.size() + _system_data._DefinedRequirements.size()); ++i)
		{
			file << "#endif" << std::endl;
		}
	}

	file << std::endl;

	//Add a constant for all systems size.
	file << "constexpr uint64 SYSTEMS_SIZE" << std::endl;
	file << "{" << std::endl;

	for (size_t i{ 0 }; i < system_data.size(); ++i)
	{
		for (const std::string &not_defined_requirement : system_data[i]._NotDefinedRequirements)
		{
			file << "#if !defined(" << not_defined_requirement.c_str() << ")" << std::endl;
		}

		for (const std::string &defined_requirement : system_data[i]._DefinedRequirements)
		{
			file << "#if defined(" << defined_requirement.c_str() << ")" << std::endl;
		}

		file << "\t";

		if (i > 0)
		{
			file << "+ ";
		}

		file << "ALIGNED_SIZE_OF(" << system_data[i]._Name.c_str() << ")" << std::endl;

		for (size_t j{ 0 }; j < (system_data[i]._NotDefinedRequirements.size() + system_data[i]._DefinedRequirements.size()); ++j)
		{
			file << "#endif" << std::endl;
		}
	}

	file << "};" << std::endl;

	file << std::endl;

	//Add the "SystemsAllocator" class.
	file << "class SystemsAllocator final" << std::endl;
	file << "{" << std::endl;
	file << "public:" << std::endl;

	file << "\tFORCE_INLINE SystemsAllocator() NOEXCEPT" << std::endl;
	file << "\t{" << std::endl;

	file << "\t\t_Memory = Memory::Allocate(SYSTEMS_SIZE);" << std::endl;
	file << "\t\tbyte *RESTRICT cursor{ static_cast<byte *RESTRICT>(_Memory) };" << std::endl;

	for (const SystemData &_system_data : system_data)
	{
		for (const std::string &not_defined_requirement : _system_data._NotDefinedRequirements)
		{
			file << "#if !defined(" << not_defined_requirement.c_str() << ")" << std::endl;
		}

		for (const std::string &defined_requirement : _system_data._DefinedRequirements)
		{
			file << "#if defined(" << defined_requirement.c_str() << ")" << std::endl;
		}

		file << "\t\t" << _system_data._Name.c_str() << "::Instance = new (cursor) " << _system_data._Name.c_str() << "();" << std::endl;
		file << "\t\tcursor += ALIGNED_SIZE_OF(" << _system_data._Name.c_str() << ");" << std::endl;

		for (size_t i{ 0 }; i < (_system_data._NotDefinedRequirements.size() + _system_data._DefinedRequirements.size()); ++i)
		{
			file << "#endif" << std::endl;
		}
	}

	file << "\t}" << std::endl;

	file << "\tFORCE_INLINE ~SystemsAllocator() NOEXCEPT" << std::endl;
	file << "\t{" << std::endl;

	for (const SystemData &_system_data : system_data)
	{
		for (const std::string &not_defined_requirement : _system_data._NotDefinedRequirements)
		{
			file << "#if !defined(" << not_defined_requirement.c_str() << ")" << std::endl;
		}

		for (const std::string &defined_requirement : _system_data._DefinedRequirements)
		{
			file << "#if defined(" << defined_requirement.c_str() << ")" << std::endl;
		}

		file << "\t\t" << _system_data._Name.c_str() << "::Instance->~" << _system_data._Name.c_str() << "();" << std::endl;
		file << "\t\t" << _system_data._Name.c_str() << "::Instance = nullptr;" << std::endl;

		for (size_t i{ 0 }; i < (_system_data._NotDefinedRequirements.size() + _system_data._DefinedRequirements.size()); ++i)
		{
			file << "#endif" << std::endl;
		}
	}

	file << "\t\tMemory::Free(_Memory);" << std::endl;

	file << "\t}" << std::endl;

	file << "private:" << std::endl;
	
	file << "\tvoid *RESTRICT _Memory;" << std::endl;

	file << "};" << std::endl;

	file << std::endl;

	file << "SystemsAllocator SYSTEMS_ALLOCATOR;" << std::endl;

	file << std::endl;

	//Add the "Systems::Initialize()" function.
	{
		file << "void Systems::Initialize() NOEXCEPT" << std::endl;
		file << "{" << std::endl;

		for (const SystemData &_system_data : system_data)
		{
			if (_system_data._Initialize)
			{
				for (const std::string &not_defined_requirement : _system_data._NotDefinedRequirements)
				{
					file << "#if !defined(" << not_defined_requirement.c_str() << ")" << std::endl;
				}

				for (const std::string &defined_requirement : _system_data._DefinedRequirements)
				{
					file << "#if defined(" << defined_requirement.c_str() << ")" << std::endl;
				}

				file << "\t" << _system_data._Name.c_str() << "::Instance->Initialize();" << std::endl;

				for (size_t i{ 0 }; i < (_system_data._NotDefinedRequirements.size() + _system_data._DefinedRequirements.size()); ++i)
				{
					file << "#endif" << std::endl;
				}
			}
		}

		file << "}" << std::endl;

		file << std::endl;
	}

	//Add the "Systems::PostInitialize()" function.
	{
		file << "void Systems::PostInitialize() NOEXCEPT" << std::endl;
		file << "{" << std::endl;

		for (const SystemData &_system_data : system_data)
		{
			if (_system_data._PostInitialize)
			{
				for (const std::string &not_defined_requirement : _system_data._NotDefinedRequirements)
				{
					file << "#if !defined(" << not_defined_requirement.c_str() << ")" << std::endl;
				}

				for (const std::string &defined_requirement : _system_data._DefinedRequirements)
				{
					file << "#if defined(" << defined_requirement.c_str() << ")" << std::endl;
				}

				file << "\t" << _system_data._Name.c_str() << "::Instance->PostInitialize();" << std::endl;

				for (size_t i{ 0 }; i < (_system_data._NotDefinedRequirements.size() + _system_data._DefinedRequirements.size()); ++i)
				{
					file << "#endif" << std::endl;
				}
			}
		}

		file << "}" << std::endl;

		file << std::endl;
	}

	//Add the "Systems::Terminate()" function.
	{
		file << "void Systems::Terminate() NOEXCEPT" << std::endl;
		file << "{" << std::endl;

		for (const SystemData &_system_data : system_data)
		{
			if (_system_data._Terminate)
			{
				for (const std::string &not_defined_requirement : _system_data._NotDefinedRequirements)
				{
					file << "#if !defined(" << not_defined_requirement.c_str() << ")" << std::endl;
				}

				for (const std::string &defined_requirement : _system_data._DefinedRequirements)
				{
					file << "#if defined(" << defined_requirement.c_str() << ")" << std::endl;
				}

				file << "\t" << _system_data._Name.c_str() << "::Instance->Terminate();" << std::endl;

				for (size_t i{ 0 }; i < (_system_data._NotDefinedRequirements.size() + _system_data._DefinedRequirements.size()); ++i)
				{
					file << "#endif" << std::endl;
				}
			}
		}

		file << "}" << std::endl;

		file << std::endl;
	}

	//Add the "Systems::PostTerminate()" function.
	{
		file << "void Systems::PostTerminate() NOEXCEPT" << std::endl;
		file << "{" << std::endl;

		for (const SystemData &_system_data : system_data)
		{
			if (_system_data._PostTerminate)
			{
				for (const std::string &not_defined_requirement : _system_data._NotDefinedRequirements)
				{
					file << "#if !defined(" << not_defined_requirement.c_str() << ")" << std::endl;
				}

				for (const std::string &defined_requirement : _system_data._DefinedRequirements)
				{
					file << "#if defined(" << defined_requirement.c_str() << ")" << std::endl;
				}

				file << "\t" << _system_data._Name.c_str() << "::Instance->PostTerminate();" << std::endl;

				for (size_t i{ 0 }; i < (_system_data._NotDefinedRequirements.size() + _system_data._DefinedRequirements.size()); ++i)
				{
					file << "#endif" << std::endl;
				}
			}
		}

		file << "}";
	}

	//Close the file.
	file.close();
}