//Header file.
#include "ScriptGenerator.h"

//Catalyst code generator.
#include "TextParsing.h"

//Constants.
#define ASSETS_DIRECTORY "..\\..\\..\\Content\\Assets"

#define HEADER_FILE_PATH "..\\..\\..\\Code\\Include\\Generated\\Script.Generated.h"
#define SOURCE_FILE_PATH "..\\..\\..\\Code\\Source\\Generated\\Script.Generated.cpp"

#define SCRIPT_DATA_FLAG_NONE (0)
#define SCRIPT_DATA_FLAG_HAS_INITIALIZE (BIT(0))
#define SCRIPT_DATA_FLAG_HAS_UPDATE (BIT(1))
#define SCRIPT_DATA_FLAG_UPDATE_WANTS_DELTA_TIME (BIT(2))
#define SCRIPT_DATA_FLAG_HAS_TERMINATE (BIT(3))

//Macros.
#define CHECK_ERROR_CODE() if (error_code) { std::cout << "Error at " << __LINE__ << ": " << error_code.message() << std::endl; }

/*
*	Returns the size of the given type.
*/
uint64 SizeOfType(const char *const type)
{
	if (strcmp(type, "bool") == 0)
	{
		return sizeof(bool);
	}

	if (strcmp(type, "float32") == 0)
	{
		return sizeof(float32);
	}

	if (strcmp(type, "uint32") == 0)
	{
		return sizeof(uint32);
	}

	if (strcmp(type, "Vector3") == 0)
	{
		return sizeof(float32) * 3;
	}

	else
	{
		ASSERT(false, "Unknown type " << type << "!");

		return 0;
	}
}

/*
*	Runs this script generator.
*/
void ScriptGenerator::Run(int32 command_line_argument_count, char *command_line_arguments[])
{
	//Cache the error code.
	std::error_code error_code;

	//Create the "CodeGeneration" directory, if it doesn't exist.
	std::filesystem::create_directory("..\\..\\..\\Code\\CodeGeneration", error_code); CHECK_ERROR_CODE();

	//Set up the script cache.
	nlohmann::json script_cache;

	if (std::filesystem::exists("..\\..\\..\\Code\\CodeGeneration\\ScriptCache.json"))
	{
		std::ifstream input_file{ "..\\..\\..\\Code\\CodeGeneration\\ScriptCache.json" };
		input_file >> script_cache;
		input_file.close();
	}

	//Gather scripts!
	GatherScripts(ASSETS_DIRECTORY, script_cache);

	//Compile scripts, if needed.
	if (_CompileScripts || !std::filesystem::exists(HEADER_FILE_PATH) || !std::filesystem::exists(SOURCE_FILE_PATH))
	{
		GenerateSourceFile(command_line_argument_count, command_line_arguments);
		GenerateHeaderFile();
	}

	//Write the content cache.
	{
		std::ofstream file{ "..\\..\\..\\Code\\CodeGeneration\\ScriptCache.json" };
		file << std::setw(4) << script_cache;
		file.close();
	}
}

/*
*	Gathers scripts in the given directory.
*/
void ScriptGenerator::GatherScripts(const char *const directory_path, nlohmann::json &script_cache)
{
	//If the directory doesn't exist, just exit.
	if (!std::filesystem::exists(directory_path))
	{
		return;
	}

	//Iterate over all scripts.
	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		//Scan recursively if this is a directory.
		if (entry.is_directory())
		{
			GatherScripts(entry.path().string().c_str(), script_cache);

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

		//Check if this is a script.
		if (extension != "Script")
		{
			continue;
		}

		//Cache the script entry.
		nlohmann::json &script_entry{ script_cache[file_path] };

		//Calculate the last write time.
		std::time_t last_write_time;

		{
			const std::filesystem::file_time_type _last_write_time{ std::filesystem::last_write_time(file_path) };
			const std::chrono::time_point<std::chrono::system_clock> time_point{ std::chrono::utc_clock::to_sys(std::chrono::file_clock::to_utc(_last_write_time)) };
			last_write_time = std::chrono::system_clock::to_time_t(time_point);
		}

		//Figure out the name.
		std::string name;

		{
			const size_t last_slash_position{ file_path.find_last_of("\\") };
			name = file_path.substr(last_slash_position + 1, file_path.length() - last_slash_position - strlen(extension.c_str()) - 2);
		}

		//Set up the new script data.
		ScriptData &new_script_data{ _ScriptData.emplace_back() };

		new_script_data._FilePath = file_path.c_str();
		new_script_data._Name = name.c_str();
		new_script_data._Flags = SCRIPT_DATA_FLAG_NONE;
		new_script_data._RequiredDataSize = 0;

		//Check if we should parse this entry.
		bool parse_entry{ true };

		if (!script_entry.is_null())
		{
			nlohmann::json &last_write_time_entry{ script_entry["LastWriteTime"] };

			if (!last_write_time_entry.is_null())
			{
				parse_entry = last_write_time != last_write_time_entry.get<std::time_t>();
			}
		}

		if (!parse_entry)
		{
			continue;
		}

		//Set that we need to compile scripts.
		_CompileScripts = true;

		//Clear the entry before writing to it.
		script_entry.clear();

		//Write the last write time.
		script_entry["LastWriteTime"] = last_write_time;
	}
}

