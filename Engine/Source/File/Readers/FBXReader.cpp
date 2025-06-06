//Header file.
#include <File/Readers/FBXReader.h>

//Core.
#include <Core/General/DynamicString.h>

//File.
#include <File/Core/File.h>

//Systems.
#include <Systems/LogSystem.h>

#if !defined(CATALYST_CONFIGURATION_FINAL)
//Third party.
#include <ThirdParty/assimp/assimp/Importer.hpp>
#include <ThirdParty/assimp/assimp/postprocess.h>
#include <ThirdParty/assimp/assimp/scene.h>

/*
*	Bone information class definition.
*/
class BoneInformation final
{

public:

	//The name.
	DynamicString _Name;

	//The index.
	uint32 _Index;

	//The bind transform.
	Matrix4x4 _BindTransform;

};

/*
*	Converts an Assimp matrix.
*/
FORCE_INLINE NO_DISCARD Matrix4x4 ConvertAssimpMatrix(const aiMatrix4x4 &matrix) NOEXCEPT
{
	aiVector3D translation;
	aiQuaternion rotation;
	aiVector3D scale;
	matrix.Decompose(scale, rotation, translation);

#if 0
	return Matrix4x4
	(
		Vector3<float32>(translation.x, translation.z, translation.y),
		Quaternion(rotation.x, rotation.z, rotation.y, rotation.w),
		Vector3<float32>(scale.x, scale.z, scale.y)
	);
#else
	return Matrix4x4
	(
		Vector3<float32>(translation.x, translation.y, translation.z),
		Quaternion(rotation.x, rotation.y, rotation.z, rotation.w),
		Vector3<float32>(scale.x, scale.y, scale.z)
	);
#endif
}

/*
*	Finds the Assimp node with the given name.
*/
FORCE_INLINE NO_DISCARD aiNode *const RESTRICT FindAssimpNode(const char *const RESTRICT name, aiNode *const RESTRICT root_node) NOEXCEPT
{
	if (StringUtilities::IsEqual(name, root_node->mName.C_Str()))
	{
		return root_node;
	}

	else
	{
		for (uint32 child_index{ 0 }; child_index < root_node->mNumChildren; ++child_index)
		{
			if (aiNode *const RESTRICT found_node{ FindAssimpNode(name, root_node->mChildren[child_index]) })
			{
				return found_node;
			}
		}
	}

	return nullptr;
}

