//Header file.
#include <Content/AssetCompilers/RenderPipelineAssetCompiler.h>

//File.
#include <File/Core/File.h>
#include <File/Utilities/TextParsingUtilities.h>

//Profiling.
#include <Profiling/Profiling.h>

//Systems.
#include <Systems/LogSystem.h>
#include <Systems/TaskSystem.h>

//STD.
#include <filesystem>
#include <fstream>
#include <string>

//Defines.
#define ENGINE_RENDERING_PATH "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Rendering"
#define GAME_RENDERING_PATH "..\\..\\..\\Rendering"

#define COMPILE_SINGLE_THREADED (1)
#define LOAD_SINGLE_THREADED (1)

/*
*	Default constructor.
*/
RenderPipelineAssetCompiler::RenderPipelineAssetCompiler() NOEXCEPT
{
	//Set the flags.
	_Flags = Flags::ALWAYS_COMPILE;
}

/*
*	Returns the asset type identifier.
*/
NO_DISCARD HashString RenderPipelineAssetCompiler::AssetTypeIdentifier() const NOEXCEPT
{
	return RenderPipelineAsset::TYPE_IDENTIFIER;
}

/*
*	Returns the current version.
*/
NO_DISCARD uint64 RenderPipelineAssetCompiler::CurrentVersion() const NOEXCEPT
{
	return 1;
}

/*
*	Compiles a single asset with the given compile context.
*/
void RenderPipelineAssetCompiler::Compile(const CompileContext &compile_context) NOEXCEPT
{
	//Set up the compile data.
	CompileData *const RESTRICT compile_data{ new (_CompileDataAllocator.Allocate()) CompileData() };

	//Set the collection.
	compile_data->_Collection = compile_context._Collection;

	//Set the file path.
	compile_data->_FilePath = compile_context._FilePath;

	//Set the name.
	compile_data->_Name = compile_context._Name;

	//Set the compilation domain.
	compile_data->_CompilationDomain = compile_context._CompilationDomain;

#if COMPILE_SINGLE_THREADED
	CompileInternal(compile_data);
#else
	//Set up the task.
	Task *const RESTRICT task{ static_cast<Task *const RESTRICT>(compile_context._TaskAllocator->Allocate()) };

	task->_Function = [](void *const RESTRICT arguments)
	{
		RenderPipelineAssetCompiler::Instance->CompileInternal(static_cast<CompileData *const RESTRICT>(arguments));
	};
	task->_Arguments = compile_data;
	task->_ExecutableOnSameThread = false;

	//Execute the task!
	TaskSystem::Instance->ExecuteTask(Task::Priority::LOW, task);

	//Add the task to the list.
	compile_context._Tasks->Emplace(task);
#endif
}

/*
*	Loads a single asset with the given load context.
*/
NO_DISCARD Asset *const RESTRICT RenderPipelineAssetCompiler::Load(const LoadContext &load_context) NOEXCEPT
{
	//Allocate the asset.
	RenderPipelineAsset *const RESTRICT new_asset{ new (_AssetAllocator.Allocate()) RenderPipelineAsset() };

	//Set up the load data.
	LoadData *const RESTRICT load_data{ new (_LoadDataAllocator.Allocate()) LoadData() };

	load_data->_StreamArchivePosition = load_context._StreamArchivePosition;
	load_data->_StreamArchive = load_context._StreamArchive;
	load_data->_Asset = new_asset;

#if LOAD_SINGLE_THREADED
	LoadInternal(load_data);
#else
	//Set up the task.
	Task *const RESTRICT task{ static_cast<Task *const RESTRICT>(load_context._TaskAllocator->Allocate()) };

	task->_Function = [](void *const RESTRICT arguments)
	{
		RenderPipelineAssetCompiler::Instance->LoadInternal(static_cast<LoadData *const RESTRICT>(arguments));
	};
	task->_Arguments = load_data;
	task->_ExecutableOnSameThread = false;

	//Execute the task!
	TaskSystem::Instance->ExecuteTask(Task::Priority::LOW, task);

	//Add the task to the list.
	load_context._Tasks->Emplace(task);
#endif

	//Return the new asset!
	return new_asset;
}

/*
*	Compiles internally.
*/
void RenderPipelineAssetCompiler::CompileInternal(CompileData *const RESTRICT compile_data) NOEXCEPT
{
	PROFILING_SCOPE("RenderPipelineAssetCompiler::CompileInternal");

	//Retrieve all the lines from the file.
	DynamicArray<DynamicString> lines;
	RetrieveLines(compile_data->_FilePath.Data(), &lines);

	//Resolve includes.
	ResolveIncludes(&lines);

	//Strip comments.
	StripComments(&lines);

	//Write it out to a debug directory, for now.
	{
		std::filesystem::create_directories("RenderPipelineAssetCompiler Debug");

		std::string file_path{ compile_data->_FilePath.Data() };
		file_path = file_path.substr(file_path.find_last_of('\\'));
		file_path = std::string("RenderPipelineAssetCompiler Debug\\") + file_path;

		std::ofstream file{ file_path };

		for (const DynamicString &line : lines)
		{
			file << line.Data() << std::endl;
		}

		file.close();
	}

	//BREAKPOINT();
}

/*
*	Loads internally.
*/
void RenderPipelineAssetCompiler::LoadInternal(LoadData *const RESTRICT load_data) NOEXCEPT
{
	PROFILING_SCOPE("RenderPipelineAssetCompiler::LoadInternal");

	//Read the data.
	uint64 stream_archive_position{ load_data->_StreamArchivePosition };
}