/*
*	Generates the source file.
*/
void ScriptGenerator::GenerateSourceFile(int32 command_line_argument_count, char *command_line_arguments[])
{
	/*
	*	Script intermediate data
	*/
	class ScriptIntermediateData final
	{

	public:

		//The variables.
		std::vector<std::string> _Variables;

		//The function names.
		std::vector<std::string> _FunctionNames;

		/*
		*	Clears the intermediate data.
		*/
		inline void Clear()
		{
			_Variables.clear();
			_FunctionNames.clear();
		}

	};

	//Set up the intermediate data.
	ScriptIntermediateData intermediate_data;

	//Container for the arguments parsed.
	std::array<std::string, 8> arguments;

	//Figure out the directory path.
	char directory_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(directory_path, "..\\..\\..\\Code\\Source\\Generated");

	//Create the directory, if it doesn't exist.
	std::filesystem::create_directory(directory_path);

	//Figure out the file path.
	char file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(file_path, "%s\\Script.Generated.cpp", directory_path);

	//Open the file!
	std::ofstream file{ file_path };

	//Add some stuff in the beginning.
	file << "//Header file." << std::endl;
	file << "#include <Generated/Script.Generated.h>" << std::endl;
	file << std::endl;

	file << "//Math." << std::endl;
	file << "#include <Math/General/Vector.h>" << std::endl;
	file << std::endl;

	file << "//Scripting." << std::endl;
	file << "#include <Scripting/ScriptNodes.h>" << std::endl;
	file << std::endl;

	file << "//Systems." << std::endl;
	file << "#include <Systems/CatalystEngineSystem.h>" << std::endl;
	
	{
		for (int32 i{ 0 }; i < command_line_argument_count; ++i)
		{
			if (strcmp(command_line_arguments[i], "-script_generator_game_include") == 0)
			{
				file << "#include <" << command_line_arguments[i + 1] << ">" << std::endl;
			}
		}
	}

	//Disable some warnings.
	file << "DISABLE_WARNING(4060) //switch statement contains no 'case' or 'default' labels" << std::endl;
	file << std::endl;

	//Add some type aliases.
	file << "#define Vector3 Vector3<float32>" << std::endl;
	file << std::endl;

	//Add all script specific things.
	for (ScriptData &script_data : _ScriptData)
	{
		//Clear the intermediate data.
		intermediate_data.Clear();

		//Add the namespace into the source file.
		file << "namespace " << script_data._Name.c_str() << std::endl;
		file << "{" << std::endl;
		//Keep track of the current variable offset.
		uint64 current_variable_offset{ 0 };

		//Open the file.
		std::ifstream script_file{ script_data._FilePath.c_str()};

		//Go through all the lines in the file.
		std::string current_line;

		while (std::getline(script_file, current_line))
		{
			//Skip comments.
			if (OnlyComment(current_line.c_str(), current_line.length()))
			{
				continue;
			}

			//Skip whitespace.
			if (OnlyWhitespace(current_line.c_str(), current_line.length()))
			{
				continue;
			}

			//Is this a variable declaration?
			{
				const size_t position{ current_line.find("Variable(") };

				/*
				*	Since some script nodes might end in this token,
				*	check if there's anything before the position that signals that this is not the correct token.
				*/
				bool is_correct_token{ false };

				if (position != std::string::npos)
				{
					is_correct_token = true;

					for (int64 i{ static_cast<int64>(position) - 1 }; i >= 0; --i)
					{
						//Only accept spaces and tabs before the position.
						if (current_line[i] != '\t' && current_line[i] != ' ')
						{
							is_correct_token = false;

							break;
						}
					}
				}

				if (is_correct_token)
				{
					const uint64 number_of_arguments
					{
						ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.data()
						)
					};

					ASSERT(number_of_arguments == 2, "A variable declaration needs two arguments!");

					//Add the define.
					file << "\t#define " << arguments[1].c_str() << " (*(static_cast<" << arguments[0].c_str() << " *const RESTRICT>(AdvancePointer(script_context._Data, " << current_variable_offset << "))))" << std::endl;

					//Update the current variable offset.
					current_variable_offset += SizeOfType(arguments[0].c_str());

					//Add the variable.
					intermediate_data._Variables.emplace_back(arguments[1]);

					continue;
				}
			}

			//Is this an function declaration?
			{
				const size_t position{ current_line.find("Event(") };

				/*
				*	Since some script nodes might end in this token,
				*	check if there's anything before the position that signals that this is not the correct token.
				*/
				bool is_correct_token{ false };

				if (position != std::string::npos)
				{
					is_correct_token = true;

					for (int64 i{ static_cast<int64>(position) - 1 }; i >= 0; --i)
					{
						//Only accept spaces and tabs before the position.
						if (current_line[i] != '\t' && current_line[i] != ' ')
						{
							is_correct_token = false;

							break;
						}
					}
				}

				if (is_correct_token)
				{
					const uint64 number_of_arguments
					{
						ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.data()
						)
					};

					ASSERT(number_of_arguments == 1, "A function declaration needs one argument!");

					//Add function signature.
					file << "\tvoid " << arguments[0].data() << "(ScriptContext &script_context) NOEXCEPT" << std::endl;

					//Add the event.
					script_data._Events.emplace_back(arguments[0]);

					continue;
				}
			}

			//Is this a function declaration?
			{
				const size_t position{ current_line.find("Function(") };

				/*
				*	Since some script nodes might end in this token,
				*	check if there's anything before the position that signals that this is not the correct token.
				*/
				bool is_correct_token{ false };

				if (position != std::string::npos)
				{
					is_correct_token = true;

					for (int64 i{ static_cast<int64>(position) - 1 }; i >= 0; --i)
					{
						//Only accept spaces and tabs before the position.
						if (current_line[i] != '\t' && current_line[i] != ' ')
						{
							is_correct_token = false;

							break;
						}
					}
				}

				if (is_correct_token)
				{
					const uint64 number_of_arguments
					{
						ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.data()
						)
					};

					ASSERT(number_of_arguments >= 2, "A function declaration needs at least two arguments!");

					//Add function signature.
					file << "\t" << arguments[0].c_str() << " " << arguments[1].c_str() << "(ScriptContext &script_context";

					if (number_of_arguments > 2)
					{
						file << ", ";
					}

					for (uint64 i{ 2 }; i < number_of_arguments; i += 2)
					{
						file << arguments[i].c_str() << " " << arguments[i + 1].c_str();

						if (i < (number_of_arguments - 2))
						{
							file << ", ";
						}
					}

					file << ")" << std::endl;

					//Update the script flags.
					if (arguments[1] == "Initialize")
					{
						script_data._Flags |= SCRIPT_DATA_FLAG_HAS_INITIALIZE;
					}

					else if (arguments[1] == "Update")
					{
						script_data._Flags |= SCRIPT_DATA_FLAG_HAS_UPDATE;

						if (number_of_arguments >= 4 && arguments[3] == "delta_time")
						{
							script_data._Flags |= SCRIPT_DATA_FLAG_UPDATE_WANTS_DELTA_TIME;
						}
					}

					else if (arguments[1] == "Terminate")
					{
						script_data._Flags |= SCRIPT_DATA_FLAG_HAS_TERMINATE;
					}

					//Add the function name.
					intermediate_data._FunctionNames.emplace_back(arguments[1].c_str());

					continue;
				}
			}

			//Modify script node calls. Only one script node call per line supported right now.
			{
				size_t position{ current_line.find("::") };

				if (position != std::string::npos)
				{
					//Figure out the start position.
					uint64 start_position{ position };

					for (int64 i{ static_cast<int64>(position) - 1 }; i >= 0; --i)
					{
						//Assume there's spaces I guess. :x
						if (current_line[i] == ' '
							|| current_line[i] == '\t')
						{
							start_position = i + 1;

							break;
						}
					}

					//Figure out the function name.
					std::string function_name;

					{
						//Figure out the end position.
						uint64 end_position{ position };

						for (uint64 i{ position + 1 }; i < current_line.length(); ++i)
						{
							if (current_line[i] == '(')
							{
								end_position = i;

								break;
							}
						}

						//Cut out the substring!
						function_name = current_line.substr(start_position, end_position - start_position);
					}

					//Parse the function arguments.
					uint64 end_position;

					const uint64 number_of_arguments
					{
						ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.data(),
							&end_position
						)
					};

					//Modify the call.
					std::string modified_call;

					//Add the "Script::" prefix.
					modified_call = "Script::";

					//Add the function name.
					modified_call += function_name;

					//Add the script context to every call.
					modified_call += "(script_context";

					//Add all the arguments.
					for (uint64 i{ 0 }; i < number_of_arguments; ++i)
					{
						if (!arguments[i].empty())
						{
							modified_call += std::string(", ") + arguments[i].c_str();
						}
					}
					
					//Add the closing bracket.
					modified_call += ")";

					//Replace the call in the current line.
					current_line.replace(start_position, end_position - start_position + 1, modified_call);
				}
			}

			//Recursively modify function calls to insert the script context at the start.
			{
				bool modified_function_call{ true };

				while (modified_function_call)
				{
					//Reset whether or not we modified a function call here, otherwise we'll loop indefinitely.
					modified_function_call = false;

					//Search for one of the function names.
					for (const std::string &function_name : intermediate_data._FunctionNames)
					{
						const size_t position{ current_line.find(function_name) };

						if (position != std::string::npos)
						{
							//Search forward in the string.
							uint64 current_position{ position + function_name.length() - 1 };

							//Check if the next character is the start parantheses.
							char next_character;
							uint64 next_character_position;

							TextParsing::GetNextNonWhitespaceCharacter(current_line.c_str(), current_position, &next_character, &next_character_position);

							//If the next non-whitespace character is not the start parantheses, then... It must not be an actual function call. :x
							if (next_character != '(')
							{
								continue;
							}

							//Update the current position.
							current_position = next_character_position;

							//Now check if the next character is the end parantheses.
							TextParsing::GetNextNonWhitespaceCharacter(current_line.c_str(), current_position, &next_character, &next_character_position);

							if (next_character == ')')
							{
								//Insert the script context now!
								current_line.insert(next_character_position, "script_context");

								//We did modify the function call!
								modified_function_call = true;
							}

							else
							{
								//Check if we have already added the script context.
								const std::string compare_string{ "script_context" };

								bool already_added_script_context{ true };

								for (uint64 i{ 0 }; i < std::min(current_line.length() - next_character_position, compare_string.length()) && already_added_script_context; ++i)
								{
									already_added_script_context = current_line[next_character_position + i] == compare_string[i];
								}

								if (!already_added_script_context)
								{
									//Insert the script context now!
									current_line.insert(next_character_position, "script_context");

									//We did modify the function call!
									modified_function_call = true;
								}
							}
						}
					}
				}
			}

			//Otherwise, just add it to the source file and hope that it's valid C++! (:
			file << "\t" << current_line << std::endl;
		}

		//Close the script file.
		script_file.close();

		//Undefine all variables.
		for (const std::string &variable : intermediate_data._Variables)
		{
			file << "\t#undef " << variable.c_str() << std::endl;
		}

		//Add the rest of the script data.
		script_data._RequiredDataSize = current_variable_offset;

		//Add the required data size.
		file << "\tconstexpr uint64 REQUIRED_DATA_SIZE{ " << script_data._RequiredDataSize << " };" << std::endl;

		//Add the closing bracket for the namespace.
		file << "}" << std::endl;

	}

	file << std::endl;

	//Add the implementation for the functions in the header file.
	file << "namespace Script" << std::endl;
	file << "{" << std::endl;

	//Add function for getting the required data size.
	file << "\tNO_DISCARD uint64 RequiredDataSize(const ScriptIdentifier script_identifier) NOEXCEPT" << std::endl;
	file << "\t{" << std::endl;

	file << "\t\tswitch (script_identifier)" << std::endl;
	file << "\t\t{" << std::endl;

	for (const ScriptData& script_data : _ScriptData)
	{
		file << "\t\t\tcase ScriptIdentifier::" << script_data._Name.c_str() << ":" << std::endl;
		file << "\t\t\t{" << std::endl;
		file << "\t\t\t\treturn " << script_data._Name.c_str() << "::REQUIRED_DATA_SIZE;" << std::endl;
		file << "\t\t\t}" << std::endl;
	}

	file << "\t\t}" << std::endl;

	file << "\t\treturn 0;" << std::endl;

	file << "\t}" << std::endl;

	//Add function for initializing scripts.
	file << "\tvoid Initialize(const ScriptIdentifier script_identifier, ScriptContext &script_context) NOEXCEPT" << std::endl;
	file << "\t{" << std::endl;

	file << "\t\tswitch (script_identifier)" << std::endl;
	file << "\t\t{" << std::endl;

	for (const ScriptData& script_data : _ScriptData)
	{
		if (TEST_BIT(script_data._Flags, SCRIPT_DATA_FLAG_HAS_INITIALIZE))
		{
			file << "\t\t\tcase ScriptIdentifier::" << script_data._Name.c_str() << ":" << std::endl;
			file << "\t\t\t{" << std::endl;
			file << "\t\t\t\t" << script_data._Name.c_str() << "::Initialize(script_context);" << std::endl;
			file << "\t\t\t\t" << "break;" << std::endl;
			file << "\t\t\t}" << std::endl;
		}
	}

	file << "\t\t}" << std::endl;

	file << "\t}" << std::endl;

	//Add function for updating scripts.
	file << "\tvoid Update(const ScriptIdentifier script_identifier, ScriptContext &script_context) NOEXCEPT" << std::endl;
	file << "\t{" << std::endl;

	file << "\t\tswitch (script_identifier)" << std::endl;
	file << "\t\t{" << std::endl;

	for (const ScriptData& script_data : _ScriptData)
	{
		if (TEST_BIT(script_data._Flags, SCRIPT_DATA_FLAG_HAS_UPDATE))
		{
			file << "\t\t\tcase ScriptIdentifier::" << script_data._Name.c_str() << ":" << std::endl;
			file << "\t\t\t{" << std::endl;

			if (TEST_BIT(script_data._Flags, SCRIPT_DATA_FLAG_UPDATE_WANTS_DELTA_TIME))
			{
				file << "\t\t\t\t" << script_data._Name.c_str() << "::Update(script_context, CatalystEngineSystem::Instance->GetDeltaTime());" << std::endl;
			}

			else
			{
				file << "\t\t\t\t" << script_data._Name.c_str() << "::Update(script_context);" << std::endl;
			}

			file << "\t\t\t\t" << "break;" << std::endl;
			file << "\t\t\t}" << std::endl;
		}
	}

	file << "\t\t}" << std::endl;

	file << "\t}" << std::endl;

	//Add function for terminating scripts.
	file << "\tvoid Terminate(const ScriptIdentifier script_identifier, ScriptContext &script_context) NOEXCEPT" << std::endl;
	file << "\t{" << std::endl;

	file << "\t\tswitch (script_identifier)" << std::endl;
	file << "\t\t{" << std::endl;

	for (const ScriptData& script_data : _ScriptData)
	{
		if (TEST_BIT(script_data._Flags, SCRIPT_DATA_FLAG_HAS_TERMINATE))
		{
			file << "\t\t\tcase ScriptIdentifier::" << script_data._Name.c_str() << ":" << std::endl;
			file << "\t\t\t{" << std::endl;
			file << "\t\t\t\t" << script_data._Name.c_str() << "::Terminate(script_context);" << std::endl;
			file << "\t\t\t\t" << "break;" << std::endl;
			file << "\t\t\t}" << std::endl;
		}
	}

	file << "\t\t}" << std::endl;

	file << "\t}" << std::endl;

	//Add function for sending events to scripts.
	file << "\tvoid Event(const ScriptIdentifier script_identifier, const HashString event, ScriptContext &script_context) NOEXCEPT" << std::endl;
	file << "\t{" << std::endl;

	file << "\t\tswitch (script_identifier)" << std::endl;
	file << "\t\t{" << std::endl;

	for (const ScriptData &script_data : _ScriptData)
	{
		file << "\t\t\tcase ScriptIdentifier::" << script_data._Name.c_str() << ":" << std::endl;
		file << "\t\t\t{" << std::endl;

		file << "\t\t\t\tswitch (event)" << std::endl;
		file << "\t\t\t\t{" << std::endl;

		for (const std::string &event : script_data._Events)
		{
			const uint64 event_hash{ CatalystHash(event.data(), event.length()) };

			file << "\t\t\t\t\tcase " << event_hash << ": /*" << event.c_str() << "*/" << std::endl;
			file << "\t\t\t\t\t{" << std::endl;
			file << "\t\t\t\t\t\t" << script_data._Name.c_str() << "::" << event.c_str() << "(script_context);" << std::endl;
			file << "\t\t\t\t\t\tbreak;" << std::endl;
			file << "\t\t\t\t\t}" << std::endl;
		}

		file << "\t\t\t\t}" << std::endl;

		file << "\t\t\t\t" << "break;" << std::endl;
		file << "\t\t\t}" << std::endl;
	}

	file << "\t\t}" << std::endl;

	file << "\t}" << std::endl;

	file << "}";

	//Close the file.
	file.close();
}