/*
*	Lists all of the meta data.
*/
FORCE_INLINE void ListMetaData(const aiMetadata *meta_data) NOEXCEPT
{
#if 0
	for (uint32 i{ 0 }; i < meta_data->mNumProperties; ++i)
	{
		LOG_INFORMATION("Key %s:", meta_data->mKeys[i].C_Str());

		switch (meta_data->mValues[i].mType)
		{
			case aiMetadataType::AI_BOOL:
			{
				LOG_INFORMATION("\t%s", *((bool*)meta_data->mValues[i].mData) ? "true" : "false");

				break;
			}

			case aiMetadataType::AI_INT32:
			{
				LOG_INFORMATION("\t%i", *((int32*)meta_data->mValues[i].mData));

				break;
			}

			case aiMetadataType::AI_UINT64:
			{
				LOG_INFORMATION("\t%llu", *((uint64*)meta_data->mValues[i].mData));

				break;
			}

			case aiMetadataType::AI_FLOAT:
			{
				LOG_INFORMATION("\t%f", *((float32*)meta_data->mValues[i].mData));

				break;
			}

			case aiMetadataType::AI_AISTRING:
			{
				const aiString &string{ *((aiString*)meta_data->mValues[i].mData) };

				LOG_INFORMATION("\t%s", string.C_Str());

				break;
			}

			case aiMetadataType::AI_AIVECTOR3D:
			{
				const aiVector3D &vector{ *((aiVector3D*)meta_data->mValues[i].mData) };

				LOG_INFORMATION("\tX: %f", vector.x);
				LOG_INFORMATION("\tY: %f", vector.y);
				LOG_INFORMATION("\tZ: %f", vector.z);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}
#endif
}

/*
*	Processes a single mesh.
*/
FORCE_INLINE void ProcessMesh(const aiScene *const RESTRICT scene, const aiNode *const RESTRICT node, const aiMesh *const RESTRICT mesh, AnimatedModelFile *const RESTRICT animated_model_file, DynamicArray<BoneInformation> *const RESTRICT bone_information) NOEXCEPT
{
	//Add a new mesh.
	animated_model_file->_Meshes.Emplace();
	AnimatedModelFile::Mesh &new_mesh{ animated_model_file->_Meshes.Back() };

	//Set the name.
	new_mesh._Name = node->mName.C_Str();

	//Process the vertices.
	{
		new_mesh._Vertices.Reserve(mesh->mNumVertices);

		for (uint32 i{ 0 }; i < mesh->mNumVertices; ++i)
		{
			//Add the new vertex.
			new_mesh._Vertices.Emplace();
			AnimatedVertex &new_vertex{ new_mesh._Vertices.Back() };

			//Set the position.
			new_vertex._Position._X = mesh->mVertices[i].x;
			new_vertex._Position._Y = mesh->mVertices[i].y;
			new_vertex._Position._Z = mesh->mVertices[i].z;

			//Set the normal.
			new_vertex._Normal._X = mesh->mNormals[i].x;
			new_vertex._Normal._Y = mesh->mNormals[i].y;
			new_vertex._Normal._Z = mesh->mNormals[i].z;

			//Set the tangent.
			new_vertex._Tangent._X = mesh->mTangents[i].x;
			new_vertex._Tangent._Y = mesh->mTangents[i].y;
			new_vertex._Tangent._Z = mesh->mTangents[i].z;

			//Set the texture coordinate.
			if (mesh->mTextureCoords[0])
			{
				new_vertex._TextureCoordinate._X = mesh->mTextureCoords[0][i].x;
				new_vertex._TextureCoordinate._Y = mesh->mTextureCoords[0][i].y;
			}

			else
			{
				new_vertex._TextureCoordinate = Vector2<float32>(0.0f, 0.0f);
			}

			//Set the bone indices and bone weights to their default state.
			new_vertex._BoneIndices = Vector4<uint32>(UINT32_MAXIMUM, UINT32_MAXIMUM, UINT32_MAXIMUM, UINT32_MAXIMUM);
			new_vertex._BoneWeights = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		}
	}

	//Process the indices.
	for (uint32 face_index{ 0 }; face_index < mesh->mNumFaces; ++face_index)
	{
		const aiFace &face{ mesh->mFaces[face_index] };

		for (uint32 index_index{ 0 }; index_index < face.mNumIndices; ++index_index)
		{
			new_mesh._Indices.Emplace(face.mIndices[index_index]);
		}
	}

	//Add the bone information.
	for (uint32 bone_index{ 0 }; bone_index < mesh->mNumBones; ++bone_index)
	{
		//Cache the bone.
		const aiBone *const RESTRICT bone{ mesh->mBones[bone_index] };

		//Retrieve the bone node.
		const aiNode *const RESTRICT bone_node{ FindAssimpNode(bone->mName.C_Str(), scene->mRootNode) };

		//Add the new bone.
		bone_information->Emplace();
		BoneInformation &new_bone_information{ bone_information->Back() };

		//Set the name.
		new_bone_information._Name = DynamicString(bone->mName.C_Str());

		//Set the index.
		new_bone_information._Index = bone_index;

		//Set the bind transform.
		new_bone_information._BindTransform = ConvertAssimpMatrix(bone->mOffsetMatrix);

		//Add the vertex weights.
		for (uint32 vertex_weight_index{ 0 }; vertex_weight_index < bone->mNumWeights; ++vertex_weight_index)
		{
			//Cache the vertex weight.
			const aiVertexWeight &vertex_weight{ bone->mWeights[vertex_weight_index] };

			//Cache the affected vertex.
			AnimatedVertex &affected_vertex{ animated_model_file->_Meshes.Back()._Vertices[vertex_weight.mVertexId] };

			//Find the first free bone index.
			bool found_free_bone{ false };

			for (uint8 i{ 0 }; i < 4; ++i)
			{
				if (affected_vertex._BoneIndices[i] == UINT32_MAXIMUM)
				{
					affected_vertex._BoneIndices[i] = bone_index;
					affected_vertex._BoneWeights[i] = vertex_weight.mWeight;

					found_free_bone = true;

					break;
				}
			}

			if (!found_free_bone)
			{
				ASSERT(false, "Handle this!");
			}
		}
	}
}

/*
*	Processes a single mesh.
*/
FORCE_INLINE void ProcessMesh(const aiScene *const RESTRICT scene, const aiNode *const RESTRICT node, const aiMesh *const RESTRICT mesh, ModelFile *const RESTRICT model_file) NOEXCEPT
{
	//Add a new mesh.
	model_file->_Meshes.Emplace();
	ModelFile::Mesh &new_mesh{ model_file->_Meshes.Back() };

	//Set the name.
	new_mesh._Name = node->mName.C_Str();

	//Process the vertices.
	{
		new_mesh._Vertices.Reserve(mesh->mNumVertices);

		for (uint32 i{ 0 }; i < mesh->mNumVertices; ++i)
		{
			//Add the new vertex.
			new_mesh._Vertices.Emplace();
			Vertex &new_vertex{ new_mesh._Vertices.Back() };

			//Set the position.
			new_vertex._Position._X = mesh->mVertices[i].x;
			new_vertex._Position._Y = mesh->mVertices[i].y;
			new_vertex._Position._Z = mesh->mVertices[i].z;

			//Set the normal.
			new_vertex._Normal._X = mesh->mNormals[i].x;
			new_vertex._Normal._Y = mesh->mNormals[i].y;
			new_vertex._Normal._Z = mesh->mNormals[i].z;

			//Set the tangent.
			new_vertex._Tangent._X = mesh->mTangents[i].x;
			new_vertex._Tangent._Y = mesh->mTangents[i].y;
			new_vertex._Tangent._Z = mesh->mTangents[i].z;

			//Set the texture coordinate.
			if (mesh->mTextureCoords[0])
			{
				new_vertex._TextureCoordinate._X = mesh->mTextureCoords[0][i].x;
				new_vertex._TextureCoordinate._Y = mesh->mTextureCoords[0][i].y;
			}

			else
			{
				new_vertex._TextureCoordinate = Vector2<float32>(0.0f, 0.0f);
			}
		}
	}

	//Process the indices.
	for (uint32 face_index{ 0 }; face_index < mesh->mNumFaces; ++face_index)
	{
		const aiFace &face{ mesh->mFaces[face_index] };

		for (uint32 index_index{ 0 }; index_index < face.mNumIndices; ++index_index)
		{
			new_mesh._Indices.Emplace(face.mIndices[index_index]);
		}
	}
}

/*
*	Processes a single node.
*/
FORCE_INLINE void ProcessNode(const aiScene *const RESTRICT scene, const aiNode *const RESTRICT node, AnimatedModelFile *const RESTRICT animated_model_file, DynamicArray<BoneInformation> *const RESTRICT bone_information) NOEXCEPT
{
	//Process all meshes.
	for (uint32 i{ 0 }; i < node->mNumMeshes; ++i)
	{
		//Cache the mesh.
		const aiMesh *const RESTRICT mesh{ scene->mMeshes[node->mMeshes[i]] };

		//Process the mesh.
		ProcessMesh(scene, node, mesh, animated_model_file, bone_information);
	}

	//Process all child nodes.
	for (uint32 i{ 0 }; i < node->mNumChildren; ++i)
	{
		ProcessNode(scene, node->mChildren[i], animated_model_file, bone_information);
	}
}

/*
*	Builds a skeleton.
*/
FORCE_INLINE void BuildSkeleton(const aiNode *const RESTRICT node, const DynamicArray<BoneInformation> &bone_information, AnimatedModelFile *const RESTRICT animated_model_file, Bone *const RESTRICT parent) NOEXCEPT
{
	//Find the bone information.
	const BoneInformation *RESTRICT _bone_information{ nullptr };

	for (const BoneInformation &__bone_information : bone_information)
	{
		if (StringUtilities::IsEqual(__bone_information._Name.Data(), node->mName.C_Str()))
		{
			_bone_information = &__bone_information;

			break;
		}
	}

	//If we have a parent (i.e. the root bone has been added), then just return if this is not a bone. Otherwise call recursively with this node's children.
	if (!_bone_information)
	{
		if (parent)
		{
			return;
		}
		
		else
		{
			for (uint32 child_index{ 0 }; child_index < node->mNumChildren; ++child_index)
			{
				BuildSkeleton(node->mChildren[child_index], bone_information, animated_model_file, nullptr);
			}

			return;
		}
	}

	//This node is a bone - Set up the bone!
	Bone bone;

	bone._Name = HashString(_bone_information->_Name.Data());
	bone._Index = _bone_information->_Index;
	bone._BindTransform = _bone_information->_BindTransform;
	bone._InverseBindTransform = bone._BindTransform;
	bone._InverseBindTransform.Inverse();

	//Call recursively.
	for (uint32 child_index{ 0 }; child_index < node->mNumChildren; ++child_index)
	{
		BuildSkeleton(node->mChildren[child_index], bone_information, animated_model_file, &bone);
	}

	//If we have a parent, add this bone to its' childrens - Otherwise this is the root node!
	if (parent)
	{
		parent->_Children.Emplace(bone);
	}

	else
	{
		animated_model_file->_ParentTransform = node->mParent ? ConvertAssimpMatrix(node->mParent->mTransformation) : MatrixConstants::IDENTITY;
		//animated_model_file->_ParentTransform.Inverse();
		animated_model_file->_Skeleton._RootBone = bone;
	}
}

/*
*	Processes a single node.
*/
FORCE_INLINE void ProcessNode(const aiScene *const RESTRICT scene, const aiNode *const RESTRICT node, ModelFile *const RESTRICT model_file) NOEXCEPT
{
	//List the meta data.
	if (node->mMetaData)
	{
		ListMetaData(node->mMetaData);
	}

	//Process all meshes.
	for (uint32 i{ 0 }; i < node->mNumMeshes; ++i)
	{
		//Cache the mesh.
		const aiMesh *const RESTRICT mesh{ scene->mMeshes[node->mMeshes[i]] };

		//Process the mesh.
		ProcessMesh(scene, node, mesh, model_file);
	}

	//Process all child nodes.
	for (uint32 i{ 0 }; i < node->mNumChildren; ++i)
	{
		ProcessNode(scene, node->mChildren[i], model_file);
	}
}

/*
*	Reads the animated model file at the given file path. Returns if the read was succesful.
*/
NO_DISCARD bool FBXReader::Read(const char *const RESTRICT file_path, AnimatedModelFile *const RESTRICT animated_model_file) NOEXCEPT
{
	//Define constants.
	constexpr uint32 POST_PROCESS_FLAGS
	{
		aiProcess_CalcTangentSpace
		| aiProcess_JoinIdenticalVertices
		| aiProcess_Triangulate
		| aiProcess_LimitBoneWeights
		| aiProcess_ImproveCacheLocality
		| aiProcess_FlipUVs
		| aiProcess_FlipWindingOrder
	};

	ASSERT(File::Exists(file_path), "File path: %s does not exist!", file_path);

	//Set up the importer.
	Assimp::Importer importer;

	//Load the scene.
	const aiScene *const RESTRICT scene{ importer.ReadFile(file_path, POST_PROCESS_FLAGS) };

	//Check if the import succeeded.
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		ASSERT(false, "Import failed: %s", importer.GetErrorString());

		return false;
	}

	//Assume Blender, for now. :x
	animated_model_file->_Creator = AnimatedModelFile::Creator::BLENDER;

	//Process the root node.
	DynamicArray<BoneInformation> bone_information;
	ProcessNode(scene, scene->mRootNode, animated_model_file, &bone_information);

	//Build the skeleton.
	BuildSkeleton(scene->mRootNode, bone_information, animated_model_file, nullptr);

	//Post process the animated model file.
	animated_model_file->PostProcess();

	//Return that the read succeeded!
	return true;
}

/*
*	Reads the animation file at the given file path. Returns if the read was succesful.
*/
NO_DISCARD bool FBXReader::Read(const char *const RESTRICT file_path, AnimationFile *const RESTRICT animation_file) NOEXCEPT
{
	//Define constants.
	constexpr uint32 POST_PROCESS_FLAGS
	{
		aiProcess_CalcTangentSpace
		| aiProcess_JoinIdenticalVertices
		| aiProcess_Triangulate
		| aiProcess_ImproveCacheLocality
		| aiProcess_FlipUVs
		| aiProcess_FlipWindingOrder
	};

	ASSERT(File::Exists(file_path), "File path: %s does not exist!", file_path);

	//Set up the importer.
	Assimp::Importer importer;

	//Load the scene.
	const aiScene *const RESTRICT scene{ importer.ReadFile(file_path, POST_PROCESS_FLAGS) };

	//Check if the import succeeded.
	if (!scene || !scene->mRootNode)
	{
		ASSERT(false, "Import failed: %s", importer.GetErrorString());

		return false;
	}

	ASSERT(scene->mNumAnimations > 0, "Scene contains no animations!");

	//Assume Blender, for now. :x
	animation_file->_Creator = AnimationFile::Creator::BLENDER;

	//Just pick the first animation.
	const aiAnimation *const RESTRICT animation{ scene->mAnimations[0] };

	//Calculate the duration.
	animation_file->_Duration = static_cast<float32>(animation->mDuration / animation->mTicksPerSecond);

	//Reserve the appropriate amount of channels.
	animation_file->_Channels.Reserve(animation->mNumChannels);

	//Add the channels!
	for (uint32 channel_index{ 0 }; channel_index < animation->mNumChannels; ++channel_index)
	{
		//Cache the channel.
		const aiNodeAnim *const RESTRICT channel{ animation->mChannels[channel_index] };

		//Add the new channel.
		animation_file->_Channels.Emplace();
		AnimationChannel &new_channel{ animation_file->_Channels.Back() };

		//Set the bone identifier.
		new_channel._BoneIdentifier = HashString(channel->mNodeName.C_Str());

		//Reserve the appropriate amount of keyframes.
		new_channel._Keyframes.Reserve(channel->mNumPositionKeys);

		//Add the keyframes.
		for (uint32 keyframe_index{ 0 }; keyframe_index < channel->mNumPositionKeys; ++keyframe_index)
		{
			//Add the new keyframe.
			new_channel._Keyframes.Emplace();
			AnimationKeyframe &new_keyframe{ new_channel._Keyframes.Back() };

			//Calculate the timestamp.
			new_keyframe._Timestamp = static_cast<float32>(channel->mPositionKeys[keyframe_index].mTime / animation->mTicksPerSecond);

			//Set the translation.
			new_keyframe._BoneTransform._Translation = Vector3<float32>(channel->mPositionKeys[keyframe_index].mValue.x, channel->mPositionKeys[keyframe_index].mValue.y, channel->mPositionKeys[keyframe_index].mValue.z);

			//Set the rotation.
			new_keyframe._BoneTransform._Rotation = Quaternion(channel->mRotationKeys[keyframe_index].mValue.x, channel->mRotationKeys[keyframe_index].mValue.y, channel->mRotationKeys[keyframe_index].mValue.z, channel->mRotationKeys[keyframe_index].mValue.w);

			//Set the scale.
			new_keyframe._BoneTransform._Scale = Vector3<float32>(channel->mScalingKeys[keyframe_index].mValue.x, channel->mScalingKeys[keyframe_index].mValue.y, channel->mScalingKeys[keyframe_index].mValue.z);
		}
	}

	//Post process the animation file.
	animation_file->PostProcess();

	return true;
}

/*
*	Reads the model file at the given file path. Returns if the read was succesful.
*/
NO_DISCARD bool FBXReader::Read(const char *const RESTRICT file_path, ModelFile *const RESTRICT model_file) NOEXCEPT
{
	//Define constants.
	constexpr uint32 POST_PROCESS_FLAGS
	{
		aiProcess_CalcTangentSpace
		| aiProcess_JoinIdenticalVertices
		| aiProcess_Triangulate
		| aiProcess_ImproveCacheLocality
		| aiProcess_FlipUVs
		| aiProcess_FlipWindingOrder
	};

	ASSERT(File::Exists(file_path), "File path: %s doesn't exist!", file_path);

	//Set up the importer.
	Assimp::Importer importer;

	//Load the scene.
	const aiScene *const RESTRICT scene{ importer.ReadFile(file_path, POST_PROCESS_FLAGS) };

	//Check if the import succeeded.
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		ASSERT(false, "Import failed: %s", importer.GetErrorString());

		return false;
	}

	//List the meta data.
	ListMetaData(scene->mMetaData);

	//Assume Blender, for now. :x
	model_file->_Creator = ModelFile::Creator::BLENDER;

	//Process the root node.
	ProcessNode(scene, scene->mRootNode, model_file);

#if 0
	//If the source asset format is Autodesk something something, assume we're importing a Quixel model and apply the correct transformation...
	for (uint32 i{ 0 }; i < scene->mMetaData->mNumProperties; ++i)
	{
		if (StringUtilities::Contains(scene->mMetaData->mKeys[i].C_Str(), "SourceAsset_Format")
			&& StringUtilities::Contains(((const aiString* const RESTRICT)scene->mMetaData->mValues[i].mData)->C_Str(), "Autodesk FBX Importer"))
		{
			for (ModelFile::Mesh &mesh : model_file->_Meshes)
			{
				for (Vertex &vertex : mesh._Vertices)
				{
					//Apply rotation.
					{
						const EulerAngles rotation{ BaseMath::DegreesToRadians(90.0f), 0.0f, 0.0f };

						vertex._Position.Rotate(rotation);
						vertex._Normal.Rotate(rotation);
						vertex._Tangent.Rotate(rotation);
					}

					//Apply scale.
					vertex._Position *= 0.01f;
				}
			}

			break;
		}
	}
#endif

	//Post process the model file.
	model_file->PostProcess();

	//Return that the read succeeded!
	return true;
}
#else
/*
*	Reads the animated model file at the given file path. Returns if the read was succesful.
*/
NO_DISCARD bool FBXReader::Read(const char *const RESTRICT file_path, AnimatedModelFile *const RESTRICT animated_model_file) NOEXCEPT
{
	ASSERT(false, "This should not be called in final builds!");

	return false;
}

