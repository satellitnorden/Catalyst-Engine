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
*	Component update class definition.
*/
class ComponentUpdate final
{

public:

	//The component name.
	std::string _ComponentName;

	//The befores.
	std::vector<std::string> _Befores;

	//The afters.
	std::vector<std::string> _Afters;

	//The sort value.
	uint32 _SortValue{ UINT_MAX / 2 };

	//The batch size. Only relevant for parallel batch updates.
	uint64 _BatchSize;

};

/*
*	Component data class definition.
*/
class ComponentData final
{

public:

	//The name.
	std::string _Name;

	//Denotes whether or not this component wants an "Initialize()" call.
	bool _Initialize;

	//Denotes whether or not this component wants a "PostInitialize()" call.
	bool _PostInitialize;

	//Denotes whether or not this component wants a "Terminate()" call.
	bool _Terminate;

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
	std::ofstream file{ "..\\..\\..\\Code\\CodeGeneration\\ComponentCache.json" };
	file << std::setw(4) << JSON;
	file.close();
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
					const size_t macro_position_1{ current_line.find("DECLARE_COMPONENT") };
					const size_t macro_position_2{ current_line.find("CATALYST_COMPONENT") };

					if (define_position == std::string::npos
						&& (macro_position_1 != std::string::npos || macro_position_2 != std::string::npos)
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

		if (name.find("Component") == std::string::npos)
		{
			name += "Component";
		}
	}

	//Add the entry for this component.
	nlohmann::json &component_entry{ (*JSON)[name.c_str()] };

	//Set the name.
	component_entry["Name"] = name.c_str();

	//Also set up some defaults.
	component_entry["Initialize"] = false;
	component_entry["PostInitialize"] = false;
	component_entry["Terminate"] = false;

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

		//Check if this component wants an "Initialize()" call.
		{
			const size_t position{ current_line.find("COMPONENT_INITIALIZE(") };

			if (position != std::string::npos)
			{
				component_entry["Initialize"] = true;

				continue;
			}
		}

		//Check if this component wants an "PostInitialize()" call.
		{
			const size_t position{ current_line.find("COMPONENT_POST_INITIALIZE(") };

			if (position != std::string::npos)
			{
				component_entry["PostInitialize"] = true;

				continue;
			}
		}

		//Check if this component wants a serial update.
		{
			const size_t position{ current_line.find("COMPONENT_SERIAL_UPDATE(") };

			if (position != std::string::npos)
			{
				//Parse the function arguments.
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

				nlohmann::json &serial_updates_entry{ component_entry["SerialUpdates"] };
				nlohmann::json &serial_update_entry{ serial_updates_entry[arguments[0].c_str()] };

				//Check if there's any befores/afters.
				for (uint64 argument_index{ 1 }; argument_index < number_of_arguments; ++argument_index)
				{
					{
						const size_t position{ arguments[argument_index].find("Before(") };

						if (position != std::string::npos)
						{
							std::array<std::string, 1> sub_arguments;

							const uint64 number_of_sub_arguments
							{
								TextParsing::ParseFunctionArguments
								(
									arguments[argument_index].c_str(),
									arguments[argument_index].length(),
									sub_arguments.data()
								)
							};

							nlohmann::json &befores_entry{ serial_update_entry["Befores"] };
							nlohmann::json &before_entry{ befores_entry[sub_arguments[0].c_str()] };

							continue;
						}
					}

					{
						const size_t position{ arguments[argument_index].find("After(") };

						if (position != std::string::npos)
						{
							std::array<std::string, 1> sub_arguments;

							const uint64 number_of_sub_arguments
							{
								TextParsing::ParseFunctionArguments
								(
									arguments[argument_index].c_str(),
									arguments[argument_index].length(),
									sub_arguments.data()
								)
							};

							nlohmann::json &afters_entry{ serial_update_entry["Afters"] };
							nlohmann::json &after_entry{ afters_entry[sub_arguments[0].c_str()] };

							continue;
						}
					}
				}

				continue;
			}
		}

