//Header file.
#include <Content/AssetCompilers/ModelAssetCompiler.h>

//File.
#include <File/Core/File.h>
#include <File/Core/BinaryOutputFile.h>
#include <File/Readers/FBXReader.h>
#include <File/Utilities/TextParsingUtilities.h>

//Profiling.
#include <Profiling/Profiling.h>

//Systems.
#include <Systems/LogSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

//STL.
#include <fstream>
#include <string>

/*
*	Model parameters class definition.
*/
class ModelParameters final
{

public:

	//The level of details.
	DynamicArray<StaticString<MAXIMUM_FILE_PATH_LENGTH>> _LevelOfDetails;

	//The collision.
	StaticString<MAXIMUM_FILE_PATH_LENGTH> _Collision;

	//The level of detail multiplier.
	float32 _LevelOfDetailMultiplier;

};

/*
*	Default constructor.
*/
ModelAssetCompiler::ModelAssetCompiler() NOEXCEPT
{
	//Set the flags.
	_Flags = Flags::NONE;
}

/*
*	Returns the asset type identifier.
*/
NO_DISCARD HashString ModelAssetCompiler::AssetTypeIdentifier() const NOEXCEPT
{
	return ModelAsset::TYPE_IDENTIFIER;
}

/*
*	Returns the current version.
*/
NO_DISCARD uint64 ModelAssetCompiler::CurrentVersion() const NOEXCEPT
{
	return 1;
}

/*
*	Compiles a single asset with the given compile context.
*/
void ModelAssetCompiler::Compile(const CompileContext &compile_context) NOEXCEPT
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
		ModelAssetCompiler::Instance->CompileInternal(static_cast<CompileData *const RESTRICT>(arguments));
	};
	task->_Arguments = compile_data;
	task->_ExecutableOnSameThread = true;

	//Execute the task!
	TaskSystem::Instance->ExecuteTask(Task::Priority::LOW, task);

	//Add the task to the list.
	compile_context._Tasks->Emplace(task);
}

