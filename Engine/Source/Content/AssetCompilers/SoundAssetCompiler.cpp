//Header file.
#include <Content/AssetCompilers/SoundAssetCompiler.h>

//File.
#include <File/Core/File.h>
#include <File/Core/BinaryOutputFile.h>
#include <File/Readers/WAVReader.h>
#include <File/Utilities/TextParsingUtilities.h>

//Profiling.
#include <Profiling/Profiling.h>

//Systems.
#include <Systems/TaskSystem.h>

//STL.
#include <fstream>
#include <string>

/*
*	Sound parameters class definition.
*/
class SoundParameters final
{

public:

	//The file.
	StaticString<MAXIMUM_FILE_PATH_LENGTH> _File;

};

/*
*	Default constructor.
*/
SoundAssetCompiler::SoundAssetCompiler() NOEXCEPT
{
	//Set the flags.
	_Flags = Flags::ALWAYS_COMPILE;
}

/*
*	Returns the asset type identifier.
*/
NO_DISCARD HashString SoundAssetCompiler::AssetTypeIdentifier() const NOEXCEPT
{
	return SoundAsset::TYPE_IDENTIFIER;
}

/*
*	Returns the current version.
*/
NO_DISCARD uint64 SoundAssetCompiler::CurrentVersion() const NOEXCEPT
{
	return 1;
}

/*
*	Compiles a single asset with the given compile context.
*/
void SoundAssetCompiler::Compile(const CompileContext &compile_context) NOEXCEPT
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
		SoundAssetCompiler::Instance->CompileInternal(static_cast<CompileData *const RESTRICT>(arguments));
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
NO_DISCARD Asset *const RESTRICT SoundAssetCompiler::Load(const LoadContext &load_context) NOEXCEPT
{
	//Allocate the asset.
	SoundAsset *const RESTRICT new_asset{ new (_AssetAllocator.Allocate()) SoundAsset() };

	//Set up the load data.
	LoadData *const RESTRICT load_data{ new (_LoadDataAllocator.Allocate()) LoadData() };

	load_data->_StreamArchivePosition = load_context._StreamArchivePosition;
	load_data->_StreamArchive = load_context._StreamArchive;
	load_data->_Asset = new_asset;

	//Set up the task.
	Task *const RESTRICT task{ static_cast<Task *const RESTRICT>(load_context._TaskAllocator->Allocate()) };

	task->_Function = [](void *const RESTRICT arguments)
	{
		SoundAssetCompiler::Instance->LoadInternal(static_cast<LoadData *const RESTRICT>(arguments));
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
void SoundAssetCompiler::CompileInternal(CompileData *const RESTRICT compile_data) NOEXCEPT
{
	PROFILING_SCOPE("SoundAssetCompiler::CompileInternal");

	//Set up the parameters.
	SoundParameters parameters;

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

	//Read the sound file.
	SoundAsset sound_asset;

	if (!WAVReader::Read(parameters._File.Data(), &sound_asset))
	{
		ASSERT(false, "Couldn't load sound file!");
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
	sprintf_s(directory_path, "%s\\Sounds", collection_directory_path);

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

	//Write the sample rate.
	output_file.Write(&sound_asset._SampleRate, sizeof(float32));

	//Write the number of channels.
	output_file.Write(&sound_asset._NumberOfChannels, sizeof(uint8));

	//Write the channel data.
	for (const DynamicArray<int16> &channel : sound_asset._Samples)
	{
		const uint64 number_of_samples{ channel.Size() };
		output_file.Write(&number_of_samples, sizeof(uint64));
		output_file.Write(channel.Data(), sizeof(int16) * number_of_samples);
	}

	//Close the output file.
	output_file.Close();
}

/*
*	Loads internally.
*/
void SoundAssetCompiler::LoadInternal(LoadData *const RESTRICT load_data) NOEXCEPT
{
	PROFILING_SCOPE("SoundAssetCompiler::LoadInternal");

	//Read the data.
	uint64 stream_archive_position{ load_data->_StreamArchivePosition };

	//Read the sample rate.
	load_data->_StreamArchive->Read(&load_data->_Asset->_SampleRate, sizeof(float32), &stream_archive_position);

	//Read the number of channels.
	load_data->_StreamArchive->Read(&load_data->_Asset->_NumberOfChannels, sizeof(uint8), &stream_archive_position);

	//Read the channel data.
	load_data->_Asset->_Samples.Upsize<true>(load_data->_Asset->_NumberOfChannels);

	for (DynamicArray<int16> &channel : load_data->_Asset->_Samples)
	{
		uint64 number_of_samples;
		load_data->_StreamArchive->Read(&number_of_samples, sizeof(uint64), &stream_archive_position);
		channel.Upsize<false>(number_of_samples);
		load_data->_StreamArchive->Read(channel.Data(), sizeof(int16) * number_of_samples, &stream_archive_position);
	}
}