#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/HashString.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>

//Rendering.
#include <Rendering/Engine/Vertex.h>
#include <Rendering/Engine/RenderingCore.h>

//Resources
#include <Resources/Core/ResourcesCore.h>

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

		//The low detail file path.
		const char *RESTRICT _LowDetailFile;

		//The medium detail file path.
		const char *RESTRICT _MediumDetailFile;

		//The high detail file path.
		const char *RESTRICT _HighDetailFile;

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

		//Load the models.
		for (uint8 i{ 0 }; i < UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails); ++i)
		{
			const char *const RESTRICT modelFile{ i == 0 ? parameters._LowDetailFile : i == 1 ? parameters._MediumDetailFile : parameters._HighDetailFile };

			DynamicArray<Vertex> vertices;
			DynamicArray<uint32> indices;
			float extent{ 0.0f };

			Assimp::Importer modelImporter;
			const aiScene *modelScene = modelImporter.ReadFile(modelFile, aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

			ProcessNode(modelScene->mRootNode, modelScene, vertices, indices, extent);

			//Write the extent to the file.
			file.Write(&extent, sizeof(float));

			//Write the size of the vertices to the file.
			const uint64 sizeOfVertices{ vertices.Size() };
			file.Write(&sizeOfVertices, sizeof(uint64));

			//Write the vertices to the file.
			file.Write(vertices.Data(), sizeof(Vertex) * sizeOfVertices);

			//Write the size of the indices to the file.
			const uint64 sizeOfIndices{ indices.Size() };
			file.Write(&sizeOfIndices, sizeof(uint64));

			//Write the vertices to the file.
			file.Write(indices.Data(), sizeof(uint32) * sizeOfIndices);

#if false
			PRINT_TO_OUTPUT("data->_Vertices[" << static_cast<uint32>(i) << "].Reserve(" << vertices.Size() << ");");

			PRINT_TO_OUTPUT("");

			for (const PhysicalVertex vertex : vertices)
			{
				char buffer[1024];

				sprintf_s(	buffer, "data->_Vertices[%u].EmplaceFast(Vector3<float>(%ff, %ff, %ff), Vector3<float>(%ff, %ff, %ff), Vector3<float>(%ff, %ff, %ff), Vector2<float>(%ff, %ff));",
							static_cast<uint32>(i),
							vertex._Position._X, vertex._Position._Y, vertex._Position._Z,
							vertex._Normal._X, vertex._Normal._Y, vertex._Normal._Z,
							vertex._Tangent._X, vertex._Tangent._Y, vertex._Tangent._Z,
							vertex._TextureCoordinate._X, vertex._TextureCoordinate._Y);

				PRINT_TO_OUTPUT(buffer);
			}

			PRINT_TO_OUTPUT("");

			PRINT_TO_OUTPUT("data->_Indices[" << static_cast<uint32>(i) << "].Reserve(" << indices.Size() << ");");

			PRINT_TO_OUTPUT("");

			for (const uint32 index : indices)
			{
				PRINT_TO_OUTPUT("data->_Indices[" << static_cast<uint32>(i) << "].EmplaceFast(" << index << ");");
			}

			PRINT_TO_OUTPUT("");
#endif
		}

		//Close the file.
		file.Close();
	}

private:

	/*
	*	Processes a single Assimp mesh.
	*/
	static void ProcessMesh(aiMesh *RESTRICT mesh, const aiScene *RESTRICT scene, DynamicArray<Vertex> &vertices, DynamicArray<uint32> &indices, float &extent) NOEXCEPT
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
	static void ProcessNode(aiNode *RESTRICT node, const aiScene *RESTRICT scene, DynamicArray<Vertex> &vertices, DynamicArray<uint32> &indices, float &extent) NOEXCEPT
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