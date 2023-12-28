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
#include <File/Writers/PNGWriter.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>

//Resources.
#include <Resources/Utilities/ProceduralTreeGenerator.h>
#include <Resources/Utilities/ResourceBuildingUtilities.h>

//Systems.
#include <Systems/LogSystem.h>
#include <Systems/ResourceSystem.h>

//Constants.
#define ENGINE_CONTENT_DEFINITIONS "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Content Definitions"
#define ENGINE_INTERMEDIATE "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate"
#define ENGINE_RAW "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw"
#define GAME_CONTENT_DEFINITIONS "..\\..\\..\\Content\\Content Definitions"
#define GAME_INTERMEDIATE "..\\..\\..\\Content\\Intermediate"
#define GAME_RAW "..\\..\\..\\Content\\Raw"

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
#if 0
		return true;
#else
		for (Entry &entry : _Entries)
		{
			if (entry._Identifier == identifier)
			{
				if (entry._LastWriteTime < last_write_time)
				{
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
#endif
	}

	/*
	*	Updates the last write time for the given identifier.
	*/
	FORCE_INLINE void UpdateLastWriteTime(const uint64 identifier, const std::filesystem::file_time_type last_write_time) NOEXCEPT
	{
		for (Entry& entry : _Entries)
		{
			if (entry._Identifier == identifier)
			{
				entry._LastWriteTime = last_write_time;

				break;
			}
		}
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
	enum class DelayedCompileType : uint8
	{
		IMPOSTOR_MATERIAL
	};
	struct DelayedCompile final
	{
		DelayedCompileType _Type;
		std::string _Name;
		std::string _FilePath;
	};

	//Keep track of delayed compiles.
	DynamicArray<DelayedCompile> delayed_compiles;

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

		//Check if this should always compile.
		bool always_compile{ false };

		if (content_type == "ALWAYS_COMPILE")
		{
			always_compile = true;

			if (!std::getline(file, content_type))
			{
				ASSERT(false, "Couldn't retrieve content type for " << file_path.data());
			}
		}

		if (content_type == "MATERIAL")
		{
			ParseMaterial(compilation_domain, content_cache, name, file);
		}

		else if (content_type == "MODEL")
		{
			ParseModel(compilation_domain, content_cache, name, file);
		}

		else if (content_type == "TEXTURE_2D")
		{
			ParseTexture2D(compilation_domain, content_cache, name, file);
		}

		else if (content_type == "PROCEDURAL_TREE_MODEL")
		{
			ParseProceduralTreeModel(compilation_domain, content_cache, name, file);
		}

		else if (content_type == "IMPOSTOR_MATERIAL")
		{
			//Impostor materials depends on other resources, so delay the compile.
			delayed_compiles.Emplace(DelayedCompile{ DelayedCompileType::IMPOSTOR_MATERIAL, name, file_path });
		}

		else
		{
			ASSERT(false, "Unknown type " << content_type.data());
		}

		//Close the file.
		file.close();

		//Update the last write time.
		if (!always_compile)
		{
			content_cache->UpdateLastWriteTime(identifier, last_write_time);
		}

		//New content was compiled!
		new_content_compiled = true;
	}

	//Process delayed compiles.
	for (const DelayedCompile &delayed_compile : delayed_compiles)
	{
		switch (delayed_compile._Type)
		{
			case DelayedCompileType::IMPOSTOR_MATERIAL:
			{
				std::ifstream file{ delayed_compile._FilePath };
				ParseImpostorMaterial(compilation_domain, content_cache, delayed_compile._Name, file);
				file.close();

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
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
			sprintf_s(intermediate_directory, ENGINE_INTERMEDIATE "\\Materials");

			break;
		}

		case CompilationDomain::GAME:
		{
			sprintf_s(intermediate_directory, GAME_INTERMEDIATE "\\Materials");

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
*	Parses a Model from the given file.
*/
void ContentCompiler::ParseModel(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, std::ifstream &file) NOEXCEPT
{
	//Calculate the intermediate directory.
	char intermediate_directory[MAXIMUM_FILE_PATH_LENGTH];

	switch (compilation_domain)
	{
		case CompilationDomain::ENGINE:
		{
			sprintf_s(intermediate_directory, ENGINE_INTERMEDIATE "\\Models");

			break;
		}

		case CompilationDomain::GAME:
		{
			sprintf_s(intermediate_directory, GAME_INTERMEDIATE "\\Models");

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
	ModelBuildParameters parameters;

	//Set the output.
	char output_buffer[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(output_buffer, "%s\\%s", intermediate_directory, name.data());
	parameters._Output = output_buffer;

	//Set the resource identifier.
	parameters._ResourceIdentifier = name.data();

	//Set some default parameters.
	parameters._Transformation = Matrix4x4();
	parameters._TextureCoordinateMultiplier = 1.0f;
	parameters._TexturCoordinateRotation = 0.0f;
	parameters._ProceduralFunction = nullptr;
	parameters._CollisionModelFilePath = nullptr;
	
	//Some strings will need to be persistent until building, so cache those.
	DynamicArray<DynamicString> persistent_strings;

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

		if (arguments[0] == "LEVEL_OF_DETAIL")
		{
			persistent_strings.Emplace();
			persistent_strings.Back() = arguments[1];
			parameters._LevelOfDetails.Emplace(persistent_strings.Back().Data());
		}

		else if (arguments[0] == "COLLISION")
		{
			persistent_strings.Emplace();
			persistent_strings.Back() = arguments[1];
			parameters._CollisionModelFilePath = persistent_strings.Back().Data();
		}

		else
		{
			ASSERT(false, "Couldn't parse argument " << arguments[0].Data());
		}
	}

	//Build!
	ResourceSystem::Instance->GetResourceBuildingSystem()->BuildModel(parameters);
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
			sprintf_s(intermediate_directory, ENGINE_INTERMEDIATE "\\Textures");

			break;
		}

		case CompilationDomain::GAME:
		{
			sprintf_s(intermediate_directory, GAME_INTERMEDIATE "\\Textures");

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
	parameters._Tint = Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f);
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

		else if (arguments[0] == "TINT")
		{
			for (uint8 i{ 0 }; i < 4; ++i)
			{
				parameters._Tint[i] = std::stof(arguments[1 + i].Data());
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

/*
*	Parses a procedural tree model from the given file.
*/
void ContentCompiler::ParseProceduralTreeModel(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, std::ifstream &file) NOEXCEPT
{
	//Calculate the intermediate directory.
	char intermediate_directory[MAXIMUM_FILE_PATH_LENGTH];

	switch (compilation_domain)
	{
		case CompilationDomain::ENGINE:
		{
			sprintf_s(intermediate_directory, ENGINE_INTERMEDIATE "\\Models");

			break;
		}

		case CompilationDomain::GAME:
		{
			sprintf_s(intermediate_directory, GAME_INTERMEDIATE "\\Models");

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

	//Parse the parameters.
	ProceduralTreeGenerator::Parameters procedural_tree_generator_parameters;

	//TODO!

	//Create the procedural tree model.
	ProceduralTreeGenerator::Output procedural_tree_generator_output;
	
	ProceduralTreeGenerator::GenerateTree(procedural_tree_generator_parameters, &procedural_tree_generator_output);

	//Set up the build parameters.
	ModelBuildParameters parameters;

	//Set the output.
	char output_buffer[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(output_buffer, "%s\\%s", intermediate_directory, name.data());
	parameters._Output = output_buffer;

	//Set the resource identifier.
	parameters._ResourceIdentifier = name.data();

	//Set some default parameters.
	parameters._Transformation = Matrix4x4();
	parameters._TextureCoordinateMultiplier = 1.0f;
	parameters._TexturCoordinateRotation = 0.0f;
	parameters._ProceduralFunction = [](DynamicArray<ModelFile> *const RESTRICT model_files, ModelFile *const RESTRICT collision_model_file, void *const RESTRICT user_data)
	{
		//Cache the output.
		const ProceduralTreeGenerator::Output &procedural_tree_generator_output{ *static_cast<ProceduralTreeGenerator::Output *const RESTRICT>(user_data) };
		
		//Add one level of detail.
		model_files->Emplace();

		//Add the main vertices/indices.
		for (uint64 i{ 0 }; i < procedural_tree_generator_output._Vertices.Size(); ++i)
		{
			model_files->Back()._Meshes.Emplace();
			ModelFile::Mesh &new_mesh{ model_files->Back()._Meshes.Back() };

			new_mesh._Vertices = procedural_tree_generator_output._Vertices[i];
			new_mesh._Indices = procedural_tree_generator_output._Indices[i];
		}

		//Add the collision vertices/indices.
		collision_model_file->_Meshes.Emplace();
		ModelFile::Mesh &new_mesh{ collision_model_file->_Meshes.Back() };

		new_mesh._Vertices = procedural_tree_generator_output._CollisionVertices;
		new_mesh._Indices = procedural_tree_generator_output._CollisionIndices;
	};
	parameters._ProceduralFunctionUserData = &procedural_tree_generator_output;
	parameters._CollisionModelFilePath = nullptr;

	//Build!
	ResourceSystem::Instance->GetResourceBuildingSystem()->BuildModel(parameters);
}

/*
*	Loads the texture 2D with the given identifier.
*/
NO_DISCARD ResourcePointer<Texture2DResource> LoadTexture2DResource(const char *const RESTRICT directory_path, const HashString identifier) NOEXCEPT
{
	/*
	*	This is a bit tricky, best we can do right now is search in a specific path,
	*	hash the names of the files, and see if they match.
	*/
	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		const std::string file_path{ entry.path().string() };
		const size_t last_slash_position{ file_path.find_last_of("\\") };
		const std::string name{ file_path.substr(last_slash_position + 1, file_path.length() - last_slash_position - strlen(".cr") - 1) };
		const HashString candidate_identifier{ name.data() };

		if (candidate_identifier == identifier)
		{
			ResourceSystem::Instance->LoadResource(file_path.data());

			return ResourceSystem::Instance->GetTexture2DResource(candidate_identifier);
		}
	}

	ASSERT(false, "Couldn't find resource!");

	return ResourcePointer<Texture2DResource>();
}

/*
*	Parses an Impostor Material from the given file.
*/
void ContentCompiler::ParseImpostorMaterial(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, std::ifstream &file) NOEXCEPT
{
	//Calculate the intermediate directory.
	char intermediate_directory[MAXIMUM_FILE_PATH_LENGTH];

	switch (compilation_domain)
	{
		case CompilationDomain::ENGINE:
		{
			sprintf_s(intermediate_directory, ENGINE_INTERMEDIATE);

			break;
		}

		case CompilationDomain::GAME:
		{
			sprintf_s(intermediate_directory, GAME_INTERMEDIATE);

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

	//Calculate the raw directory.
	char raw_directory[MAXIMUM_FILE_PATH_LENGTH];

	switch (compilation_domain)
	{
		case CompilationDomain::ENGINE:
		{
			sprintf_s(raw_directory, ENGINE_RAW "\\Textures\\Impostor Textures");

			break;
		}

		case CompilationDomain::GAME:
		{
			sprintf_s(raw_directory, GAME_RAW "\\Textures\\Impostor Textures");

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Create the directory, if it doesn't exist.
	File::CreateDirectory(raw_directory);

	//Read all of the lines.
	ResourcePointer<ModelResource> model_resource;
	DynamicArray<ResourcePointer<MaterialResource>> material_resources;
	Resolution dimensions;

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

		if (arguments[0] == "IMPOSTOR_MATERIAL")
		{
			//Skip. (:
			continue;
		}

		else if (arguments[0] == "MODEL")
		{
			//Assume it was created in the standardized format.
			char buffer[MAXIMUM_FILE_PATH_LENGTH];

			switch (compilation_domain)
			{
				case CompilationDomain::ENGINE:
				{
					sprintf_s(buffer, ENGINE_INTERMEDIATE "\\Models\\%s.cr", arguments[1].Data());

					break;
				}

				case CompilationDomain::GAME:
				{
					sprintf_s(buffer, GAME_INTERMEDIATE "\\Models\\%s.cr", arguments[1].Data());

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			ResourceSystem::Instance->LoadResource(buffer);
			model_resource = ResourceSystem::Instance->GetModelResource(HashString(arguments[1].Data()));
		}

		else if (arguments[0] == "MATERIAL")
		{
			//Assume it was created in the standardized format.
			char buffer[MAXIMUM_FILE_PATH_LENGTH];

			switch (compilation_domain)
			{
				case CompilationDomain::ENGINE:
				{
					sprintf_s(buffer, ENGINE_INTERMEDIATE "\\Materials\\%s.cr", arguments[1].Data());

					break;
				}

				case CompilationDomain::GAME:
				{
					sprintf_s(buffer, GAME_INTERMEDIATE "\\Materials\\%s.cr", arguments[1].Data());

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			ResourceSystem::Instance->LoadResource(buffer);
			material_resources.Emplace();
			material_resources.Back() = ResourceSystem::Instance->GetMaterialResource(HashString(arguments[1].Data()));
		}

		else if (arguments[0] == "DIMENSIONS")
		{
			dimensions._Width = static_cast<uint32>(std::stoul(arguments[1].Data()));
			dimensions._Height = static_cast<uint32>(std::stoul(arguments[2].Data()));
		}

		else if (arguments[0] != "ALWAYS_COMPILE")
		{
			ASSERT(false, "Couldn't parse argument " << arguments[0].Data());
		}
	}

	//Load all textures for all materials.
	DynamicArray<ResourcePointer<Texture2DResource>> albedo_thickness_textures;
	DynamicArray<ResourcePointer<Texture2DResource>> normal_map_displacement_textures;
	DynamicArray<ResourcePointer<Texture2DResource>> material_properties_textures;
	DynamicArray<ResourcePointer<Texture2DResource>> opacity_textures;

	{
		/*
		*	Cache the directory path of texture 2D resources.
		*	Assume it was created in the standardized format.
		*/
		char buffer[MAXIMUM_FILE_PATH_LENGTH];

		switch (compilation_domain)
		{
			case CompilationDomain::ENGINE:
			{
				sprintf_s(buffer, ENGINE_INTERMEDIATE "\\Textures");

				break;
			}

			case CompilationDomain::GAME:
			{
				sprintf_s(buffer, GAME_INTERMEDIATE "\\Textures");

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		for (const ResourcePointer<MaterialResource> &material_resource : material_resources)
		{
			/*
			*	Emplace one resource pointer for each material component,
			*	helps a bit in the next steps by being able to assume a 1-to-1 mesh-to-material-to-texture correlation.
			*/
			albedo_thickness_textures.Emplace();
			normal_map_displacement_textures.Emplace();
			material_properties_textures.Emplace();
			opacity_textures.Emplace();

			if (material_resource->_AlbedoThicknessComponent._Type == MaterialResource::MaterialResourceComponent::Type::TEXTURE)
			{
				albedo_thickness_textures.Back() = LoadTexture2DResource(buffer, material_resource->_AlbedoThicknessComponent._TextureResource->_Header._ResourceIdentifier);
			}

			if (material_resource->_NormalMapDisplacementComponent._Type == MaterialResource::MaterialResourceComponent::Type::TEXTURE)
			{
				normal_map_displacement_textures.Back() = LoadTexture2DResource(buffer, material_resource->_NormalMapDisplacementComponent._TextureResource->_Header._ResourceIdentifier);
			}

			if (material_resource->_MaterialPropertiesComponent._Type == MaterialResource::MaterialResourceComponent::Type::TEXTURE)
			{
				material_properties_textures.Back() = LoadTexture2DResource(buffer, material_resource->_MaterialPropertiesComponent._TextureResource->_Header._ResourceIdentifier);
			}

			if (material_resource->_OpacityComponent._Type == MaterialResource::MaterialResourceComponent::Type::TEXTURE)
			{
				opacity_textures.Back() = LoadTexture2DResource(buffer, material_resource->_OpacityComponent._TextureResource->_Header._ResourceIdentifier);
			}
		}
	}

	//Cache the axis aligned bounding box.
	const AxisAlignedBoundingBox3D axis_aligned_bounding_box{ model_resource->_ModelSpaceAxisAlignedBoundingBox };

	//Create the impostor textures!
	Texture2D<Vector4<float32>> impostor_albedo_texture{ dimensions._Width, dimensions._Height };
	Texture2D<Vector4<float32>> impostor_normal_map_texture{ dimensions._Width, dimensions._Height };
	Texture2D<Vector4<float32>> impostor_material_properties_texture{ dimensions._Width, dimensions._Height };
	Texture2D<Vector4<float32>> impostor_opacity_texture{ dimensions._Width, dimensions._Height };

	for (uint32 Y{ 0 }; Y < dimensions._Height; ++Y)
	{
		for (uint32 X{ 0 }; X < dimensions._Width; ++X)
		{
			//Calculate the normalized coordinate.
			const Vector2<float32> normalized_coordinate{ static_cast<float32>(X) / static_cast<float32>(dimensions._Width - 1), static_cast<float32>(Y) / static_cast<float32>(dimensions._Height - 1) };

			//Calculate the camera position.
			Vector3<float32> camera_position;

			camera_position._X = CatalystBaseMath::LinearlyInterpolate(axis_aligned_bounding_box._Minimum._X, axis_aligned_bounding_box._Maximum._X, normalized_coordinate._X);
			camera_position._Y = CatalystBaseMath::LinearlyInterpolate(axis_aligned_bounding_box._Minimum._Y, axis_aligned_bounding_box._Maximum._Y, normalized_coordinate._Y);
			camera_position._Z = axis_aligned_bounding_box._Minimum._Z - 1.0f;

			//Construct the ray.
			Ray ray;

			ray.SetOrigin(camera_position);
			ray.SetDirection(Vector3<float32>(0.0f, 0.0f, 1.0f));

			//Intersect every triangle and check for hits.
			float32 intersection_distance{ FLOAT32_MAXIMUM };
			uint64 intersected_mesh_index{ 0 };
			Vector3<float32> intersected_normal;
			Vector3<float32> intersected_tangent;
			Vector2<float32> intersected_texture_coordinate;

			for (uint64 mesh_index{ 0 }, size{ model_resource->_Meshes.Size() }; mesh_index < size; ++mesh_index)
			{
				//Cache the mesh.
				const Mesh &mesh{ model_resource->_Meshes[mesh_index] };

				//Cache the mesh level of detail.
				const Mesh::MeshLevelOfDetail &mesh_level_of_detail{ mesh._MeshLevelOfDetails[0] };

				for (uint32 index{ 0 }; index < mesh_level_of_detail._Indices.Size(); index += 3)
				{
					//Construct the triangle.
					Triangle triangle;

					triangle._Vertices[0] = mesh_level_of_detail._Vertices[mesh_level_of_detail._Indices[index + 0]]._Position;
					triangle._Vertices[1] = mesh_level_of_detail._Vertices[mesh_level_of_detail._Indices[index + 1]]._Position;
					triangle._Vertices[2] = mesh_level_of_detail._Vertices[mesh_level_of_detail._Indices[index + 2]]._Position;

					//Intersect the triangle.
					float32 temporary_intersection_distance;

					if (CatalystGeometryMath::RayTriangleIntersection(ray, triangle, &temporary_intersection_distance))
					{
						if (intersection_distance > temporary_intersection_distance)
						{
							//Calculate the vertex properties.
							Vector3<float32> normal;
							Vector3<float32> tangent;
							Vector2<float32> texture_coordinate;

							{
								//Calculate the intersected texture coordinate.
								Vector3<float32> intersection_point = ray._Origin + ray._Direction * temporary_intersection_distance;

								//Calculate the barycentric coordinate.
								Vector3<float32> barycentric_coordinate{ CatalystGeometryMath::CalculateBarycentricCoordinates(triangle, intersection_point) };

								//Calculate the normal.
								normal = mesh_level_of_detail._Vertices[mesh_level_of_detail._Indices[index + 0]]._Normal * barycentric_coordinate[0] + mesh_level_of_detail._Vertices[mesh_level_of_detail._Indices[index + 1]]._Normal * barycentric_coordinate[1] + mesh_level_of_detail._Vertices[mesh_level_of_detail._Indices[index + 2]]._Normal * barycentric_coordinate[2];

								//Calculate the tangent.
								tangent = mesh_level_of_detail._Vertices[mesh_level_of_detail._Indices[index + 0]]._Tangent * barycentric_coordinate[0] + mesh_level_of_detail._Vertices[mesh_level_of_detail._Indices[index + 1]]._Tangent * barycentric_coordinate[1] + mesh_level_of_detail._Vertices[mesh_level_of_detail._Indices[index + 2]]._Tangent * barycentric_coordinate[2];

								//Calculate the texture coordinate.
								texture_coordinate = mesh_level_of_detail._Vertices[mesh_level_of_detail._Indices[index + 0]]._TextureCoordinate * barycentric_coordinate[0] + mesh_level_of_detail._Vertices[mesh_level_of_detail._Indices[index + 1]]._TextureCoordinate * barycentric_coordinate[1] + mesh_level_of_detail._Vertices[mesh_level_of_detail._Indices[index + 2]]._TextureCoordinate * barycentric_coordinate[2];
							}

							//Check the mask texture if this was really a hit.
							bool was_really_a_hit{ true };

							if (opacity_textures[mesh_index])
							{
								const Vector4<float32> mask_value{ opacity_textures[mesh_index]->_Texture2D.Sample(texture_coordinate, AddressMode::CLAMP_TO_EDGE) };

								was_really_a_hit = mask_value[0] >= 0.5f;
							}

							if (was_really_a_hit)
							{
								intersection_distance = temporary_intersection_distance;
								intersected_mesh_index = mesh_index;
								intersected_normal = normal;
								intersected_tangent = tangent;
								intersected_texture_coordinate = texture_coordinate;
							}
						}
					}
				}
			}

			//Was there a hit?
			if (intersection_distance != FLOAT32_MAXIMUM)
			{
				//Fill in the textures.
				impostor_albedo_texture.At(X, Y) = albedo_thickness_textures[intersected_mesh_index]->_Texture2D.Sample(intersected_texture_coordinate, AddressMode::CLAMP_TO_EDGE);
				impostor_opacity_texture.At(X, Y) = Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f);

				//Calculate the normal
				{
					Matrix3x3 tangent_space_matrix{ intersected_tangent, Vector3<float32>::CrossProduct(intersected_normal, intersected_tangent), intersected_normal };

					tangent_space_matrix._Matrix[0] = Vector3<float32>::Normalize(tangent_space_matrix._Matrix[0]);
					tangent_space_matrix._Matrix[1] = Vector3<float32>::Normalize(tangent_space_matrix._Matrix[1]);
					tangent_space_matrix._Matrix[2] = Vector3<float32>::Normalize(tangent_space_matrix._Matrix[2]);

					const Vector4<float32> normal_map_displacement_sample{ normal_map_displacement_textures[intersected_mesh_index]->_Texture2D.Sample(intersected_texture_coordinate, AddressMode::CLAMP_TO_EDGE) };
					Vector3<float32> normal_map_sample{ normal_map_displacement_sample._X, normal_map_displacement_sample._Y, normal_map_displacement_sample._Z };

					normal_map_sample._X = normal_map_sample._X * 2.0f - 1.0f;
					normal_map_sample._Y = normal_map_sample._Y * 2.0f - 1.0f;
					normal_map_sample._Z = normal_map_sample._Z * 2.0f - 1.0f;

					normal_map_sample.Normalize();

					Vector3<float32> surface_normal{ tangent_space_matrix * normal_map_sample };

					surface_normal *= -1.0f;

					surface_normal.Normalize();

					//Flip the normal if we hit the backside.
					if (Vector3<float32>::DotProduct(ray._Direction, surface_normal) < 0.0f)
					{
						surface_normal *= -1.0f;
					}

					surface_normal._X = surface_normal._X * 0.5f + 0.5f;
					surface_normal._Y = surface_normal._Y * 0.5f + 0.5f;
					surface_normal._Z = surface_normal._Z * 0.5f + 0.5f;

					impostor_normal_map_texture.At(X, Y) = Vector4<float32>(surface_normal, 0.5f);
					impostor_material_properties_texture.At(X, Y) = material_properties_textures[intersected_mesh_index]->_Texture2D.Sample(intersected_texture_coordinate, AddressMode::CLAMP_TO_EDGE);
				}
			}

			else
			{
				//Fill in the textures.
				impostor_albedo_texture.At(X, Y) = Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f);
				impostor_normal_map_texture.At(X, Y) = Vector4<float32>(0.5f, 0.5f, 1.0f, 0.5f);
				impostor_material_properties_texture.At(X, Y) = Vector4<float32>(1.0f, 0.0f, 1.0f, 0.0f);
				impostor_opacity_texture.At(X, Y) = Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f);
			}
		}
	}

	//Blur the black pixels of the albedo until there are none left!
	for (;;)
	{
		uint64 number_of_black_pixels{ 0 };

		Texture2D<Vector4<float32>> temporary_impostor_albedo_texture{ impostor_albedo_texture };

		for (uint32 Y{ 0 }; Y < dimensions._Height; ++Y)
		{
			for (uint32 X{ 0 }; X < dimensions._Width; ++X)
			{
				Vector4<float32> &impostor_albedo_texel{ impostor_albedo_texture.At(X, Y) };

				if (impostor_albedo_texel == Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f))
				{
					Vector4<float32> total_color{ 0.0f, 0.0f, 0.0f, 0.0f };
					float32 total_weight{ 0.0f };

					for (int8 y{ -1 }; y <= 1; ++y)
					{
						for (int8 x{ -1 }; x <= 1; ++x)
						{
							const uint32 sub_x{ CatalystBaseMath::Clamp<uint32>(X + x, 0, temporary_impostor_albedo_texture.GetWidth() - 1) };
							const uint32 sub_y{ CatalystBaseMath::Clamp<uint32>(Y + y, 0, temporary_impostor_albedo_texture.GetHeight() - 1) };

							const Vector4<float32>& temporary_impostor_albedo_texel{ temporary_impostor_albedo_texture.At(sub_x, sub_y) };

							if (temporary_impostor_albedo_texel != Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f))
							{
								total_color += temporary_impostor_albedo_texel;
								total_weight += 1.0f;
							}
						}
					}

					//Normalize the total color.
					if (total_weight > 0.0f)
					{
						total_color /= total_weight;

						//Write the blurred value.
						impostor_albedo_texel = total_color;
					}

					++number_of_black_pixels;
				}
			}
		}

		//If there are no black pixels left, it's done, finally!
		if (number_of_black_pixels == 0)
		{
			break;
		}
	}

	//Write the textures.
	{
		char buffer[128];
		sprintf_s(buffer, "%s\\%s_AlbedoThickness.png", raw_directory, name.data());

		PNGWriter::Write(impostor_albedo_texture, buffer);
	}

	{
		char buffer[128];
		sprintf_s(buffer, "%s\\%s_NormalMapDisplacement.png", raw_directory, name.data());

		PNGWriter::Write(impostor_normal_map_texture, buffer);
	}

	{
		char buffer[128];
		sprintf_s(buffer, "%s\\%s_MaterialProperties.png", raw_directory, name.data());

		PNGWriter::Write(impostor_material_properties_texture, buffer);
	}

	{
		char buffer[128];
		sprintf_s(buffer, "%s\\%s_Opacity.png", raw_directory, name.data());

		PNGWriter::Write(impostor_opacity_texture, buffer);
	}

	//Build the textures
	{
		//Build the texture 2D.
		Texture2DBuildParameters texture_2d_build_parameters;

		char output_buffer[128];
		sprintf_s(output_buffer, "%s\\Textures\\Impostors\\%s_AlbedoThickness", intermediate_directory, name.data());
		texture_2d_build_parameters._Output = output_buffer;
		char identifier_buffer[128];
		sprintf_s(identifier_buffer, "%s_AlbedoThickness", name.data());
		texture_2d_build_parameters._ID = identifier_buffer;
		texture_2d_build_parameters._DefaultWidth = 0;
		texture_2d_build_parameters._DefaultHeight = 0;
		char file_1_buffer[128];
		sprintf_s(file_1_buffer, "%s\\%s_AlbedoThickness.png", raw_directory, name.data());
		texture_2d_build_parameters._File1 = file_1_buffer;
		texture_2d_build_parameters._File2 = nullptr;
		texture_2d_build_parameters._File3 = nullptr;
		texture_2d_build_parameters._File4 = nullptr;
		texture_2d_build_parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		texture_2d_build_parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		texture_2d_build_parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		texture_2d_build_parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		texture_2d_build_parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		texture_2d_build_parameters._ApplyGammaCorrection = false;
		texture_2d_build_parameters._TransformFunction = nullptr;
		texture_2d_build_parameters._BaseMipmapLevel = 0;
		texture_2d_build_parameters._MipmapLevels = 2;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(texture_2d_build_parameters);
	}

	{
		//Build the texture 2D.
		Texture2DBuildParameters texture_2d_build_parameters;

		char output_buffer[128];
		sprintf_s(output_buffer, "%s\\Textures\\Impostors\\%s_NormalMapDisplacement", intermediate_directory, name.data());
		texture_2d_build_parameters._Output = output_buffer;
		char identifier_buffer[128];
		sprintf_s(identifier_buffer, "%s_NormalMapDisplacement", name.data());
		texture_2d_build_parameters._ID = identifier_buffer;
		texture_2d_build_parameters._DefaultWidth = 0;
		texture_2d_build_parameters._DefaultHeight = 0;
		char file_1_buffer[128];
		sprintf_s(file_1_buffer, "%s\\%s_NormalMapDisplacement.png", raw_directory, name.data());
		texture_2d_build_parameters._File1 = file_1_buffer;
		texture_2d_build_parameters._File2 = nullptr;
		texture_2d_build_parameters._File3 = nullptr;
		texture_2d_build_parameters._File4 = nullptr;
		texture_2d_build_parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		texture_2d_build_parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		texture_2d_build_parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		texture_2d_build_parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		texture_2d_build_parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		texture_2d_build_parameters._ApplyGammaCorrection = false;
		texture_2d_build_parameters._TransformFunction = nullptr;
		texture_2d_build_parameters._BaseMipmapLevel = 0;
		texture_2d_build_parameters._MipmapLevels = 2;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(texture_2d_build_parameters);
	}

	{
		//Build the texture 2D.
		Texture2DBuildParameters texture_2d_build_parameters;

		char output_buffer[128];
		sprintf_s(output_buffer, "%s\\Textures\\Impostors\\%s_MaterialProperties", intermediate_directory, name.data());
		texture_2d_build_parameters._Output = output_buffer;
		char identifier_buffer[128];
		sprintf_s(identifier_buffer, "%s_MaterialProperties", name.data());
		texture_2d_build_parameters._ID = identifier_buffer;
		texture_2d_build_parameters._DefaultWidth = 0;
		texture_2d_build_parameters._DefaultHeight = 0;
		char file_1_buffer[128];
		sprintf_s(file_1_buffer, "%s\\%s_MaterialProperties.png", raw_directory, name.data());
		texture_2d_build_parameters._File1 = file_1_buffer;
		texture_2d_build_parameters._File2 = nullptr;
		texture_2d_build_parameters._File3 = nullptr;
		texture_2d_build_parameters._File4 = nullptr;
		texture_2d_build_parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		texture_2d_build_parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		texture_2d_build_parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		texture_2d_build_parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		texture_2d_build_parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		texture_2d_build_parameters._ApplyGammaCorrection = false;
		texture_2d_build_parameters._TransformFunction = nullptr;
		texture_2d_build_parameters._BaseMipmapLevel = 0;
		texture_2d_build_parameters._MipmapLevels = 2;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(texture_2d_build_parameters);
	}

	{
		//Build the texture 2D.
		Texture2DBuildParameters texture_2d_build_parameters;

		char output_buffer[128];
		sprintf_s(output_buffer, "%s\\Textures\\Impostors\\%s_Opacity", intermediate_directory, name.data());
		texture_2d_build_parameters._Output = output_buffer;
		char identifier_buffer[128];
		sprintf_s(identifier_buffer, "%s_Opacity", name.data());
		texture_2d_build_parameters._ID = identifier_buffer;
		texture_2d_build_parameters._DefaultWidth = 0;
		texture_2d_build_parameters._DefaultHeight = 0;
		char file_1_buffer[128];
		sprintf_s(file_1_buffer, "%s\\%s_Opacity.png", raw_directory, name.data());
		texture_2d_build_parameters._File1 = file_1_buffer;
		texture_2d_build_parameters._File2 = nullptr;
		texture_2d_build_parameters._File3 = nullptr;
		texture_2d_build_parameters._File4 = nullptr;
		texture_2d_build_parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		texture_2d_build_parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		texture_2d_build_parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		texture_2d_build_parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		texture_2d_build_parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		texture_2d_build_parameters._ApplyGammaCorrection = true;
		texture_2d_build_parameters._TransformFunction = nullptr;
		texture_2d_build_parameters._BaseMipmapLevel = 0;
		texture_2d_build_parameters._MipmapLevels = 2;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(texture_2d_build_parameters);
	}

	//Build the impostor material.
	{
		//Build the material.
		MaterialBuildParameters material_build_parameters;

		char output_buffer[128];
		sprintf_s(output_buffer, "%s\\Materials\\%s", intermediate_directory, name.data());
		material_build_parameters._Output = output_buffer;
		char identifier_buffer[128];
		sprintf_s(identifier_buffer, "%s", name.data());
		material_build_parameters._ID = identifier_buffer;
		material_build_parameters._Type = MaterialResource::Type::MASKED;

		material_build_parameters._AlbedoThicknessComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
		char albedo_identifier_buffer[128];
		sprintf_s(albedo_identifier_buffer, "%s_AlbedoThickness", name.data());
		material_build_parameters._AlbedoThicknessComponent._TextureResourceIdentifier = HashString(albedo_identifier_buffer);
		
		material_build_parameters._NormalMapDisplacementComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
		char normal_map_identifier_buffer[128];
		sprintf_s(normal_map_identifier_buffer, "%s_NormalMapDisplacement", name.data());
		material_build_parameters._NormalMapDisplacementComponent._TextureResourceIdentifier = HashString(normal_map_identifier_buffer);
		
		material_build_parameters._MaterialPropertiesComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
		char material_properties_identifier_buffer[128];
		sprintf_s(material_properties_identifier_buffer, "%s_MaterialProperties", name.data());
		material_build_parameters._MaterialPropertiesComponent._TextureResourceIdentifier = HashString(material_properties_identifier_buffer);
		
		material_build_parameters._OpacityComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
		char opacity_identifier_buffer[128];
		sprintf_s(opacity_identifier_buffer, "%s_Opacity", name.data());
		material_build_parameters._OpacityComponent._TextureResourceIdentifier = HashString(opacity_identifier_buffer);
		
		material_build_parameters._EmissiveMultiplier = 0.0f;
		material_build_parameters._DoubleSided = false;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildMaterial(material_build_parameters);
	}

	//TODO: Unload the resources. (:
}
#endif