/*
*	Loads a single asset with the given load context.
*/
NO_DISCARD Asset *const RESTRICT ModelAssetCompiler::Load(const LoadContext &load_context) NOEXCEPT
{
	//Allocate the asset.
	ModelAsset *const RESTRICT new_asset{ new (_AssetAllocator.Allocate()) ModelAsset() };

	//Set up the load data.
	LoadData *const RESTRICT load_data{ new (_LoadDataAllocator.Allocate()) LoadData() };

	load_data->_StreamArchivePosition = load_context._StreamArchivePosition;
	load_data->_StreamArchive = load_context._StreamArchive;
	load_data->_Asset = new_asset;

	//Set up the task.
	Task *const RESTRICT task{ static_cast<Task *const RESTRICT>(load_context._TaskAllocator->Allocate()) };

	task->_Function = [](void *const RESTRICT arguments)
	{
		ModelAssetCompiler::Instance->LoadInternal(static_cast<LoadData *const RESTRICT>(arguments));
	};
	task->_Arguments = load_data;
	task->_ExecutableOnSameThread = true;

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
void ModelAssetCompiler::CompileInternal(CompileData *const RESTRICT compile_data) NOEXCEPT
{
	PROFILING_SCOPE("ModelAssetCompiler::CompileInternal");

	//Set up the parameters.
	ModelParameters parameters;

	//Set defaults.
	parameters._LevelOfDetailMultiplier = 32.0f;

	//Open the input file.
	std::ifstream input_file{ compile_data->_FilePath.Data() };

	//Iterate over the lines and fill in the parameters.
	{
		StaticArray<DynamicString, 8> arguments;
		std::string current_line;

		while (std::getline(input_file, current_line))
		{
			//Is this a level of detail declaration?
			{
				const size_t position{ current_line.find("LevelOfDetail(") };

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

					ASSERT(number_of_arguments == 1, "LevelOfDetail() needs one argument!");

					parameters._LevelOfDetails.Emplace(arguments[0].Data());

					continue;
				}
			}

			//Is this a collision declaration?
			{
				const size_t position{ current_line.find("Collision(") };

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

					ASSERT(number_of_arguments == 1, "Collision() needs one argument!");

					parameters._Collision = arguments[0].Data();

					continue;
				}
			}

			//Is this a level of detail multiplier declaration?
			{
				const size_t position{ current_line.find("LevelOfDetailMultiplier(") };

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

					ASSERT(number_of_arguments == 1, "LevelOfDetailMultiplier() needs one argument!");

					parameters._LevelOfDetailMultiplier = std::stof(arguments[0].Data());

					continue;
				}
			}

			//Couldn't figure out what this line is?
			ASSERT(false, "Unknown line %s", current_line.c_str());
		}
	}

	//Close the input file.
	input_file.close();

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
	sprintf_s(directory_path, "%s\\Models", collection_directory_path);

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

	//Set up all of the model files and the collision model file.
	DynamicArray<ModelFile> model_files;
	ModelFile collision_model_file;

	//Read all model files into memory.
	model_files.Upsize<true>(parameters._LevelOfDetails.Size());

	for (uint64 i{ 0 }, size{ model_files.Size() }; i < size; ++i)
	{
		FBXReader::Read(parameters._LevelOfDetails[i].Data(), &model_files[i]);
	}

	//Determine the model space axis aligned bounding box.
	{
		//Iterate over all vertices in all meshes and expand the bounding box.
		AxisAlignedBoundingBox3D axis_aligned_bounding_box;

		for (ModelFile &model_file : model_files)
		{
			for (ModelFile::Mesh &mesh : model_file._Meshes)
			{
				for (Vertex& vertex : mesh._Vertices)
				{
					axis_aligned_bounding_box.Expand(vertex._Position);
				}
			}
		}

		//Write the axis-aligned bounding box to the file.
		output_file.Write(&axis_aligned_bounding_box, sizeof(AxisAlignedBoundingBox3D));
	}

	//Write the number of meshes.
	const uint64 number_of_meshes{ model_files[0]._Meshes.Size() };
	ASSERT(number_of_meshes <= RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL, "This model has more than the maximum number of meshes, either increase the maximum number of meshes or decrease the number of meshes on this model!");
	output_file.Write(&number_of_meshes, sizeof(uint64));

	//Write the number of level of details.
	const uint64 number_of_level_of_details{ model_files.Size() };
	output_file.Write(&number_of_level_of_details, sizeof(uint64));

	//Process each mesh individually.
	for (uint64 mesh_index{ 0 }; mesh_index < number_of_meshes; ++mesh_index)
	{
		for (uint64 level_of_detail_index{ 0 }; level_of_detail_index < number_of_level_of_details; ++level_of_detail_index)
		{
			//Write the number of vertices to the file.
			const uint64 number_of_vertices{ model_files[level_of_detail_index]._Meshes[mesh_index]._Vertices.Size() };
			output_file.Write(&number_of_vertices, sizeof(uint64));

			//Write the vertices to the file.
			output_file.Write(model_files[level_of_detail_index]._Meshes[mesh_index]._Vertices.Data(), sizeof(Vertex) * number_of_vertices);

			//Write the number of indices to the file.
			const uint64 number_of_indices{ model_files[level_of_detail_index]._Meshes[mesh_index]._Indices.Size() };
			output_file.Write(&number_of_indices, sizeof(uint64));

			//Write the vertices to the file.
			output_file.Write(model_files[level_of_detail_index]._Meshes[mesh_index]._Indices.Data(), sizeof(uint32) * number_of_indices);
		}
	}

	//Check if there exists a collision model.
	if (parameters._Collision)
	{
		//Write that there exists a collision model.
		bool collision_model_exists{ true };
		output_file.Write(&collision_model_exists, sizeof(bool));

		//Read the model file.
		if (!collision_model_file.IsValid())
		{
			FBXReader::Read(parameters._Collision.Data(), &collision_model_file);
		}

		//Build the collision model.
		CollisionModelData collision_model_data;
		PhysicsSystem::Instance->BuildCollisionModel(collision_model_file, &collision_model_data);

		//Write the collision model data to the output file.
		output_file.Write(&collision_model_data._Type, sizeof(CollisionModelData::Type));

		const uint64 collision_model_data_size{ collision_model_data._Data.Size() };
		output_file.Write(&collision_model_data_size, sizeof(uint64));
		output_file.Write(collision_model_data._Data.Data(), collision_model_data_size);
	}

	else
	{
		//Write that there doesn't exist a collision model.
		bool collision_model_exists{ false };
		output_file.Write(&collision_model_exists, sizeof(bool));
	}

	//Write the level of detail multiplier.
	output_file.Write(&parameters._LevelOfDetailMultiplier, sizeof(float32));

	//Close the output file.
	output_file.Close();
}

