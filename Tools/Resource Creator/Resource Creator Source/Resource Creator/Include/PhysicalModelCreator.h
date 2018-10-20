#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/HashString.h>

//Math.
#include <Math/CatalystBaseMath.h>

//Rendering.
#include <Rendering/Engine/PhysicalVertex.h>

//Resources
#include <Resources/ResourcesCore.h>

//Third party libraries.
#pragma optimize("", off)
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#pragma optimize("", on)

class PhysicalModelCreator final
{

public:

	class PhysicalModelCreationParameters final
	{

	public:

		//The output file path.
		const char *RESTRICT _Output;

		//The resource id.
		const char *RESTRICT _ID;

		//The file path.
		const char *RESTRICT _File;

	};

	/*
	*	Creates a physical model resource file.
	*/
	static void CreatePhysicalModel(const PhysicalModelCreationParameters &parameters) noexcept
	{
		//What should the material be called?
		DynamicString fileName{ parameters._Output };
		fileName += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> file{ fileName.CString() };

		//Write the resource type to the file.
		constexpr uint8 resourceType{ static_cast<uint8>(ResourceType::PhysicalModel) };
		file.Write(&resourceType, sizeof(ResourceType));

		//Write the resource ID to the file.
		const HashString resourceID{ parameters._ID };
		file.Write(&resourceID, sizeof(HashString));

		//Load the model.
		DynamicArray<PhysicalVertex> vertices;
		DynamicArray<uint32> indices;
		float extent{ 0.0f };

		Assimp::Importer modelImporter;
		const aiScene *modelScene = modelImporter.ReadFile(parameters._File, aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

		ProcessNode(modelScene->mRootNode, modelScene, vertices, indices, extent);

		//Write the extent to the file.
		file.Write(&extent, sizeof(float));

		//Write the size of the vertices to the file.
		const uint64 sizeOfVertices{ vertices.Size() };
		file.Write(&sizeOfVertices, sizeof(uint64));

		//Write the vertices to the file.
		file.Write(vertices.Data(), sizeof(PhysicalVertex) * sizeOfVertices);

		//Write the size of the indices to the file.
		const uint64 sizeOfIndices{ indices.Size() };
		file.Write(&sizeOfIndices, sizeof(uint64));

		//Write the vertices to the file.
		file.Write(indices.Data(), sizeof(uint32) * sizeOfIndices);

		//Close the file.
		file.Close();
	}

private:

	/*
	*	Processes a single Assimp mesh.
	*/
	static void ProcessMesh(aiMesh *RESTRICT mesh, const aiScene *RESTRICT scene, DynamicArray<PhysicalVertex> &vertices, DynamicArray<uint32> &indices, float &extent) NOEXCEPT
	{
		//Process the vertices.
		for (uint32 i = 0; i < mesh->mNumVertices; ++i)
		{
			vertices.EmplaceSlow(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z, mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

			extent = CatalystBaseMath::Maximum(extent, CatalystBaseMath::Absolute(vertices.Back()._Position._X));
			extent = CatalystBaseMath::Maximum(extent, CatalystBaseMath::Absolute(vertices.Back()._Position._Y));
			extent = CatalystBaseMath::Maximum(extent, CatalystBaseMath::Absolute(vertices.Back()._Position._Z));
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

};