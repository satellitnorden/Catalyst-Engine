//Header file.
#include <Content/AssetCompilers/AudioAssetCompiler.h>

//File.
#include <File/Core/File.h>
#include <File/Core/BinaryOutputFile.h>
#include <File/Readers/WAVReader.h>
#include <File/Utilities/TextParsingUtilities.h>

//Profiling.
#include <Profiling/Profiling.h>

//STL.
#include <fstream>
#include <string>

/*
*	Audio parameters class definition.
*/
class AudioParameters final
{

public:

	//The file.
	StaticString<MAXIMUM_FILE_PATH_LENGTH> _File;

};

/*
*	Default constructor.
*/
AudioAssetCompiler::AudioAssetCompiler() NOEXCEPT
{
	//Set the flags.
	_Flags = Flags::NONE;
}

/*
*	Returns the asset type identifier.
*/
NO_DISCARD HashString AudioAssetCompiler::AssetTypeIdentifier() const NOEXCEPT
{
	return AudioAsset::TYPE_IDENTIFIER;
}

/*
*	Returns the current version.
*/
NO_DISCARD uint64 AudioAssetCompiler::CurrentVersion() const NOEXCEPT
{
	return 1;
}

/*
*	Compiles a single asset with the given compile context.
*/
void AudioAssetCompiler::Compile(const CompileContext &compile_context) NOEXCEPT
{
	PROFILING_SCOPE("AudioAssetCompiler::Compile");

	//Set up the parameters.
	AudioParameters parameters;

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

	//Read the audio stream.
	AudioStream audio_stream;

	if (!WAVReader::Read(parameters._File.Data(), &audio_stream))
	{
		ASSERT(false, "Couldn't load audio file!");
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
	sprintf_s(directory_path, "%s\\Audio", collection_directory_path);

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

	//Write the sample rate.
	const uint32 sample_rate{ audio_stream.GetSampleRate() };
	output_file.Write(&sample_rate, sizeof(uint32));

	//Write the number of channels.
	const uint8 number_of_channels{ audio_stream.GetNumberOfChannels() };
	output_file.Write(&number_of_channels, sizeof(uint8));

	//Write the format.
	const Audio::Format format{ audio_stream.GetFormat() };
	output_file.Write(&format, sizeof(Audio::Format));

	//Write the number of samples.
	const uint32 number_of_samples{ audio_stream.GetNumberOfSamples() };
	output_file.Write(&number_of_samples, sizeof(uint32));

	//Write the data.
	output_file.Write(audio_stream.GetData(), audio_stream.GetDataSize());

	//Close the output file.
	output_file.Close();
}

/*
*	Loads a single asset with the given load context.
*/
void AudioAssetCompiler::Load(const LoadContext &load_context) NOEXCEPT
{
	PROFILING_SCOPE("AudioAssetCompiler::Load");

	//Allocate the asset.
	AudioAsset *const RESTRICT new_asset{ static_cast<AudioAsset *const RESTRICT>(load_context._Asset) };

	//Read the data.
	uint64 stream_archive_position{ load_context._StreamArchivePosition };

	//Read the sample rate.
	uint32 sample_rate;
	load_context._StreamArchive->Read(&sample_rate, sizeof(uint32), &stream_archive_position);
	new_asset->_AudioStream.SetSampleRate(sample_rate);

	//Read the number of channels.
	uint8 number_of_channels;
	load_context._StreamArchive->Read(&number_of_channels, sizeof(uint8), &stream_archive_position);
	new_asset->_AudioStream.SetNumberOfChannels(number_of_channels);

	//Read the format.
	Audio::Format format;
	load_context._StreamArchive->Read(&format, sizeof(Audio::Format), &stream_archive_position);
	new_asset->_AudioStream.SetFormat(format);

	//Read the number of samples.
	uint32 number_of_samples;
	load_context._StreamArchive->Read(&number_of_samples, sizeof(uint32), &stream_archive_position);
	new_asset->_AudioStream.SetNumberOfSamples(number_of_samples);

	//Read the data.
	DynamicArray<byte> data;
	data.Upsize<false>(new_asset->_AudioStream.GetDataSize());
	load_context._StreamArchive->Read(data.Data(), new_asset->_AudioStream.GetDataSize(), &stream_archive_position);
	new_asset->_AudioStream.SetDataInternal(std::move(data));

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Update the total CPU/GPU memory.
	{
		uint64 cpu_memory{ 0 };

		cpu_memory += sizeof(uint32); //Sample Rate
		cpu_memory += sizeof(uint8); //Number Of Channels
		cpu_memory += sizeof(Audio::Format); //Format
		cpu_memory += sizeof(uint32); //Number Of Samples
		cpu_memory += new_asset->_AudioStream.GetDataSize(); //Data

		_TotalCPUMemory.FetchAdd(cpu_memory);
	}
#endif
}