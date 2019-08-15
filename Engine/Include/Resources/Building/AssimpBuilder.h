#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Animation.
#include <Animation/AnimatedVertex.h>

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
	FORCE_INLINE static void BuildAnimatedModel(const char *const RESTRICT file, DynamicArray<AnimatedVertex> *const RESTRICT vertices, DynamicArray<uint32> *const RESTRICT indices) NOEXCEPT
	{
		//Load the model.
		Assimp::Importer modelImporter;
		const aiScene *modelScene = modelImporter.ReadFile(file, aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

		//Process the node(s).
		ProcessAnimatedModelNode(modelScene->mRootNode, modelScene, vertices, indices);
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
		for (uint32 i = 0; i < mesh->mNumVertices; ++i)
		{
			const Vector3<float> position{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			const Vector3<float> normal{ mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
			const Vector3<float> tangent{ mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
			const Vector3<int32> bone_indices{ 0, 1, 2 };
			const Vector3<float> bone_weights{ 1.0f, 0.0f, 0.0f };
			const Vector2<float> texture_coordinate{ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

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