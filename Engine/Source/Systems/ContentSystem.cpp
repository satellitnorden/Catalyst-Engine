#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
//Header file.
#include <Systems/ContentSystem.h>

//Core.
#include <Core/Algorithms/HashAlgorithms.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/Time.h>

//Content.
#include <Content/Core/ContentCache.h>
#include <Content/AssetCompilers/MaterialAssetCompiler.h>
#include <Content/AssetCompilers/ModelAssetCompiler.h>
#include <Content/AssetCompilers/ScriptAssetCompiler.h>
#include <Content/AssetCompilers/Texture2DAssetCompiler.h>
#include <Content/AssetCompilers/TextureCubeAssetCompiler.h>

//File.
#include <File/Core/FileCore.h>
#include <File/Utilities/TextParsingUtilities.h>
#include <File/Writers/PNGWriter.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>

//Memory.
#include <Memory/PoolAllocator.h>

//Profiling.
#include <Profiling/Profiling.h>

//Resources.
#include <Resources/Utilities/ProceduralTreeGenerator.h>
#include <Resources/Utilities/ResourceBuildingUtilities.h>

//Systems.
#include <Systems/LogSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/TaskSystem.h>

//Constants.
#define ENGINE_ASSETS "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Assets"
#define ENGINE_COMPILED "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Compiled"
#define ENGINE_CONTENT_DEFINITIONS "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Content Definitions"
#define ENGINE_INTERMEDIATE "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Intermediate"
#define ENGINE_RAW "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Raw"
#define GAME_ASSETS "..\\..\\..\\Content\\Assets"
#define GAME_COMPILED "..\\..\\..\\Content\\Compiled"
#define GAME_CONTENT_DEFINITIONS "..\\..\\..\\Content\\Content Definitions"
#define GAME_INTERMEDIATE "..\\..\\..\\Content\\Intermediate"
#define GAME_RAW "..\\..\\..\\Content\\Raw"

//Singleton definition.
DEFINE_SINGLETON(ContentSystem);

/*
*	Initializes the content system.
*/
void ContentSystem::Initialize() NOEXCEPT
{
	//Register the native asset compilers
	RegisterAssetCompiler(MaterialAssetCompiler::Instance.Get());
	RegisterAssetCompiler(ModelAssetCompiler::Instance.Get());
	RegisterAssetCompiler(ScriptAssetCompiler::Instance.Get());
	RegisterAssetCompiler(Texture2DAssetCompiler::Instance.Get());
	RegisterAssetCompiler(TextureCubeAssetCompiler::Instance.Get());
}

/*
*	Registers an asset compiler.
*/
void ContentSystem::RegisterAssetCompiler(AssetCompiler *const RESTRICT asset_compiler) NOEXCEPT
{
	//Add the compiler.
	_AssetCompilers.Add(asset_compiler->AssetTypeIdentifier(), asset_compiler);

	//Also add en entry into the assets.
	_Assets.Add(asset_compiler->AssetTypeIdentifier(), HashTable<HashString, Asset *RESTRICT>());
}

