//Header file.
#include <Content/AssetCompilers/AnimatedModelAssetCompiler.h>

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
*	Animated model parameters class definition.
*/
class AnimatedModelParameters final
{

public:

	//The file.
	StaticString<MAXIMUM_FILE_PATH_LENGTH> _File;

};

/*
*	Default constructor.
*/
AnimatedModelAssetCompiler::AnimatedModelAssetCompiler() NOEXCEPT
{
	//Set the flags.
	_Flags = Flags::ALWAYS_COMPILE;
}

/*
*	Returns the asset type identifier.
*/
NO_DISCARD HashString AnimatedModelAssetCompiler::AssetTypeIdentifier() const NOEXCEPT
{
	return AnimatedModelAsset::TYPE_IDENTIFIER;
}

/*
*	Returns the current version.
*/
NO_DISCARD uint64 AnimatedModelAssetCompiler::CurrentVersion() const NOEXCEPT
{
	return 1;
}

/*
*	Compiles a single asset with the given compile context.
*/
void AnimatedModelAssetCompiler::Compile(const CompileContext &compile_context) NOEXCEPT
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
		AnimatedModelAssetCompiler::Instance->CompileInternal(static_cast<CompileData *const RESTRICT>(arguments));
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
NO_DISCARD Asset *const RESTRICT AnimatedModelAssetCompiler::Load(const LoadContext &load_context) NOEXCEPT
{
	//Allocate the asset.
	AnimatedModelAsset *const RESTRICT new_asset{ new (_AssetAllocator.Allocate()) AnimatedModelAsset() };

	//Set up the load data.
	LoadData *const RESTRICT load_data{ new (_LoadDataAllocator.Allocate()) LoadData() };

	load_data->_StreamArchivePosition = load_context._StreamArchivePosition;
	load_data->_StreamArchive = load_context._StreamArchive;
	load_data->_Asset = new_asset;

	//Set up the task.
	Task *const RESTRICT task{ static_cast<Task *const RESTRICT>(load_context._TaskAllocator->Allocate()) };

	task->_Function = [](void *const RESTRICT arguments)
	{
		AnimatedModelAssetCompiler::Instance->LoadInternal(static_cast<LoadData *const RESTRICT>(arguments));
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
*	Writes a single bone.
*/
FORCE_INLINE void WriteBone(const Bone &bone, BinaryOutputFile *const RESTRICT output_file, uint32 *const RESTRICT total_number_of_bones) NOEXCEPT
{
	//Write the name.
	output_file->Write(&bone._Name, sizeof(HashString));

	//Write the index.
	output_file->Write(&bone._Index, sizeof(uint32));

	//Write the bind transform.
	output_file->Write(&bone._BindTransform, sizeof(Matrix4x4));

	//Write the inverse bind transform.
	output_file->Write(&bone._InverseBindTransform, sizeof(Matrix4x4));

	//Write the number of children.
	const uint64 number_of_children{ bone._Children.Size() };
	output_file->Write(&number_of_children, sizeof(uint64));

	//Update the total number of bones.
	++(*total_number_of_bones);

	//Write all the children!
	for (const Bone &child : bone._Children)
	{
		WriteBone(child, output_file, total_number_of_bones);
	}
}

/*
*	Compiles internally.
*/
void AnimatedModelAssetCompiler::CompileInternal(CompileData *const RESTRICT compile_data) NOEXCEPT
{
	PROFILING_SCOPE("AnimatedModelAssetCompiler::CompileInternal");

	//Set up the parameters.
	AnimatedModelParameters parameters;

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

	//Read the animated model file.
	AnimatedModelFile animated_model_file;
	
	FBXReader::Read(parameters._File.Data(), &animated_model_file);

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
	sprintf_s(directory_path, "%s\\AnimatedModels", collection_directory_path);

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

	//Determine the model space axis aligned bounding box.
	{
		//Iterate over all vertices in all meshes and expand the bounding box.
		AxisAlignedBoundingBox3D axis_aligned_bounding_box;

		for (AnimatedModelFile::Mesh &mesh : animated_model_file._Meshes)
		{
			for (AnimatedVertex &vertex : mesh._Vertices)
			{
				axis_aligned_bounding_box.Expand(vertex._Position);
			}
		}

		//Write the axis-aligned bounding box to the file.
		output_file.Write(&axis_aligned_bounding_box, sizeof(AxisAlignedBoundingBox3D));
	}

	//Write the number of meshes.
	const uint64 number_of_meshes{ animated_model_file._Meshes.Size() };
	ASSERT(number_of_meshes <= RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL, "This model has more than the maximum number of meshes, either increase the maximum number of meshes or decrease the number of meshes on this model!");
	output_file.Write(&number_of_meshes, sizeof(uint64));

	//Process each mesh individually.
	for (uint64 mesh_index{ 0 }; mesh_index < number_of_meshes; ++mesh_index)
	{
		//Write the number of vertices to the file.
		const uint64 number_of_vertices{ animated_model_file._Meshes[mesh_index]._Vertices.Size() };
		output_file.Write(&number_of_vertices, sizeof(uint64));

		//Write the vertices to the file.
		output_file.Write(animated_model_file._Meshes[mesh_index]._Vertices.Data(), sizeof(AnimatedVertex) * number_of_vertices);

		//Write the number of indices to the file.
		const uint64 number_of_indices{ animated_model_file._Meshes[mesh_index]._Indices.Size() };
		output_file.Write(&number_of_indices, sizeof(uint64));

		//Write the indices to the file.
		output_file.Write(animated_model_file._Meshes[mesh_index]._Indices.Data(), sizeof(uint32) * number_of_indices);
	}

	//Write the root bone.
	uint32 total_number_of_bones{ 0 };
	WriteBone(animated_model_file._Skeleton._RootBone, &output_file, &total_number_of_bones);

	//Write the total number of bones.
	output_file.Write(&total_number_of_bones, sizeof(uint32));

	//Close the output file.
	output_file.Close();
}

/*
*	Reads a bone.
*/
FORCE_INLINE void ReadBone(Bone *const RESTRICT bone, StreamArchive *const RESTRICT stream_archive, uint64 *const RESTRICT stream_archive_position) NOEXCEPT
{
	//Read the name.
	stream_archive->Read(&bone->_Name, sizeof(HashString), stream_archive_position);

	//Read the index.
	stream_archive->Read(&bone->_Index, sizeof(uint32), stream_archive_position);

	//Read the bind transform.
	stream_archive->Read(&bone->_BindTransform, sizeof(Matrix4x4), stream_archive_position);

	//Read the inverse bind transform.
	stream_archive->Read(&bone->_InverseBindTransform, sizeof(Matrix4x4), stream_archive_position);

	//Read the number of children.
	uint64 number_of_children;
	stream_archive->Read(&number_of_children, sizeof(uint64), stream_archive_position);

	//Upsize accordingly.
	bone->_Children.Upsize<true>(number_of_children);

	//Read all children.
	for (Bone &child : bone->_Children)
	{
		ReadBone(&child, stream_archive, stream_archive_position);
	}
}

/*
*	Loads internally.
*/
void AnimatedModelAssetCompiler::LoadInternal(LoadData *const RESTRICT load_data) NOEXCEPT
{
	PROFILING_SCOPE("AnimatedModelAssetCompiler::LoadInternal");

	//Read the data.
	uint64 stream_archive_position{ load_data->_StreamArchivePosition };

	//Read the axis aligned bounding box.
	load_data->_StreamArchive->Read(&load_data->_Asset->_ModelSpaceAxisAlignedBoundingBox, sizeof(AxisAlignedBoundingBox3D), &stream_archive_position);

	//Read the number of meshes.
	uint64 number_of_meshes;
	load_data->_StreamArchive->Read(&number_of_meshes, sizeof(uint64), &stream_archive_position);

	ASSERT(number_of_meshes == 1, "Hrrm!");

	//Set up the vertices/indices.
	DynamicArray<DynamicArray<AnimatedVertex>> vertices;
	DynamicArray<DynamicArray<uint32>> indices;

	vertices.Upsize<true>(number_of_meshes);
	indices.Upsize<true>(number_of_meshes);

	for (uint64 mesh_index{ 0 }; mesh_index < number_of_meshes; ++mesh_index)
	{
		//Read the number of vertices.
		uint64 number_of_vertices;
		load_data->_StreamArchive->Read(&number_of_vertices, sizeof(uint64), &stream_archive_position);

		//Read the vertices.
		vertices[mesh_index].Upsize<false>(number_of_vertices);
		load_data->_StreamArchive->Read(vertices[mesh_index].Data(), sizeof(AnimatedVertex) * number_of_vertices, &stream_archive_position);

		//Read the number of indices.
		uint64 number_of_indices;
		load_data->_StreamArchive->Read(&number_of_indices, sizeof(uint64), &stream_archive_position);

		//Read the indices.
		indices[mesh_index].Upsize<false>(number_of_indices);
		load_data->_StreamArchive->Read(indices[mesh_index].Data(), sizeof(uint32) * number_of_indices, &stream_archive_position);
	}

	//Create the buffers.
	{
		const void *const RESTRICT data_chunks[]{ vertices[0].Data()};
		const uint64 data_sizes[]{ sizeof(AnimatedVertex) * vertices[0].Size() };
		RenderingSystem::Instance->CreateBuffer(data_sizes[0], BufferUsage::StorageBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &load_data->_Asset->_VertexBuffer);
		RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &load_data->_Asset->_VertexBuffer);
	}

	{
		const void *const RESTRICT data_chunks[]{ indices[0].Data()};
		const uint64 data_sizes[]{ sizeof(uint32) * indices[0].Size() };
		RenderingSystem::Instance->CreateBuffer(data_sizes[0], BufferUsage::IndexBuffer | BufferUsage::StorageBuffer, MemoryProperty::DeviceLocal, &load_data->_Asset->_IndexBuffer);
		RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &load_data->_Asset->_IndexBuffer);
	}

	//Write the index count.
	load_data->_Asset->_IndexCount = static_cast<uint32>(indices[0].Size());

	//Read the bones.
	ReadBone(&load_data->_Asset->_Skeleton._RootBone, load_data->_StreamArchive, &stream_archive_position);

	//Read the total number of bones.
	load_data->_StreamArchive->Read(&load_data->_Asset->_Skeleton._TotalNumberOfBones, sizeof(uint32), &stream_archive_position);
}