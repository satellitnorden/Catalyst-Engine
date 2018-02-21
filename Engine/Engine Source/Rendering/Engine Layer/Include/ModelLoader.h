#pragma once

//Engine core.
#include <EngineCore.h>

//Math.
#include <GameMath.h>

//Rendering.
#include <PhysicalVertex.h>

//Third party libraries.
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace ModelLoader
{

	/*
	*	Processes a single Assimp mesh.
	*/
	static void ProcessMesh(aiMesh *RESTRICT mesh, const aiScene *RESTRICT scene, DynamicArray<PhysicalVertex> &vertices, DynamicArray<uint32> &indices, float &extent) NOEXCEPT
	{
		//Process the vertices.
		for (uint32 i = 0; i < mesh->mNumVertices; ++i)
		{
			vertices.EmplaceSlow(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z, mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

			extent = GameMath::Maximum(extent, GameMath::Absolute(vertices.Back().position.X));
			extent = GameMath::Maximum(extent, GameMath::Absolute(vertices.Back().position.Y));
			extent = GameMath::Maximum(extent, GameMath::Absolute(vertices.Back().position.Z));
		}

		//Process the indices.
		for (uint32 i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace face = mesh->mFaces[i];

			for (unsigned int j = 0; j < face.mNumIndices; ++j)
			{
				indices.EmplaceSlow(face.mIndices[j]);
			}
		}
	}

	/*
	*	Processes a single Assimp node.
	*/
	static void ProcessNode(aiNode *RESTRICT node, const aiScene *RESTRICT scene, DynamicArray<PhysicalVertex> &vertices, DynamicArray<uint32> &indices, float &extent) NOEXCEPT
	{
		//Process all meshes.
		for (uint32 i = 0; i < node->mNumMeshes; ++i)
		{
			ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene, vertices, indices, extent);
		}

		//Process all nodes.
		for (uint32 i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene, vertices, indices, extent);
		}
	}

	/*
	*	Given a path, loads a model and outputs the vertices and indices in the supplied containers.
	*/
	static void LoadModel(const char *RESTRICT modelPath, DynamicArray<PhysicalVertex> &vertices, DynamicArray<uint32> &indices, float &extent) NOEXCEPT
	{
		vertices.Reserve(1'000'000);
		indices.Reserve(1'000'000);

		Assimp::Importer modelImporter;
		const aiScene *modelScene = modelImporter.ReadFile(modelPath, aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

		ProcessNode(modelScene->mRootNode, modelScene, vertices, indices, extent);
	}

}