/*
*	Compiles the content for engine.
*	Returns if new content was compiled.
*/
NO_DISCARD bool ContentSystem::CompileEngine() NOEXCEPT
{
	//Cache the start time.
	const TimePoint start_time{ GetCurrentTimePoint() };

	//Call PreCompile() on all asset compilers.
	for (AssetCompiler *const RESTRICT asset_compiler : _AssetCompilers.ValueIterator())
	{
		asset_compiler->PreCompile(CompilationDomain::ENGINE);
	}

	//Remember if new content was compiled.
	bool new_content_compiled{ false };
	
	//Parse engine content definitions.
	{
		//Set up the content cache.
		char content_cache_file_path[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(content_cache_file_path, "%s\\ContentCache.content_cache", ENGINE_CONTENT_DEFINITIONS);
		ContentCache content_cache{ content_cache_file_path };

		//Scan assets.
		new_content_compiled |= ScanAssetsInDirectory(CompilationDomain::ENGINE, &content_cache, ENGINE_ASSETS, nullptr);

		//Parse the content definitions.
		new_content_compiled |= ParseContentDefinitionsInDirectory(CompilationDomain::ENGINE, &content_cache, ENGINE_CONTENT_DEFINITIONS);

		//Write the content cache.
		content_cache.Write(content_cache_file_path);
	}

	//Wait for all tasks to finish.
	bool all_tasks_finished{ _Tasks.Empty() };

	while (!all_tasks_finished)
	{
		TaskSystem::Instance->DoWork(Task::Priority::LOW);

		all_tasks_finished = true;

		for (const Task *const RESTRICT task : _Tasks)
		{
			all_tasks_finished &= task->IsExecuted();
		}
	}

	//Clear the tasks.
	_Tasks.Clear();

	//Call PostCompile() on all asset compilers.
	for (AssetCompiler *const RESTRICT asset_compiler : _AssetCompilers.ValueIterator())
	{
		asset_compiler->PostCompile(CompilationDomain::ENGINE);
	}

	//Create asset collections if new content was compiled.
	if (new_content_compiled)
	{
		PROFILING_SCOPE("ContentSystem::CreateAssetCollections");

		CreateAssetCollections(ENGINE_COMPILED, nullptr);
	}

	LOG_INFORMATION("ContentSystem::CompileEngine took %f seconds.", start_time.GetSecondsSince());

	return new_content_compiled;
}

/*
*	Compiles the content for game.
*	Returns if new content was compiled.
*/
NO_DISCARD bool ContentSystem::CompileGame() NOEXCEPT
{
	//Cache the start time.
	const TimePoint start_time{ GetCurrentTimePoint() };

	//Call PreCompile() on all asset compilers.
	for (AssetCompiler *const RESTRICT asset_compiler : _AssetCompilers.ValueIterator())
	{
		asset_compiler->PreCompile(CompilationDomain::GAME);
	}

	//Remember if new content was compiled.
	bool new_content_compiled{ false };

	//Parse game content definitions.
	{
		//Set up the content cache.
		char content_cache_file_path[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(content_cache_file_path, "%s\\ContentCache.content_cache", GAME_CONTENT_DEFINITIONS);
		ContentCache content_cache{ content_cache_file_path };

		//Scan assets.
		new_content_compiled |= ScanAssetsInDirectory(CompilationDomain::GAME, &content_cache, GAME_ASSETS, nullptr);

		//Parse the content definitions.
		new_content_compiled |= ParseContentDefinitionsInDirectory(CompilationDomain::GAME, &content_cache, GAME_CONTENT_DEFINITIONS);

		//Write the content cache.
		content_cache.Write(content_cache_file_path);
	}

	//Wait for all tasks to finish.
	bool all_tasks_finished{ _Tasks.Empty() };

	while (!all_tasks_finished)
	{
		TaskSystem::Instance->DoWork(Task::Priority::LOW);

		all_tasks_finished = true;

		for (const Task *const RESTRICT task : _Tasks)
		{
			all_tasks_finished &= task->IsExecuted();
		}
	}

	//Clear the tasks.
	_Tasks.Clear();

	//Call PostCompile() on all asset compilers.
	for (AssetCompiler *const RESTRICT asset_compiler : _AssetCompilers.ValueIterator())
	{
		asset_compiler->PostCompile(CompilationDomain::GAME);
	}

	//Create asset collections if new content was compiled.
	if (new_content_compiled)
	{
		PROFILING_SCOPE("ContentSystem::CreateAssetCollections");

		CreateAssetCollections(GAME_COMPILED, nullptr);
	}

	LOG_INFORMATION("ContentSystem::CompileGame took %f seconds.", start_time.GetSecondsSince());

	return new_content_compiled;
}

/*
*	Loads assets from the given directory path.
*/
void ContentSystem::LoadAssets(const char *const RESTRICT directory_path) NOEXCEPT
{
	//Iterate over the items in the directory.
	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		//Cache the file path.
		const std::string file_path{ entry.path().u8string() };

		//Scan recursively if this is a directory.
		if (entry.is_directory())
		{
			LoadAssets(file_path.c_str());

			continue;
		}

		//Retrieve the file extension.
		const File::Extension file_extension{ File::GetExtension(file_path.c_str()) };

		//Is this an asset?
		if (file_extension == File::Extension::CA)
		{
			LoadAsset(file_path.c_str());
		}

		//Is this an asset collection?
		else if (file_extension == File::Extension::CAC)
		{
			LoadAssetCollection(file_path.c_str());
		}
	}

	//Call PostLoad() on all asset compilers.
	for (AssetCompiler *const RESTRICT asset_compiler : _AssetCompilers.ValueIterator())
	{
		asset_compiler->PostLoad();
	}
}

/*
*	Scans assets in the given directory.
*	Returns if new content was compiled.
*/
NO_DISCARD bool ContentSystem::ScanAssetsInDirectory
(
	const CompilationDomain compilation_domain,
	ContentCache *const RESTRICT content_cache,
	const char *const RESTRICT directory_path,
	const char *const RESTRICT collection
) NOEXCEPT
{
	//Remember if new content was compiled.
	bool new_content_compiled{ false };

	//Scan the directory!
	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		//Scan recursively if this is a directory.
		if (entry.is_directory())
		{
			//Cache the directory path.
			const std::string _directory_path{ entry.path().u8string() };

			//Check if this should be a collection.
			const char *RESTRICT _collection{ collection };

			//If we already have a package, no need to check for a new one.
			if (!_collection)
			{
				const size_t position{ _directory_path.find("COLLECTION ") };

				if (position != std::string::npos)
				{
					_collection = &_directory_path[position + strlen("COLLECTION ")];
				}
			}

			new_content_compiled |= ScanAssetsInDirectory(compilation_domain, content_cache, entry.path().u8string().data(), _collection);

			continue;
		}

		//Cache the file path string.
		const std::string file_path{ entry.path().string() };

		//Calculate the identifier.
		const uint64 identifier{ HashAlgorithms::MurmurHash64(file_path.data(), sizeof(char) * file_path.length()) };

		//Retrieve the last write time.
		const std::filesystem::file_time_type last_write_time{ std::filesystem::last_write_time(file_path) };

		//Figure out the extension.
		std::string extension;

		{
			const size_t last_dot_position{ file_path.find_last_of(".") };
			extension = file_path.substr(last_dot_position + 1, std::string::npos);
		}

		//Figure out the name.
		std::string name;

		{
			const size_t last_slash_position{ file_path.find_last_of("\\") };
			name = file_path.substr(last_slash_position + 1, file_path.length() - last_slash_position - strlen(extension.c_str()) - 2);
		}

		//Find the asset compiler for this file type.
		AssetCompiler* RESTRICT asset_compiler;

		{
			AssetCompiler* const RESTRICT* const RESTRICT _asset_compiler{ _AssetCompilers.Find(HashString(extension.c_str())) };
			asset_compiler = _asset_compiler ? *_asset_compiler : nullptr;
		}

		//Couldn't find an asset compiler, ignore!
		if (!asset_compiler)
		{
			continue;
		}

		//Check if it needs a recompile.
		if (!TEST_BIT(asset_compiler->_Flags, AssetCompiler::Flags::ALWAYS_COMPILE) && !content_cache->NeedsRecompile(identifier, last_write_time))
		{
			continue;
		}

		LOG_INFORMATION("Compiling %s", file_path.data());

		//Set up the compile context.
		AssetCompiler::CompileContext compile_context;

		compile_context._CompilationDomain = compilation_domain;
		compile_context._Identifier = identifier;
		compile_context._LastWriteTime = last_write_time;
		compile_context._Collection = collection;
		compile_context._FilePath = file_path.c_str();
		compile_context._Name = name.c_str();
		compile_context._ContentCache = content_cache;
		compile_context._TaskAllocator = &_TaskAllocator;
		compile_context._Tasks = &_Tasks;

		//Compile!
		asset_compiler->Compile(compile_context);

		//Update the last write time.
		content_cache->UpdateLastWriteTime(identifier, last_write_time);

		//New content was compiled!
		new_content_compiled = true;
	}

	return new_content_compiled;
}