/*
*	Loads internally.
*/
void ModelAssetCompiler::LoadInternal(LoadData *const RESTRICT load_data) NOEXCEPT
{
	PROFILING_SCOPE("ModelAssetCompiler::LoadInternal");

	//Read the data.
	uint64 stream_archive_position{ load_data->_StreamArchivePosition };

	//Read the axis aligned bounding box.
	load_data->_StreamArchive->Read(&load_data->_Asset->_ModelSpaceAxisAlignedBoundingBox, sizeof(AxisAlignedBoundingBox3D), &stream_archive_position);

	//Read the number of meshes.
	uint64 number_of_meshes;
	load_data->_StreamArchive->Read(&number_of_meshes, sizeof(uint64), &stream_archive_position);

	//Read the number of level of details.
	uint64 number_of_level_of_details;
	load_data->_StreamArchive->Read(&number_of_level_of_details, sizeof(uint64), &stream_archive_position);

	//Set up the vertices/indices.
	DynamicArray<DynamicArray<DynamicArray<Vertex>>> vertices;
	DynamicArray<DynamicArray<DynamicArray<uint32>>> indices;

	vertices.Upsize<true>(number_of_meshes);
	indices.Upsize<true>(number_of_meshes);

	for (uint64 mesh_index{ 0 }; mesh_index < number_of_meshes; ++mesh_index)
	{
		vertices[mesh_index].Upsize<true>(number_of_level_of_details);
		indices[mesh_index].Upsize<true>(number_of_level_of_details);

		for (uint64 j{ 0 }; j < number_of_level_of_details; ++j)
		{
			//Read the number of vertices.
			uint64 number_of_vertices;
			load_data->_StreamArchive->Read(&number_of_vertices, sizeof(uint64), &stream_archive_position);

			//Read the vertices.
			vertices[mesh_index][j].Upsize<false>(number_of_vertices);
			load_data->_StreamArchive->Read(vertices[mesh_index][j].Data(), sizeof(Vertex) * number_of_vertices, &stream_archive_position);

			//Read the number of indices.
			uint64 number_of_indices;
			load_data->_StreamArchive->Read(&number_of_indices, sizeof(uint64), &stream_archive_position);

			//Read the indices.
			indices[mesh_index][j].Upsize<false>(number_of_indices);
			load_data->_StreamArchive->Read(indices[mesh_index][j].Data(), sizeof(uint32) * number_of_indices, &stream_archive_position);
		}
	}

	//Set up the collision model data.
	CollisionModelData collision_model_data;

	//Read if there exists a collision model.
	bool collision_model_exists;
	load_data->_StreamArchive->Read(&collision_model_exists, sizeof(bool), &stream_archive_position);

	//Read the collision model data.
	if (collision_model_exists)
	{
		load_data->_StreamArchive->Read(&collision_model_data._Type, sizeof(CollisionModelData::Type), &stream_archive_position);

		uint64 collision_model_data_size;
		load_data->_StreamArchive->Read(&collision_model_data_size, sizeof(uint64), &stream_archive_position);

		collision_model_data._Data.Upsize<false>(collision_model_data_size);

		load_data->_StreamArchive->Read(collision_model_data._Data.Data(), collision_model_data_size, &stream_archive_position);
	}

	else
	{
		collision_model_data._Type = CollisionModelData::Type::NONE;
	}

	//Create the meshes.
	load_data->_Asset->_Meshes.Upsize<true>(number_of_meshes);

	for (uint64 mesh_index{ 0 }; mesh_index < number_of_meshes; ++mesh_index)
	{
		load_data->_Asset->_Meshes[mesh_index]._MeshLevelOfDetails.Upsize<true>(number_of_level_of_details);

		for (uint64 level_of_detail_index{ 0 }; level_of_detail_index < number_of_level_of_details; ++level_of_detail_index)
		{
			//Copy the vertices/indices.
			load_data->_Asset->_Meshes[mesh_index]._MeshLevelOfDetails[level_of_detail_index]._Vertices = std::move(vertices[mesh_index][level_of_detail_index]);
			load_data->_Asset->_Meshes[mesh_index]._MeshLevelOfDetails[level_of_detail_index]._Indices = std::move(indices[mesh_index][level_of_detail_index]);

			//Create the buffers.
			{
				const void *const RESTRICT data_chunks[]{ load_data->_Asset->_Meshes[mesh_index]._MeshLevelOfDetails[level_of_detail_index]._Vertices.Data() };
				const uint64 data_sizes[]{ sizeof(Vertex) * load_data->_Asset->_Meshes[mesh_index]._MeshLevelOfDetails[level_of_detail_index]._Vertices.Size() };
				RenderingSystem::Instance->CreateBuffer(data_sizes[0], BufferUsage::StorageBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &load_data->_Asset->_Meshes[mesh_index]._MeshLevelOfDetails[level_of_detail_index]._VertexBuffer);
				RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &load_data->_Asset->_Meshes[mesh_index]._MeshLevelOfDetails[level_of_detail_index]._VertexBuffer);
			}

			{
				const void *const RESTRICT data_chunks[]{ load_data->_Asset->_Meshes[mesh_index]._MeshLevelOfDetails[level_of_detail_index]._Indices.Data() };
				const uint64 data_sizes[]{ sizeof(uint32) * load_data->_Asset->_Meshes[mesh_index]._MeshLevelOfDetails[level_of_detail_index]._Indices.Size() };
				RenderingSystem::Instance->CreateBuffer(data_sizes[0], BufferUsage::IndexBuffer | BufferUsage::StorageBuffer, MemoryProperty::DeviceLocal, &load_data->_Asset->_Meshes[mesh_index]._MeshLevelOfDetails[level_of_detail_index]._IndexBuffer);
				RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &load_data->_Asset->_Meshes[mesh_index]._MeshLevelOfDetails[level_of_detail_index]._IndexBuffer);
			}

			//Write the index count.
			load_data->_Asset->_Meshes[mesh_index]._MeshLevelOfDetails[level_of_detail_index]._IndexCount = static_cast<uint32>(load_data->_Asset->_Meshes[mesh_index]._MeshLevelOfDetails[level_of_detail_index]._Indices.Size());
		}
	}

	//Create the collision model, if there is one.
	if (collision_model_data._Type != CollisionModelData::Type::NONE)
	{
		PhysicsSystem::Instance->CreateCollisionModel(collision_model_data, &load_data->_Asset->_CollisionModel);
	}

	else
	{
		load_data->_Asset->_CollisionModel = nullptr;
	}

	//Read the level of detail multiplier.
	load_data->_StreamArchive->Read(&load_data->_Asset->_LevelOfDetailMultiplier, sizeof(float32), &stream_archive_position);
}