		//Check if this component wants a parallel batch update.
		{
			const size_t position{ current_line.find("COMPONENT_PARALLEL_BATCH_UPDATE(") };

			if (position != std::string::npos)
			{
				//Parse the function arguments.
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

				nlohmann::json &parallel_batch_updates_entry{ component_entry["ParallelBatchUpdates"] };
				nlohmann::json &parallel_batch_update_entry{ parallel_batch_updates_entry[arguments[0].c_str()] };

				//Write the batch size.
				const uint64 batch_size{ std::stoull(arguments[1].c_str()) };
				parallel_batch_update_entry["BatchSize"] = batch_size;

				//Check if there's any befores/afters.
				for (uint64 argument_index{ 1 }; argument_index < number_of_arguments; ++argument_index)
				{
					{
						const size_t position{ arguments[argument_index].find("Before(") };

						if (position != std::string::npos)
						{
							std::array<std::string, 1> sub_arguments;

							const uint64 number_of_sub_arguments
							{
								TextParsing::ParseFunctionArguments
								(
									arguments[argument_index].c_str(),
									arguments[argument_index].length(),
									sub_arguments.data()
								)
							};

							nlohmann::json &befores_entry{ parallel_batch_update_entry["Befores"] };
							nlohmann::json &before_entry{ befores_entry[sub_arguments[0].c_str()] };

							continue;
						}
					}

					{
						const size_t position{ arguments[argument_index].find("After(") };

						if (position != std::string::npos)
						{
							std::array<std::string, 1> sub_arguments;

							const uint64 number_of_sub_arguments
							{
								TextParsing::ParseFunctionArguments
								(
									arguments[argument_index].c_str(),
									arguments[argument_index].length(),
									sub_arguments.data()
								)
							};

							nlohmann::json &afters_entry{ parallel_batch_update_entry["Afters"] };
							nlohmann::json &after_entry{ afters_entry[sub_arguments[0].c_str()] };

							continue;
						}
					}
				}

				continue;
			}
		}

		//Check if this component wants a parallel sub instance update.
		{
			const size_t position{ current_line.find("COMPONENT_PARALLEL_SUB_INSTANCE_UPDATE(") };

			if (position != std::string::npos)
			{
				//Parse the function arguments.
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

				nlohmann::json &parallel_sub_instance_updates_entry{ component_entry["ParallelSubInstanceUpdates"] };
				nlohmann::json &parallel_sub_instance_update_entry{ parallel_sub_instance_updates_entry[arguments[0].c_str()] };

				//Check if there's any befores/afters.
				for (uint64 argument_index{ 1 }; argument_index < number_of_arguments; ++argument_index)
				{
					{
						const size_t position{ arguments[argument_index].find("Before(") };

						if (position != std::string::npos)
						{
							std::array<std::string, 1> sub_arguments;

							const uint64 number_of_sub_arguments
							{
								TextParsing::ParseFunctionArguments
								(
									arguments[argument_index].c_str(),
									arguments[argument_index].length(),
									sub_arguments.data()
								)
							};

							nlohmann::json &befores_entry{ parallel_sub_instance_update_entry["Befores"] };
							nlohmann::json &before_entry{ befores_entry[sub_arguments[0].c_str()] };

							continue;
						}
					}

					{
						const size_t position{ arguments[argument_index].find("After(") };

						if (position != std::string::npos)
						{
							std::array<std::string, 1> sub_arguments;

							const uint64 number_of_sub_arguments
							{
								TextParsing::ParseFunctionArguments
								(
									arguments[argument_index].c_str(),
									arguments[argument_index].length(),
									sub_arguments.data()
								)
							};

							nlohmann::json &afters_entry{ parallel_sub_instance_update_entry["Afters"] };
							nlohmann::json &after_entry{ afters_entry[sub_arguments[0].c_str()] };

							continue;
						}
					}
				}

				continue;
			}
		}