/*
*	Retrieves all lines from the given file.
*/
void RenderPipelineAssetCompiler::RetrieveLines(const char *const RESTRICT file_path, DynamicArray<DynamicString> *const RESTRICT lines) NOEXCEPT
{
	std::ifstream file{ file_path };

	std::string line;

	while (std::getline(file, line))
	{
		lines->Emplace(line.c_str());
	}

	file.close();
}

/*
*	Strips comments out of the given lines.
*/
void RenderPipelineAssetCompiler::StripComments(DynamicArray<DynamicString> *const RESTRICT lines) NOEXCEPT
{
	for (uint64 i{ 0 }; i < lines->Size();)
	{
		if (TextParsingUtilities::OnlyComment(lines->At(i).Data(), lines->At(i).Length()))
		{
			lines->EraseAt<true>(i);
		}

		else
		{
			++i;
		}
	}
}

/*
*	Searches for the given file, first in the engine directory, then the game directory.
*/
NO_DISCARD bool RenderPipelineAssetCompiler::SearchForFile
(
	const char *const RESTRICT engine_directory_path,
	const char *const RESTRICT game_directory_path,
	char *const RESTRICT file_path
) NOEXCEPT
{
	//Keep a buffer around.
	char buffer[MAXIMUM_FILE_PATH_LENGTH];

	//Check if the file exists in the engine directory path.
	sprintf_s(buffer, "%s\\%s", engine_directory_path, file_path);

	if (File::Exists(buffer))
	{
		sprintf_s(file_path, MAXIMUM_FILE_PATH_LENGTH, "%s", buffer);
		return true;
	}

	//Check if the file exists in the game directory path.
	sprintf_s(buffer, "%s\\%s", game_directory_path, file_path);

	if (File::Exists(buffer))
	{
		sprintf_s(file_path, MAXIMUM_FILE_PATH_LENGTH, "%s", buffer);
		return true;
	}

	//File could not be found!
	return false;
}

/*
*	Resolves an include.
*/
void RenderPipelineAssetCompiler::ResolveInclude
(
	const DynamicString &current_line,
	const char *const RESTRICT extension,
	const char *const RESTRICT sub_folder,
	DynamicArray<DynamicString> *const RESTRICT lines,
	const uint64 current_line_index
) NOEXCEPT
{
	//Retrieve the argument.
	DynamicString argument;

	const uint64 number_of_arguments_parsed
	{
		TextParsingUtilities::ParseFunctionArguments
		(
			current_line.Data(),
			current_line.Length(),
			&argument
		)
	};
	ASSERT(number_of_arguments_parsed == 1, "Invalid number of arguments!");

	//Retrieve the file path.
	char file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(file_path, "%s.%s", argument.Data(), extension);

	//Search for the file in both the engine and game directories.
	char engine_directory_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(engine_directory_path, "%s\\%s", ENGINE_RENDERING_PATH, sub_folder);

	char game_directory_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(game_directory_path, "%s\\%s", GAME_RENDERING_PATH, sub_folder);

	const bool found_file
	{
		SearchForFile
		(
			engine_directory_path,
			game_directory_path,
			file_path
		)
	};
	ASSERT(found_file, "Couldn't find file!");

	//Retrieve the include lines.
	DynamicArray<DynamicString> include_lines;
	RetrieveLines(file_path, &include_lines);

	//Erase the line, don't need it anymore.
	lines->EraseAt<true>(current_line_index);

	//Remember the original size.
	const uint64 original_size{ lines->Size() };

	//Make space for the new lines.
	lines->Resize<true>(lines->Size() + include_lines.Size());

	//Move the original lines.
	for (int64 original_line_index{ static_cast<int64>(original_size) - 1 }; original_line_index >= static_cast<int64>(current_line_index); --original_line_index)
	{
		lines->At(original_line_index + include_lines.Size()) = lines->At(original_line_index);
	}

	//Copy in the include lines.
	for (uint64 include_index{ 0 }; include_index < include_lines.Size(); ++include_index)
	{
		lines->At(current_line_index + include_index) = include_lines.At(include_index);
	}
}

/*
*	Resolves includes.
*/
void RenderPipelineAssetCompiler::ResolveIncludes(DynamicArray<DynamicString> *const RESTRICT lines) NOEXCEPT
{
	//Iterate over all lines.
	for (uint64 current_line_index{ 0 }; current_line_index < lines->Size();)
	{
		//Cache the current line.
		const DynamicString &current_line{ lines->At(current_line_index) };

		//Resolve common vertex shader includes.
		if (current_line.Find("IncludeCommonVertexShader("))
		{
			ResolveInclude
			(
				current_line,
				"common_shader",
				"Common Shaders",
				lines,
				current_line_index
			);
		}

		//Resolve common ray closest hit includes.
		else if (current_line.Find("IncludeCommonRayClosestHitShader("))
		{
			ResolveInclude
			(
				current_line,
				"common_shader",
				"Common Shaders",
				lines,
				current_line_index
			);
		}

		//Resolve shader function library includes.
		else if (current_line.Find("IncludeShaderFunctionLibrary("))
		{
			ResolveInclude
			(
				current_line,
				"shader_function_library",
				"Shader Function Libraries",
				lines,
				current_line_index
			);
		}

		//Resolve storage buffer includes.
		else if (current_line.Find("IncludeStorageBuffer("))
		{
			ResolveInclude
			(
				current_line,
				"storage_buffer_definition",
				"Storage Buffer Definitions",
				lines,
				current_line_index
			);
		}

		//Resolve uniform buffer includes.
		else if (current_line.Find("IncludeUniformBuffer("))
		{
			ResolveInclude
			(
				current_line,
				"uniform_buffer_definition",
				"Uniform Buffer Definitions",
				lines,
				current_line_index
			);
		}

		//Otherwise, increment the current line index.
		else
		{
			++current_line_index;
		}
	}
}