/*
*	Reads the animated model file at the given file path. Returns if the read was succesful.
*/
NO_DISCARD bool FBXReader::Read(const char *const RESTRICT file_path, AnimationFile *const RESTRICT animation_file) NOEXCEPT
{
	ASSERT(false, "This should not be called in final builds!");

	return false;
}

/*
*	Reads the model file at the given file path. Returns if the read was succesful.
*/
NO_DISCARD bool FBXReader::Read(const char *const RESTRICT file_path, ModelFile *const RESTRICT model_file) NOEXCEPT
{
	ASSERT(false, "This should not be called in final builds!");

	return false;
}
#endif

#if 0 //Old implementation here, for reference.

//File.
#include <File/Core/BinaryFile.h>

//Third party.
#include <ThirdParty/zlib/zlib.h>

//Macros.
#define FBX_READER_PRINT_TO_OUTPUT(MESSAGE) /*PRINT_TO_OUTPUT(MESSAGE)*/

/*
*	Temporary data class definition.
*/
class TemporaryData final
{

public:

	//Enumeration covering all mapping information types.
	enum class MappingInformationType : uint8
	{
		BY_POLYGON_VERTEX,

		UNKNOWN
	};

	//Enumeration covering all reference information types.
	enum class ReferenceInformationType : uint8
	{
		DIRECT,
		INDEX_TO_DIRECT,

