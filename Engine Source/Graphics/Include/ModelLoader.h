#pragma once

//Engine core.
#include <EngineCore.h>

//Graphics.
#include <Vertex.h>

//Math.
#include <GameMath.h>

//Third party libraries.
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace ModelLoader
{

	/*
	*	Processes a single Assimp mesh.
	*/
	void ProcessMesh(aiMesh *CATALYST_RESTRICT mesh, const aiScene *CATALYST_RESTRICT scene, DynamicArray<Vertex> &vertices, DynamicArray<uint32> &indices, float &extent) CATALYST_NOEXCEPT
	{
		//Process the vertices.
		for (uint32 i = 0; i < mesh->mNumVertices; ++i)
		{
			vertices.Emplace(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z, mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

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
				indices.Emplace(face.mIndices[j]);
			}
		}
	}

	/*
	*	Processes a single Assimp node.
	*/
	void ProcessNode(aiNode *CATALYST_RESTRICT node, const aiScene *CATALYST_RESTRICT scene, DynamicArray<Vertex> &vertices, DynamicArray<uint32> &indices, float &extent) CATALYST_NOEXCEPT
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
	void LoadModel(const char *CATALYST_RESTRICT modelPath, DynamicArray<Vertex> &vertices, DynamicArray<uint32> &indices, float &extent) CATALYST_NOEXCEPT
	{
		vertices.Reserve(1'000'000);
		indices.Reserve(1'000'000);

		Assimp::Importer modelImporter;
		const aiScene *modelScene = modelImporter.ReadFile(modelPath, aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

		ProcessNode(modelScene->mRootNode, modelScene, vertices, indices, extent);
	}

}