		//Check if this component wants a post update.
		{
			const size_t position{ current_line.find("COMPONENT_POST_UPDATE(") };

			if (position != std::string::npos)
			{
				//Parse the function arguments.
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

				nlohmann::json &post_updates_entry{ component_entry["PostUpdates"] };
				nlohmann::json &post_update_entry{ post_updates_entry[arguments[0].c_str()] };

				//Check if there's any befores/afters.
				for (uint64 argument_index{ 1 }; argument_index < number_of_arguments; ++argument_index)
				{
					{
						const size_t position{ arguments[argument_index].find("Before(") };

						if (position != std::string::npos)
						{
							std::array<std::string, 1> sub_arguments;

							const uint64 number_of_sub_arguments
							{
								TextParsing::ParseFunctionArguments
								(
									arguments[argument_index].c_str(),
									arguments[argument_index].length(),
									sub_arguments.data()
								)
							};

							nlohmann::json &befores_entry{ post_update_entry["Befores"] };
							nlohmann::json &before_entry{ befores_entry[sub_arguments[0].c_str()] };

							continue;
						}
					}

					{
						const size_t position{ arguments[argument_index].find("After(") };

						if (position != std::string::npos)
						{
							std::array<std::string, 1> sub_arguments;

							const uint64 number_of_sub_arguments
							{
								TextParsing::ParseFunctionArguments
								(
									arguments[argument_index].c_str(),
									arguments[argument_index].length(),
									sub_arguments.data()
								)
							};

							nlohmann::json &afters_entry{ post_update_entry["Afters"] };
							nlohmann::json &after_entry{ afters_entry[sub_arguments[0].c_str()] };

							continue;
						}
					}
				}

				continue;
			}
		}