		UNKNOWN
	};

	//The mapping information type.
	MappingInformationType _MappingInformationType{ MappingInformationType::UNKNOWN };

	//The mapping information type.
	ReferenceInformationType _ReferenceInformationType{ ReferenceInformationType::UNKNOWN };

	//Denotes if tangents were found.
	bool _FoundTangents{ false };

	//The vertex position lookup.
	DynamicArray<Vector3<float32>> _VertexPositionLookup;

	//The vertex index lookup.
	DynamicArray<uint32> _VertexIndexLookup;

	//The texture coordinate lookup.
	DynamicArray<Vector2<float32>> _TextureCoordinateLookup;

	//The material names.
	DynamicArray<DynamicString> _MaterialNames;

};

/*
*	Reads a node.
*/
FORCE_INLINE static bool ReadNode(BinaryFile<BinaryFileMode::IN> *const RESTRICT binary_file, TemporaryData *const RESTRICT temporary_data, ModelFile *const RESTRICT model_file) NOEXCEPT
{
	//Read the end offset.
	uint32 end_offset;
	binary_file->Read(&end_offset, sizeof(uint32));

	//Read the number of properties.
	uint32 number_of_properties;
	binary_file->Read(&number_of_properties, sizeof(uint32));

	//The the property list length.
	uint32 property_list_length;
	binary_file->Read(&property_list_length, sizeof(uint32));

	//Read the name length.
	uint8 name_length;
	binary_file->Read(&name_length, sizeof(uint8));

	//Read the name.
	DynamicString name;
	name.SetLength(name_length);
	binary_file->Read(name.Data(), name_length);

	//Is this the start of a new geometry definition?
	if (name == "Geometry")
	{
		//Emplace the new mesh.
		model_file->_Meshes.Emplace();

		//Reset the vertex position lookup.
		temporary_data->_VertexPositionLookup.Clear();

		//Reset the vertex index lookup.
		temporary_data->_VertexIndexLookup.Clear();

		//Reset the texture coordinate lookup.
		temporary_data->_TextureCoordinateLookup.Clear();
	}

	//Read the properties.
	for (uint32 i{ 0 }; i < number_of_properties; ++i)
	{
		//Read the type code.
		char type_code;
		binary_file->Read(&type_code, sizeof(char));

		switch (type_code)
		{
			case 'C':
			{
				//Read the bool.
				bool value;
				binary_file->Read(&value, sizeof(bool));

				FBX_READER_PRINT_TO_OUTPUT(name.Data() << " value: " << value);

				break;
			}

			case 'D':
			{
				//Read the float64.
				float64 value;
				binary_file->Read(&value, sizeof(float64));

				FBX_READER_PRINT_TO_OUTPUT(name.Data() << " value: " << value);

				break;
			}

			case 'I':
			{
				//Read the int32.
				int32 value;
				binary_file->Read(&value, sizeof(int32));

				FBX_READER_PRINT_TO_OUTPUT(name.Data() << " value: " << value);

				break;
			}

			case 'L':
			{
				//Read the int64.
				int64 value;
				binary_file->Read(&value, sizeof(int64));

				FBX_READER_PRINT_TO_OUTPUT(name.Data() << " value: " << value);

				break;
			}

			case 'R':
			{
				//Read the raw data length.
				uint32 raw_data_length;
				binary_file->Read(&raw_data_length, sizeof(uint32));

				//Read the raw data.
				DynamicArray<char> raw_data;
				raw_data.Upsize<false>(raw_data_length);

				binary_file->Read(raw_data.Data(), raw_data_length);

				FBX_READER_PRINT_TO_OUTPUT(name.Data() << " value: " << raw_data.Data());

				break;
			}

			case 'S':
			{
				//Read the string length.
				uint32 string_length;
				binary_file->Read(&string_length, sizeof(uint32));

				//Read the value.
				DynamicString value;
				value.SetLength(string_length);

				binary_file->Read(value.Data(), string_length);

				FBX_READER_PRINT_TO_OUTPUT(name.Data() << " value: " << value.Data());

				//Is this the creator value?
				if (name == "Creator")
				{
					if (value.Find("Blender"))
					{
						model_file->_Creator = ModelFile::Creator::BLENDER;
					}
				}

				//Is this the mapping information type value?
				if (name == "MappingInformationType")
				{
					if (value == "ByPolygonVertex")
					{
						temporary_data->_MappingInformationType = TemporaryData::MappingInformationType::BY_POLYGON_VERTEX;
					}
				}

				//Is this the reference information type value?
				if (name == "ReferenceInformationType")
				{
					if (value == "Direct")
					{
						temporary_data->_ReferenceInformationType = TemporaryData::ReferenceInformationType::DIRECT;
					}

					if (value == "IndexToDirect")
					{
						temporary_data->_ReferenceInformationType = TemporaryData::ReferenceInformationType::INDEX_TO_DIRECT;
					}
				}

				//Is this the name of the material?
				if (name == "Material")
				{
					if (value != "")
					{
						temporary_data->_MaterialNames.Emplace(value);
					}
				}

				break;
			}

			case 'd':
			{
				//Read the array length.
				uint32 array_length;
				binary_file->Read(&array_length, sizeof(uint32));

				//Read the encoding.
				uint32 encoding;
				binary_file->Read(&encoding, sizeof(uint32));

				//Read the compressed length.
				uint32 compressed_length;
				binary_file->Read(&compressed_length, sizeof(uint32));

				//Read the values.
				DynamicArray<float64> values;
				values.Upsize<false>(array_length);

				if (encoding)
				{
					//Calculate the uncompressed length.
					const uint64 uncompressed_length{ sizeof(float64) * array_length };

					//Allocate the decompressed buffer.
					uint8* const RESTRICT decompressed_buffer{ static_cast<uint8* const RESTRICT>(Memory::Allocate(uncompressed_length)) };

					//Allocate and read the compressed buffer.
					uint8* const RESTRICT compressed_buffer{ static_cast<uint8* const RESTRICT>(Memory::Allocate(compressed_length)) };
					binary_file->Read(compressed_buffer, compressed_length);

					//Uncompress!
					uLongf destination_length{ static_cast<uLongf>(uncompressed_length) };
					uLong source_length{ static_cast<uLong>(compressed_length) };

					uncompress2(decompressed_buffer, &destination_length, compressed_buffer, &source_length);

					ASSERT(destination_length == uncompressed_length, "uncompressed_length does not match data!");
					ASSERT(source_length == compressed_length, "compressed_length does not match data!");

					//Read the values.
					Memory::Copy(values.Data(), decompressed_buffer, sizeof(float64) * array_length);

					//Free the buffers.
					Memory::Free(decompressed_buffer);
					Memory::Free(compressed_buffer);
				}

				else
				{
					binary_file->Read(values.Data(), sizeof(float64) * array_length);
				}

				//Is this the vertices?
				if (name == "Vertices")
				{
					for (uint64 i{ 0 }; i < values.Size() / 3; ++i)
					{
						//Write to the vertex position lookup.
						temporary_data->_VertexPositionLookup.Emplace(static_cast<float32>(values[i * 3 + 0]), static_cast<float32>(values[i * 3 + 1]), static_cast<float32>(values[i * 3 + 2]));
					}
				}

				//Is this the normals?
				if (name == "Normals")
				{
					//Cache the mesh.
					ModelFile::Mesh& mesh{ model_file->_Meshes.Back() };

					switch (temporary_data->_MappingInformationType)
					{
					case TemporaryData::MappingInformationType::BY_POLYGON_VERTEX:
					{
						switch (temporary_data->_ReferenceInformationType)
						{
						case TemporaryData::ReferenceInformationType::DIRECT:
						{
							for (uint64 i{ 0 }; i < values.Size(); i += 3)
							{
								//Create the new vertex.
								mesh._Vertices.Emplace();
								Vertex& new_vertex{ mesh._Vertices.Back() };

								//Create the new index.
								mesh._Indices.Emplace(mesh._Indices.Empty() ? 0 : mesh._Indices.Back() + 1);

								//Retrieve the vertex index.
								const uint32 vertex_index{ temporary_data->_VertexIndexLookup[i / 3] };

								//Retrieve the vertex position.
								const Vector3<float32> vertex_position{ temporary_data->_VertexPositionLookup[vertex_index] };

								//Set the position.
								new_vertex._Position = vertex_position;

								//Set the normal.
								new_vertex._Normal = Vector3<float32>(static_cast<float32>(values[i + 0]), static_cast<float32>(values[i + 1]), static_cast<float32>(values[i + 2]));
							}

							break;
						}

						default:
						{
							ASSERT(false, "Invalid case!");

							break;
						}
						}

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
					}
				}

				//Is this the tangents?
				if (name == "Tangents")
				{
					//Report that tangents were found.
					temporary_data->_FoundTangents = true;

					//Cache the mesh.
					ModelFile::Mesh& mesh{ model_file->_Meshes.Back() };

					switch (temporary_data->_MappingInformationType)
					{
					case TemporaryData::MappingInformationType::BY_POLYGON_VERTEX:
					{
						switch (temporary_data->_ReferenceInformationType)
						{
						case TemporaryData::ReferenceInformationType::DIRECT:
						{
							for (uint64 i{ 0 }; i < values.Size(); i += 3)
							{
								//Retrieve the vertex.
								Vertex& vertex{ mesh._Vertices[i / 3] };

								//Set the tangent.
								vertex._Tangent = Vector3<float32>(static_cast<float32>(values[i + 0]), static_cast<float32>(values[i + 1]), static_cast<float32>(values[i + 2]));

								ASSERT(vertex._Normal != vertex._Tangent, "Normal and tangent are the same for " << binary_file->GetFilePath() << ", this is bad. / : Maybe the model isn't UV-wrapped properly?");
							}

							break;
						}

						default:
						{
							ASSERT(false, "Invalid case!");

							break;
						}
						}

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
					}
				}

				//Is this the UV's?
				if (name == "UV")
				{
					switch (temporary_data->_MappingInformationType)
					{
					case TemporaryData::MappingInformationType::BY_POLYGON_VERTEX:
					{
						switch (temporary_data->_ReferenceInformationType)
						{
						case TemporaryData::ReferenceInformationType::INDEX_TO_DIRECT:
						{
							for (uint64 i{ 0 }; i < values.Size(); i += 2)
							{
								//Write into the texture coordinate lookup.
								temporary_data->_TextureCoordinateLookup.Emplace(static_cast<float32>(values[i + 0]), static_cast<float32>(values[i + 1]));
							}

							break;
						}

						default:
						{
							ASSERT(false, "Invalid case!");

							break;
						}
						}

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
					}
				}

				for (uint64 i{ 0 }; i < values.Size(); ++i)
				{
					FBX_READER_PRINT_TO_OUTPUT(name.Data() << " value #" << i << ": " << values[i]);
				}

				break;
			}

			case 'i':
			{
				//Read the array length.
				uint32 array_length;
				binary_file->Read(&array_length, sizeof(uint32));

				//Read the encoding.
				uint32 encoding;
				binary_file->Read(&encoding, sizeof(uint32));

				//Read the compressed length.
				uint32 compressed_length;
				binary_file->Read(&compressed_length, sizeof(uint32));

				//Read the values.
				DynamicArray<int32> values;
				values.Upsize<false>(array_length);

				if (encoding)
				{
					//Calculate the uncompressed length.
					const uint64 uncompressed_length{ sizeof(int32) * array_length };

					//Allocate the decompressed buffer.
					uint8* const RESTRICT decompressed_buffer{ static_cast<uint8* const RESTRICT>(Memory::Allocate(uncompressed_length)) };

					//Allocate and read the compressed buffer.
					uint8* const RESTRICT compressed_buffer{ static_cast<uint8* const RESTRICT>(Memory::Allocate(compressed_length)) };
					binary_file->Read(compressed_buffer, compressed_length);

					//Uncompress!
					uLongf destination_length{ static_cast<uLongf>(uncompressed_length) };
					uLong source_length{ static_cast<uLong>(compressed_length) };

					uncompress2(decompressed_buffer, &destination_length, compressed_buffer, &source_length);

					ASSERT(destination_length == uncompressed_length, "uncompressed_length does not match data!");
					ASSERT(source_length == compressed_length, "compressed_length does not match data!");

					//Read the values.
					Memory::Copy(values.Data(), decompressed_buffer, sizeof(int32) * array_length);

					//Free the buffers.
					Memory::Free(decompressed_buffer);
					Memory::Free(compressed_buffer);
				}

				else
				{
					binary_file->Read(values.Data(), sizeof(int32) * array_length);
				}

				//Is this the polygon vertex indices?
				if (name == "PolygonVertexIndex")
				{
					//Cache the mesh.
					ModelFile::Mesh& mesh{ model_file->_Meshes.Back() };

					for (uint64 i{ 0 }; i < values.Size(); ++i)
					{
						//Calculate the actual index.
						const uint32 actual_index{ static_cast<uint32>(values[i] >= 0 ? values[i] : (~values[i])) };

						//Write to the vertex index lookup.
						temporary_data->_VertexIndexLookup.Emplace(actual_index);
					}
				}

				//Is this the UV indices?
				if (name == "UVIndex")
				{
					//Cache the mesh.
					ModelFile::Mesh& mesh{ model_file->_Meshes.Back() };

					switch (temporary_data->_MappingInformationType)
					{
					case TemporaryData::MappingInformationType::BY_POLYGON_VERTEX:
					{
						switch (temporary_data->_ReferenceInformationType)
						{
						case TemporaryData::ReferenceInformationType::INDEX_TO_DIRECT:
						{
							for (uint64 i{ 0 }; i < values.Size(); ++i)
							{
								//Retrieve the vertex.
								Vertex& vertex{ mesh._Vertices[i] };

								//Retrieve the texture coordinate index.
								const uint32 texture_coordinate_index{ static_cast<uint32>(values[i]) };

								//Write the texture coordinate.
								vertex._TextureCoordinate = temporary_data->_TextureCoordinateLookup[texture_coordinate_index];
							}

							break;
						}

						default:
						{
							ASSERT(false, "Invalid case!");

							break;
						}
						}

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
					}
				}

				for (uint64 i{ 0 }; i < values.Size(); ++i)
				{
					FBX_READER_PRINT_TO_OUTPUT(name.Data() << " value #" << i << ": " << values[i]);
				}

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	//Is there a nested list in this node?
	uint32 number_of_children{ 0 };

	if (binary_file->GetCurrentPosition() < end_offset)
	{
		while (binary_file->GetCurrentPosition() + 13 < end_offset)
		{
			//Read the new node.
			ReadNode(binary_file, temporary_data, model_file);

			//Update the number of children.
			++number_of_children;
		}

		//Read the null record.
		uint8 null_record[13];
		binary_file->Read(null_record, 13);

		ASSERT(binary_file->GetCurrentPosition() == end_offset, "oh no");
	}

	//Return whether or not this node is valid.
	return	!(number_of_children == 0
			&& number_of_properties == 0
			&& name_length == 0);
}

/*
*	Reads the model file at the given file path. Returns if the read was succesful.
*/
NO_DISCARD bool FBXReader::Read(const char *const RESTRICT file_path, ModelFile *const RESTRICT model_file) NOEXCEPT
{
	ASSERT(File::Exists(file_path), "File path: " << file_path << " doesn't exist!");

	//Open the binary file.
	BinaryFile<BinaryFileMode::IN> binary_file{ file_path };

	//Read the header title.
	char header_title[21];
	binary_file.Read(header_title, 21);

	//Read the header unknown.
	char header_unknown[2];
	binary_file.Read(header_unknown, 2);

	//Read the header version.
	uint32 header_version;
	binary_file.Read(&header_version, sizeof(uint32));

	//Read the nodes.
	TemporaryData temporary_data;

	while (ReadNode(&binary_file, &temporary_data, model_file));

	ASSERT(temporary_data._FoundTangents, "No tangents were found, please export " << file_path << " with tangents.");

	//Close the binary file.
	binary_file.Close();

	//Sort the meshes based on the material names.
	//ASSERT(model_file->_Meshes.Size() == temporary_data._MaterialNames.Size(), "Mismatch detected!");

	if (model_file->_Meshes.Size() == temporary_data._MaterialNames.Size())
	{
		for (uint64 iterator{ 1 }; iterator != model_file->_Meshes.Size(); ++iterator)
		{
			uint64 reverse_iterator{ iterator };

			while (reverse_iterator != 0 && strcmp(temporary_data._MaterialNames[reverse_iterator].Data(), temporary_data._MaterialNames[reverse_iterator - 1].Data()) < 0)
			{
				Swap(&model_file->_Meshes[reverse_iterator], &model_file->_Meshes[reverse_iterator - 1]);

				--reverse_iterator;
			}
		}
	}

	//Post process the model file.
	model_file->PostProcess();

	//Return that the read succeeded!
	return true;
}
#endif