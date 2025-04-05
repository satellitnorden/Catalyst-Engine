//Header file.
#include <Content/AssetCompilers/SettingsAssetCompiler.h>

//File.
#include <File/Core/File.h>
#include <File/Core/BinaryOutputFile.h>

//Profiling.
#include <Profiling/Profiling.h>

//Settings.
#include <Settings/Settings.h>

//Systems.
#include <Systems/TaskSystem.h>

//Third party.
#include <ThirdParty/json/json.hpp>

//STL.
#include <fstream>

/*
*	Default constructor.
*/
SettingsAssetCompiler::SettingsAssetCompiler() NOEXCEPT
{
	//Set the flags.
	_Flags = Flags::NONE;
}

/*
*	Returns the asset type identifier.
*/
NO_DISCARD HashString SettingsAssetCompiler::AssetTypeIdentifier() const NOEXCEPT
{
	return SettingsAsset::TYPE_IDENTIFIER;
}

/*
*	Returns the current version.
*/
NO_DISCARD uint64 SettingsAssetCompiler::CurrentVersion() const NOEXCEPT
{
	return 1;
}

/*
*	Compiles a single asset with the given compile context.
*/
void SettingsAssetCompiler::Compile(const CompileContext &compile_context) NOEXCEPT
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

	//Set up the task.
	Task *const RESTRICT task{ static_cast<Task *const RESTRICT>(compile_context._TaskAllocator->Allocate()) };

	task->_Function = [](void *const RESTRICT arguments)
	{
		SettingsAssetCompiler::Instance->CompileInternal(static_cast<CompileData *const RESTRICT>(arguments));
	};
	task->_Arguments = compile_data;
	task->_ExecutableOnSameThread = false;

	//Execute the task!
	TaskSystem::Instance->ExecuteTask(Task::Priority::LOW, task);

	//Add the task to the list.
	compile_context._Tasks->Emplace(task);
}

/*
*	Loads a single asset with the given load context.
*/
NO_DISCARD Asset *const RESTRICT SettingsAssetCompiler::Load(const LoadContext &load_context) NOEXCEPT
{
	//Allocate the asset.
	SettingsAsset *const RESTRICT new_asset{ new (_AssetAllocator.Allocate()) SettingsAsset() };

	//Set up the load data.
	LoadData *const RESTRICT load_data{ new (_LoadDataAllocator.Allocate()) LoadData() };

	load_data->_StreamArchivePosition = load_context._StreamArchivePosition;
	load_data->_StreamArchive = load_context._StreamArchive;
	load_data->_Asset = new_asset;

	//Set up the task.
	Task *const RESTRICT task{ static_cast<Task *const RESTRICT>(load_context._TaskAllocator->Allocate()) };

	task->_Function = [](void *const RESTRICT arguments)
	{
		SettingsAssetCompiler::Instance->LoadInternal(static_cast<LoadData *const RESTRICT>(arguments));
	};
	task->_Arguments = load_data;
	task->_ExecutableOnSameThread = false;

	//Execute the task!
	TaskSystem::Instance->ExecuteTask(Task::Priority::LOW, task);

	//Add the task to the list.
	load_context._Tasks->Emplace(task);

	//Return the new asset!
	return new_asset;
}

/*
*	Compiles internally.
*/
void SettingsAssetCompiler::CompileInternal(CompileData *const RESTRICT compile_data) NOEXCEPT
{
	PROFILING_SCOPE("SettingsAssetCompiler::CompileInternal");

	//Set up the stream archive.
	StreamArchive stream_archive;

	//Parse the JSON object.
	nlohmann::json JSON;
	
	{
		std::ifstream input_file{ compile_data->_FilePath.Data() };
		input_file >> JSON;
		input_file.close();
	}

	//Serialize the settings to the stream archive.
	Settings::SerializeToStreamArchive(JSON, &stream_archive);

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
	sprintf_s(directory_path, "%s\\Settings", collection_directory_path);

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

	//Write the number of objects.
	const uint64 number_of_objects{ JSON.size() };
	output_file.Write(&number_of_objects, sizeof(uint64));

	//Write the stream archive size.
	const uint64 stream_archive_size{ stream_archive.Size() };
	output_file.Write(&stream_archive_size, sizeof(uint64));

	//Write the stream archive data.
	output_file.Write(stream_archive.Data(), stream_archive_size);

	//Close the output file.
	output_file.Close();
}

/*
*	Loads internally.
*/
void SettingsAssetCompiler::LoadInternal(LoadData *const RESTRICT load_data) NOEXCEPT
{
	PROFILING_SCOPE("SettingsAssetCompiler::LoadInternal");

	//Read the data.
	uint64 stream_archive_position{ load_data->_StreamArchivePosition };

	//Read the number of objects.
	load_data->_StreamArchive->Read(&load_data->_Asset->_NumberOfObjects, sizeof(uint64), &stream_archive_position);

	//Read the stream archive size.
	uint64 stream_archive_size;
	load_data->_StreamArchive->Read(&stream_archive_size, sizeof(uint64), &stream_archive_position);

	//Allocate the appropriate amount of data.
	load_data->_Asset->_StreamArchive.Resize(stream_archive_size);

	//Read the stream archive.
	load_data->_StreamArchive->Read(load_data->_Asset->_StreamArchive.Data(), stream_archive_size, &stream_archive_position);
}