//Header file.
#include <Systems/ContentSystem.h>

//Core.
#include <Core/Algorithms/HashAlgorithms.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/Time.h>

//Content.
#include <Content/Core/ContentCache.h>
#include <Content/AssetCompilers/AnimatedModelAssetCompiler.h>
#include <Content/AssetCompilers/EntityAssetCompiler.h>
#include <Content/AssetCompilers/FontAssetCompiler.h>
#include <Content/AssetCompilers/ImpostorMaterialAssetCompiler.h>
#include <Content/AssetCompilers/LevelAssetCompiler.h>
#include <Content/AssetCompilers/MaterialAssetCompiler.h>
#include <Content/AssetCompilers/ModelAssetCompiler.h>
#include <Content/AssetCompilers/SettingsAssetCompiler.h>
#include <Content/AssetCompilers/Texture2DAssetCompiler.h>
#include <Content/AssetCompilers/TextureCubeAssetCompiler.h>

//File.
#include <File/Core/BinaryInputFile.h>

//Profiling.
#include <Profiling/Profiling.h>

//Systems.
#include <Systems/LogSystem.h>
#include <Systems/TaskSystem.h>

//STL.
#include <filesystem>

//Constants.
#define ENGINE_ASSETS "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Assets"
#define ENGINE_COMPILED "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Compiled"
#define GAME_ASSETS "..\\..\\..\\Content\\Assets"
#define GAME_COMPILED "..\\..\\..\\Content\\Compiled"

//Singleton definition.
DEFINE_SINGLETON(ContentSystem);

/*
*	Initializes the content system.
*/
void ContentSystem::Initialize() NOEXCEPT
{
	//Register the native asset compilers
	RegisterAssetCompiler(AnimatedModelAssetCompiler::Instance.Get());
	RegisterAssetCompiler(EntityAssetCompiler::Instance.Get());
	RegisterAssetCompiler(FontAssetCompiler::Instance.Get());
	RegisterAssetCompiler(ImpostorMaterialAssetCompiler::Instance.Get());
	RegisterAssetCompiler(LevelAssetCompiler::Instance.Get());
	RegisterAssetCompiler(MaterialAssetCompiler::Instance.Get());
	RegisterAssetCompiler(ModelAssetCompiler::Instance.Get());
	RegisterAssetCompiler(SettingsAssetCompiler::Instance.Get());
	RegisterAssetCompiler(Texture2DAssetCompiler::Instance.Get());
	RegisterAssetCompiler(TextureCubeAssetCompiler::Instance.Get());
}

