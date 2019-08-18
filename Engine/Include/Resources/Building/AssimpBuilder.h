#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Algorithms/SortingAlgorithms.h>
#include <Core/Containers/DynamicArray.h>

//Animation.
#include <Animation/AnimatedVertex.h>
#include <Animation/Skeleton.h>

//Rendering.
#include <Rendering/Native/Vertex.h>

//Third party.
#include <ThirdParty/assimp/Importer.hpp>
#include <ThirdParty/assimp/scene.h>
#include <ThirdParty/assimp/postprocess.h>

class AssimpBuilder final
{

public:

	/*
	*	Builds an animated model.
	*/
	FORCE_INLINE static void BuildAnimatedModel(const char *const RESTRICT file, DynamicArray<AnimatedVertex> *const RESTRICT vertices, DynamicArray<uint32> *const RESTRICT indices, Skeleton *const RESTRICT skeleton) NOEXCEPT
	{
		//Load the model.
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(file, aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

		//Process the node(s).
		ProcessAnimatedModelNode(scene->mRootNode, scene, vertices, indices);

		//Calculate the skeleton, starting with the root node.
		aiNode *RESTRICT root_node{ FindRootNode(scene->mRootNode, scene) };
		ASSERT(root_node, "Could not find root node!");
		aiBone *RESTRICT root_bone{ FindBone(scene->mRootNode, scene, root_node->mName) };
		ASSERT(root_bone, "Could not find root bone!");

		ConvertBone(root_bone, &skeleton->_RootBone);

		//Calculate all the children.
		AddChildBones(root_node, scene, &skeleton->_RootBone);
	}

	/*
	*	Builds a model.
	*/
	FORCE_INLINE static void BuildModel(const char *const RESTRICT file, DynamicArray<Vertex> *const RESTRICT vertices, DynamicArray<uint32> *const RESTRICT indices) NOEXCEPT
	{
		//Load the model.
		Assimp::Importer modelImporter;
		const aiScene *modelScene = modelImporter.ReadFile(file, aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

		//Process the node(s).
		ProcessModelNode(modelScene->mRootNode, modelScene, vertices, indices);
	}

private:

	/*
	*	Processes a single Assimp animated model mesh.
	*/
	static void ProcessAnimatedModelMesh(aiMesh *RESTRICT mesh, const aiScene *RESTRICT scene, DynamicArray<AnimatedVertex> *const RESTRICT vertices, DynamicArray<uint32> *const RESTRICT indices) NOEXCEPT
	{
		//Process the vertices.
		class BoneWeight final
		{

		public:

			//The index.
			int32 _Index;

			//The weight.
			float _Weight;

			/*
			*	Constructor taking all values as arguments.
			*/
			FORCE_INLINE constexpr BoneWeight(const int32 initial_index, const float initial_weight) NOEXCEPT
				:
				_Index(initial_index),
				_Weight(initial_weight)
			{

			}

		};

		DynamicArray<BoneWeight> temporary_bone_weights;

		temporary_bone_weights.Reserve(mesh->mNumBones);

		for (uint32 i = 0; i < mesh->mNumVertices; ++i)
		{
			//Calculate the regular vertex values.
			const Vector3<float> position{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			const Vector3<float> normal{ mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
			const Vector3<float> tangent{ mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
			const Vector2<float> texture_coordinate{ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

			//Go through all bones and see which one has an effect on this vertex.
			temporary_bone_weights.ClearFast();

			Memory::Set(temporary_bone_weights.Data(), 0, sizeof(BoneWeight) * mesh->mNumBones);

			for (uint32 j{ 0 }; j < mesh->mNumBones; ++j)
			{
				for (uint32 k{ 0 }; k < mesh->mBones[j]->mNumWeights; ++k)
				{
					if (mesh->mBones[j]->mWeights[k].mVertexId == i)
					{
						temporary_bone_weights.EmplaceFast(j, mesh->mBones[j]->mWeights[k].mWeight);
					}
				}
			}

			//Need to sort the bone weights if they're over three and chose the three with the highest weights.
			if (temporary_bone_weights.Size() > 3)
			{
				SortingAlgorithms::InsertionSort<BoneWeight>
				(
				temporary_bone_weights.Begin(),
				temporary_bone_weights.End(),
				nullptr,
				[](const void *const RESTRICT userData, const BoneWeight *const RESTRICT first, const BoneWeight *const RESTRICT second)
				{
					return first->_Weight > second->_Weight;
				}
				);

			}

			//Normalize the weights so that they add up to 1.0f.
			float total_weight{ temporary_bone_weights[0]._Weight + temporary_bone_weights[1]._Weight + temporary_bone_weights[2]._Weight };
			total_weight = 1.0f / total_weight;

			temporary_bone_weights[0]._Weight *= total_weight;
			temporary_bone_weights[1]._Weight *= total_weight;
			temporary_bone_weights[2]._Weight *= total_weight;

			const Vector3<int32> bone_indices{ temporary_bone_weights[0]._Index, temporary_bone_weights[1]._Index, temporary_bone_weights[2]._Index };
			const Vector3<float> bone_weights{ temporary_bone_weights[0]._Weight, temporary_bone_weights[1]._Weight, temporary_bone_weights[2]._Weight };

			vertices->EmplaceSlow(position, normal, tangent, bone_indices, bone_weights, texture_coordinate);

			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._Position._X), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._Position._Y), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._Position._Z), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._Normal._X), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._Normal._Y), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._Normal._Z), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._Tangent._X), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._Tangent._Y), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._Tangent._Z), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._BoneWeights._X), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._BoneWeights._Y), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._BoneWeights._Z), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._TextureCoordinate._X), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._TextureCoordinate._Y), "NaN detected!");
		}

		//Process the indices.
		for (uint32 i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace face = mesh->mFaces[i];

			for (uint32 j = 0; j < face.mNumIndices; ++j)
			{
				indices->EmplaceSlow(face.mIndices[j]);
			}
		}
	}

	/*
	*	Processes a single Assimp node.
	*/
	static void ProcessAnimatedModelNode(aiNode *RESTRICT node, const aiScene *RESTRICT scene, DynamicArray<AnimatedVertex> *const RESTRICT vertices, DynamicArray<uint32> *const RESTRICT indices) NOEXCEPT
	{
		//Process all meshes.
		for (uint32 i = 0; i < node->mNumMeshes; ++i)
		{
			ProcessAnimatedModelMesh(scene->mMeshes[node->mMeshes[i]], scene, vertices, indices);
		}

		//Process all nodes.
		for (uint32 i = 0; i < node->mNumChildren; ++i)
		{
			ProcessAnimatedModelNode(node->mChildren[i], scene, vertices, indices);
		}
	}

	/*
	*	Finds the root aiBone, nullptr if none was found.
	*/
	FORCE_INLINE RESTRICTED static NO_DISCARD aiNode *const RESTRICT FindRootNode(aiNode *RESTRICT node, const aiScene *RESTRICT scene) NOEXCEPT
	{
		if (FindBone(scene->mRootNode, scene, node->mName))
		{
			return node;
		}

		for (uint32 i = 0; i < node->mNumChildren; ++i)
		{
			if (aiNode *const RESTRICT found_node{ FindRootNode(node->mChildren[i], scene) })
			{
				return found_node;
			}
		}
		
		return nullptr;
	}

	/*
	*	Finds the aiBone with the given name in any of the meshes in the scene graph. nullptr if none was found.
	*/
	FORCE_INLINE RESTRICTED static NO_DISCARD aiBone *const RESTRICT FindBone(aiNode *RESTRICT node, const aiScene *RESTRICT scene, const aiString &name) NOEXCEPT
	{
		for (uint32 i = 0; i < node->mNumMeshes; ++i)
		{
			const aiMesh *const RESTRICT mesh{ scene->mMeshes[node->mMeshes[i]] };

			for (uint32 j{ 0 }; j < mesh->mNumBones; ++j)
			{
				if (mesh->mBones[j]->mName == name)
				{
					return mesh->mBones[j];
				}
			}
		}

		for (uint32 i = 0; i < node->mNumChildren; ++i)
		{
			if (aiBone *const RESTRICT found_bone{ FindBone(node->mChildren[i], scene, name) })
			{
				return found_bone;
			}
		}

		return nullptr;
	}

	/*
	*	Adds the child bones for the given bone.
	*/
	FORCE_INLINE static void AddChildBones(aiNode *RESTRICT node, const aiScene *RESTRICT scene, Bone *const RESTRICT bone) NOEXCEPT
	{
		//Check all child nodes, and see if they are bones.
		for (uint32 i{ 0 }; i < node->mNumChildren; ++i)
		{
			if (aiBone *const RESTRICT found_bone{ FindBone(scene->mRootNode, scene, node->mChildren[i]->mName) })
			{
				bone->_Children.EmplaceSlow();
				ConvertBone(found_bone, &bone->_Children.Back());

				AddChildBones(node->mChildren[i], scene, &bone->_Children.Back());
			}
		}
	}

	/*
	*	Converts an aiBone to  a Bone.
	*/
	FORCE_INLINE static void ConvertBone(const aiBone *const RESTRICT in, Bone *const RESTRICT out) NOEXCEPT
	{
		out->_Name = HashString(in->mName.C_Str());
		Memory::Copy(&out->_BindTransform, &in->mOffsetMatrix, sizeof(Matrix4));
		out->_InverseBindTransform = out->_BindTransform;
		out->_InverseBindTransform.Inverse();
	}

	/*
	*	Processes a single Assimp model mesh.
	*/
	static void ProcessModelMesh(aiMesh *RESTRICT mesh, const aiScene *RESTRICT scene, DynamicArray<Vertex> *const RESTRICT vertices, DynamicArray<uint32> *const RESTRICT indices) NOEXCEPT
	{
		//Process the vertices.
		for (uint32 i = 0; i < mesh->mNumVertices; ++i)
		{
			vertices->EmplaceSlow(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z, mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._Position._X), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._Position._Y), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._Position._Z), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._Normal._X), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._Normal._Y), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._Normal._Z), "NaN detected!");
			//ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._Tangent._X), "NaN detected!");
			//ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._Tangent._Y), "NaN detected!");
			//ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._Tangent._Z), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._TextureCoordinate._X), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(vertices->Back()._TextureCoordinate._Y), "NaN detected!");
		}

		//Process the indices.
		for (uint32 i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace face = mesh->mFaces[i];

			for (uint32 j = 0; j < face.mNumIndices; ++j)
			{
				indices->EmplaceSlow(face.mIndices[j]);
			}
		}
	}

	/*
	*	Processes a single Assimp node.
	*/
	static void ProcessModelNode(aiNode *RESTRICT node, const aiScene *RESTRICT scene, DynamicArray<Vertex> *const RESTRICT vertices, DynamicArray<uint32> *const RESTRICT indices) NOEXCEPT
	{
		//Process all meshes.
		for (uint32 i = 0; i < node->mNumMeshes; ++i)
		{
			ProcessModelMesh(scene->mMeshes[node->mMeshes[i]], scene, vertices, indices);
		}

		//Process all nodes.
		for (uint32 i = 0; i < node->mNumChildren; ++i)
		{
			ProcessModelNode(node->mChildren[i], scene, vertices, indices);
		}
	}

};