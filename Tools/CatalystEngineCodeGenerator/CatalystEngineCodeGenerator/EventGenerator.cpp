//Header file.
#include "EventGenerator.h"

//Catalyst code generator.
#include "TextParsing.h"

//Constants.
#define ENGINE_CODE_INCLUDE_DIRECTORY "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Include"
#define ENGINE_CODE_SOURCE_DIRECTORY "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Source"
#define GAME_CODE_INCLUDE_DIRECTORY "..\\..\\..\\Code\\Include"
#define GAME_CODE_SOURCE_DIRECTORY "..\\..\\..\\Code\\Source"

/*
*	Runs this event generator.
*/
void EventGenerator::Run()
{
	//Create the "CodeGeneration" directory, if it doesn't exist.
	std::filesystem::create_directory("..\\..\\..\\Code\\CodeGeneration");

	//Set up the JSON object.
	nlohmann::json JSON;

	//Read the cache, if it exists.
#if defined(NDEBUG)
	if (std::filesystem::exists("..\\..\\..\\Code\\CodeGeneration\\EventCache.json"))
	{
		std::ifstream input_file{ "..\\..\\..\\Code\\CodeGeneration\\EventCache.json" };
		input_file >> JSON;
		input_file.close();
	}
#endif

	//Gather components!
	bool new_files_parsed{ false };

	new_files_parsed |= GatherEvents(ENGINE_CODE_INCLUDE_DIRECTORY, &JSON);
	new_files_parsed |= GatherEvents(GAME_CODE_INCLUDE_DIRECTORY, &JSON);

	//Generate the header and source file.
	if (new_files_parsed)
	{
		GenerateHeaderFile(JSON);
		GenerateSourceFile(JSON);
	}

	//Write the JSON to file.
	std::ofstream file{ "..\\..\\..\\Code\\CodeGeneration\\EventCache.json" };
	file << std::setw(4) << JSON;
	file.close();
}

/*
*	Gathers events in the given directory.
*/
bool EventGenerator::GatherEvents(const char *const directory_path, nlohmann::json *const JSON)
{
	//Remember if new files were parsed.
	bool new_files_parsed{ false };

	//Iterate over all files.
	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		//Gather recursively if this is a directory.
		if (entry.is_directory())
		{
			new_files_parsed |= GatherEvents(entry.path().string().c_str(), JSON);

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

		//Check if this is a C++ header file.
		if (extension != "h")
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
					const size_t macro_position_1{ current_line.find("CATALYST_FREE_EVENT") };
					const size_t macro_position_2{ current_line.find("CATALYST_SYSTEM_EVENT") };
					const size_t macro_position_3{ current_line.find("CATALYST_COMPONENT_EVENT") };

					if (define_position == std::string::npos
						&& (macro_position_1 != std::string::npos || macro_position_2 != std::string::npos || macro_position_3 != std::string::npos)
						&& !OnlyComment(current_line.c_str(), current_line.length()))
					{
						nlohmann::json &events_entry{ entry["Events"] };

						ParseEvent(file, current_line, &events_entry);

						//Let's just say, if it contains events, then new files were parsed.
						new_files_parsed = true;
					}
				}
			}
		}
	}

	return new_files_parsed;
}

