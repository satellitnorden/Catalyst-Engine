//Header file.
#include <Content/AssetCompilers/ScriptAssetCompiler.h>

//Core.
#include <Core/Containers/StaticArray.h>

//Content.
#include <Content/Core/ContentCache.h>

//File.
#include <File/Core/FileCore.h>
#include <File/Utilities/TextParsingUtilities.h>

//Constants.
#define SCRIPT_DATA_FLAG_NONE (0)
#define SCRIPT_DATA_FLAG_HAS_INITIALIZE (BIT(0))
#define SCRIPT_DATA_FLAG_HAS_UPDATE (BIT(1))
#define SCRIPT_DATA_FLAG_UPDATE_WANTS_DELTA_TIME (BIT(2))
#define SCRIPT_DATA_FLAG_HAS_TERMINATE (BIT(3))

#define DUMMY_FILE (0) //For debug purposes.

//Singleton definition.
DEFINE_SINGLETON(ScriptAssetCompiler);

/*
*	Returns the size of the given type.
*/
FORCE_INLINE NO_DISCARD uint64 SizeOfType(const char *const RESTRICT type) NOEXCEPT
{
	if (StringUtilities::IsEqual(type, "float32"))
	{
		return sizeof(float32);
	}

	else
	{
		ASSERT(false, "Unknown type " << type << "!");

		return 0;
	}
}

/*
*	Default constructor.
*/
ScriptAssetCompiler::ScriptAssetCompiler() NOEXCEPT
{
	//Set the flags.
	_Flags = Flags::ALWAYS_COMPILE;
}

/*
*	Runs before compilation in the specified domain has finished.
*/
void ScriptAssetCompiler::PreCompile(const CompilationDomain compilation_domain) NOEXCEPT
{
	//Scripts don't need to be compiled if they haven't changed.
	_CompileScripts = false;
}

/*
*	Compiles a single asset with the given compile context.
*/
void ScriptAssetCompiler::Compile(const CompileContext &compile_context) NOEXCEPT
{
	//Set up the new script data.
	_ScriptData.Emplace();
	ScriptData &new_script_data{ _ScriptData.Back() };

	new_script_data._FilePath = compile_context._FilePath;
	new_script_data._Name = compile_context._Name;
	new_script_data._Flags = SCRIPT_DATA_FLAG_NONE;
	new_script_data._RequiredDataSize = 0;

	//Check if a compilation is needed.
	_CompileScripts |= compile_context._ContentCache->NeedsRecompile(compile_context._Identifier, compile_context._LastWriteTime);
}

/*
*	Runs after compilation in the specified domain has finished.
*/
void ScriptAssetCompiler::PostCompile(const CompilationDomain compilation_domain) NOEXCEPT
{
	//Check if we should compile scripts.
	if (_CompileScripts)
	{
		GenerateSourceFile(compilation_domain);
		GenerateHeaderFile(compilation_domain);
	}
}

/*
*	Adds an extra include.
*/
void ScriptAssetCompiler::AddExtraInclude(const char *const RESTRICT extra_include) NOEXCEPT
{
	_ExtraIncludes.Emplace(extra_include);
}

