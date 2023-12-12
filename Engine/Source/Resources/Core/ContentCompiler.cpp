#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
//Header file.
#include <Resources/Core/ContentCompiler.h>

//Core.
#include <Core/Algorithms/HashAlgorithms.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/Time.h>

//File.
#include <File/Core/FileCore.h>
#include <File/Core/BinaryFile.h>
#include <File/Utilities/TextParsingUtilities.h>

//Systems.
#include <Systems/LogSystem.h>
#include <Systems/ResourceSystem.h>

//Constants.
#define ENGINE_CONTENT_DEFINITIONS "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Content Definitions"
#define ENGINE_INTERMEDIATE "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate"
#define GAME_CONTENT_DEFINITIONS "..\\..\\..\\Content\\Content Definitions"
#define GAME_INTERMEDIATE "..\\..\\..\\Content\\Intermediate"

//Singleton definition.
DEFINE_SINGLETON(ContentCompiler);

/*
*	Content cache class definition.
*/
class ContentCache final
{

public:

	//The current version.
	constexpr static uint64 CURRENT_VERSION{ 1 };

	/*
	*	Entry class definition.
	*/
	class Entry final
	{

	public:

		//The identifier.
		uint64 _Identifier;

		//The last write time.
		std::filesystem::file_time_type _LastWriteTime;

	};

	//The version.
	uint64 _Version;

	//The entries.
	DynamicArray<Entry> _Entries;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE ContentCache(const char *const RESTRICT file_path) NOEXCEPT
	{
		if (!File::Exists(file_path))
		{
			return;
		}

		//Read the file.
		BinaryFile<BinaryFileMode::IN> file{ file_path };

		//Read the version.
		uint64 version;
		file.Read(&version, sizeof(uint64));

		//If this is not the current version, disregard.
		if (version != CURRENT_VERSION)
		{
			file.Close();

			return;
		}

		//Read the number of entries.
		uint64 number_of_entries;
		file.Read(&number_of_entries, sizeof(uint64));

		_Entries.Upsize<false>(number_of_entries);

		//Read the entries.
		for (Entry &entry : _Entries)
		{
			file.Read(&entry, sizeof(Entry));
		}

		//Close the file.
		file.Close();
	}

	/*
	*	Returns if the given identifier with the given last write time needs a recompile.
	*/
	FORCE_INLINE NO_DISCARD bool NeedsRecompile(const uint64 identifier, const std::filesystem::file_time_type last_write_time) NOEXCEPT
	{
		for (Entry &entry : _Entries)
		{
			if (entry._Identifier == identifier)
			{
				if (entry._LastWriteTime < last_write_time)
				{
					entry._LastWriteTime = last_write_time;

					return true;
				}

				else
				{
					return false;
				}
			}
		}

		_Entries.Emplace();
		Entry &new_entry{ _Entries.Back() };

		new_entry._Identifier = identifier;
		new_entry._LastWriteTime = last_write_time;

		return true;
	}

	/*
	*	Writes this shader cache.
	*/
	FORCE_INLINE void Write(const char* const RESTRICT file_path) NOEXCEPT
	{
		//Open the file.
		BinaryFile<BinaryFileMode::OUT> file{ file_path };

		//Write the version.
		const uint64 version{ CURRENT_VERSION };
		file.Write(&version, sizeof(uint64));

		//Write the number of entries.
		const uint64 number_of_entries{ _Entries.Size() };
		file.Write(&number_of_entries, sizeof(uint64));

		//Write the entries.
		for (const Entry &entry : _Entries)
		{
			file.Write(&entry._Identifier, sizeof(uint64));
			file.Write(&entry._LastWriteTime, sizeof(std::filesystem::file_time_type));
		}

		//Close the file.
		file.Close();
	}

};

