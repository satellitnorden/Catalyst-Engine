//Header file.
#include <Content/AssetCompilers/EntityAssetCompiler.h>

//File.
#include <File/Core/File.h>
#include <File/Core/BinaryOutputFile.h>

//Entities.
#include <Entities/Core/EntitySerialization.h>

//Profiling.
#include <Profiling/Profiling.h>

//Third party.
#include <ThirdParty/json/json.hpp>

//STL.
#include <fstream>

/*
*	Default constructor.
*/
EntityAssetCompiler::EntityAssetCompiler() NOEXCEPT
{
	//Set the flags.
	_Flags = Flags::NONE;
}

/*
*	Returns the asset type identifier.
*/
NO_DISCARD HashString EntityAssetCompiler::AssetTypeIdentifier() const NOEXCEPT
{
	return EntityAsset::TYPE_IDENTIFIER;
}

/*
*	Returns the current version.
*/
NO_DISCARD uint64 EntityAssetCompiler::CurrentVersion() const NOEXCEPT
{
	return 1;
}

/*
*	Compiles a single asset with the given compile context.
*/
void EntityAssetCompiler::Compile(const CompileContext &compile_context) NOEXCEPT
{
	PROFILING_SCOPE("EntityAssetCompiler::Compile");

	//Set up the stream archive.
	StreamArchive stream_archive;

	//Parse the JSON object.
	nlohmann::json JSON;

	{
		std::ifstream input_file{ compile_context._FilePath.Data() };
		input_file >> JSON;
		input_file.close();
	}

	//Read the entity statistics.
	EntityStatistics entity_statistics;

	if (JSON.contains("EntityStatistics"))
	{
		const nlohmann::json &entity_statistics_entry{ JSON["EntityStatistics"] };

		{
			const nlohmann::json &axis_aligned_bounding_box_entry{ entity_statistics_entry["AxisAlignedBoundingBox"] };

			{
				const nlohmann::json &minimum_entry{ axis_aligned_bounding_box_entry["Minimum"] };

				entity_statistics._AxisAlignedBoundingBox._Minimum._X = minimum_entry["X"];
				entity_statistics._AxisAlignedBoundingBox._Minimum._Y = minimum_entry["Y"];
				entity_statistics._AxisAlignedBoundingBox._Minimum._Z = minimum_entry["Z"];
			}

			{
				const nlohmann::json &maximum_entry{ axis_aligned_bounding_box_entry["Maximum"] };

				entity_statistics._AxisAlignedBoundingBox._Maximum._X = maximum_entry["X"];
				entity_statistics._AxisAlignedBoundingBox._Maximum._Y = maximum_entry["Y"];
				entity_statistics._AxisAlignedBoundingBox._Maximum._Z = maximum_entry["Z"];
			}
		}
	}

	//Serialize to the stream archive.
	EntitySerialization::SerializeToStreamArchive(JSON, &stream_archive);

	//Determine the collection directory.
	char collection_directory_path[MAXIMUM_FILE_PATH_LENGTH];

	if (compile_context._Collection)
	{
		sprintf_s(collection_directory_path, "%s\\COLLECTION %s", GetCompiledDirectoryPath(compile_context._CompilationDomain), compile_context._Collection.Data());
	}

	else
	{
		sprintf_s(collection_directory_path, "%s\\COLLECTION Default", GetCompiledDirectoryPath(compile_context._CompilationDomain));
	}

	//Create the compiled directory, if it doesn't exist.
	File::CreateDirectory(collection_directory_path);

	//Determine the directory path.
	char directory_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(directory_path, "%s\\Entities", collection_directory_path);

	//Create the compiled directory, if it doesn't exist.
	File::CreateDirectory(directory_path);

	//Determine the output file path.
	char output_file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(output_file_path, "%s\\%s.ca", directory_path, compile_context._Name.Data());

	//Open the output file.
	BinaryOutputFile output_file{ output_file_path };

	//Write the asset header to the file.
	AssetHeader asset_header{ AssetTypeIdentifier(), CurrentVersion(), HashString(compile_context._Name.Data()), compile_context._Name.Data() };
	output_file.Write(&asset_header, sizeof(AssetHeader));

	//Write the entity statistics.
	output_file.Write(&entity_statistics, sizeof(EntityStatistics));

	//Write the stream archive size.
	const uint64 stream_archive_size{ stream_archive.Size() };
	output_file.Write(&stream_archive_size, sizeof(uint64));

	//Write the stream archive data.
	output_file.Write(stream_archive.Data(), stream_archive_size);

	//Close the output file.
	output_file.Close();
}

/*
*	Loads a single asset with the given load context.
*/
void EntityAssetCompiler::Load(const LoadContext &load_context) NOEXCEPT
{
	PROFILING_SCOPE("EntityAssetCompiler::Load");

	//Allocate the asset.
	EntityAsset *const RESTRICT new_asset{ static_cast<EntityAsset *const RESTRICT>(load_context._Asset) };

	//Read the data.
	uint64 stream_archive_position{ load_context._StreamArchivePosition };

	//Read the entity statistics.
	load_context._StreamArchive->Read(&new_asset->_EntityStatistics, sizeof(EntityStatistics), &stream_archive_position);

	//Read the stream archive size.
	uint64 stream_archive_size;
	load_context._StreamArchive->Read(&stream_archive_size, sizeof(uint64), &stream_archive_position);

	//Allocate the necessary data for the stream archive.
	new_asset->_StreamArchive.Resize(stream_archive_size);

	//Read the stream archive data.
	load_context._StreamArchive->Read(new_asset->_StreamArchive.Data(), stream_archive_size, &stream_archive_position);
}