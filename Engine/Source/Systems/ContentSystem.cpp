//Header file.
#include <Systems/ContentSystem.h>

//Core.
#include <Core/Algorithms/HashAlgorithms.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/Time.h>

//Content.
#include <Content/Core/ContentCache.h>
#include <Content/AssetCompilers/AnimatedModelAssetCompiler.h>
#include <Content/AssetCompilers/AnimationAssetCompiler.h>
#include <Content/AssetCompilers/AudioAssetCompiler.h>
#include <Content/AssetCompilers/EntityAssetCompiler.h>
#include <Content/AssetCompilers/FontAssetCompiler.h>
#include <Content/AssetCompilers/ImpostorMaterialAssetCompiler.h>
#include <Content/AssetCompilers/LevelAssetCompiler.h>
#include <Content/AssetCompilers/MaterialAssetCompiler.h>
#include <Content/AssetCompilers/ModelAssetCompiler.h>
#include <Content/AssetCompilers/RawDataAssetCompiler.h>
#include <Content/AssetCompilers/RenderPipelineAssetCompiler.h>
#include <Content/AssetCompilers/SettingsAssetCompiler.h>
#include <Content/AssetCompilers/Texture2DAssetCompiler.h>
#include <Content/AssetCompilers/TextureCubeAssetCompiler.h>

//File.
#include <File/Core/BinaryInputFile.h>
#include <File/Core/BinaryOutputFile.h>

//Profiling.
#include <Profiling/Profiling.h>

//Systems.
#if !defined(CATALYST_CONFIGURATION_FINAL)
	#include <Systems/DebugSystem.h>
	#include <Systems/ImGuiSystem.h>
#endif
#include <Systems/LogSystem.h>
#include <Systems/TaskSystem.h>

//Third party.
#if !defined(CATALYST_CONFIGURATION_FINAL)
	#include <ImGui/imgui.h>
#endif

//STL.
#include <filesystem>

//Constants.
#define ENGINE_ASSETS "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Assets"
#define ENGINE_RENDERING "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Rendering"
#define ENGINE_COMPILED "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Compiled"
#define GAME_ASSETS "..\\..\\..\\Content\\Assets"
#define GAME_RENDERING "..\\..\\..\\Rendering"
#define GAME_COMPILED "..\\..\\..\\Content\\Compiled"

#if !defined(CATALYST_CONFIGURATION_FINAL)
//Denotes whether or not the statistics window is open.
bool STATISTICS_WINDOW_OPEN{ false };
bool UNCOMPRESSED_TEXTURES_WINDOW_OPEN{ false };
#endif

