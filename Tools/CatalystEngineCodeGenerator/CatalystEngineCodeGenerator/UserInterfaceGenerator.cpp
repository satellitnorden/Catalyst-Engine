//Header file.
#include "UserInterfaceGenerator.h"

//Catalyst code generator.
#include "TextParsing.h"

//Constants.
#define ENGINE_CODE_INCLUDE_DIRECTORY "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Include"
#define ENGINE_CODE_SOURCE_DIRECTORY "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Source"
#define GAME_CODE_INCLUDE_DIRECTORY "..\\..\\..\\Code\\Include"
#define GAME_CODE_SOURCE_DIRECTORY "..\\..\\..\\Code\\Source"

#define HEADER_FILE_PATH "..\\..\\..\\Code\\Include\\Generated\\UserInterface.Generated.h"
#define SOURCE_FILE_PATH "..\\..\\..\\Code\\Source\\Generated\\UserInterface.Generated.cpp"

/*
*	User interface scene data class definition.
*/
class UserInterfaceSceneData final
{

public:

	//The name.
	std::string _Name;

};

/*
*	Runs this user interface generator.
*/
void UserInterfaceGenerator::Run()
{
	//Create the "CodeGeneration" directory, if it doesn't exist.
	std::filesystem::create_directory("..\\..\\..\\Code\\CodeGeneration");

	//Set up the JSON object.
	nlohmann::json JSON;

	//Read the cache, if it exists.
//#if defined(NDEBUG)
	if (std::filesystem::exists("..\\..\\..\\Code\\CodeGeneration\\UserInterfaceCache.json"))
	{
		std::ifstream input_file{ "..\\..\\..\\Code\\CodeGeneration\\UserInterfaceCache.json" };
		input_file >> JSON;
		input_file.close();
	}
//#endif

	//Gather user interface scenes!
	bool new_files_parsed{ false };

	new_files_parsed |= GatherUserInterfaceScenes(ENGINE_CODE_INCLUDE_DIRECTORY, &JSON);
	new_files_parsed |= GatherUserInterfaceScenes(ENGINE_CODE_SOURCE_DIRECTORY, &JSON);
	new_files_parsed |= GatherUserInterfaceScenes(GAME_CODE_INCLUDE_DIRECTORY, &JSON);
	new_files_parsed |= GatherUserInterfaceScenes(GAME_CODE_SOURCE_DIRECTORY, &JSON);

	//Generate the files.
	if (new_files_parsed || !std::filesystem::exists(HEADER_FILE_PATH) || !std::filesystem::exists(SOURCE_FILE_PATH))
	{
		GenerateFiles(JSON);
	}

	//Write the JSON to file.
	std::ofstream file{ "..\\..\\..\\Code\\CodeGeneration\\UserInterfaceCache.json" };
	file << std::setw(4) << JSON;
	file.close();
}

