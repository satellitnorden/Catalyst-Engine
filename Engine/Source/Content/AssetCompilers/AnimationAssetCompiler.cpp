//Header file.
#include <Content/AssetCompilers/AnimationAssetCompiler.h>

//File.
#include <File/Core/File.h>
#include <File/Core/BinaryOutputFile.h>
#include <File/Readers/FBXReader.h>
#include <File/Utilities/TextParsingUtilities.h>

//Profiling.
#include <Profiling/Profiling.h>

//Systems.
#include <Systems/LogSystem.h>
#include <Systems/RenderingSystem.h>

//STL.
#include <fstream>
#include <string>

/*
*	Animation parameters class definition.
*/
class AnimationParameters final
{

public:

	//The file.
	StaticString<MAXIMUM_FILE_PATH_LENGTH> _File;

};

/*
*	Default constructor.
*/
AnimationAssetCompiler::AnimationAssetCompiler() NOEXCEPT
{
	//Set the flags.
	_Flags = Flags::NONE;
}

/*
*	Returns the asset type identifier.
*/
NO_DISCARD HashString AnimationAssetCompiler::AssetTypeIdentifier() const NOEXCEPT
{
	return AnimationAsset::TYPE_IDENTIFIER;
}

/*
*	Returns the current version.
*/
NO_DISCARD uint64 AnimationAssetCompiler::CurrentVersion() const NOEXCEPT
{
	return 1;
}

/*
*	Compiles a single asset with the given compile context.
*/
void AnimationAssetCompiler::Compile(const CompileContext &compile_context) NOEXCEPT
{
	PROFILING_SCOPE("AnimationAssetCompiler::Compile");

	//Set up the parameters.
	AnimationParameters parameters;

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

	//Read the animation file.
	AnimationFile animation_file;

	const bool read_successful{ FBXReader::Read(parameters._File.Data(), &animation_file) };

	ASSERT(read_successful, "Couldn't read animation file!");

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
	sprintf_s(directory_path, "%s\\Animations", collection_directory_path);

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

	//Write the duration.
	output_file.Write(&animation_file._Duration, sizeof(float32));

	//Write the number of channels.
	const uint64 number_of_channels{ animation_file._Channels.Size() };
	output_file.Write(&number_of_channels, sizeof(uint64));

	//Write the channels.
	for (const AnimationChannel &channel : animation_file._Channels)
	{
		//Write the bone identifier.
		output_file.Write(&channel._BoneIdentifier, sizeof(HashString));

		//Write the number of keyframes.
		const uint64 number_of_keyframes{ channel._Keyframes.Size() };
		output_file.Write(&number_of_keyframes, sizeof(uint64));

		//Write the keyframes.
		for (const AnimationKeyframe &keyframe : channel._Keyframes)
		{
			//Write the timestamp.
			output_file.Write(&keyframe._Timestamp, sizeof(float32));

			//Write the bone transform.
			output_file.Write(&keyframe._BoneTransform, sizeof(BoneTransform));
		}
	}

	//Close the output file.
	output_file.Close();
}

/*
*	Loads a single asset with the given load context.
*/
void AnimationAssetCompiler::Load(const LoadContext &load_context) NOEXCEPT
{
	PROFILING_SCOPE("AnimationAssetCompiler::Load");

	//Allocate the asset.
	AnimationAsset *const RESTRICT new_asset{ static_cast<AnimationAsset *const RESTRICT>(load_context._Asset) };

	//Read the data.
	uint64 stream_archive_position{ load_context._StreamArchivePosition };

	//Read the duration.
	load_context._StreamArchive->Read(&new_asset->_Duration, sizeof(float32), &stream_archive_position);

	//Read the number of channels.
	uint64 number_of_channels;
	load_context._StreamArchive->Read(&number_of_channels, sizeof(uint64), &stream_archive_position);

	//Resize the channels.
	new_asset->_Channels.Upsize<true>(number_of_channels);

	//Read the channels.
	for (AnimationChannel &channel : new_asset->_Channels)
	{
		//Read the bone identifier.
		load_context._StreamArchive->Read(&channel._BoneIdentifier, sizeof(HashString), &stream_archive_position);

		//Read the number of keyframes.
		uint64 number_of_keyframes;
		load_context._StreamArchive->Read(&number_of_keyframes, sizeof(uint64), &stream_archive_position);

		//Resize the keyframes.
		channel._Keyframes.Upsize<false>(number_of_keyframes);

		//Read the keyframes.
		for (AnimationKeyframe &keyframe : channel._Keyframes)
		{
			//Read the timestamp.
			load_context._StreamArchive->Read(&keyframe._Timestamp, sizeof(float32), &stream_archive_position);

			//Read the bone transform..
			load_context._StreamArchive->Read(&keyframe._BoneTransform, sizeof(BoneTransform), &stream_archive_position);
		}
	}
}