/*
*	Runs the resource compiler for engine.
*	Returns if new resources were compiled.
*/
NO_DISCARD bool ContentCompiler::RunEngine() NOEXCEPT
{
	//Cache the start time.
	const TimePoint start_time{ GetCurrentTimePoint() };

	//Remember if new content was compiled.
	bool new_content_compiled{ false };
	
	//Parse engine content definitions.
	{
		//Set up the content cache.
		char content_cache_file_path[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(content_cache_file_path, "%s\\ContentCache.content_cache", ENGINE_CONTENT_DEFINITIONS);
		ContentCache content_cache{ content_cache_file_path };

		//Parse the content definitions.
		new_content_compiled |= ParseContentDefinitionsInDirectory(CompilationDomain::ENGINE, &content_cache, ENGINE_CONTENT_DEFINITIONS);

		//Write the content cache.
		content_cache.Write(content_cache_file_path);
	}

	LOG_INFORMATION("Content Compiler Engine took %f seconds.", start_time.GetSecondsSince());

	return new_content_compiled;
}

/*
*	Runs the resource compiler for game.
*	Returns if new resources were compiled.
*/
NO_DISCARD bool ContentCompiler::RunGame() NOEXCEPT
{
	//Cache the start time.
	const TimePoint start_time{ GetCurrentTimePoint() };

	//Remember if new content was compiled.
	bool new_content_compiled{ false };

	//Parse game content definitions.
	{
		//Set up the content cache.
		char content_cache_file_path[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(content_cache_file_path, "%s\\ContentCache.content_cache", GAME_CONTENT_DEFINITIONS);
		ContentCache content_cache{ content_cache_file_path };

		//Parse the content definitions.
		new_content_compiled |= ParseContentDefinitionsInDirectory(CompilationDomain::GAME, &content_cache, GAME_CONTENT_DEFINITIONS);

		//Write the content cache.
		content_cache.Write(content_cache_file_path);
	}

	LOG_INFORMATION("Content Compiler Game took %f seconds.", start_time.GetSecondsSince());

	return new_content_compiled;
}

/*
*	Parses content definitions in the given directory.
*	Returns if new content was compiled.
*/
NO_DISCARD bool ContentCompiler::ParseContentDefinitionsInDirectory(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const char *const RESTRICT directory_path) NOEXCEPT
{
	//Remember if new content was compiled.
	bool new_content_compiled{ false };

	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		//Parse recursively if this is a directory.
		if (entry.is_directory())
		{
			new_content_compiled |= ParseContentDefinitionsInDirectory(compilation_domain, content_cache, entry.path().u8string().data());

			continue;
		}

		//Cache the file path string.
		const std::string file_path{ entry.path().string()};

		//Skip if not a .content_definition file.
		if (file_path.find(".content_definition") == std::string::npos)
		{
			continue;
		}

		//Calculate the identifier.
		const uint64 identifier{ HashAlgorithms::MurmurHash64(file_path.data(), sizeof(char) * file_path.length()) };

		//Retrieve the last write time.
		const std::filesystem::file_time_type last_write_time{ std::filesystem::last_write_time(file_path) };

		//Check if it needs a recompile.
		if (!content_cache->NeedsRecompile(identifier, last_write_time))
		{
			continue;
		}

		LOG_INFORMATION("Compiling %s", file_path.data());

		//Figre out the name.
		std::string name;

		{
			const size_t last_slash_position{ file_path.find_last_of("\\") };
			name = file_path.substr(last_slash_position + 1, file_path.length() - last_slash_position - strlen(".content_definition") - 1);
		}

		//Open the file.
		std::ifstream file{ file_path };

		//Retrieve the content type.
		std::string content_type;

		if (!std::getline(file, content_type))
		{
			ASSERT(false, "Couldn't retrieve content type for " << file_path.data());
		}

		if (content_type == "MATERIAL")
		{
			ParseMaterial(compilation_domain, content_cache, name, file);
		}

		else if (content_type == "TEXTURE_2D")
		{
			ParseTexture2D(compilation_domain, content_cache, name, file);
		}

		else
		{
			ASSERT(false, "Unknown type " << content_type.data());
		}

		//Close the file.
		file.close();

		//New content was compiled!
		new_content_compiled = true;
	}

	return new_content_compiled;
}

/*
*	Parses a Material from the given file.
*/
void ContentCompiler::ParseMaterial(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, std::ifstream &file) NOEXCEPT
{
	//Calculate the intermediate directory.
	char intermediate_directory[MAXIMUM_FILE_PATH_LENGTH];

	switch (compilation_domain)
	{
		case CompilationDomain::ENGINE:
		{
			sprintf_s(intermediate_directory, ENGINE_INTERMEDIATE "\\Material");

			break;
		}

		case CompilationDomain::GAME:
		{
			sprintf_s(intermediate_directory, GAME_INTERMEDIATE "\\Material");

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Create the directory, if it doesn't exist.
	File::CreateDirectory(intermediate_directory);

	//Set up the build parameters.
	MaterialBuildParameters parameters;

	//Set the output.
	char output_buffer[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(output_buffer, "%s\\%s", intermediate_directory, name.data());
	parameters._Output = output_buffer;

	//Set the id.
	parameters._ID = name.data();

	//Set some default parameters.
	parameters._Type = MaterialResource::Type::OPAQUE;
	parameters._AlbedoThicknessComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
	parameters._AlbedoThicknessComponent._Color = Color(Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f));
	parameters._NormalMapDisplacementComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
	parameters._NormalMapDisplacementComponent._Color = Color(Vector4<float32>(0.5f, 0.5f, 1.0f, 0.5f));
	parameters._MaterialPropertiesComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
	parameters._MaterialPropertiesComponent._Color = Color(Vector4<float32>(1.0f, 0.0f, 1.0f, 0.0f));
	parameters._OpacityComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
	parameters._OpacityComponent._Color = Color(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
	parameters._EmissiveMultiplier = 0.0f;
	parameters._DoubleSided = false;

	//Read all of the lines.
	std::string line;
	StaticArray<DynamicString, 5> arguments;

	while (std::getline(file, line))
	{
		//Skip lines with only whitespace.
		if (TextParsingUtilities::OnlyWhitespace(line.data(), line.length()))
		{
			continue;
		}

		//Parse the arguments.
		TextParsingUtilities::ParseSpaceSeparatedArguments
		(
			line.data(),
			line.length(),
			arguments.Data()
		);

		if (arguments[0] == "MASKED")
		{
			parameters._Type = MaterialResource::Type::MASKED;
		}

		else if (arguments[0] == "OPAQUE")
		{
			parameters._Type = MaterialResource::Type::OPAQUE;
		}

		else if (arguments[0] == "TRANSLUCENT")
		{
			parameters._Type = MaterialResource::Type::TRANSLUCENT;
		}

		else if (arguments[0] == "ALBEDO_THICKNESS_COLOR")
		{
			parameters._AlbedoThicknessComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._AlbedoThicknessComponent._Color = Color(Vector4<float32>(std::stof(arguments[1].Data()), std::stof(arguments[2].Data()), std::stof(arguments[3].Data()), std::stof(arguments[4].Data())));
		}

		else if (arguments[0] == "ALBEDO_THICKNESS_TEXTURE")
		{
			parameters._AlbedoThicknessComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
			parameters._AlbedoThicknessComponent._TextureResourceIdentifier = HashString(arguments[1].Data());
		}

		else if (arguments[0] == "NORMAL_MAP_DISPLACEMENT_COLOR")
		{
			parameters._NormalMapDisplacementComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._NormalMapDisplacementComponent._Color = Color(Vector4<float32>(std::stof(arguments[1].Data()), std::stof(arguments[2].Data()), std::stof(arguments[3].Data()), std::stof(arguments[4].Data())));
		}

		else if (arguments[0] == "NORMAL_MAP_DISPLACEMENT_TEXTURE")
		{
			parameters._NormalMapDisplacementComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
			parameters._NormalMapDisplacementComponent._TextureResourceIdentifier = HashString(arguments[1].Data());
		}

		else if (arguments[0] == "MATERIAL_PROPERTIES_COLOR")
		{
			parameters._MaterialPropertiesComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._MaterialPropertiesComponent._Color = Color(Vector4<float32>(std::stof(arguments[1].Data()), std::stof(arguments[2].Data()), std::stof(arguments[3].Data()), std::stof(arguments[4].Data())));
		}

		else if (arguments[0] == "MATERIAL_PROPERTIES_TEXTURE")
		{
			parameters._MaterialPropertiesComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
			parameters._MaterialPropertiesComponent._TextureResourceIdentifier = HashString(arguments[1].Data());
		}

		else if (arguments[0] == "OPACITY_COLOR")
		{
			parameters._OpacityComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._OpacityComponent._Color = Color(Vector4<float32>(std::stof(arguments[1].Data()), std::stof(arguments[2].Data()), std::stof(arguments[3].Data()), std::stof(arguments[4].Data())));
		}

		else if (arguments[0] == "OPACITY_TEXTURE")
		{
			parameters._OpacityComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
			parameters._OpacityComponent._TextureResourceIdentifier = HashString(arguments[1].Data());
		}

		else if (arguments[0] == "EMISSIVE_MULTIPLIER")
		{
			parameters._EmissiveMultiplier = std::stof(arguments[1].Data());
		}

		else if (arguments[0] == "DOUBLE_SIDED")
		{
			parameters._DoubleSided = true;
		}

		else
		{
			ASSERT(false, "Couldn't parse argument " << arguments[0]);
		}
	}

	//Build!
	ResourceSystem::Instance->GetResourceBuildingSystem()->BuildMaterial(parameters);
}

/*
*	Parses a Texture2D from the given file.
*/
void ContentCompiler::ParseTexture2D(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, std::ifstream &file) NOEXCEPT
{
	//Calculate the intermediate directory.
	char intermediate_directory[MAXIMUM_FILE_PATH_LENGTH];

	switch (compilation_domain)
	{
		case CompilationDomain::ENGINE:
		{
			sprintf_s(intermediate_directory, ENGINE_INTERMEDIATE "\\Texture2D");

			break;
		}

		case CompilationDomain::GAME:
		{
			sprintf_s(intermediate_directory, GAME_INTERMEDIATE "\\Texture2D");

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Create the directory, if it doesn't exist.
	File::CreateDirectory(intermediate_directory);

	//Set up the build parameters.
	Texture2DBuildParameters parameters;

	//Set the output.
	char output_buffer[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(output_buffer, "%s\\%s", intermediate_directory, name.data());
	parameters._Output = output_buffer;

	//Set the id.
	parameters._ID = name.data();

	//Set some default parameters.
	parameters._DefaultWidth = 0;
	parameters._DefaultHeight = 0;
	parameters._File1 = nullptr;
	parameters._File2 = nullptr;
	parameters._File3 = nullptr;
	parameters._File4 = nullptr;
	parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
	parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::RED);
	parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::GREEN);
	parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::BLUE);
	parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::ALPHA);
	parameters._ApplyGammaCorrection = false;
	parameters._TransformFunction = nullptr;
	parameters._BaseMipmapLevel = 0;
	parameters._MipmapLevels = 1;

	//Declare some buffers that is needed to retain data.
	char file_1_buffer[MAXIMUM_FILE_PATH_LENGTH];
	char file_2_buffer[MAXIMUM_FILE_PATH_LENGTH];
	char file_3_buffer[MAXIMUM_FILE_PATH_LENGTH];
	char file_4_buffer[MAXIMUM_FILE_PATH_LENGTH];

	//Read all of the lines.
	std::string line;
	StaticArray<DynamicString, 5> arguments;

	while (std::getline(file, line))
	{
		//Skip lines with only whitespace.
		if (TextParsingUtilities::OnlyWhitespace(line.data(), line.length()))
		{
			continue;
		}

		//Parse the arguments.
		TextParsingUtilities::ParseSpaceSeparatedArguments
		(
			line.data(),
			line.length(),
			arguments.Data()
		);

		if (arguments[0] == "FILE_1")
		{
			sprintf_s(file_1_buffer, "%s", arguments[1].Data());
			parameters._File1 = file_1_buffer;
		}

		else if (arguments[0] == "FILE_2")
		{
			sprintf_s(file_2_buffer, "%s", arguments[1].Data());
			parameters._File2 = file_2_buffer;
		}

		else if (arguments[0] == "FILE_3")
		{
			sprintf_s(file_3_buffer, "%s", arguments[1].Data());
			parameters._File3 = file_3_buffer;
		}

		else if (arguments[0] == "FILE_4")
		{
			sprintf_s(file_4_buffer, "%s", arguments[1].Data());
			parameters._File4 = file_4_buffer;
		}

		else if (arguments[0] == "DEFAULT")
		{
			for (uint8 i{ 0 }; i < 4; ++i)
			{
				parameters._Default[i] = std::stof(arguments[1 + i].Data());
			}
		}

		else if (arguments[0] == "CHANNEL_MAPPING")
		{
			Texture2DBuildParameters::ChannelMapping *RESTRICT channel_mapping{ nullptr };

			if (arguments[1] == "RED")
			{
				channel_mapping = &parameters._ChannelMappings[0];
			}

			else if (arguments[1] == "GREEN")
			{
				channel_mapping = &parameters._ChannelMappings[1];
			}

			else if (arguments[1] == "BLUE")
			{
				channel_mapping = &parameters._ChannelMappings[2];
			}

			else if (arguments[1] == "ALPHA")
			{
				channel_mapping = &parameters._ChannelMappings[3];
			}

			else
			{
				ASSERT(false, "Couldn't parse argument " << arguments[1].Data());
			}

			if (arguments[2] == "FILE_1")
			{
				channel_mapping->_File = Texture2DBuildParameters::File::FILE_1;
			}

			else if (arguments[2] == "FILE_2")
			{
				channel_mapping->_File = Texture2DBuildParameters::File::FILE_2;
			}

			else if (arguments[2] == "FILE_3")
			{
				channel_mapping->_File = Texture2DBuildParameters::File::FILE_3;
			}

			else if (arguments[2] == "FILE_4")
			{
				channel_mapping->_File = Texture2DBuildParameters::File::FILE_4;
			}

			else if (arguments[2] == "DEFAULT")
			{
				channel_mapping->_File = Texture2DBuildParameters::File::DEFAULT;
			}

			else
			{
				ASSERT(false, "Couldn't parse argument " << arguments[2].Data());
			}

			if (arguments[3] == "RED")
			{
				channel_mapping->_Channel = Texture2DBuildParameters::Channel::RED;
			}

			else if (arguments[3] == "GREEN")
			{
				channel_mapping->_Channel = Texture2DBuildParameters::Channel::GREEN;
			}

			else if (arguments[3] == "BLUE")
			{
				channel_mapping->_Channel = Texture2DBuildParameters::Channel::BLUE;
			}

			else if (arguments[3] == "ALPHA")
			{
				channel_mapping->_Channel = Texture2DBuildParameters::Channel::ALPHA;
			}

			else
			{
				ASSERT(false, "Couldn't parse argument " << arguments[3].Data());
			}
		}

		else if (arguments[0] == "APPLY_GAMMA_CORRECTION")
		{
			parameters._ApplyGammaCorrection = true;
		}

		else if (arguments[0] == "BASE_MIPMAP_LEVEL")
		{
			parameters._BaseMipmapLevel = static_cast<uint8>(std::stoul(arguments[1].Data()));
		}

		else if (arguments[0] == "MIPMAP_LEVELS")
		{
			parameters._MipmapLevels = static_cast<uint8>(std::stoul(arguments[1].Data()));
		}

		else
		{
			ASSERT(false, "Couldn't parse argument " << arguments[0].Data());
		}
	}

	//Build!
	ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
}
#endif