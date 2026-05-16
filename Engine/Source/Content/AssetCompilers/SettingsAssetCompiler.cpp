//Header file.
#include <Content/AssetCompilers/SettingsAssetCompiler.h>

//File.
#include <File/Core/File.h>
#include <File/Core/BinaryOutputFile.h>

//Profiling.
#include <Profiling/Profiling.h>

//Settings.
#include <Settings/Settings.h>

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
	PROFILING_SCOPE("SettingsAssetCompiler::Compile");

	//Set up the stream archive.
	StreamArchive stream_archive;

	//Parse the JSON object.
	nlohmann::json JSON;

	{
		std::ifstream input_file{ compile_context._FilePath.Data() };
		input_file >> JSON;
		input_file.close();
	}

	//Serialize the settings to the stream archive.
	Settings::SerializeToStreamArchive(JSON, &stream_archive);

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
	sprintf_s(directory_path, "%s\\Settings", collection_directory_path);

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
*	Loads a single asset with the given load context.
*/
void SettingsAssetCompiler::Load(const LoadContext &load_context) NOEXCEPT
{
	PROFILING_SCOPE("SettingsAssetCompiler::Load");

	//Allocate the asset.
	SettingsAsset *const RESTRICT new_asset{ static_cast<SettingsAsset *const RESTRICT>(load_context._Asset) };

	//Read the data.
	uint64 stream_archive_position{ load_context._StreamArchivePosition };

	//Read the number of objects.
	load_context._StreamArchive->Read(&new_asset->_NumberOfObjects, sizeof(uint64), &stream_archive_position);

	//Read the stream archive size.
	uint64 stream_archive_size;
	load_context._StreamArchive->Read(&stream_archive_size, sizeof(uint64), &stream_archive_position);

	//Allocate the appropriate amount of data.
	new_asset->_StreamArchive.Resize(stream_archive_size);

	//Read the stream archive.
	load_context._StreamArchive->Read(new_asset->_StreamArchive.Data(), stream_archive_size, &stream_archive_position);
}