/*
*	Creates asset collections from the given directory path.
*/
void ContentSystem::CreateAssetCollections(const char *const RESTRICT directory_path, BinaryFile<BinaryFileMode::OUT> *const RESTRICT file) NOEXCEPT
{
	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		//If this is a collection directory, create the file.
		if (entry.is_directory())
		{
			//Cache the directory path.
			const std::string _directory_path{ entry.path().u8string() };

			//Determine the sub-directory path.
			std::string sub_directory_path{ _directory_path };

			{
				const size_t position{ _directory_path.find_last_of('\\') };

				if (position != std::string::npos)
				{
					sub_directory_path = _directory_path.substr(position + 1);
				}
			}

			//Check for colletions.
			const size_t position{ sub_directory_path.find("COLLECTION ") };

			if (position != std::string::npos)
			{
				//Determine out the collection name.
				const std::string collection_name{ &sub_directory_path[position + strlen("COLLECTION ")] };

				//Determine the collection path.
				char collection_path[MAXIMUM_FILE_PATH_LENGTH];
				sprintf_s(collection_path, "%s\\..\\Collections\\%s.cac", directory_path, collection_name.c_str());

				//Set up the file.
				BinaryFile<BinaryFileMode::OUT> _file{ collection_path };
				
				//Call recursively!
				CreateAssetCollections(_directory_path.c_str(), &_file);

				//Close the file.
				_file.Close();
			}

			else
			{
				//Call recursively!
				CreateAssetCollections(_directory_path.c_str(), file);
			}

			continue;
		}

		//Cache the file path.
		const std::string file_path{ entry.path().u8string() };

		//Open the input file.
		BinaryFile<BinaryFileMode::IN> input_file{ file_path.c_str() };

		//Write the file size.
		const uint64 file_size{ input_file.Size() };
		file->Write(&file_size, sizeof(uint64));

		//Write the data
		DynamicArray<byte> input_buffer;
		input_buffer.Upsize<false>(file_size);
		input_file.Read(input_buffer.Data(), file_size);
		file->Write(input_buffer.Data(), file_size);

		//Close the input file.
		input_file.Close();
	}
}

