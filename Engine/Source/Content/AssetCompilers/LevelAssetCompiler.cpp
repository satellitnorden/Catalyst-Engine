//Header file.
#include <Content/AssetCompilers/LevelAssetCompiler.h>

//Components.
#include <Components/Core/Component.h>

//File.
#include <File/Core/File.h>
#include <File/Core/BinaryOutputFile.h>

//Entities.
#include <Entities/Core/EntitySerialization.h>

//Profiling.
#include <Profiling/Profiling.h>

//Systems.
#include <Systems/TaskSystem.h>

//World.
#include <World/Core/WorldTransform.h>

//Third party.
#include <ThirdParty/json/json.hpp>

//STL.
#include <fstream>

/*
*	Default constructor.
*/
LevelAssetCompiler::LevelAssetCompiler() NOEXCEPT
{
	//Set the flags.
	_Flags = Flags::NONE;
}

/*
*	Returns the asset type identifier.
*/
NO_DISCARD HashString LevelAssetCompiler::AssetTypeIdentifier() const NOEXCEPT
{
	return LevelAsset::TYPE_IDENTIFIER;
}

/*
*	Returns the current version.
*/
NO_DISCARD uint64 LevelAssetCompiler::CurrentVersion() const NOEXCEPT
{
	return 1;
}