/*
*	Parses an event.
*/
void EventGenerator::ParseEvent(std::ifstream &file, std::string &current_line, nlohmann::json *const JSON)
{
	//Track the current position.
	size_t current_position{ current_line.find_first_of('(') };

	if (current_position == std::string::npos)
	{
		ASSERT(false, "Malformed event!");
	}

	++current_position;

	//Retrieve the singleton name, if any.
	std::string singleton_name;

	
	{
		size_t last_character_position{ current_position };

		while (	current_line[last_character_position] != ' '
				&& current_line[last_character_position] != ','
				&& current_line[last_character_position] != ')')
		{
			++last_character_position;
		}

		singleton_name = current_line.substr(current_position, last_character_position - current_position);

		current_position = last_character_position;

		while (current_line[current_position] == ' ' || current_line[current_position] == ',') ++current_position;
	}

	//Retrieve the name.
	std::string name;

	{
		size_t last_character_position{ current_position };

		while (current_line[last_character_position] != ' '
			&& current_line[last_character_position] != ','
			&& current_line[last_character_position] != ')')
		{
			++last_character_position;
		}

		name = current_line.substr(current_position, last_character_position - current_position );

		current_position = last_character_position;

		while (current_line[current_position] == ' ' || current_line[current_position] == ',') ++current_position;
	}

	//Parse the argument types and names (if any.
	std::vector<std::string> argument_types;
	std::vector<std::string> argument_names;

	if (current_line[current_position] != ')')
	{
		while (true)
		{
			const std::string current_substring{ current_line.substr(current_position) };
			const size_t next_separator_position{ std::min(current_substring.find_first_of(','), current_substring.find_first_of(')')) };

			if (next_separator_position == std::string::npos)
			{
				ASSERT(false, "Malformed event!");
			}

			size_t name_start{ next_separator_position - 1 };

			while (	current_substring[name_start] != ' '
					&& current_substring[name_start] != '&')
			{
				--name_start;
			}

			++name_start;

			const std::string argument_type{ current_substring.substr(0, name_start) };
			const std::string argument_name{ current_substring.substr(name_start, next_separator_position - name_start) };

			argument_types.emplace_back(argument_type);
			argument_names.emplace_back(argument_name);

			if (current_substring[next_separator_position] == ')')
			{
				break;
			}

			else
			{
				current_position += next_separator_position + 1;

				while (current_line[current_position] == ' ') ++current_position;
			}
		}
	}

	//Add the entry for this event.
	nlohmann::json &event_entry{ (*JSON)[name.c_str()] };

	//Set the singleton name.
	event_entry["SingletonName"] = singleton_name;

	//Set the arguments.
	if (argument_types.size() > 0)
	{
		event_entry["ArgumentTypes"] = argument_types;
	}

	if (argument_names.size() > 0)
	{
		event_entry["ArgumentNames"] = argument_names;
	}
}

/*
*	Gathers unique events.
*/
void EventGenerator::GatherUniqueEvents(const nlohmann::json &JSON, std::vector<Event> *const unique_events)
{
	for (auto file_iterator{ JSON.begin() }; file_iterator != JSON.end(); ++file_iterator)
	{
		if (file_iterator->contains("Events"))
		{
			const nlohmann::json &events_entry{ (*file_iterator)["Events"] };

			for (auto event_iterator{ events_entry.begin() }; event_iterator != events_entry.end(); ++event_iterator)
			{
				Event event;

				event._Name = event_iterator.key();

				if (event_iterator->contains("ArgumentTypes"))
				{
					event._ArgumentTypes = (*event_iterator)["ArgumentTypes"].get<std::vector<std::string>>();
					event._ArgumentNames = (*event_iterator)["ArgumentNames"].get<std::vector<std::string>>();
				}
	
				EventReference event_reference;

				event_reference._File = file_iterator.key();

				event_reference._SingletonName = (*event_iterator)["SingletonName"].get<std::string>();

				const auto unique_event_iterator{ std::find(unique_events->begin(), unique_events->end(), event) };

				if (unique_event_iterator == unique_events->end())
				{
					event._References.emplace_back(event_reference);
					unique_events->emplace_back(std::move(event));
				}

				else
				{
					unique_event_iterator->_References.emplace_back(event_reference);
				}
			}
		}
	}
}