/*
*	Load a single asset from the given file path.
*/
void ContentSystem::LoadAsset(const char *const RESTRICT file_path) NOEXCEPT
{
	//Load the file into a stream archive.
	StreamArchive stream_archive;

	{
		PROFILING_SCOPE("ContentSystem::LoadAsset::CreateStreamArchive");

		BinaryFile<BinaryFileMode::IN> file{ file_path };

		{
			PROFILING_SCOPE("ContentSystem::LoadAsset::AllocateStreamArchive");
			stream_archive.Resize(file.Size());
		}
		
		{
			PROFILING_SCOPE("ContentSystem::LoadAsset::ReadStreamArchive");
			file.Read(stream_archive.Data(), file.Size());
		}

		file.Close();
	}

	//Set up the stream archive position.
	uint64 stream_archive_position{ 0 };

	//Read the asset header.
	AssetHeader asset_header;
	stream_archive.Read(&asset_header, sizeof(AssetHeader), &stream_archive_position);

	//Find the asset compiler for this asset type.
	AssetCompiler *RESTRICT asset_compiler;

	{
		AssetCompiler *const RESTRICT *const RESTRICT _asset_compiler{ _AssetCompilers.Find(asset_header._AssetTypeIdentifier) };
		asset_compiler = _asset_compiler ? *_asset_compiler : nullptr;
	}

	//Couldn't find an asset compiler, ignore!
	if (!asset_compiler)
	{
		ASSERT(false, "Couldn't find asset compiler!");

		return;
	}

	//Set up the load context.
	AssetCompiler::LoadContext load_context;

	load_context._StreamArchive = &stream_archive;
	load_context._StreamArchivePosition = stream_archive_position;
	load_context._TaskAllocator = &_TaskAllocator;
	load_context._Tasks = &_Tasks;

	//Load!
	Asset *const RESTRICT new_asset{ asset_compiler->Load(load_context) };

	//Copy the asset header.
	Memory::Copy(&new_asset->_Header, &asset_header, sizeof(AssetHeader));

	//Add it to the list of assets.
	HashTable<HashString, Asset *RESTRICT> *const RESTRICT assets{ _Assets.Find(asset_compiler->AssetTypeIdentifier()) };

	assets->Add(asset_header._AssetIdentifier, new_asset);

	//Wait for all tasks to finish.
	bool all_tasks_finished{ _Tasks.Empty() };

	while (!all_tasks_finished)
	{
		TaskSystem::Instance->DoWork(Task::Priority::LOW);

		all_tasks_finished = true;

		for (const Task *const RESTRICT task : _Tasks)
		{
			all_tasks_finished &= task->IsExecuted();
		}
	}

	//Clear the tasks.
	_Tasks.Clear();
}