/*
*	Initializes the content system.
*/
void ContentSystem::Initialize() NOEXCEPT
{
	//Register the native asset compilers
	RegisterAssetCompiler(AnimatedModelAssetCompiler::Instance);
	RegisterAssetCompiler(AnimationAssetCompiler::Instance);
	RegisterAssetCompiler(AudioAssetCompiler::Instance);
	RegisterAssetCompiler(EntityAssetCompiler::Instance);
	RegisterAssetCompiler(FontAssetCompiler::Instance);
	RegisterAssetCompiler(ImpostorMaterialAssetCompiler::Instance);
	RegisterAssetCompiler(LevelAssetCompiler::Instance);
	RegisterAssetCompiler(MaterialAssetCompiler::Instance);
	RegisterAssetCompiler(ModelAssetCompiler::Instance);
	RegisterAssetCompiler(RawDataAssetCompiler::Instance);
	RegisterAssetCompiler(RenderPipelineAssetCompiler::Instance);
	RegisterAssetCompiler(SettingsAssetCompiler::Instance);
	RegisterAssetCompiler(Texture2DAssetCompiler::Instance);
	RegisterAssetCompiler(TextureCubeAssetCompiler::Instance);

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Register debug commands.
	DebugSystem::Instance->RegisterCheckboxDebugCommand
	(
		"Statistics\\Assets",
		[](DebugCommand *const RESTRICT debug_command, void *const RESTRICT user_data)
		{
			const bool is_checked{ debug_command->_State._CheckboxState._IsChecked };

			if (debug_command->_State._CheckboxState._IsChecked && !STATISTICS_WINDOW_OPEN)
			{
				ImGuiSystem::Instance->RegisterGameWindow
				(
					ImGuiSystem::GameWindow::LEFT,
					[](const Vector2<float32> minimum, const Vector2<float32> maximum)
					{
						return ContentSystem::Instance->WindowCallback(minimum, maximum);
					}
				);

				UNCOMPRESSED_TEXTURES_WINDOW_OPEN = false;
			}

			STATISTICS_WINDOW_OPEN = is_checked;
		},
		nullptr
	);

	DebugSystem::Instance->RegisterCheckboxDebugCommand
	(
		"Statistics\\Uncompressed Textures",
		[](DebugCommand *const RESTRICT debug_command, void *const RESTRICT user_data)
		{
			const bool is_checked{ debug_command->_State._CheckboxState._IsChecked };

			if (debug_command->_State._CheckboxState._IsChecked && !UNCOMPRESSED_TEXTURES_WINDOW_OPEN)
			{
				ImGuiSystem::Instance->RegisterGameWindow
				(
					ImGuiSystem::GameWindow::LEFT,
					[](const Vector2<float32> minimum, const Vector2<float32> maximum)
					{
						return ContentSystem::Instance->WindowCallback(minimum, maximum);
					}
				);

				STATISTICS_WINDOW_OPEN = false;
			}

			UNCOMPRESSED_TEXTURES_WINDOW_OPEN = is_checked;
		},
		nullptr
	);
#endif
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
*	Registers a callback for when an asset is compiled.
*	Should return if the compilation should be re-run, for example if one asset being compiled generates other assets that then need to be compiled.
*/
void ContentSystem::RegisterOnAssetCompiledCallback(OnAssetCompiledCallback callback) NOEXCEPT
{
	//Add the callback.
	_OnAssetCompiledCallbacks.Emplace(callback);
}

/*
*	Compiles the content for engine.
*	Returns if new content was compiled.
*/
bool ContentSystem::CompileEngine() NOEXCEPT
{
	return CompileInternal(CompilationDomain::ENGINE);
}

/*
*	Compiles the content for game.
*	Returns if new content was compiled.
*/
bool ContentSystem::CompileGame() NOEXCEPT
{
	return CompileInternal(CompilationDomain::GAME);
}
#endif

/*
*	Load a single asset collection from the given file path.
*/
void ContentSystem::LoadAssetCollection(const char *const RESTRICT file_path) NOEXCEPT
{
	PROFILING_SCOPE("ContentSystem::LoadAssetCollection");

	//Remember the start time.
	TimePoint start_time;

	//Open the input file.
	BinaryInputFile input_file{ file_path };

	//Load the file into a stream archive.
	StreamArchive stream_archive;

	{
		PROFILING_SCOPE("ContentSystem::LoadAssetCollection::CreateStreamArchive");


		{
			PROFILING_SCOPE("ContentSystem::LoadAssetCollection::AllocateStreamArchive");
			stream_archive.Resize(input_file.Size());
		}

		{
			PROFILING_SCOPE("ContentSystem::LoadAssetCollection::ReadStreamArchive");

			stream_archive.SetMode(StreamArchive::Mode::READ);
			
			if (input_file.IsMapped())
			{
				stream_archive.SetData(static_cast<byte* const RESTRICT>(const_cast<void* const RESTRICT>(input_file.GetMappedData())), input_file.Size());
			}

			else
			{
				input_file.Read(stream_archive.Data(), input_file.Size());
			}
		}
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

		//Allocate the load data.
		LoadData *const RESTRICT load_data{ new (_LoadDataAllocator.Allocate()) LoadData() };

		//Set the asset compiler.
		load_data->_AssetCompiler = asset_compiler;

		//Set up the load context.
		load_data->_Context._StreamArchive = &stream_archive;
		load_data->_Context._StreamArchivePosition = stream_archive_position;
		load_data->_Context._Asset = asset_compiler->AllocateAsset();

		//Copy the asset header.
		Memory::Copy(&load_data->_Context._Asset->_Header, &asset_header, sizeof(AssetHeader));

		//Set up the task.
		load_data->_Task._Function = [](void *const RESTRICT arguments)
		{
			LoadData *const RESTRICT load_data{ static_cast<LoadData *const RESTRICT>(arguments) };

			load_data->_AssetCompiler->Load(load_data->_Context);
		};
		load_data->_Task._Arguments = load_data;
		load_data->_Task._ExecutableOnSameThread = false;

		//Execute the task!
		TaskSystem::Instance->ExecuteTask(Task::Priority::LOW, &load_data->_Task);

		//Add the load data.
		_LoadData.Emplace(load_data);

		//Advance the stream archive position.
		stream_archive_position = original_stream_archive_position + file_size + sizeof(uint64);
	}

	//Wait for all the load data to finish.
	while (!_LoadData.Empty())
	{
		for (uint64 load_data_index{ 0 }; load_data_index < _LoadData.Size();)
		{
			//Cache the load data.
			LoadData *const RESTRICT load_data{ _LoadData[load_data_index] };

			//Is the load finished?
			if (load_data->_Task.IsExecuted())
			{
				//Add it to the list of assets.
				HashTable<HashString, Asset *RESTRICT> *const RESTRICT assets{ _Assets.Find(load_data->_AssetCompiler->AssetTypeIdentifier()) };
				assets->Add(load_data->_Context._Asset->_Header._AssetIdentifier, load_data->_Context._Asset);

				//Deallocate the load data.
				_LoadDataAllocator.Free(load_data);

				//Remove the load data.
				_LoadData.EraseAt<false>(load_data_index);

				LOG_INFORMATION("Number of loads left: %llu", _LoadData.Size());
			}

			else
			{
				++load_data_index;
			}
		}

		if (!_LoadData.Empty())
		{
			TaskSystem::Instance->DoWork(Task::Priority::LOW);
		}
	}

	//Call PostLoad() on all asset compilers.
	for (AssetCompiler *const RESTRICT asset_compiler : _AssetCompilers.ValueIterator())
	{
		asset_compiler->PostLoad();
	}

	//Close the input file.
	input_file.Close();

	LOG_INFORMATION("%s took %f seconds to load", file_path, start_time.GetSecondsSince());
}

/*
*	Compiles internally.
*	Returns if new content was compiled.
*/
NO_DISCARD bool ContentSystem::CompileInternal(const CompilationDomain compilation_domain) NOEXCEPT
{
	//Cache the start time.
	const TimePoint start_time;

	//Cache the directories.
	const char *RESTRICT domain_name{ nullptr };
	const char *RESTRICT assets_directory{ nullptr };
	const char *RESTRICT rendering_directory{ nullptr };
	const char *RESTRICT compiled_directory{ nullptr };

	switch (compilation_domain)
	{
		case CompilationDomain::ENGINE:
		{
			domain_name = "engine";
			assets_directory = ENGINE_ASSETS;
			rendering_directory = ENGINE_RENDERING;
			compiled_directory = ENGINE_COMPILED;

			break;
		}

		case CompilationDomain::GAME:
		{
			domain_name = "game";
			assets_directory = GAME_ASSETS;
			rendering_directory = GAME_RENDERING;
			compiled_directory = GAME_COMPILED;

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Call PreCompile() on all asset compilers.
	for (AssetCompiler *const RESTRICT asset_compiler : _AssetCompilers.ValueIterator())
	{
		asset_compiler->PreCompile(compilation_domain);
	}

	//Compile!
	CompileResult compile_result;

RECOMPILE:
	//Set up the content cache.
	ContentCache content_cache;

	char content_cache_file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(content_cache_file_path, "%s\\ContentCache.json", compiled_directory);
	content_cache.Read(content_cache_file_path);

	//Always reset '_TriggerCompile'.
	compile_result._TriggerRecompile = false;

	//Compile assets.
	CompileAssetsInDirectory(compilation_domain, &content_cache, assets_directory, nullptr, &compile_result);
	CompileAssetsInDirectory(compilation_domain, &content_cache, rendering_directory, nullptr, &compile_result);

	//Wait for all the compile data to finish.
	while (!_CompileData.Empty())
	{
		for (uint64 compile_data_index{ 0 }; compile_data_index < _CompileData.Size();)
		{
			//Cache the compile data.
			CompileData *const RESTRICT compile_data{ _CompileData[compile_data_index] };

			//Is the compile finished?
			if (compile_data->_Task.IsExecuted())
			{
				//Update the content cache entry.
				ContentCache::Entry *const RESTRICT content_cache_entry{ content_cache.GetEntry(compile_data->_Context._FilePath.Data()) };
				content_cache_entry->Update(compile_data->_AssetCompiler->CurrentVersion(), compile_data->_Context._Dependencies);

				//Destroy the compile data.
				compile_data->~CompileData();

				//Deallocate the compile data.
				_CompileDataAllocator.Free(compile_data);

				//Remove the compile data.
				_CompileData.EraseAt<false>(compile_data_index);

				LOG_INFORMATION("Number of %s compiles left: %llu", domain_name, _CompileData.Size());
			}

			else
			{
				++compile_data_index;
			}
		}

		if (!_CompileData.Empty())
		{
			TaskSystem::Instance->DoWork(Task::Priority::LOW);
		}
	}

	//Call PostCompile() on all asset compilers.
	for (AssetCompiler *const RESTRICT asset_compiler : _AssetCompilers.ValueIterator())
	{
		asset_compiler->PostCompile(compilation_domain);
	}

	//Write the content cache.
	content_cache.Write(content_cache_file_path);

	//Recompile, if requested.
	if (compile_result._TriggerRecompile)
	{
		goto RECOMPILE;
	}

	//Create asset collections if new assets were compiled.
	if (compile_result._NewAssetsCompiled)
	{
		PROFILING_SCOPE("ContentSystem::CreateAssetCollections");

		CreateAssetCollections(compiled_directory, nullptr);
	}

	LOG_INFORMATION("Compiling content for %s took %f seconds.", domain_name, start_time.GetSecondsSince());

	return compile_result._NewAssetsCompiled;
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
	//Skip if the directory doesn't exist.
	if (!std::filesystem::exists(std::string(directory_path)))
	{
		return;
	}

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

		//Figure out the extension.
		std::string extension;

		{
			const size_t last_dot_position{ file_path.find_last_of(".") };
			extension = file_path.substr(last_dot_position + 1, std::string::npos);
		}

		//Find the asset compiler for this file type.
		AssetCompiler *RESTRICT asset_compiler;

		{
			AssetCompiler *const RESTRICT *const RESTRICT _asset_compiler{ _AssetCompilers.Find(HashString(extension.c_str())) };
			asset_compiler = _asset_compiler ? *_asset_compiler : nullptr;
		}

		//Couldn't find an asset compiler, ignore!
		if (!asset_compiler)
		{
			continue;
		}

		//Retrieve the content cache entry.
		ContentCache::Entry *const RESTRICT content_cache_entry{ content_cache->GetEntry(file_path.c_str()) };

		//Check if it needs aecompile.
		if (!TEST_BIT(asset_compiler->_Flags, AssetCompiler::Flags::ALWAYS_COMPILE) && !content_cache_entry->NeedsCompile(asset_compiler->CurrentVersion()))
		{
			continue;
		}

		LOG_INFORMATION("Compiling %s", file_path.data());

		//Figure out the name.
		std::string name;

		{
			const size_t last_slash_position{ file_path.find_last_of("\\") };
			name = file_path.substr(last_slash_position + 1, file_path.length() - last_slash_position - strlen(extension.c_str()) - 2);
		}

		//Allocate the compile data.
		CompileData *const RESTRICT compile_data{ new (_CompileDataAllocator.Allocate()) CompileData() };

		//Set the asset compiler.
		compile_data->_AssetCompiler = asset_compiler;

		//Set up the compile context.
		compile_data->_Context._CompilationDomain = compilation_domain;
		compile_data->_Context._Collection = collection;
		compile_data->_Context._FilePath = file_path.c_str();
		compile_data->_Context._Name = name.c_str();

		//Set up the task.
		compile_data->_Task._Function = [](void *const RESTRICT arguments)
		{
			CompileData *const RESTRICT compile_data{ static_cast<CompileData *const RESTRICT>(arguments) };

			compile_data->_AssetCompiler->Compile(compile_data->_Context);
		};
		compile_data->_Task._Arguments = compile_data;
		compile_data->_Task._ExecutableOnSameThread = false;

		//Execute the task!
		TaskSystem::Instance->ExecuteTask(Task::Priority::LOW, &compile_data->_Task);

		//Add the compile data.
		_CompileData.Emplace(compile_data);

		//New asset was compiled!
		compile_result->_NewAssetsCompiled = true;

		//Trigger a recompile if this asset compiler has requested it.
		if (asset_compiler->_TriggerRecompile)
		{
			compile_result->_TriggerRecompile = true;
		}

		//Call any callbacks for when an asset is compiled.
		for (OnAssetCompiledCallback callback : _OnAssetCompiledCallbacks)
		{
			compile_result->_TriggerRecompile |= callback(asset_compiler->AssetTypeIdentifier(), file_path.c_str());
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

		//Ignore the .gitignore file.
		if (file_path.find(".gitignore") != std::string::npos)
		{
			continue;
		}

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

#if !defined(CATALYST_CONFIGURATION_FINAL)
/*
*	The window callback.
*/
NO_DISCARD bool ContentSystem::WindowCallback(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT
{
	if (STATISTICS_WINDOW_OPEN)
	{
		//Begin the window.
		ImGuiSystem::BeginWindowParameters parameters;

		parameters._Name = "Asset Statistics";
		parameters._Minimum = minimum;
		parameters._Maximum = maximum;

		ImGuiSystem::Instance->BeginWindow(parameters);

		//Add all the statistics, counting up some totals as we go.
		uint64 total_cpu_memory{ 0 };
		uint64 total_gpu_memory{ 0 };

		for (AssetCompiler *const RESTRICT asset_compiler : _AssetCompilers.ValueIterator())
		{
			AssetCompiler::Statistics statistics;

			if (asset_compiler->GetStatistics(&statistics))
			{
				ImGui::Text(statistics._AssetTypeName);

				{
					char buffer[64];
					sprintf_s(buffer, "Count: %llu", _Assets[asset_compiler->AssetTypeIdentifier()].Size());

					ImGui::Text(buffer);
				}

				{
					char buffer[64];
					Memory::PrintMemoryString(buffer, ARRAY_LENGTH(buffer), "CPU Memory", statistics._TotalCPUMemory);

					ImGui::Text(buffer);
				}

				{
					char buffer[64];
					Memory::PrintMemoryString(buffer, ARRAY_LENGTH(buffer), "GPU Memory", statistics._TotalGPUMemory);

					ImGui::Text(buffer);
				}

				ImGui::Separator();

				total_cpu_memory += statistics._TotalCPUMemory;
				total_gpu_memory += statistics._TotalGPUMemory;
			}
		}

		ImGui::Text("Total");

		{
			char buffer[64];
			Memory::PrintMemoryString(buffer, ARRAY_LENGTH(buffer), "CPU Memory", total_cpu_memory);

			ImGui::Text(buffer);
		}

		{
			char buffer[64];
			Memory::PrintMemoryString(buffer, ARRAY_LENGTH(buffer), "GPU Memory", total_gpu_memory);

			ImGui::Text(buffer);
		}

		//End the window.
		ImGui::End();

		return STATISTICS_WINDOW_OPEN;
	}
	
	else if (UNCOMPRESSED_TEXTURES_WINDOW_OPEN)
	{
		//Begin the window.
		ImGuiSystem::BeginWindowParameters parameters;

		parameters._Name = "Uncompressed Textures";
		parameters._Minimum = minimum;
		parameters._Maximum = maximum;

		ImGuiSystem::Instance->BeginWindow(parameters);

		//List all uncompressed textures.
		for (Asset *const RESTRICT asset : _Assets[HashString("Texture2D")].ValueIterator())
		{
			Texture2DAsset *const RESTRICT texture_asset{ static_cast<Texture2DAsset *const RESTRICT>(asset) };

			if (texture_asset->_Compression._Mode == TextureCompression::Mode::NONE)
			{
				ImGui::Text(texture_asset->_Header._AssetName.Data());
			}
		}

		//End the window.
		ImGui::End();

		return UNCOMPRESSED_TEXTURES_WINDOW_OPEN;
	}

	else
	{
		return false;
	}
}
#endif