/*
*	Generates the header file.
*/
void EventGenerator::GenerateHeaderFile(const nlohmann::json &JSON)
{
	//Gather the unique event signatures.
	std::vector<Event> unique_events;
	GatherUniqueEvents(JSON, &unique_events);

	//Figure out the directory path.
	char directory_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(directory_path, "..\\..\\..\\Code\\Include\\Generated");

	//Create the directory, if it doesn't exist.
	std::filesystem::create_directory(directory_path);

	//Figure out the file path.
	char file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(file_path, "%s\\Event.Generated.h", directory_path);

	//Open the file.
	std::ofstream file{ file_path };

	//Add "#pragma once".
	file << "#pragma once" << std::endl;
	file << std::endl;

	//Add some default includes.
	file << "//World." << std::endl;
	file << "#include <World/Core/WorldTransform.h>" << std::endl;
	file << std::endl;

	//Add all of the unique event signatures.
	file << "//Events." << std::endl;

	for (size_t unique_event_index{ 0 }; unique_event_index < unique_events.size(); ++unique_event_index)
	{
		const Event &unique_event{ unique_events[unique_event_index] };

		file << "void Send" << unique_event._Name.c_str() << "(";

		if (unique_event._ArgumentTypes.size() > 0)
		{
			for (size_t i{ 0 }; i < unique_event._ArgumentTypes.size(); ++i)
			{
				file << unique_event._ArgumentTypes[i].c_str() << unique_event._ArgumentNames[i].c_str();

				if (i < (unique_event._ArgumentTypes.size() - 1))
				{
					file << ", ";
				}
			}
		}

		file << ");";

		if (unique_event_index < (unique_events.size() - 1))
		{
			file << std::endl;
		}
	}

	//Close the file.
	file.close();
}

/*
*	Generates the source file.
*/
void EventGenerator::GenerateSourceFile(const nlohmann::json &JSON)
{
	//Gather the unique events.
	std::vector<Event> unique_events;
	GatherUniqueEvents(JSON, &unique_events);

	//Figure out the directory path.
	char directory_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(directory_path, "..\\..\\..\\Code\\Source\\Generated");

	//Create the directory, if it doesn't exist.
	std::filesystem::create_directory(directory_path);

	//Figure out the file path.
	char file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(file_path, "%s\\Event.Generated.cpp", directory_path);

	//Open the file.
	std::ofstream file{ file_path };

	//Add the header include.
	file << "//Header file." << std::endl;
	file << "#include <Generated/Event.Generated.h>" << std::endl;
	file << std::endl;

	//Add all unique includes.
	{
		std::vector<std::string> unique_includes;

		for (const Event &unique_event : unique_events)
		{
			for (const EventReference &event_reference : unique_event._References)
			{
				if (std::find(unique_includes.begin(), unique_includes.end(), event_reference._File) == unique_includes.end())
				{
					unique_includes.emplace_back(event_reference._File);
				}
			}
		}

		file << "//Includes." << std::endl;

		for (const std::string &unique_include : unique_includes)
		{
			file << "#include \"" << unique_include.c_str() << "\"" << std::endl;
		}

		file << std::endl;
	}

	//Add all events.
	file << "//Events." << std::endl;

	for (size_t unique_event_index{ 0 }; unique_event_index < unique_events.size(); ++unique_event_index)
	{
		const Event &unique_event{ unique_events[unique_event_index] };

		file << "void Send" << unique_event._Name.c_str() << "(";

		if (unique_event._ArgumentTypes.size() > 0)
		{
			for (size_t i{ 0 }; i < unique_event._ArgumentTypes.size(); ++i)
			{
				file << unique_event._ArgumentTypes[i].c_str() << unique_event._ArgumentNames[i].c_str();

				if (i < (unique_event._ArgumentTypes.size() - 1))
				{
					file << ", ";
				}
			}
		}

		file << ")" << std::endl;
		file << "{" << std::endl;

		for (const EventReference &event_reference : unique_event._References)
		{
			file << "\t" << event_reference._SingletonName.c_str() << "::Instance->" << unique_event._Name.c_str() << "(";

			for (size_t i{ 0 }; i < unique_event._ArgumentNames.size(); ++i)
			{
				file << unique_event._ArgumentNames[i].c_str();

				if (i < (unique_event._ArgumentNames.size() - 1))
				{
					file << ", ";
				}
			}

			file << ");" << std::endl;
		}

		file << "}";

		if (unique_event_index < (unique_events.size() - 1))
		{
			file << std::endl;
		}
	}

	//Close the file.
	file.close();
}