		//Check if this component wants an "Terminate()" call.
		{
			const size_t position{ current_line.find("COMPONENT_TERMINATE(") };

			if (position != std::string::npos)
			{
				component_entry["Terminate"] = true;

				continue;
			}
		}
	}
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

	file << "//Systems." << std::endl;
	file << "#include <Systems/TaskSystem.h>" << std::endl;
	file << std::endl;

	//Add includes for all components.
	file << "//Components." << std::endl;

	//Add all component data.
	std::vector<ComponentData> component_data;
	std::map<std::string, std::vector<ComponentUpdate>> serial_updates;
	std::map<std::string, std::vector<ComponentUpdate>> parallel_batch_updates;
	std::map<std::string, std::vector<ComponentUpdate>> parallel_sub_instance_updates;
	std::map<std::string, std::vector<ComponentUpdate>> post_updates;

	for (auto file_iterator{ JSON.begin() }; file_iterator != JSON.end(); ++file_iterator)
	{
		const nlohmann::json &file_entry{ *file_iterator };

		if (!file_entry.contains("Components"))
		{
			continue;
		}

		const nlohmann::json& components_entry{ file_entry["Components"] };

		//Add the include.
		file << "#include \"" << file_iterator.key().c_str() << "\"" << std::endl;

		for (auto component_iterator{ components_entry.begin() }; component_iterator != components_entry.end(); ++component_iterator)
		{
			//Cache the component entry.
			const nlohmann::json &component_entry{ *component_iterator };

			//Add the component data.
			{
				ComponentData &new_component_data{ component_data.emplace_back() };

				//Set the name.
				new_component_data._Name = component_iterator.key().c_str();

				//Set whether or not this component wants an "Initialize()" call.
				new_component_data._Initialize = component_entry["Initialize"];

				//Set whether or not this component wants a "PostInitialize()" call.
				new_component_data._PostInitialize = component_entry["PostInitialize"];

				//Set whether or not this component wants a "Terminate()" call.
				new_component_data._Terminate = component_entry["Terminate"];

				//Check if this component wants any serial updates.
				if (component_entry.contains("SerialUpdates"))
				{
					const nlohmann::json &serial_updates_entry{ component_entry["SerialUpdates"] };

					for (auto serial_update_iterator{ serial_updates_entry.begin() }; serial_update_iterator != serial_updates_entry.end(); ++serial_update_iterator)
					{
						const nlohmann::json &serial_update_entry{ *serial_update_iterator };

						ComponentUpdate &new_update{ serial_updates[serial_update_iterator.key()].emplace_back() };

						new_update._ComponentName = new_component_data._Name;

						if (serial_update_entry.contains("Befores"))
						{
							const nlohmann::json &befores_entry{ serial_update_entry["Befores"] };

							for (auto before_iterator{ befores_entry.begin() }; before_iterator != befores_entry.end(); ++before_iterator)
							{
								new_update._Befores.emplace_back(before_iterator.key());
							}
						}

						if (serial_update_entry.contains("Afters"))
						{
							const nlohmann::json &afters_entry{ serial_update_entry["Afters"] };

							for (auto after_iterator{ afters_entry.begin() }; after_iterator != afters_entry.end(); ++after_iterator)
							{
								new_update._Afters.emplace_back(after_iterator.key());
							}
						}
					}
				}

				//Check if this component wants any parallel batch updates.
				if (component_entry.contains("ParallelBatchUpdates"))
				{
					const nlohmann::json &parallel_batch_updates_entry{ component_entry["ParallelBatchUpdates"] };

					for (auto parallel_batch_update_iterator{ parallel_batch_updates_entry.begin() }; parallel_batch_update_iterator != parallel_batch_updates_entry.end(); ++parallel_batch_update_iterator)
					{
						const nlohmann::json &parallel_batch_update_entry{ *parallel_batch_update_iterator };

						ComponentUpdate &new_update{ parallel_batch_updates[parallel_batch_update_iterator.key()].emplace_back() };

						new_update._ComponentName = new_component_data._Name;

						if (parallel_batch_update_entry.contains("Befores"))
						{
							const nlohmann::json &befores_entry{ parallel_batch_update_entry["Befores"] };

							for (auto before_iterator{ befores_entry.begin() }; before_iterator != befores_entry.end(); ++before_iterator)
							{
								new_update._Befores.emplace_back(before_iterator.key());
							}
						}

						if (parallel_batch_update_entry.contains("Afters"))
						{
							const nlohmann::json &afters_entry{ parallel_batch_update_entry["Afters"] };

							for (auto after_iterator{ afters_entry.begin() }; after_iterator != afters_entry.end(); ++after_iterator)
							{
								new_update._Afters.emplace_back(after_iterator.key());
							}
						}

						new_update._BatchSize = parallel_batch_update_entry["BatchSize"];
					}
				}

				//Check if this component wants any parallel sub instance updates.
				if (component_entry.contains("ParallelSubInstanceUpdates"))
				{
					const nlohmann::json &parallel_sub_instance_updates_entry{ component_entry["ParallelSubInstanceUpdates"] };

					for (auto parallel_sub_instance_update_iterator{ parallel_sub_instance_updates_entry.begin() }; parallel_sub_instance_update_iterator != parallel_sub_instance_updates_entry.end(); ++parallel_sub_instance_update_iterator)
					{
						const nlohmann::json &parallel_sub_instance_update_entry{ *parallel_sub_instance_update_iterator };

						ComponentUpdate &new_update{ parallel_sub_instance_updates[parallel_sub_instance_update_iterator.key()].emplace_back() };

						new_update._ComponentName = new_component_data._Name;

						if (parallel_sub_instance_update_entry.contains("Befores"))
						{
							const nlohmann::json &befores_entry{ parallel_sub_instance_update_entry["Befores"] };

							for (auto before_iterator{ befores_entry.begin() }; before_iterator != befores_entry.end(); ++before_iterator)
							{
								new_update._Befores.emplace_back(before_iterator.key());
							}
						}

						if (parallel_sub_instance_update_entry.contains("Afters"))
						{
							const nlohmann::json &afters_entry{ parallel_sub_instance_update_entry["Afters"] };

							for (auto after_iterator{ afters_entry.begin() }; after_iterator != afters_entry.end(); ++after_iterator)
							{
								new_update._Afters.emplace_back(after_iterator.key());
							}
						}
					}
				}

				//Check if this component wants any post updates.
				if (component_entry.contains("PostUpdates"))
				{
					const nlohmann::json &post_updates_entry{ component_entry["PostUpdates"] };

					for (auto post_update_iterator{ post_updates_entry.begin() }; post_update_iterator != post_updates_entry.end(); ++post_update_iterator)
					{
						const nlohmann::json &post_update_entry{ *post_update_iterator };

						ComponentUpdate &new_update{ post_updates[post_update_iterator.key()].emplace_back() };

						new_update._ComponentName = new_component_data._Name;

						if (post_update_entry.contains("Befores"))
						{
							const nlohmann::json &befores_entry{ post_update_entry["Befores"] };

							for (auto before_iterator{ befores_entry.begin() }; before_iterator != befores_entry.end(); ++before_iterator)
							{
								new_update._Befores.emplace_back(before_iterator.key());
							}
						}

						if (post_update_entry.contains("Afters"))
						{
							const nlohmann::json &afters_entry{ post_update_entry["Afters"] };

							for (auto after_iterator{ afters_entry.begin() }; after_iterator != afters_entry.end(); ++after_iterator)
							{
								new_update._Afters.emplace_back(after_iterator.key());
							}
						}
					}
				}
			}
		}
	}

	file << std::endl;

	//Add the static variable definitions for all component instances.
	file << "//Static variable definitions." << std::endl;

	for (const ComponentData &_component_data : component_data)
	{
		file << _component_data._Name.c_str() << " *RESTRICT " << _component_data._Name.c_str() << "::Instance;" << std::endl;
	}

	file << std::endl;

	//Add the components memory.
	file << "void *RESTRICT COMPONENTS_MEMORY;" << std::endl;

	file << std::endl;

	//Add the "ParallelUpdate" class definition.
	file << "/*" << std::endl;
	file << "*\tParallel update class definition." << std::endl;
	file << "*/" << std::endl;

	file << "class ParallelUpdate final" << std::endl;
	file << "{" << std::endl;
	file << std::endl;
	file << "public:" << std::endl;
	file << std::endl;

	file << "\t//The task." << std::endl;
	file << "\tTask _Task;" << std::endl;
	file << std::endl;

	file << "\tunion" << std::endl;
	file << "\t{" << std::endl;

	file << "\t\tstruct" << std::endl;
	file << "\t\t{" << std::endl;

	file << "\t\t\t//The start instance index." << std::endl;
	file << "\t\t\tuint64 _StartInstanceIndex;" << std::endl;
	file << std::endl;

	file << "\t\t\t//The end instance index." << std::endl;
	file << "\t\t\tuint64 _EndInstanceIndex;" << std::endl;

	file << "\t\t};" << std::endl;

	file << "\t\tstruct" << std::endl;
	file << "\t\t{" << std::endl;

	file << "\t\t\t//The instance index." << std::endl;
	file << "\t\t\tuint64 _InstanceIndex;" << std::endl;
	file << std::endl;

	file << "\t\t\t//The sub instance index." << std::endl;
	file << "\t\t\tuint64 _SubInstanceIndex;" << std::endl;

	file << "\t\t};" << std::endl;

	file << "\t};" << std::endl;

	file << "};" << std::endl;
	file << std::endl;

	//Add the parallel updates container.
	file << "//Container for all parallel updates." << std::endl;
	file << "DynamicArray<ParallelUpdate> PARALLEL_UPDATES;" << std::endl;
	file << std::endl;

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
		file << "\t\t\treturn " << component_data[i]._Name.c_str() << "::Instance;" << std::endl;
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

	file << "\t{" << std::endl;

	file << "\t\tconstexpr uint64 COMPONENTS_SIZE" << std::endl;
	file << "\t\t{" << std::endl;
	
	for (uint64 i{ 0 }; i < component_data.size(); ++i)
	{
		file << "\t\t\t";

		if (i > 0)
		{
			file << "+ ";
		}

		file << "sizeof(" << component_data[i]._Name.c_str() << ")" << std::endl;
	}

	file << "\t\t};" << std::endl;

	file << "\t\tCOMPONENTS_MEMORY = Memory::Allocate(COMPONENTS_SIZE);" << std::endl;
	file << "\t\tbyte *RESTRICT cursor{ static_cast<byte *RESTRICT>(COMPONENTS_MEMORY) };" << std::endl;

	for (uint64 i{ 0 }; i < component_data.size(); ++i)
	{
		file << "\t\t" << component_data[i]._Name.c_str() << "::Instance = new (cursor) " << component_data[i]._Name.c_str() << "();" << std::endl;
		file << "\t\tcursor += sizeof(" << component_data[i]._Name.c_str() << ");" << std::endl;
	}

	file << "\t}" << std::endl;

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

	//Set up the "Components::AllocateInitializationData()" function.
	file << "NO_DISCARD ComponentInitializationData *const RESTRICT Components::AllocateInitializationData(Component *const RESTRICT component) NOEXCEPT" << std::endl;
	file << "{" << std::endl;

	file << "\tswitch(component->_Identifier)" << std::endl;
	file << "\t{" << std::endl;

	for (uint64 i{ 0 }; i < component_data.size(); ++i)
	{
		const uint64 component_identifier{ CatalystHash(component_data[i]._Name.data(), component_data[i]._Name.length()) };

		file << "\t\tcase " << component_identifier << ":" << std::endl;
		file << "\t\t{" << std::endl;
		file << "\t\t\treturn " << component_data[i]._Name.c_str() << "::Instance->AllocateInitializationData();" << std::endl;
		file << "\t\t}" << std::endl;
	}

	file << "\t\tdefault:" << std::endl;
	file << "\t\t{" << std::endl;
	file << "\t\t\tASSERT(false, \"Unknown component!\");" << std::endl;
	file << "\t\t\treturn nullptr;" << std::endl;
	file << "\t\t}" << std::endl;

	file << "\t}" << std::endl;

	file << "}" << std::endl;
	file << std::endl;

	//Set up the "Components::FreeInitializationData()" function.
	file << "void Components::FreeInitializationData(Component *const RESTRICT component, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT" << std::endl;
	file << "{" << std::endl;

	file << "\tswitch(component->_Identifier)" << std::endl;
	file << "\t{" << std::endl;

	for (uint64 i{ 0 }; i < component_data.size(); ++i)
	{
		const uint64 component_identifier{ CatalystHash(component_data[i]._Name.data(), component_data[i]._Name.length()) };

		file << "\t\tcase " << component_identifier << ":" << std::endl;
		file << "\t\t{" << std::endl;
		file << "\t\t\t" << component_data[i]._Name.c_str() << "::Instance->FreeInitializationData(initialization_data);" << std::endl;
		file << "\t\t\tbreak;" << std::endl;
		file << "\t\t}" << std::endl;
	}

	file << "\t\tdefault:" << std::endl;
	file << "\t\t{" << std::endl;
	file << "\t\t\tASSERT(false, \"Unknown component!\");" << std::endl;
	file << "\t\t\tbreak;" << std::endl;
	file << "\t\t}" << std::endl;

	file << "\t}" << std::endl;

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

	for (uint64 i{ 0 }; i < component_data.size(); ++i)
	{
		file << "\t" << component_data[i]._Name.c_str() << "::Instance->~" << component_data[i]._Name.c_str() << "();" << std::endl;
	}

	file << "\tMemory::Free(COMPONENTS_MEMORY);" << std::endl;

	file << "}";
}