/*
*	Load a single asset collection from the given file path.
*/
void ContentSystem::LoadAssetCollection(const char *const RESTRICT file_path) NOEXCEPT
{
	//Load the file into a stream archive.
	StreamArchive stream_archive;

	{
		PROFILING_SCOPE("ContentSystem::LoadAssetCollection::CreateStreamArchive");

		BinaryFile<BinaryFileMode::IN> file{ file_path };

		{
			PROFILING_SCOPE("ContentSystem::LoadAssetCollection::AllocateStreamArchive");
			stream_archive.Resize(file.Size());
		}

		{
			PROFILING_SCOPE("ContentSystem::LoadAssetCollection::ReadStreamArchive");
			file.Read(stream_archive.Data(), file.Size());
		}

		file.Close();
	}

	//Read the whole file.
	uint64 stream_archive_position{ 0 };

	while (stream_archive_position < stream_archive.Size())
	{
		//Remember the original stream archive position.
		const uint64 original_stream_archive_position{ stream_archive_position };

		//Read the file size.
		uint64 file_size{ 0 };
		stream_archive.Read(&file_size, sizeof(uint64), &stream_archive_position);

		//Read the asset header.
		AssetHeader asset_header;
		stream_archive.Read(&asset_header, sizeof(AssetHeader), &stream_archive_position);

		//Find the asset compiler for this asset type.
		AssetCompiler *RESTRICT asset_compiler;

		{
			AssetCompiler *const RESTRICT *const RESTRICT _asset_compiler{ _AssetCompilers.Find(asset_header._AssetTypeIdentifier) };
			asset_compiler = _asset_compiler ? *_asset_compiler : nullptr;
		}

		//Couldn't find an asset compiler, ignore!
		if (!asset_compiler)
		{
			ASSERT(false, "Couldn't find asset compiler!");

			stream_archive_position = original_stream_archive_position + file_size + sizeof(uint64);

			continue;
		}

		//Set up the load context.
		AssetCompiler::LoadContext load_context;

		load_context._StreamArchive = &stream_archive;
		load_context._StreamArchivePosition = stream_archive_position;
		load_context._TaskAllocator = &_TaskAllocator;
		load_context._Tasks = &_Tasks;

		//Load!
		Asset *const RESTRICT new_asset{ asset_compiler->Load(load_context) };

		//Copy the asset header.
		Memory::Copy(&new_asset->_Header, &asset_header, sizeof(AssetHeader));

		//Add it to the list of assets.
		HashTable<HashString, Asset *RESTRICT> *const RESTRICT assets{ _Assets.Find(asset_compiler->AssetTypeIdentifier()) };

		assets->Add(asset_header._AssetIdentifier, new_asset);

		//Advance the stream archive position.
		stream_archive_position = original_stream_archive_position + file_size + sizeof(uint64);
	}

	//Wait for all tasks to finish.
	uint64 number_of_tasks_left{ _Tasks.Size() };

	while (number_of_tasks_left > 0)
	{
		uint64 _number_of_tasks_left{ 0 };

		TaskSystem::Instance->DoWork(Task::Priority::LOW);

		for (const Task *const RESTRICT task : _Tasks)
		{
			if (!task->IsExecuted())
			{
				++_number_of_tasks_left;
			}
		}

		if (number_of_tasks_left != _number_of_tasks_left)
		{
			number_of_tasks_left = _number_of_tasks_left;

			LOG_INFORMATION("Number of ascynhronous compile tasks left: %llu", number_of_tasks_left);
		}
	}

	//Clear the tasks.
	_Tasks.Clear();
}