/*
*	Compiles a single asset with the given compile context.
*/
void LevelAssetCompiler::Compile(const CompileContext &compile_context) NOEXCEPT
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
		LevelAssetCompiler::Instance->CompileInternal(static_cast<CompileData *const RESTRICT>(arguments));
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
NO_DISCARD Asset *const RESTRICT LevelAssetCompiler::Load(const LoadContext &load_context) NOEXCEPT
{
	//Allocate the asset.
	LevelAsset *const RESTRICT new_asset{ new (_AssetAllocator.Allocate()) LevelAsset() };

	//Set up the load data.
	LoadData *const RESTRICT load_data{ new (_LoadDataAllocator.Allocate()) LoadData() };

	load_data->_StreamArchivePosition = load_context._StreamArchivePosition;
	load_data->_StreamArchive = load_context._StreamArchive;
	load_data->_Asset = new_asset;

	//Set up the task.
	Task *const RESTRICT task{ static_cast<Task *const RESTRICT>(load_context._TaskAllocator->Allocate()) };

	task->_Function = [](void *const RESTRICT arguments)
	{
		LevelAssetCompiler::Instance->LoadInternal(static_cast<LoadData *const RESTRICT>(arguments));
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
void LevelAssetCompiler::CompileInternal(CompileData *const RESTRICT compile_data) NOEXCEPT
{
	PROFILING_SCOPE("LevelAssetCompiler::CompileInternal");

	//Set up the stream archive.
	StreamArchive stream_archive;

	//Parse the JSON object.
	nlohmann::json JSON;
	
	{
		std::ifstream input_file{ compile_data->_FilePath.Data() };
		input_file >> JSON;
		input_file.close();
	}

	//Read the level statistics.
	LevelStatistics level_statistics;

	if (JSON.contains("LevelStatistics"))
	{
		const nlohmann::json &level_statistics_entry{ JSON["LevelStatistics"] };

		{
			const nlohmann::json &axis_aligned_bounding_box_entry{ level_statistics_entry["AxisAlignedBoundingBox"] };

			{
				const nlohmann::json &minimum_entry{ axis_aligned_bounding_box_entry["Minimum"] };

				level_statistics._AxisAlignedBoundingBox._Minimum._X = minimum_entry["X"];
				level_statistics._AxisAlignedBoundingBox._Minimum._Y = minimum_entry["Y"];
				level_statistics._AxisAlignedBoundingBox._Minimum._Z = minimum_entry["Z"];
			}

			{
				const nlohmann::json &maximum_entry{ axis_aligned_bounding_box_entry["Maximum"] };

				level_statistics._AxisAlignedBoundingBox._Maximum._X = maximum_entry["X"];
				level_statistics._AxisAlignedBoundingBox._Maximum._Y = maximum_entry["Y"];
				level_statistics._AxisAlignedBoundingBox._Maximum._Z = maximum_entry["Z"];
			}
		}

		level_statistics._Radius = level_statistics_entry["Radius"];
	}

	//Cache the entities JSON object.
	const nlohmann::json &entities{ JSON["Entities"] };

	//Count the number of entities.
	uint64 number_of_entities{ 0 };

	for (auto entity_iterator{ entities.begin() }; entity_iterator != entities.end(); ++entity_iterator)
	{
		//Cache the entity entry.
		const nlohmann::json &entity_entry{ *entity_iterator };

		//Figure out if this entity should be serialized.
		bool serialize{ true };

		if (entity_entry.contains("EditorData"))
		{
			const nlohmann::json &editor_data_entry{ entity_entry["EditorData"] };

			if (editor_data_entry.contains("Serialize"))
			{
				serialize = editor_data_entry["Serialize"];
			}
		}

		if (serialize)
		{
			++number_of_entities;
		}
	}


	//Write the number of entities.
	stream_archive.Write(&number_of_entities, sizeof(uint64));

	//Write all the entries!
	for (auto entity_iterator{ entities.begin() }; entity_iterator != entities.end(); ++entity_iterator)
	{
		//Cache the entity entry.
		const nlohmann::json &entity_entry{ *entity_iterator };

		//Figure out if this entity should be serialized.
		bool serialize{ true };

		if (entity_entry.contains("EditorData"))
		{
			const nlohmann::json &editor_data_entry{ entity_entry["EditorData"] };

			if (editor_data_entry.contains("Serialize"))
			{
				serialize = editor_data_entry["Serialize"];
			}
		}

		if (!serialize)
		{
			continue;
		}

		//Write the identifier.
		const uint64 identifier{ entity_entry["Identifier"] };
		stream_archive.Write(&identifier, sizeof(uint64));

		//Serialize to the stream archive.
		EntitySerialization::SerializeToStreamArchive(entity_entry, &stream_archive);
	}

	//Serialize entity links.
	{
		//Cache the entity links entry.
		const nlohmann::json &entity_links_entry{ JSON["EntityLinks"] };

		//Write the number of entity links.
		const uint64 number_of_entity_links{ entity_links_entry.size() };

		stream_archive.Write(&number_of_entity_links, sizeof(uint64));

		//Serialize the entity links.
		for (const nlohmann::json &entity_link : entity_links_entry)
		{
			const uint64 from{ entity_link["From"] };
			const uint64 to{ entity_link["To"] };

			stream_archive.Write(&from, sizeof(uint64));
			stream_archive.Write(&to, sizeof(uint64));
		}
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
	sprintf_s(directory_path, "%s\\Levels", collection_directory_path);

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

	//Write the level statistics.
	output_file.Write(&level_statistics, sizeof(LevelStatistics));

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
void LevelAssetCompiler::LoadInternal(LoadData *const RESTRICT load_data) NOEXCEPT
{
	PROFILING_SCOPE("LevelAssetCompiler::LoadInternal");

	//Read the data.
	uint64 stream_archive_position{ load_data->_StreamArchivePosition };

	//Read the level statistics.
	load_data->_StreamArchive->Read(&load_data->_Asset->_LevelStatistics, sizeof(LevelStatistics), &stream_archive_position);

	//Read the stream archive size.
	uint64 stream_archive_size;
	load_data->_StreamArchive->Read(&stream_archive_size, sizeof(uint64), &stream_archive_position);

	//Allocate the necessary data for the stream archive.
	load_data->_Asset->_StreamArchive.Resize(stream_archive_size);

	//Read the stream archive data.
	load_data->_StreamArchive->Read(load_data->_Asset->_StreamArchive.Data(), stream_archive_size, &stream_archive_position);
}