/*
*	Terminates the content system.
*/
void ContentSystem::Terminate() NOEXCEPT
{
	//Tell asset pointers to stop reference counting.
	AssetPointerData::_DoReferenceCounting = false;
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

#if !defined(CATALYST_CONFIGURATION_FINAL)
/*
*	Compiles the content for engine.
*	Returns if new content was compiled.
*/
bool ContentSystem::CompileEngine() NOEXCEPT
{
	//Cache the start time.
	const TimePoint start_time;

	//Call PreCompile() on all asset compilers.
	for (AssetCompiler *const RESTRICT asset_compiler : _AssetCompilers.ValueIterator())
	{
		asset_compiler->PreCompile(CompilationDomain::ENGINE);
	}

	//Compile!
	CompileResult compile_result;

RECOMPILE:

	//Always reset '_TriggerCompile'.
	compile_result._TriggerRecompile = false;

	{
		//Set up the content cache.
		char content_cache_file_path[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(content_cache_file_path, "%s\\ContentCache", ENGINE_COMPILED);
		ContentCache content_cache{ content_cache_file_path };

		//Compile assets.
		CompileAssetsInDirectory(CompilationDomain::ENGINE, &content_cache, ENGINE_ASSETS, nullptr, &compile_result);

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

	//Recompile, if requested.
	if (compile_result._TriggerRecompile)
	{
		goto RECOMPILE;
	}

	//Create asset collections if new assets were compiled.
	if (compile_result._NewAssetsCompiled)
	{
		PROFILING_SCOPE("ContentSystem::CreateAssetCollections");

		CreateAssetCollections(ENGINE_COMPILED, nullptr);
	}

	LOG_INFORMATION("ContentSystem::CompileEngine took %f seconds.", start_time.GetSecondsSince());

	return compile_result._NewAssetsCompiled;
}

/*
*	Compiles the content for game.
*	Returns if new content was compiled.
*/
bool ContentSystem::CompileGame() NOEXCEPT
{
	//Cache the start time.
	const TimePoint start_time;

	//Call PreCompile() on all asset compilers.
	for (AssetCompiler *const RESTRICT asset_compiler : _AssetCompilers.ValueIterator())
	{
		asset_compiler->PreCompile(CompilationDomain::GAME);
	}
	
	//Compile!
	CompileResult compile_result;

RECOMPILE:

	//Always reset '_TriggerCompile'.
	compile_result._TriggerRecompile = false;

	{
		//Set up the content cache.
		char content_cache_file_path[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(content_cache_file_path, "%s\\ContentCache", GAME_COMPILED);
		ContentCache content_cache{ content_cache_file_path };

		//Compile assets.
		CompileAssetsInDirectory(CompilationDomain::GAME, &content_cache, GAME_ASSETS, nullptr, &compile_result);

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

	//Recompile, if requested.
	if (compile_result._TriggerRecompile)
	{
		goto RECOMPILE;
	}

	//Create asset collections if new assets were compiled.
	if (compile_result._NewAssetsCompiled)
	{
		PROFILING_SCOPE("ContentSystem::CreateAssetCollections");

		CreateAssetCollections(GAME_COMPILED, nullptr);
	}

	LOG_INFORMATION("ContentSystem::CompileGame took %f seconds.", start_time.GetSecondsSince());

	return compile_result._NewAssetsCompiled;
}
#endif

/*
*	Loads assets from the given directory path.
*/
void ContentSystem::LoadAssets(const char *const RESTRICT directory_path) NOEXCEPT
{
	//Iterate over the items in the directory.
	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		//Cache the file path.
		const std::string file_path{ entry.path().string() };

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
*	Load a single asset from the given file path.
*/
void ContentSystem::LoadAsset(const char *const RESTRICT file_path) NOEXCEPT
{
	//Load the file into a stream archive.
	StreamArchive stream_archive;

	{
		PROFILING_SCOPE("ContentSystem::LoadAsset::CreateStreamArchive");

		BinaryInputFile file{ file_path };

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
	load_context._Asset = nullptr;

	//Load!
	Asset* const RESTRICT new_asset{ asset_compiler->Load(load_context) };

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

		BinaryInputFile file{ file_path };

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
		load_context._Asset = nullptr;

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

	//Call PostLoad() on all asset compilers.
	for (AssetCompiler *const RESTRICT asset_compiler : _AssetCompilers.ValueIterator())
	{
		asset_compiler->PostLoad();
	}
}

/*
*	Compiles assets in the given directory.
*/
void ContentSystem::CompileAssetsInDirectory
(
	const CompilationDomain compilation_domain,
	ContentCache *const RESTRICT content_cache,
	const char *const RESTRICT directory_path,
	const char *const RESTRICT collection,
	CompileResult *const RESTRICT compile_result
) NOEXCEPT
{
	//Scan the directory!
	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		//Scan recursively if this is a directory.
		if (entry.is_directory())
		{
			//Cache the directory path.
			const std::string _directory_path{ entry.path().string() };

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

			CompileAssetsInDirectory(compilation_domain, content_cache, entry.path().string().data(), _collection, compile_result);

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

		//New asset was compiled!
		compile_result->_NewAssetsCompiled = true;

		//Trigger a recompile if this asset compiler has requested it.
		if (asset_compiler->_TriggerRecompile)
		{
			compile_result->_TriggerRecompile = true;
		}
	}
}

/*
*	Creates asset collections from the given directory path.
*/
void ContentSystem::CreateAssetCollections(const char *const RESTRICT directory_path, BinaryOutputFile *const RESTRICT file) NOEXCEPT
{
	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		//If this is a collection directory, create the file.
		if (entry.is_directory())
		{
			//Cache the directory path.
			const std::string _directory_path{ entry.path().string() };

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
				BinaryOutputFile _file{ collection_path };
				
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
		const std::string file_path{ entry.path().string() };

		//Ignore the content cache.
		if (file_path.find("ContentCache") != std::string::npos)
		{
			continue;
		}

		//Open the input file.
		BinaryInputFile input_file{ file_path.c_str() };

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