/*
*	Parses content definitions in the given directory.
*	Returns if new content was compiled.
*/
NO_DISCARD bool ContentSystem::ParseContentDefinitionsInDirectory(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const char *const RESTRICT directory_path) NOEXCEPT
{
	enum class DelayedCompileType : uint8
	{
		IMPOSTOR_MATERIAL
	};
	struct DelayedCompile final
	{
		DelayedCompileType _Type;
		DynamicString _Package;
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

		//Remember the package.
		DynamicString package;

		//Remember if this should always compile.
		bool always_compile{ false };

		//Iterate over the lines.
		std::string current_line;

		while (std::getline(file, current_line))
		{
			if (current_line.find("PACKAGE") != std::string::npos)
			{
				StaticArray<DynamicString, 2> arguments;

				TextParsingUtilities::ParseSpaceSeparatedArguments
				(
					current_line.data(),
					current_line.length(),
					arguments.Data()
				);

				package = arguments[1];
			}

			else if (current_line == "ALWAYS_COMPILE")
			{
				always_compile = true;
			}

			else if (current_line == "FONT")
			{
				ParseFont(compilation_domain, content_cache, name, package, file);
			}

			else if (current_line == "PROCEDURAL_TREE_MODEL")
			{
				ParseProceduralTreeModel(compilation_domain, content_cache, name, package, file);
			}

			else if (current_line == "IMPOSTOR_MATERIAL")
			{
				//Impostor materials depends on other resources, so delay the compile.
				delayed_compiles.Emplace(DelayedCompile{ DelayedCompileType::IMPOSTOR_MATERIAL, package, name, file_path });

				break;
			}

			else
			{
				ASSERT(false, "Unknown line " << current_line.data());
			}
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
				ParseImpostorMaterial(compilation_domain, content_cache, delayed_compile._Name, delayed_compile._Package, file);
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
*	Parses a Font from the given file.
*/
void ContentSystem::ParseFont(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, const DynamicString &package, std::ifstream &file) NOEXCEPT
{
	//Calculate the intermediate directory.
	char intermediate_directory[MAXIMUM_FILE_PATH_LENGTH];

	switch (compilation_domain)
	{
		case CompilationDomain::ENGINE:
		{
			sprintf_s(intermediate_directory, ENGINE_INTERMEDIATE "\\%s\\Fonts", package.Length() > 0 ? package.Data() : "");

			break;
		}

		case CompilationDomain::GAME:
		{
			sprintf_s(intermediate_directory, GAME_INTERMEDIATE "\\%s\\Fonts", package.Length() > 0 ? package.Data() : "");

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
	FontBuildParameters parameters;

	//Set the output.
	char output_buffer[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(output_buffer, "%s\\%s", intermediate_directory, name.data());
	parameters._Output = output_buffer;

	//Set the id.
	parameters._ID = name.data();

	//Read all of the lines.
	std::string line;
	StaticArray<DynamicString, 2> arguments;

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

		if (arguments[0] == "FILE")
		{
			parameters._File = arguments[1].Data();
		}

		else
		{
			ASSERT(false, "Couldn't parse argument " << arguments[0]);
		}
	}

	//Build!
	ResourceSystem::Instance->GetResourceBuildingSystem()->BuildFont(parameters);
}

/*
*	Parses a procedural tree model from the given file.
*/
void ContentSystem::ParseProceduralTreeModel(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, const DynamicString &package, std::ifstream &file) NOEXCEPT
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

	/*
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
	*/
}

/*
*	Loads the texture 2D with the given identifier.
*/
NO_DISCARD AssetPointer<Texture2DAsset> LoadTexture2DResource(const char *const RESTRICT directory_path, const HashString identifier) NOEXCEPT
{
#if 0
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
#endif
	return AssetPointer<Texture2DAsset>();
}

/*
*	Parses an Impostor Material from the given file.
*/
void ContentSystem::ParseImpostorMaterial(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, const DynamicString &package, std::ifstream &file) NOEXCEPT
{
#if 0
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
	DynamicArray<AssetPointer<Texture2DAsset>> albedo_thickness_textures;
	DynamicArray<AssetPointer<Texture2DAsset>> normal_map_displacement_textures;
	DynamicArray<AssetPointer<Texture2DAsset>> material_properties_textures;
	DynamicArray<AssetPointer<Texture2DAsset>> opacity_textures;

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
				albedo_thickness_textures.Back() = LoadTexture2DResource(buffer, material_resource->_AlbedoThicknessComponent._Texture->_Header._AssetIdentifier);
			}

			if (material_resource->_NormalMapDisplacementComponent._Type == MaterialResource::MaterialResourceComponent::Type::TEXTURE)
			{
				normal_map_displacement_textures.Back() = LoadTexture2DResource(buffer, material_resource->_NormalMapDisplacementComponent._Texture->_Header._AssetIdentifier);
			}

			if (material_resource->_MaterialPropertiesComponent._Type == MaterialResource::MaterialResourceComponent::Type::TEXTURE)
			{
				material_properties_textures.Back() = LoadTexture2DResource(buffer, material_resource->_MaterialPropertiesComponent._Texture->_Header._AssetIdentifier);
			}

			if (material_resource->_OpacityComponent._Type == MaterialResource::MaterialResourceComponent::Type::TEXTURE)
			{
				opacity_textures.Back() = LoadTexture2DResource(buffer, material_resource->_OpacityComponent._Texture->_Header._AssetIdentifier);
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
								//const Vector4<float32> mask_value{ opacity_textures[mesh_index]->_Texture2D.Sample(texture_coordinate, AddressMode::CLAMP_TO_EDGE) };

								//was_really_a_hit = mask_value[0] >= 0.5f;
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
				//impostor_albedo_texture.At(X, Y) = albedo_thickness_textures[intersected_mesh_index]->_Texture2D.Sample(intersected_texture_coordinate, AddressMode::CLAMP_TO_EDGE);
				impostor_opacity_texture.At(X, Y) = Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f);

				//Calculate the normal
				{
					Matrix3x3 tangent_space_matrix{ intersected_tangent, Vector3<float32>::CrossProduct(intersected_normal, intersected_tangent), intersected_normal };

					tangent_space_matrix._Matrix[0] = Vector3<float32>::Normalize(tangent_space_matrix._Matrix[0]);
					tangent_space_matrix._Matrix[1] = Vector3<float32>::Normalize(tangent_space_matrix._Matrix[1]);
					tangent_space_matrix._Matrix[2] = Vector3<float32>::Normalize(tangent_space_matrix._Matrix[2]);

					//const Vector4<float32> normal_map_displacement_sample{ normal_map_displacement_textures[intersected_mesh_index]->_Texture2D.Sample(intersected_texture_coordinate, AddressMode::CLAMP_TO_EDGE) };
					//Vector3<float32> normal_map_sample{ normal_map_displacement_sample._X, normal_map_displacement_sample._Y, normal_map_displacement_sample._Z };

					//normal_map_sample._X = normal_map_sample._X * 2.0f - 1.0f;
					//normal_map_sample._Y = normal_map_sample._Y * 2.0f - 1.0f;
					//normal_map_sample._Z = normal_map_sample._Z * 2.0f - 1.0f;

					//normal_map_sample.Normalize();

					//Vector3<float32> surface_normal{ tangent_space_matrix * normal_map_sample };

					//surface_normal *= -1.0f;

					//surface_normal.Normalize();

					//Flip the normal if we hit the backside.
					//if (Vector3<float32>::DotProduct(ray._Direction, surface_normal) < 0.0f)
					//{
					//	surface_normal *= -1.0f;
					//}

					//surface_normal._X = surface_normal._X * 0.5f + 0.5f;
					//surface_normal._Y = surface_normal._Y * 0.5f + 0.5f;
					//surface_normal._Z = surface_normal._Z * 0.5f + 0.5f;

					//impostor_normal_map_texture.At(X, Y) = Vector4<float32>(surface_normal, 0.5f);
					//impostor_material_properties_texture.At(X, Y) = material_properties_textures[intersected_mesh_index]->_Texture2D.Sample(intersected_texture_coordinate, AddressMode::CLAMP_TO_EDGE);
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
#endif
}
#endif