/*
*	Generates the header file.
*/
void ScriptGenerator::GenerateHeaderFile()
{
	//Figure out the directory path.
	char directory_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(directory_path, "..\\..\\..\\Code\\Include\\Generated");

	//Create the directory, if it doesn't exist.
	std::filesystem::create_directory(directory_path);

	//Figure out the file path.
	char file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(file_path, "%s\\Script.Generated.h", directory_path);

	//Open the file.
	std::ofstream file{ file_path };

	//Add header stuff.
	file << "#pragma once" << std::endl;
	file << std::endl;

	file << "//Core." << std::endl;
	file << "#include <Core/Essential/CatalystEssential.h>" << std::endl;
	file << "#include <Core/General/Enumeration.h>" << std::endl;
	file << "#include <Core/General/HashString.h>" << std::endl;
	file << std::endl;

	file << "//Scripting." << std::endl;
	file << "#include <Scripting/ScriptContext.h>" << std::endl;
	file << std::endl;

	//Add an enum for all the script identifiers.
	file << "//Enumeration covering all script identifiers." << std::endl;
	file << "CATALYST_ENUMERATION" << std::endl;
	file << "(" << std::endl;
	file << "\tScriptIdentifier," << std::endl;
	file << "\tuint32," << std::endl;
	file << "\tNone," << std::endl;

	for (uint64 i{ 0 }; i < _ScriptData.size(); ++i)
	{
		file << "\t" << _ScriptData[i]._Name.c_str();

		if (i < (_ScriptData.size() - 1))
		{
			file << "," << std::endl;
		}

		else
		{
			file << std::endl;
		}
	}

	file << ");" << std::endl;

	file << std::endl;

	//Add the Script namespace.
	file << "/*" << std::endl;
	file << "*\tThis namespace is the main interface for scripts." << std::endl;
	file << "*/" << std::endl;
	file << "namespace Script" << std::endl;
	file << "{" << std::endl;
	file << std::endl;
	file << "\t/*" << std::endl;
	file << "\t*\tReturns the required data size for the script with the given identifier." << std::endl;
	file << "\t*/" << std::endl;
	file << "\tNO_DISCARD uint64 RequiredDataSize(const ScriptIdentifier script_identifier) NOEXCEPT;" << std::endl;
	file << std::endl;
	file << "\t/*" << std::endl;
	file << "\t*\tInitializes the script with the given identifier." << std::endl;
	file << "\t*/" << std::endl;
	file << "\tvoid Initialize(const ScriptIdentifier script_identifier, ScriptContext &script_context) NOEXCEPT;" << std::endl;
	file << std::endl;
	file << "\t/*" << std::endl;
	file << "\t*\tUpdates the script with the given identifier." << std::endl;
	file << "\t*/" << std::endl;
	file << "\tvoid Update(const ScriptIdentifier script_identifier, ScriptContext &script_context) NOEXCEPT;" << std::endl;
	file << std::endl;
	file << "\t/*" << std::endl;
	file << "\t*\tTerminates the script with the given identifier." << std::endl;
	file << "\t*/" << std::endl;
	file << "\tvoid Terminate(const ScriptIdentifier script_identifier, ScriptContext &script_context) NOEXCEPT;" << std::endl;
	file << std::endl;
	file << "\t/*" << std::endl;
	file << "\t*\tSend the given event the script with the given identifier." << std::endl;
	file << "\t*/" << std::endl;
	file << "\tvoid Event(const ScriptIdentifier script_identifier, const HashString event, ScriptContext &script_context) NOEXCEPT;" << std::endl;
	file << std::endl;
	file << "}" << std::endl;

	//Close the file.
	file.close();
}