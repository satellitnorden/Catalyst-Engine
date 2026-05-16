//Header file.
#include <Content/AssetCompilers/RawDataAssetCompiler.h>

//File.
#include <File/Core/File.h>
#include <File/Core/BinaryInputFile.h>
#include <File/Core/BinaryOutputFile.h>
#include <File/Utilities/TextParsingUtilities.h>

//Profiling.
#include <Profiling/Profiling.h>

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
	PROFILING_SCOPE("RawDataAssetCompiler::Compile");

	//Set up the parameters.
	RawDataParameters parameters;

	//Open the input file.
	std::ifstream input_file{ compile_context._FilePath.Data() };

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
	sprintf_s(directory_path, "%s\\RawData", collection_directory_path);

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

	//Write the data size.
	const uint64 data_size{ data.Size() };
	output_file.Write(&data_size, sizeof(uint64));

	//Write the data.
	output_file.Write(data.Data(), data_size);

	//Close the output file.
	output_file.Close();
}

/*
*	Loads a single asset with the given load context.
*/
void RawDataAssetCompiler::Load(const LoadContext &load_context) NOEXCEPT
{
	PROFILING_SCOPE("RawDataAssetCompiler::Load");

	//Allocate the asset.
	RawDataAsset *const RESTRICT new_asset{ static_cast<RawDataAsset *const RESTRICT>(load_context._Asset) };

	//Read the data.
	uint64 stream_archive_position{ load_context._StreamArchivePosition };

	//Read the data size.
	uint64 data_size{ 0 };
	load_context._StreamArchive->Read(&data_size, sizeof(uint64), &stream_archive_position);

	//Read the data.
	new_asset->_Data.Upsize<false>(data_size);
	load_context._StreamArchive->Read(new_asset->_Data.Data(), data_size, &stream_archive_position);

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Update the total CPU/GPU memory.
	{
		uint64 cpu_memory{ 0 };

		cpu_memory += data_size; //Data

		_TotalCPUMemory.FetchAdd(cpu_memory);
	}
#endif
}