/*
*	Generates the source file.
*/
void ScriptAssetCompiler::GenerateSourceFile(const CompilationDomain compilation_domain) NOEXCEPT
{
	/*
	*	Script intermediate data
	*/
	class ScriptIntermediateData final
	{

	public:

		//The variables.
		DynamicArray<DynamicString> _Variables;

	};

	//Set up the intermediate data.
	ScriptIntermediateData intermediate_data;

	//Container for the arguments parsed.
	StaticArray<DynamicString, 8> arguments;

	//Figure out the directory path.
	char directory_path[MAXIMUM_FILE_PATH_LENGTH];

	switch (compilation_domain)
	{
		case CompilationDomain::ENGINE:
		{
			sprintf_s(directory_path, "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Source\\Generated");

			break;
		}

		case CompilationDomain::GAME:
		{
			sprintf_s(directory_path, "..\\..\\..\\Code\\Source\\Generated");

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Create the directory, if it doesn't exist.
	File::CreateDirectory(directory_path);

	//Figure out the file path.
	char file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(file_path, "%s\\Script.Generated.cpp%s", directory_path, DUMMY_FILE ? ".txt" : "");

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
	file << "#include <Scripting/ScriptCore.h>" << std::endl;
	file << "#include <Scripting/ScriptNodes.h>" << std::endl;
	file << std::endl;

	if (!_ExtraIncludes.Empty())
	{
		for (const DynamicString &extra_include : _ExtraIncludes)
		{
			file << "#include <" << extra_include.Data() << ">" << std::endl;
		}

		file << std::endl;
	}

	//Disable some warnings.
	file << "DISABLE_WARNING(4244)" << std::endl;
	file << std::endl;

	file << "#if SCRIPTS_ENABLED" << std::endl;
	file << std::endl;

	//Add some type aliases.
	file << "#define Vector3 Vector3<float32>" << std::endl;
	file << std::endl;

	for (ScriptData &script_data : _ScriptData)
	{
		//Add the namespace into the source file.
		file << "namespace " << script_data._Name.Data() << std::endl;
		file << "{" << std::endl;

		//Keep track of the current variable offset.
		uint64 current_variable_offset{ 0 };

		//Open the file.
		std::ifstream script_file{ script_data._FilePath.Data()};

		//Go through all the lines in the file.
		std::string current_line;

		while (std::getline(script_file, current_line))
		{
			//Skip comments.
			if (TextParsingUtilities::OnlyComment(current_line.c_str(), current_line.length()))
			{
				continue;
			}

			//Skip whitespace.
			if (TextParsingUtilities::OnlyWhitespace(current_line.c_str(), current_line.length()))
			{
				continue;
			}

			//Is this a variable declaration?
			{
				const size_t position{ current_line.find("Variable(") };

				if (position != std::string::npos)
				{
					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.Data()
						)
					};

					ASSERT(number_of_arguments == 2, "A variable declaration needs two arguments!");

					//Add the define.
					file << "\t#define " << arguments[1].Data() << " *(static_cast<" << arguments[0].Data() << " *const RESTRICT>(AdvancePointer(script_context._Data, " << current_variable_offset << ")))" << std::endl;

					//Update the current variable offset.
					current_variable_offset += SizeOfType(arguments[0].Data());

					//Add the variable.
					intermediate_data._Variables.Emplace(arguments[1]);

					continue;
				}
			}

			//Is this a function declaration?
			{
				const size_t position{ current_line.find("Function(") };

				if (position != std::string::npos)
				{
					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.Data()
						)
					};

					ASSERT(number_of_arguments >= 2, "A function declaration needs at least two arguments!");

					//Add function signature.
					file << "\t" << arguments[0].Data() << " " << arguments[1].Data() << "(ScriptContext &script_context";

					if (number_of_arguments > 2)
					{
						file << ", ";
					}

					for (uint64 i{ 2 }; i < number_of_arguments; i += 2)
					{
						file << arguments[i].Data() << " " << arguments[i + 1].Data();

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

					continue;
				}
			}

			//Recursively modify "Node()" function calls.
			{
				size_t position{ current_line.find("Node(") };

				while (position != std::string::npos)
				{
					uint64 end_position{ 0 };
					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.Data(),
							&end_position
						)
					};

					std::string modified_call = "Script::";
					modified_call += arguments[0].Data();
					modified_call += "(script_context";

					if (number_of_arguments > 1)
					{
						modified_call += ", ";
					}

					for (uint64 i{ 1 }; i < number_of_arguments; ++i)
					{
						modified_call += arguments[i].Data();

						if (i < (number_of_arguments - 1))
						{
							modified_call += ", ";
						}
					}

					modified_call += ")";

					//Replace the call in the current line.
					current_line.replace(position, end_position - position + 1, modified_call);

					position = current_line.find("Node(");
				}
			}

			//Otherwise, just add it to the source file and hope that it's valid C++! (:
			file << "\t" << current_line << std::endl;
		}

		//Close the script file.
		script_file.close();

		//Undefine all variables.
		for (const DynamicString& variable : intermediate_data._Variables)
		{
			file << "\t#undef " << variable.Data() << std::endl;
		}

		//Add the closing bracket for the namespace.
		file << "}" << std::endl;

		//Add the rest of the script data.
		script_data._RequiredDataSize = current_variable_offset;
	}

	//Close the file.
	file << "#endif";
	file.close();
}

