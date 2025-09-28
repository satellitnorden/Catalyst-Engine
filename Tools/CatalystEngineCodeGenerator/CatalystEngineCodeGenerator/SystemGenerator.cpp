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

	//The not defined requirements.
	std::vector<std::string> _NotDefinedRequirements;

	//The defined requirements.
	std::vector<std::string> _DefinedRequirements;

	//Denotes whether or not this systems wants an 'Initialize()' call.
	bool _Initialize;

	//Denotes whether or not this systems wants a 'PostInitialize()' call.
	bool _PostInitialize;

	//The update ranges.
	std::vector<std::pair<std::string, std::string>> _UpdateRanges;

	//Denotes whether or not this systems wants a 'Terminate()' call.
	bool _Terminate;

	//Denotes whether or not this systems wants a 'PostTerminate()' call.
	bool _PostTerminate;

};

/*
*	Returns the update task name for the given system and it's update range.
*/
std::string GetUpdateTaskName(const SystemData &system_data, const std::pair<std::string, std::string> &update_range)
{
	return system_data._Name + update_range.first + std::string("To") + update_range.second + std::string("Task");
}

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

		//Check if this system wants has any requested updateranges.
		{
			size_t position{ current_line.find("SYSTEM_UPDATE(") };

			if (position != std::string::npos)
			{
				std::string _current_line{ current_line };

				position = _current_line.find("RANGE(");

				while (position != std::string::npos)
				{
					_current_line = _current_line.substr(position + strlen("RANGE("));

					const size_t first_phase_start_index{ 0 };
					const size_t first_phase_end_index{ _current_line.find_first_of(",") };
					const std::string start_phase{ _current_line.substr(first_phase_start_index, first_phase_end_index) };

					size_t second_phase_start_index{ first_phase_end_index + 1 };

					while (_current_line[second_phase_start_index] == ' ')
					{
						++second_phase_start_index;
					}

					const size_t second_phase_end_index{ _current_line.find_first_of(")") };

					const std::string end_phase{ _current_line.substr(second_phase_start_index, second_phase_end_index - second_phase_start_index) };

					nlohmann::json update_range_entry;

					update_range_entry["StartPhase"] = start_phase;
					update_range_entry["EndPhase"] = end_phase;

					system_entry["UpdateRanges"].emplace_back(update_range_entry);

					position = _current_line.find("RANGE(");
				}

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

	file << "//Profiling." << std::endl;
	file << "#include <Profiling/Profiling.h>" << std::endl;
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

				//Add the update ranges.
				if (system_entry.contains("UpdateRanges"))
				{
					for (const nlohmann::json &update_range_entry : system_entry["UpdateRanges"])
					{
						new_system_data._UpdateRanges.emplace_back(update_range_entry["StartPhase"].get<std::string>(), update_range_entry["EndPhase"].get<std::string>());
					}
				}

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

	//Gather the unique start and end update phases.
	std::vector<std::string> unique_start_update_phases;
	std::vector<std::string> unique_end_update_phases;

	for (const SystemData &_system_data : system_data)
	{
		for (const std::pair<std::string, std::string> &update_range : _system_data._UpdateRanges)
		{
			if (std::find(unique_start_update_phases.begin(), unique_start_update_phases.end(), update_range.first) == unique_start_update_phases.end())
			{
				unique_start_update_phases.emplace_back(update_range.first);
			}

			if (std::find(unique_end_update_phases.begin(), unique_end_update_phases.end(), update_range.second) == unique_end_update_phases.end())
			{
				unique_end_update_phases.emplace_back(update_range.second);
			}
		}
	}

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

	//Add all of the update tasks for all systems.
	for (const SystemData &_system_data : system_data)
	{
		for (const std::pair<std::string, std::string> &update_range : _system_data._UpdateRanges)
		{
			file << "Task " << GetUpdateTaskName(_system_data, update_range).c_str() << ";" << std::endl;
		}
	}

	file << std::endl;

	//Add the "SystemsInitializer" class.
	file << "class SystemsInitializer final" << std::endl;
	file << "{" << std::endl;
	file << "public:" << std::endl;

	file << "\tFORCE_INLINE SystemsInitializer() NOEXCEPT" << std::endl;
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

	file << std::endl;

	for (const SystemData &_system_data : system_data)
	{
		if (_system_data._UpdateRanges.empty())
		{
			continue;
		}

		for (const std::string &not_defined_requirement : _system_data._NotDefinedRequirements)
		{
			file << "#if !defined(" << not_defined_requirement.c_str() << ")" << std::endl;
		}

		for (const std::string &defined_requirement : _system_data._DefinedRequirements)
		{
			file << "#if defined(" << defined_requirement.c_str() << ")" << std::endl;
		}

		for (const SystemData &_system_data : system_data)
		{
			for (const std::pair<std::string, std::string>& update_range : _system_data._UpdateRanges)
			{
				const std::string update_string{ GetUpdateTaskName(_system_data, update_range) };

				file << "\t\t" << update_string.c_str() << "._Function = [](void *const RESTRICT /*arguments*/)" << std::endl;
				file << "\t\t{" << std::endl;
				file << "\t\t\t" << _system_data._Name.c_str() << "::Instance->Update(UpdatePhase::" << update_range.first.c_str() << ");" << std::endl;
				file << "\t\t};" << std::endl;
				file << "\t\t" << update_string.c_str() << "._Arguments = nullptr;" << std::endl;
				file << "\t\t" << update_string.c_str() << "._ExecutableOnSameThread = false;" << std::endl;
			}
		}

		for (size_t i{ 0 }; i < (_system_data._NotDefinedRequirements.size() + _system_data._DefinedRequirements.size()); ++i)
		{
			file << "#endif" << std::endl;
		}
	}

	file << "\t}" << std::endl;

	file << "\tFORCE_INLINE ~SystemsInitializer() NOEXCEPT" << std::endl;
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

	file << "SystemsInitializer SYSTEMS_INITIALIZER;" << std::endl;

	file << std::endl;

	//Add the "Systems::Initialize()" function.
	{
		file << "void Systems::Initialize() NOEXCEPT" << std::endl;
		file << "{" << std::endl;

		file << "\tPROFILING_SCOPE(\"Systems::Initialize()\");" << std::endl;
		file << std::endl;

#if 0
		file << "\tconstexpr uint64 NUMBER_OF_TASKS" << std::endl;
		file << "\t{" << std::endl;

		for (size_t system_data_index{ 0 }; system_data_index < system_data.size(); ++system_data_index)
		{
			const SystemData &_system_data{ system_data[system_data_index] };

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

				file << "\t\t+ 1 //" << _system_data._Name.c_str() << std::endl;

				for (size_t i{ 0 }; i < (_system_data._NotDefinedRequirements.size() + _system_data._DefinedRequirements.size()); ++i)
				{
					file << "#endif" << std::endl;
				}
			}
		}

		file << "\t};" << std::endl;

		file << std::endl;

		file << "\tStaticArray<Task, NUMBER_OF_TASKS> tasks;" << std::endl;
		file << "\tuint64 task_index{ 0 };" << std::endl;

		file << std::endl;

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

				file << "\ttasks[task_index]._Function = [](void *const RESTRICT /*arguments*/)" << std::endl;
				file << "\t{" << std::endl;
				file << "\t\t" << _system_data._Name.c_str() << "::Instance->Initialize();" << std::endl;
				file << "\t};" << std::endl;
				file << "\ttasks[task_index]._Arguments = nullptr;" << std::endl;
				file << "\ttasks[task_index]._ExecutableOnSameThread = false;" << std::endl;
				file << std::endl;
				file << "\tTaskSystem::Instance->ExecuteTask(Task::Priority::HIGH, &tasks[task_index]);" << std::endl;
				file << std::endl;
				file << "\t++task_index;" << std::endl;
				file << std::endl;

				for (size_t i{ 0 }; i < (_system_data._NotDefinedRequirements.size() + _system_data._DefinedRequirements.size()); ++i)
				{
					file << "#endif" << std::endl;
				}
			}
		}

		file << "\tbool all_tasks_finished{ false };" << std::endl;
		file << std::endl;

		file << "\twhile (!all_tasks_finished)" << std::endl;
		file << "\t{" << std::endl;

		file << "\t\tTaskSystem::Instance->DoWork(Task::Priority::HIGH);" << std::endl;
		file << std::endl;

		file << "\t\tall_tasks_finished = true;" << std::endl;
		file << std::endl;

		file << "\t\tfor (uint64 task_index{ 0 }; task_index < NUMBER_OF_TASKS; ++task_index)" << std::endl;
		file << "\t\t{" << std::endl;
		file << "\t\t\tall_tasks_finished &= tasks[task_index].IsExecuted();" << std::endl;
		file << "\t\t}" << std::endl;

		file << "\t}" << std::endl;