/*
*	Gathers user interface scenes in the given directory.
*/
bool UserInterfaceGenerator::GatherUserInterfaceScenes(const char *const directory_path, nlohmann::json *const JSON)
{
	//Remember if new files were parsed.
	bool new_files_parsed{ false };

	//Iterate over all files.
	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		//Gather recursively if this is a directory.
		if (entry.is_directory())
		{
			new_files_parsed |= GatherUserInterfaceScenes(entry.path().string().c_str(), JSON);

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
					const size_t macro_position{ current_line.find("CATALYST_USER_INTERFACE_SCENE") };

					if (define_position == std::string::npos
						&& macro_position != std::string::npos
						&& !OnlyComment(current_line.c_str(), current_line.length()))
					{
						nlohmann::json &systems_entry{ entry["UserInterfaceScenes"] };

						ParseUserInterfaceScene(file, current_line, &systems_entry);

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
*	Parses a user interface scene.
*/
void UserInterfaceGenerator::ParseUserInterfaceScene(std::ifstream &file, std::string &current_line, nlohmann::json *const JSON)
{
	//Retrieve the name.
	std::string name;

	while (name.empty())
	{
		//Retrieve the line.
		std::getline(file, current_line);

		char character;
		size_t position{ 0 };
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
	nlohmann::json &user_interface_scene_entry{ (*JSON)[name.c_str()] };

	//Set the name.
	user_interface_scene_entry["Name"] = name.c_str();

	while (std::getline(file, current_line))
	{
		//Stop if we've reached the end.
		if (current_line == ");")
		{
			break;
		}
	}
}

/*
*	Generates the files.
*/
void UserInterfaceGenerator::GenerateFiles(const nlohmann::json &JSON)
{
	//Figure out the directory paths.
	char include_directory_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(include_directory_path, "..\\..\\..\\Code\\Include\\Generated");

	char source_directory_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(source_directory_path, "..\\..\\..\\Code\\Source\\Generated");

	//Create the directories, if it doesn't exist.
	std::filesystem::create_directory(include_directory_path);
	std::filesystem::create_directory(source_directory_path);

	//Figure out the header file path.
	char header_file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(header_file_path, "%s\\UserInterface.Generated.h", include_directory_path);

	//Generate the header file.
	{
		//Open the file.
		std::ofstream file{ header_file_path };

		//Add header stuff.
		file << "#pragma once" << std::endl;
		file << std::endl;

		file << "//Core." << std::endl;
		file << "#include <Core/Essential/CatalystEssential.h>" << std::endl;
		file << "#include <Core/General/Enumeration.h>" << std::endl;
		file << std::endl;

		file << "CATALYST_ENUMERATION" << std::endl;
		file << "(" << std::endl;
		file << "\tUserInterfaceSceneIdentifier," << std::endl;
		file << "\tuint8," << std::endl;
		file << "\tNone," << std::endl;

		for (auto file_iterator{ JSON.begin() }; file_iterator != JSON.end(); ++file_iterator)
		{
			const nlohmann::json &file_entry{ *file_iterator };

			if (!file_entry.contains("UserInterfaceScenes"))
			{
				continue;
			}

			const nlohmann::json &user_interface_scenes_entry{ file_entry["UserInterfaceScenes"] };

			for (auto user_interface_scene_iterator{ user_interface_scenes_entry.begin() }; user_interface_scene_iterator != user_interface_scenes_entry.end(); ++user_interface_scene_iterator)
			{
				//Cache the system entry.
				const nlohmann::json &user_interface_scene_entry{ *user_interface_scene_iterator };

				//Add the identifier!
				file << "\t" << user_interface_scene_iterator.key().c_str() << "," << std::endl;
			}
		}

		file << ");";

		//Close the file.
		file.close();
	}

	//Figure out the source file path.
	char source_file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(source_file_path, "%s\\UserInterface.Generated.cpp", source_directory_path);

	//Generate the source file.
	{
		//Open the file.
		std::ofstream file{ source_file_path };

		//Add header stuff.
		file << "//Header file." << std::endl;
		file << "#include <UserInterface/UserInterfaceSceneFactory.h>" << std::endl;
		file << std::endl;

		file << "//Memory." << std::endl;
		file << "#include <Memory/PoolAllocator.h>" << std::endl;
		file << std::endl;

		//Iterate over all files and add includes and user interface scene data.
		file << "//User interface." << std::endl;

		std::vector<UserInterfaceSceneData> user_interface_scene_data;

		for (auto file_iterator{ JSON.begin() }; file_iterator != JSON.end(); ++file_iterator)
		{
			const nlohmann::json &file_entry{ *file_iterator };

			if (!file_entry.contains("UserInterfaceScenes"))
			{
				continue;
			}

			//Add the include.
			file << "#include \"" << file_iterator.key().c_str() << "\"" << std::endl;

			const nlohmann::json &user_interface_scenes_entry{ file_entry["UserInterfaceScenes"] };

			for (auto user_interface_scene_iterator{ user_interface_scenes_entry.begin() }; user_interface_scene_iterator != user_interface_scenes_entry.end(); ++user_interface_scene_iterator)
			{
				//Cache the system entry.
				const nlohmann::json &user_interface_scene_entry{ *user_interface_scene_iterator };

				//Add the new user interface scene data.
				UserInterfaceSceneData &_user_interface_scene_data{ user_interface_scene_data.emplace_back() };

				//Set the name.
				_user_interface_scene_data._Name = user_interface_scene_iterator.key().c_str();
			}
		}

		file << std::endl;

		//Add all allocators.
		file << "//Allocators." << std::endl;

		for (const UserInterfaceSceneData &_user_interface_scene_data : user_interface_scene_data)
		{
			file << "PoolAllocator<sizeof(" << _user_interface_scene_data._Name.c_str() << ")> _" << _user_interface_scene_data._Name.c_str() << "Allocator;" << std::endl;
		}

		file << std::endl;

		//Add all locks.
		file << "//Locks." << std::endl;

		for (const UserInterfaceSceneData& _user_interface_scene_data : user_interface_scene_data)
		{
			file << "Spinlock _" << _user_interface_scene_data._Name.c_str() << "Lock;" << std::endl;
		}

		file << std::endl;

		file << "namespace UserInterfaceSceneFactory" << std::endl;
		file << "{" << std::endl;

		//Add the "Create()" function.
		file << "\tNO_DISCARD UserInterfaceScene *const RESTRICT Create(const UserInterfaceSceneIdentifier identifier) NOEXCEPT" << std::endl;
		file << "\t{" << std::endl;
		file << "\t\tswitch (identifier)" << std::endl;
		file << "\t\t{" << std::endl;

		for (const UserInterfaceSceneData& _user_interface_scene_data : user_interface_scene_data)
		{
			file << "\t\t\tcase UserInterfaceSceneIdentifier::" << _user_interface_scene_data._Name.c_str() << ":" << std::endl;
			file << "\t\t\t{" << std::endl;
			file << "\t\t\t\t_" << _user_interface_scene_data._Name.c_str() << "Lock.Lock();" << std::endl;
			file << "\t\t\t\tvoid *const RESTRICT memory{ _" << _user_interface_scene_data._Name.c_str() << "Allocator.Allocate() };" << std::endl;
			file << "\t\t\t\t_" << _user_interface_scene_data._Name.c_str() << "Lock.Unlock();" << std::endl;
			file << "\t\t\t\treturn new (memory) " << _user_interface_scene_data._Name.c_str() << "();" << std::endl;
			file << "\t\t\t}" << std::endl;
		}

		file << "\t\t\tdefault:" << std::endl;
		file << "\t\t\t{" << std::endl;
		file << "\t\t\t\tASSERT(false, \"Invalid identifier!\");" << std::endl;
		file << "\t\t\t\treturn nullptr;" << std::endl;
		file << "\t\t\t}" << std::endl;

		file << "\t\t}" << std::endl;
		file << "\t}" << std::endl;

		//Add the "Destroy()" function.
		file << "\tvoid Destroy(const UserInterfaceSceneIdentifier identifier, UserInterfaceScene *const RESTRICT user_interface_scene) NOEXCEPT" << std::endl;
		file << "\t{" << std::endl;
		file << "\t\tswitch (identifier)" << std::endl;
		file << "\t\t{" << std::endl;

		for (const UserInterfaceSceneData &_user_interface_scene_data : user_interface_scene_data)
		{
			file << "\t\t\tcase UserInterfaceSceneIdentifier::" << _user_interface_scene_data._Name.c_str() << ":" << std::endl;
			file << "\t\t\t{" << std::endl;
			file << "\t\t\t\tstatic_cast<" << _user_interface_scene_data._Name.c_str() << " *const RESTRICT>(user_interface_scene)->~" << _user_interface_scene_data._Name.c_str() << "();" << std::endl;
			file << "\t\t\t\t_" << _user_interface_scene_data._Name.c_str() << "Lock.Lock();" << std::endl;
			file << "\t\t\t\t_" << _user_interface_scene_data._Name.c_str() << "Allocator.Free(user_interface_scene);" << std::endl;
			file << "\t\t\t\t_" << _user_interface_scene_data._Name.c_str() << "Lock.Unlock();" << std::endl;
			file << "\t\t\t\tbreak;" << std::endl;
			file << "\t\t\t}" << std::endl;
		}

		file << "\t\t\tdefault:" << std::endl;
		file << "\t\t\t{" << std::endl;
		file << "\t\t\t\tASSERT(false, \"Invalid identifier!\");" << std::endl;
		file << "\t\t\t\tbreak;" << std::endl;
		file << "\t\t\t}" << std::endl;

		file << "\t\t}" << std::endl;
		file << "\t}" << std::endl;

		file << "}";

		//Close the file.
		file.close();
	}

#if 0
	//Figure out the source file path.
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

		file << _system_data._Name.c_str() << " *RESTRICT " << _system_data._Name.c_str() << "::Instance;" << std::endl;

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

		file << "sizeof(" << system_data[i]._Name.c_str() << ")" << std::endl;

		for (size_t j{ 0 }; j < (system_data[i]._NotDefinedRequirements.size() + system_data[i]._DefinedRequirements.size()); ++j)
		{
			file << "#endif" << std::endl;
		}
	}

	file << "};" << std::endl;

	file << std::endl;

	//Add the "SystemsManager" class.
	file << "class SystemsManager final" << std::endl;
	file << "{" << std::endl;
	file << "public:" << std::endl;

	file << "\tFORCE_INLINE SystemsManager() NOEXCEPT" << std::endl;
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
		file << "\t\tcursor += sizeof(" << _system_data._Name.c_str() << ");" << std::endl;

		for (size_t i{ 0 }; i < (_system_data._NotDefinedRequirements.size() + _system_data._DefinedRequirements.size()); ++i)
		{
			file << "#endif" << std::endl;
		}
	}

	file << "\t}" << std::endl;

	file << "\tFORCE_INLINE ~SystemsManager() NOEXCEPT" << std::endl;
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

	file << "SystemsManager SYSTEMS_MANAGER;";

	/*
	//Try to satisfy befores/afters for all updates.
	const auto BEFORE_AFTER_SORT
	{
		[](std::map<std::string, std::vector<ComponentUpdate>> &updates)
		{
			for (std::pair<const std::string, std::vector<ComponentUpdate>> &update : updates)
			{
				for (ComponentUpdate &_update : update.second)
				{
					//Try to satisfy any befores.
					for (const std::string &before : _update._Befores)
					{
						for (ComponentUpdate &sub_update : update.second)
						{
							if (before == sub_update._ComponentName)
							{
								if (_update._SortValue >= sub_update._SortValue)
								{
									_update._SortValue = sub_update._SortValue - 1;
								}

								break;
							}
						}
					}

					//Try to satisfy any afters.
					for (const std::string &after : _update._Afters)
					{
						for (ComponentUpdate &sub_update : update.second)
						{
							if (after == sub_update._ComponentName)
							{
								if (_update._SortValue <= sub_update._SortValue)
								{
									_update._SortValue = sub_update._SortValue + 1;
								}

								break;
							}
						}
					}
				}

				//Sort the list now based on the sort value.
				std::sort
				(
					update.second.begin(),
					update.second.end(),
					[](const ComponentUpdate &A, const ComponentUpdate &B)
					{
						return A._SortValue < B._SortValue;
					}
				);
			}
		}
	};

	BEFORE_AFTER_SORT(serial_updates);
	BEFORE_AFTER_SORT(parallel_batch_updates);
	BEFORE_AFTER_SORT(parallel_sub_instance_updates);
	BEFORE_AFTER_SORT(post_updates);

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

	file << "}" << std::endl;
	file << std::endl;

	//Set up the "Components::Initialize()" function.
	file << "void Components::Initialize() NOEXCEPT" << std::endl;
	file << "{" << std::endl;

	for (uint64 i{ 0 }; i < component_data.size(); ++i)
	{
		if (component_data[i]._Initialize)
		{
			file << "\t" << component_data[i]._Name.c_str() << "::Instance->Initialize();" << std::endl;
		}
	}

	file << "}" << std::endl;
	file << std::endl;

	//Set up the "Components::PostInitialize()" function.
	file << "void Components::PostInitialize() NOEXCEPT" << std::endl;
	file << "{" << std::endl;

	for (uint64 i{ 0 }; i < component_data.size(); ++i)
	{
		if (component_data[i]._PostInitialize)
		{
			file << "\t" << component_data[i]._Name.c_str() << "::Instance->PostInitialize();" << std::endl;
		}
	}

	file << "}" << std::endl;
	file << std::endl;

	//Set up the "Components::Update()" function.
	file << "void Components::Update(const UpdatePhase update_phase) NOEXCEPT" << std::endl;
	file << "{" << std::endl;

	//Clear the parallel updates.
	file << "\t//Clear the parallel updates." << std::endl;
	file << "\tPARALLEL_UPDATES.Clear();" << std::endl;
	file << std::endl;

	//Fill up parallel batch updates.
	file << "\t//Fill up parallel batch updates." << std::endl;
	file << "\tswitch (update_phase)" << std::endl;
	file << "\t{" << std::endl;

	for (const std::pair<std::string, std::vector<ComponentUpdate>> &parallel_batch_update : parallel_batch_updates)
	{
		file << "\t\tcase " << parallel_batch_update.first.c_str() << ":" << std::endl;
		file << "\t\t{" << std::endl;

		for (const ComponentUpdate &update : parallel_batch_update.second)
		{
			file << "\t\t\t{" << std::endl;
			file << "\t\t\t\tconst uint64 number_of_instances{ " << update._ComponentName.c_str() << "::Instance->NumberOfInstances() };" << std::endl;
			file << "\t\t\t\tfor (uint64 batch_start_index{ 0 }; batch_start_index < number_of_instances; batch_start_index += " << update._BatchSize << ")" << std::endl;
			file << "\t\t\t\t{" << std::endl;
			file << "\t\t\t\t\tPARALLEL_UPDATES.Emplace();" << std::endl;
			file << "\t\t\t\t\tParallelUpdate &parallel_batch_update{ PARALLEL_UPDATES.Back() };" << std::endl;
			file << "\t\t\t\t\tparallel_batch_update._Task._Function = [](void *const RESTRICT arguments)" << std::endl;
			file << "\t\t\t\t\t{" << std::endl;
			file << "\t\t\t\t\t\tParallelUpdate *const RESTRICT parallel_batch_update{ static_cast<ParallelUpdate *const RESTRICT>(arguments) };" << std::endl;
			file << "\t\t\t\t\t\t" << update._ComponentName.c_str() << "::Instance->ParallelBatchUpdate(" << parallel_batch_update.first.c_str() << ", parallel_batch_update->_StartInstanceIndex, parallel_batch_update->_EndInstanceIndex);" << std::endl;
			file << "\t\t\t\t\t};" << std::endl;
			file << "\t\t\t\t\tparallel_batch_update._Task._Arguments = nullptr;" << std::endl;
			file << "\t\t\t\t\tparallel_batch_update._Task._ExecutableOnSameThread = false;" << std::endl;
			file << "\t\t\t\t\tparallel_batch_update._StartInstanceIndex = batch_start_index;" << std::endl;
			file << "\t\t\t\t\tparallel_batch_update._EndInstanceIndex = batch_start_index + BaseMath::Minimum<uint64>(" << update._BatchSize << ", number_of_instances - batch_start_index);" << std::endl;
			file << "\t\t\t\t}" << std::endl;
			file << "\t\t\t}" << std::endl;
		}

		file << "\t\t\tbreak;" << std::endl;
		file << "\t\t}" << std::endl;
	}

	file << "\t}" << std::endl;
	file << std::endl;

	//Fill up parallel sub instance updates.
	file << "\t//Fill up parallel sub instance updates." << std::endl;
	file << "\tswitch (update_phase)" << std::endl;
	file << "\t{" << std::endl;

	for (const std::pair<std::string, std::vector<ComponentUpdate>> &parallel_sub_instance_update : parallel_sub_instance_updates)
	{
		file << "\t\tcase " << parallel_sub_instance_update.first.c_str() << ":" << std::endl;
		file << "\t\t{" << std::endl;

		for (const ComponentUpdate &update : parallel_sub_instance_update.second)
		{
			file << "\t\t\t{" << std::endl;
			file << "\t\t\t\tconst uint64 number_of_instances{ " << update._ComponentName.c_str() << "::Instance->NumberOfInstances() };" << std::endl;
			file << "\t\t\t\tfor (uint64 instance_index{ 0 }; instance_index < number_of_instances; ++instance_index)" << std::endl;
			file << "\t\t\t\t{" << std::endl;
			file << "\t\t\t\t\tconst uint64 number_of_sub_instances{ " << update._ComponentName.c_str() << "::Instance->NumberOfSubInstances(instance_index) };" << std::endl;
			file << "\t\t\t\t\tfor (uint64 sub_instance_index{ 0 }; sub_instance_index < number_of_sub_instances; ++sub_instance_index)" << std::endl;
			file << "\t\t\t\t\t{" << std::endl;
			file << "\t\t\t\t\t\tPARALLEL_UPDATES.Emplace();" << std::endl;
			file << "\t\t\t\t\t\tParallelUpdate &parallel_sub_instance_update{ PARALLEL_UPDATES.Back() };" << std::endl;
			file << "\t\t\t\t\t\tparallel_sub_instance_update._Task._Function = [](void *const RESTRICT arguments)" << std::endl;
			file << "\t\t\t\t\t\t{" << std::endl;
			file << "\t\t\t\t\t\t\tParallelUpdate *const RESTRICT parallel_sub_instance_update{ static_cast<ParallelUpdate *const RESTRICT>(arguments) };" << std::endl;
			file << "\t\t\t\t\t\t\t" << update._ComponentName.c_str() << "::Instance->ParallelSubInstanceUpdate(" << parallel_sub_instance_update.first.c_str() << ", parallel_sub_instance_update->_InstanceIndex, parallel_sub_instance_update->_SubInstanceIndex);" << std::endl;
			file << "\t\t\t\t\t\t};" << std::endl;
			file << "\t\t\t\t\t\tparallel_sub_instance_update._Task._Arguments = nullptr;" << std::endl;
			file << "\t\t\t\t\t\tparallel_sub_instance_update._Task._ExecutableOnSameThread = false;" << std::endl;
			file << "\t\t\t\t\t\tparallel_sub_instance_update._InstanceIndex = instance_index;" << std::endl;
			file << "\t\t\t\t\t\tparallel_sub_instance_update._SubInstanceIndex = sub_instance_index;" << std::endl;
			file << "\t\t\t\t\t}" << std::endl;
			file << "\t\t\t\t}" << std::endl;
			file << "\t\t\t}" << std::endl;
		}

		file << "\t\t\tbreak;" << std::endl;
		file << "\t\t}" << std::endl;
	}

	file << "\t}" << std::endl;
	file << std::endl;

	//Execute all parallel updates.
	file << "\t//Execute all parallel updates." << std::endl;
	file << "\tfor (ParallelUpdate &parallel_update : PARALLEL_UPDATES)" << std::endl;
	file << "\t{" << std::endl;
	file << "\t\tparallel_update._Task._Arguments = &parallel_update;" << std::endl;
	file << "\t\tTaskSystem::Instance->ExecuteTask(Task::Priority::HIGH, &parallel_update._Task);" << std::endl;
	file << "\t}" << std::endl;
	file << std::endl;

	//Do serial updates.
	file << "\t//Do serial updates." << std::endl;
	file << "\tswitch (update_phase)" << std::endl;
	file << "\t{" << std::endl;

	for (const std::pair<std::string, std::vector<ComponentUpdate>> &serial_update : serial_updates)
	{
		file << "\t\tcase " << serial_update.first.c_str() << ":" << std::endl;
		file << "\t\t{" << std::endl;

		for (const ComponentUpdate &update : serial_update.second)
		{
			file << "\t\t\t" << update._ComponentName.c_str() << "::Instance->SerialUpdate(update_phase);" << std::endl;
		}

		file << "\t\t\tbreak;" << std::endl;
		file << "\t\t}" << std::endl;
	}

	file << "\t}" << std::endl;
	file << std::endl;

	//Wait for parallel batch updates to finish.
	file << "\t//Wait for parallel updates to finish." << std::endl;
	file << "\tbool all_done{ PARALLEL_UPDATES.Empty() };" << std::endl;
	file << "\twhile (!all_done)" << std::endl;
	file << "\t{" << std::endl;
	file << "\t\tTaskSystem::Instance->DoWork(Task::Priority::HIGH);" << std::endl;
	file << "\t\tall_done = true;" << std::endl;
	file << "\t\tfor (ParallelUpdate &parallel_update : PARALLEL_UPDATES)" << std::endl;
	file << "\t\t{" << std::endl;
	file << "\t\t\tall_done &= parallel_update._Task.IsExecuted();" << std::endl;
	file << "\t\t}" << std::endl;
	file << "\t}" << std::endl;
	file << std::endl;

	//Post-update components.
	file << "\t//Post-update components." << std::endl;

	file << "\tswitch (update_phase)" << std::endl;
	file << "\t{" << std::endl;

	for (const std::pair<std::string, std::vector<ComponentUpdate>> &post_update : post_updates)
	{
		file << "\t\tcase " << post_update.first.c_str() << ":" << std::endl;
		file << "\t\t{" << std::endl;

		for (const ComponentUpdate &update : post_update.second)
		{
			file << "\t\t\t" << update._ComponentName.c_str() << "::Instance->PostUpdate(update_phase);" << std::endl;
		}

		file << "\t\t\tbreak;" << std::endl;
		file << "\t\t}" << std::endl;
	}

	file << "\t}" << std::endl;

	file << "}" << std::endl;
	file << std::endl;

	//Set up the "Components::Terminate()" function.
	file << "void Components::Terminate() NOEXCEPT" << std::endl;
	file << "{" << std::endl;

	for (uint64 i{ 0 }; i < component_data.size(); ++i)
	{
		if (component_data[i]._Terminate)
		{
			file << "\t" << component_data[i]._Name.c_str() << "::Instance->Terminate();" << std::endl;
		}
	}

	file << "}";
	*/

	//Close the file.
	file.close();
#endif
}