/*
*	Generates the header file.
*/
void ScriptAssetCompiler::GenerateHeaderFile(const CompilationDomain compilation_domain) NOEXCEPT
{
	//Figure out the directory path.
	char directory_path[MAXIMUM_FILE_PATH_LENGTH];

	switch (compilation_domain)
	{
		case CompilationDomain::ENGINE:
		{
			sprintf_s(directory_path, "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Include\\Generated");

			break;
		}

		case CompilationDomain::GAME:
		{
			sprintf_s(directory_path, "..\\..\\..\\Code\\Include\\Generated");

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Create the directory, if it doesn't exist.
	File::CreateDirectory(directory_path);

	//Figure out the file path.
	char file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(file_path, "%s\\Script.Generated.h%s", directory_path, DUMMY_FILE ? ".txt" : "");

	//Open the file.
	std::ofstream file{ file_path };

	//Add header stuff.
	file << "#pragma once" << std::endl;
	file << std::endl;

	file << "//Core." << std::endl;
	file << "#include <Core/Essential/CatalystEssential.h>" << std::endl;
	file << std::endl;

	file << "//Scripting." << std::endl;
	file << "#include <Scripting/ScriptCore.h>" << std::endl;
	file << "#include <Scripting/ScriptContext.h>" << std::endl;
	file << std::endl;

	file << "//Systems." << std::endl;
	file << "#include <Systems/CatalystEngineSystem.h>" << std::endl;
	file << std::endl;

	//Add an enum for all the script identifiers.
	file << "enum class ScriptIdentifier : uint32" << std::endl;
	file << "{" << std::endl;

	for (uint64 i{ 0 }; i < _ScriptData.Size(); ++i)
	{
		file << "\t" << _ScriptData[i]._Name.Data() << " = " << i << "," << std::endl;
	}

	file << "\tNUMBER_OF_SCRIPT_IDENTIFIERS = " << _ScriptData.Size() << std::endl;

	file << "};" << std::endl;

	file << std::endl;

	file << "#if SCRIPTS_ENABLED" << std::endl;

	file << std::endl;

	//Add function declarations for all scripts.
	for (const ScriptData &script_data : _ScriptData)
	{
		file << "namespace " << script_data._Name.Data() << std::endl;
		file << "{" << std::endl;

		file << "\tconstexpr uint64 REQUIRED_DATA_SIZE{ " << script_data._RequiredDataSize << " };" << std::endl;

		if (TEST_BIT(script_data._Flags, SCRIPT_DATA_FLAG_HAS_INITIALIZE))
		{
			file << "\tvoid Initialize(ScriptContext &script_context);" << std::endl;
		}

		if (TEST_BIT(script_data._Flags, SCRIPT_DATA_FLAG_HAS_UPDATE))
		{
			if (TEST_BIT(script_data._Flags, SCRIPT_DATA_FLAG_UPDATE_WANTS_DELTA_TIME))
			{
				file << "\tvoid Update(ScriptContext &script_context, float32 delta_time);" << std::endl;
			}

			else
			{
				file << "\tvoid Update(ScriptContext &script_context);" << std::endl;
			}
		}

		if (TEST_BIT(script_data._Flags, SCRIPT_DATA_FLAG_HAS_TERMINATE))
		{
			file << "\tvoid Terminate(ScriptContext &script_context);" << std::endl;
		}

		file << "}" << std::endl;
	}

	file << std::endl;

	//Add function for getting the required data size.
	file << "FORCE_INLINE constexpr NO_DISCARD uint64 GetRequiredDataSize(const ScriptIdentifier script_identifier) NOEXCEPT" << std::endl;
	file << "{" << std::endl;

	file << "\tswitch (script_identifier)" << std::endl;
	file << "\t{" << std::endl;

	for (const ScriptData& script_data : _ScriptData)
	{
		file << "\t\tcase ScriptIdentifier::" << script_data._Name.Data() << ":" << std::endl;
		file << "\t\t{" << std::endl;
		file << "\t\t\treturn " << script_data._Name.Data() << "::REQUIRED_DATA_SIZE;" << std::endl;
		file << "\t\t}" << std::endl;
	}

	file << "\t}" << std::endl;

	file << "\treturn 0;" << std::endl;

	file << "}" << std::endl;

	//Add function for initializing scripts.
	file << "FORCE_INLINE void InitializeScript(const ScriptIdentifier script_identifier, ScriptContext &script_context) NOEXCEPT" << std::endl;
	file << "{" << std::endl;

	file << "\tswitch (script_identifier)" << std::endl;
	file << "\t{" << std::endl;

	for (const ScriptData &script_data : _ScriptData)
	{
		if (TEST_BIT(script_data._Flags, SCRIPT_DATA_FLAG_HAS_INITIALIZE))
		{
			file << "\t\tcase ScriptIdentifier::" << script_data._Name.Data() << ":" << std::endl;
			file << "\t\t{" << std::endl;
			file << "\t\t\t" << script_data._Name.Data() << "::Initialize(script_context);" << std::endl;
			file << "\t\t\t" << "break;" << std::endl;
			file << "\t\t}" << std::endl;
		}
	}

	file << "\t}" << std::endl;

	file << "}" << std::endl;

	//Add function for updating scripts.
	file << "FORCE_INLINE void UpdateScript(const ScriptIdentifier script_identifier, ScriptContext &script_context) NOEXCEPT" << std::endl;
	file << "{" << std::endl;

	file << "\tswitch (script_identifier)" << std::endl;
	file << "\t{" << std::endl;

	for (const ScriptData &script_data : _ScriptData)
	{
		if (TEST_BIT(script_data._Flags, SCRIPT_DATA_FLAG_HAS_UPDATE))
		{
			file << "\t\tcase ScriptIdentifier::" << script_data._Name.Data() << ":" << std::endl;
			file << "\t\t{" << std::endl;

			if (TEST_BIT(script_data._Flags, SCRIPT_DATA_FLAG_UPDATE_WANTS_DELTA_TIME))
			{
				file << "\t\t\t" << script_data._Name.Data() << "::Update(script_context, CatalystEngineSystem::Instance->GetDeltaTime());" << std::endl;
			}

			else
			{
				file << "\t\t\t" << script_data._Name.Data() << "::Update(script_context);" << std::endl;
			}

			file << "\t\t\t" << "break;" << std::endl;
			file << "\t\t}" << std::endl;
		}
	}

	file << "\t}" << std::endl;

	file << "}" << std::endl;

	//Add function for terminating scripts.
	file << "FORCE_INLINE void TerminateScript(const ScriptIdentifier script_identifier, ScriptContext &script_context) NOEXCEPT" << std::endl;
	file << "{" << std::endl;

	file << "\tswitch (script_identifier)" << std::endl;
	file << "\t{" << std::endl;

	for (const ScriptData &script_data : _ScriptData)
	{
		if (TEST_BIT(script_data._Flags, SCRIPT_DATA_FLAG_HAS_TERMINATE))
		{
			file << "\t\tcase ScriptIdentifier::" << script_data._Name.Data() << ":" << std::endl;
			file << "\t\t{" << std::endl;
			file << "\t\t\t" << script_data._Name.Data() << "::Terminate(script_context);" << std::endl;
			file << "\t\t\t" << "break;" << std::endl;
			file << "\t\t}" << std::endl;
		}
	}

	file << "\t}" << std::endl;

	file << "}" << std::endl;

	file << "#else" << std::endl;
	file << "FORCE_INLINE uint64 GetRequiredDataSize(const ScriptIdentifier script_identifier) { return 0; }" << std::endl;
	file << "FORCE_INLINE void InitializeScript(const ScriptIdentifier script_identifier, ScriptContext &script_context) { }" << std::endl;
	file << "FORCE_INLINE void UpdateScript(const ScriptIdentifier script_identifier, ScriptContext &script_context) { }" << std::endl;
	file << "FORCE_INLINE void TerminateScript(const ScriptIdentifier script_identifier, ScriptContext &script_context) { }" << std::endl;
	file << "#endif";

	//Close the file.
	file.close();
}