#else
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
#endif

		file << "}" << std::endl;

		file << std::endl;
	}

	//Add the "Systems::PostInitialize()" function.
	{
		file << "void Systems::PostInitialize() NOEXCEPT" << std::endl;
		file << "{" << std::endl;

		file << "\tPROFILING_SCOPE(\"Systems::PostInitialize()\");" << std::endl;
		file << std::endl;

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

	//Add the "Systems::Update()" function.
	{
		file << "void Systems::Update(const UpdatePhase phase) NOEXCEPT" << std::endl;
		file << "{" << std::endl;

		file << "\tswitch (phase)" << std::endl;
		file << "\t{" << std::endl;

		for (const std::string unique_end_update_phase : unique_end_update_phases)
		{
			file << "\t\tcase UpdatePhase::" << unique_end_update_phase.c_str() << ":" << std::endl;
			file << "\t\t{" << std::endl;

			file << "\t\t\tbool all_tasks_finished;" << std::endl;
			file << std::endl;

			file << "\t\t\tdo" << std::endl;
			file << "\t\t\t{" << std::endl;
			file << "\t\t\t\tall_tasks_finished = true;" << std::endl;
			file << std::endl;

			for (const SystemData &_system_data : system_data)
			{
				for (const std::pair<std::string, std::string> &update_range : _system_data._UpdateRanges)
				{
					if (update_range.second == unique_end_update_phase)
					{
						for (const std::string& not_defined_requirement : _system_data._NotDefinedRequirements)
						{
							file << "#if !defined(" << not_defined_requirement.c_str() << ")" << std::endl;
						}

						for (const std::string& defined_requirement : _system_data._DefinedRequirements)
						{
							file << "#if defined(" << defined_requirement.c_str() << ")" << std::endl;
						}

						file << "\t\t\t\tall_tasks_finished &= " << GetUpdateTaskName(_system_data, update_range).c_str() << ".IsExecuted();" << std::endl;

						for (size_t i{ 0 }; i < (_system_data._NotDefinedRequirements.size() + _system_data._DefinedRequirements.size()); ++i)
						{
							file << "#endif" << std::endl;
						}
					}
				}
			}

			file << "\t\t\t\tif (!all_tasks_finished) TaskSystem::Instance->DoWork(Task::Priority::HIGH);" << std::endl;

			file << "\t\t\t} while (!all_tasks_finished);" << std::endl;
			file << std::endl;

			file << "\t\t\tbreak;" << std::endl;
			file << "\t\t}" << std::endl;
		}

		file << "\t}" << std::endl;

		file << std::endl;

		file << "\tswitch (phase)" << std::endl;
		file << "\t{" << std::endl;

		for (const std::string unique_start_update_phase : unique_start_update_phases)
		{
			file << "\t\tcase UpdatePhase::" << unique_start_update_phase.c_str() << ":" << std::endl;
			file << "\t\t{" << std::endl;

			for (const SystemData &_system_data : system_data)
			{
				for (const std::pair<std::string, std::string>& update_range : _system_data._UpdateRanges)
				{
					if (update_range.first == unique_start_update_phase)
					{
						for (const std::string &not_defined_requirement : _system_data._NotDefinedRequirements)
						{
							file << "#if !defined(" << not_defined_requirement.c_str() << ")" << std::endl;
						}

						for (const std::string &defined_requirement : _system_data._DefinedRequirements)
						{
							file << "#if defined(" << defined_requirement.c_str() << ")" << std::endl;
						}

						file << "\t\t\tTaskSystem::Instance->ExecuteTask(Task::Priority::HIGH, &" << GetUpdateTaskName(_system_data, update_range).c_str() << ");" << std::endl;

						for (size_t i{ 0 }; i < (_system_data._NotDefinedRequirements.size() + _system_data._DefinedRequirements.size()); ++i)
						{
							file << "#endif" << std::endl;
						}
					}
				}
			}

			file << std::endl;

			file << "\t\t\tbreak;" << std::endl;
			file << "\t\t}" << std::endl;
		}

		file << "\t}" << std::endl;

		file << "}" << std::endl;

		file << std::endl;
	}

	//Add the "Systems::Terminate()" function.
	{
		file << "void Systems::Terminate() NOEXCEPT" << std::endl;
		file << "{" << std::endl;

		file << "\tPROFILING_SCOPE(\"Systems::Terminate()\");" << std::endl;
		file << std::endl;

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

		file << "\tPROFILING_SCOPE(\"Systems::PostTerminate()\");" << std::endl;
		file << std::endl;

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