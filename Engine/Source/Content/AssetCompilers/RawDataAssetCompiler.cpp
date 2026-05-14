//Header file.
#include <Content/AssetCompilers/RawDataAssetCompiler.h>

//File.
#include <File/Core/File.h>
#include <File/Core/BinaryInputFile.h>
#include <File/Core/BinaryOutputFile.h>
#include <File/Utilities/TextParsingUtilities.h>

//Profiling.
#include <Profiling/Profiling.h>

//Systems.
#include <Systems/TaskSystem.h>

//STL.
#include <fstream>
#include <string>

/*
*	Raw data parameters class definition.
*/
class RawDataParameters final
{

public:

	//The file.
	StaticString<MAXIMUM_FILE_PATH_LENGTH> _File;

};

/*
*	Default constructor.
*/
RawDataAssetCompiler::RawDataAssetCompiler() NOEXCEPT
{
	//Set the flags.
	_Flags = Flags::NONE;
}

/*
*	Returns the asset type identifier.
*/
NO_DISCARD HashString RawDataAssetCompiler::AssetTypeIdentifier() const NOEXCEPT
{
	return RawDataAsset::TYPE_IDENTIFIER;
}

/*
*	Returns the current version.
*/
NO_DISCARD uint64 RawDataAssetCompiler::CurrentVersion() const NOEXCEPT
{
	return 1;
}

/*
*	Compiles a single asset with the given compile context.
*/
void RawDataAssetCompiler::Compile(const CompileContext &compile_context) NOEXCEPT
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

#if SINGLE_THREADED
	CompileInternal(compile_data);
#else
	//Set up the task.
	Task *const RESTRICT task{ static_cast<Task *const RESTRICT>(compile_context._TaskAllocator->Allocate()) };

	task->_Function = [](void *const RESTRICT arguments)
	{
		RawDataAssetCompiler::Instance->CompileInternal(static_cast<CompileData *const RESTRICT>(arguments));
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
NO_DISCARD Asset *const RESTRICT RawDataAssetCompiler::Load(const LoadContext &load_context) NOEXCEPT
{
	//Allocate the asset.
	RawDataAsset *const RESTRICT new_asset{ new (_AssetAllocator.Allocate()) RawDataAsset() };

	//Set up the load data.
	LoadData *const RESTRICT load_data{ new (_LoadDataAllocator.Allocate()) LoadData() };

	load_data->_StreamArchivePosition = load_context._StreamArchivePosition;
	load_data->_StreamArchive = load_context._StreamArchive;
	load_data->_Asset = new_asset;

#if SINGLE_THREADED
	LoadInternal(load_data);
#else
	//Set up the task.
	Task *const RESTRICT task{ static_cast<Task *const RESTRICT>(load_context._TaskAllocator->Allocate()) };

	task->_Function = [](void *const RESTRICT arguments)
	{
		RawDataAssetCompiler::Instance->LoadInternal(static_cast<LoadData *const RESTRICT>(arguments));
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
void RawDataAssetCompiler::CompileInternal(CompileData *const RESTRICT compile_data) NOEXCEPT
{
	PROFILING_SCOPE("RawDataAssetCompiler::CompileInternal");

	//Set up the parameters.
	RawDataParameters parameters;

	//Open the input file.
	std::ifstream input_file{ compile_data->_FilePath.Data() };

	//Iterate over the lines and fill in the parameters.
	{
		StaticArray<DynamicString, 1> arguments;
		std::string current_line;

		while (std::getline(input_file, current_line))
		{
			//Is this a file declaration?
			{
				const size_t position{ current_line.find("File(") };

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

					ASSERT(number_of_arguments == 1, "File() needs one argument!");

					parameters._File = arguments[0].Data();

					continue;
				}
			}

			//Couldn't figure out what this line is?
			ASSERT(false, "Unknown line %s", current_line.c_str());
		}
	}

	//Close the input file.
	input_file.close();

	//Read the data.
	DynamicArray<byte> data;

	{
		BinaryInputFile file{ parameters._File.Data() };

		data.Upsize<false>(file.Size());
		file.Read(data.Data(), file.Size());

		file.Close();
	}

	//Determine the collection directory.
	char collection_directory_path[MAXIMUM_FILE_PATH_LENGTH];

	if (compile_data->_Collection)
	{
		sprintf_s(collection_directory_path, "%s\\COLLECTION %s", GetCompiledDirectoryPath(compile_data->_CompilationDomain), compile_data->_Collection.Data());
	}

	else
	{
		sprintf_s(collection_directory_path, "%s\\COLLECTION Default", GetCompiledDirectoryPath(compile_data->_CompilationDomain));
	}

	//Create the compiled directory, if it doesn't exist.
	File::CreateDirectory(collection_directory_path);

	//Determine the directory path.
	char directory_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(directory_path, "%s\\RawData", collection_directory_path);

	//Create the compiled directory, if it doesn't exist.
	File::CreateDirectory(directory_path);
	
	//Determine the output file path.
	char output_file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(output_file_path, "%s\\%s.ca", directory_path, compile_data->_Name.Data());

	//Open the output file.
	BinaryOutputFile output_file{ output_file_path };

	//Write the asset header to the file.
	AssetHeader asset_header{ AssetTypeIdentifier(), CurrentVersion(), HashString(compile_data->_Name.Data()), compile_data->_Name.Data() };
	output_file.Write(&asset_header, sizeof(AssetHeader));

	//Write the data size.
	const uint64 data_size{ data.Size() };
	output_file.Write(&data_size, sizeof(uint64));

	//Write the data.
	output_file.Write(data.Data(), data_size);

	//Close the output file.
	output_file.Close();
}

/*
*	Loads internally.
*/
void RawDataAssetCompiler::LoadInternal(LoadData *const RESTRICT load_data) NOEXCEPT
{
	PROFILING_SCOPE("RawDataAssetCompiler::LoadInternal");

	//Read the data.
	uint64 stream_archive_position{ load_data->_StreamArchivePosition };

	//Read the data size.
	uint64 data_size{ 0 };
	load_data->_StreamArchive->Read(&data_size, sizeof(uint64), &stream_archive_position);

	//Read the data.
	load_data->_Asset->_Data.Upsize<false>(data_size);
	load_data->_StreamArchive->Read(load_data->_Asset->_Data.Data(), data_size, &stream_archive_position);

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Update the total CPU/GPU memory.
	{
		uint64 cpu_memory{ 0 };

		cpu_memory += data_size; //Data

		_TotalCPUMemory.FetchAdd(cpu_memory);
	}
#endif
}