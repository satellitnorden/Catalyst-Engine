#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/HashString.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>

//Resources
#include <Resources/Core/ResourcesCore.h>

//Vegetation.
#include <Vegetation/VegetationVertex.h>

//Third party libraries.
#pragma optimize("", off)
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#pragma optimize("", on)

class TreeVegetationModelCreator final
{

public:

	class TreeVegetationModelCreationParameters final
	{

	public:

		//Enumeration covering all axis.
		enum class Axis : uint8
		{
			X,
			Y,
			Z
		};

		//The output file path.
		const char *RESTRICT _Output;

		//The resource id.
		const char *RESTRICT _ID;

		//The low detail crown file path.
		const char *RESTRICT _LowCrownFile;

		//The medium detail crown file path.
		const char *RESTRICT _MediumCrownFile;

		//The high detail crown file path.
		const char *RESTRICT _HighCrownFile;

		//The low detail trunk file path.
		const char *RESTRICT _LowTrunkFile;

		//The medium detail trunk file path.
		const char *RESTRICT _MediumTrunkFile;

		//The high detail trunk file path.
		const char *RESTRICT _HighTrunkFile;

		//Denotes the up axis.
		Axis _UpAxis;

	};

	/*
	*	Creates a tree vegetation model resource file.
	*/
	static void CreateTreeVegetationModel(const TreeVegetationModelCreationParameters &parameters) noexcept
	{
		//What should the material be called?
		DynamicString fileName{ parameters._Output };
		fileName += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> file{ fileName.CString() };

		//Write the resource type to the file.
		constexpr ResourceType resourceType{ ResourceType::TreeVegetationModel };
		file.Write(&resourceType, sizeof(ResourceType));

		//Write the resource ID to the file.
		const HashString resourceID{ parameters._ID };
		file.Write(&resourceID, sizeof(HashString));

		//Load the models.
		for (uint8 i{ 0 }; i < 2; ++i)
		{
			for (uint8 j{ 0 }; j < 3; ++j)
			{
				const char *RESTRICT filePath;

				if (i == 0)
				{
					if (j == 0)
					{
						filePath = parameters._LowCrownFile;
					}

					else if (j == 1)
					{
						filePath = parameters._MediumCrownFile;
					}

					else
					{
						filePath = parameters._HighCrownFile;
					}
				}

				else
				{
					if (j == 0)
					{
						filePath = parameters._LowTrunkFile;
					}

					else if (j == 1)
					{
						filePath = parameters._MediumTrunkFile;
					}

					else
					{
						filePath = parameters._HighTrunkFile;
					}
				}

				Assimp::Importer modelImporter;
				const aiScene *modelScene = modelImporter.ReadFile(filePath, aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

				//Determine the height range.
				Vector2<float> heightRange{ FLOAT_MAXIMUM, -FLOAT_MAXIMUM };

				ProcessNode(modelScene->mRootNode, modelScene, parameters._UpAxis, &heightRange);

				//Process the vertices/indices.
				DynamicArray<VegetationVertex> vertices;
				DynamicArray<uint32> indices;
				float extent{ 0.0f };

				ProcessNode(modelScene->mRootNode, modelScene, parameters._UpAxis, heightRange, vertices, indices, extent);

				//Write the extent to the file.
				file.Write(&extent, sizeof(float));

				//Write the size of the vertices to the file.
				const uint64 sizeOfVertices{ vertices.Size() };
				file.Write(&sizeOfVertices, sizeof(uint64));

				//Write the vertices to the file.
				file.Write(vertices.Data(), sizeof(VegetationVertex) * sizeOfVertices);

				//Write the size of the indices to the file.
				const uint64 sizeOfIndices{ indices.Size() };
				file.Write(&sizeOfIndices, sizeof(uint64));

				//Write the vertices to the file.
				file.Write(indices.Data(), sizeof(uint32) * sizeOfIndices);
			}
		}
		
		//Close the file.
		file.Close();
	}

private:

	/*
	*	Processes a single Assimp mesh.
	*/
	static void ProcessMesh(aiMesh *RESTRICT mesh, const aiScene *RESTRICT scene, const TreeVegetationModelCreationParameters::Axis upAxis, Vector2<float> *const RESTRICT heightRange) NOEXCEPT
	{
		//Process the vertices.
		for (uint32 i = 0; i < mesh->mNumVertices; ++i)
		{
			float upValue;

			if (upAxis == TreeVegetationModelCreationParameters::Axis::X)
			{
				upValue = mesh->mVertices[i].x;
			}

			else if (upAxis == TreeVegetationModelCreationParameters::Axis::Y)
			{
				upValue = mesh->mVertices[i].y;
			}

			else
			{
				upValue = mesh->mVertices[i].z;
			}

			heightRange->_X = CatalystBaseMath::Minimum<float>(heightRange->_X, upValue);
			heightRange->_Y = CatalystBaseMath::Maximum<float>(heightRange->_Y, upValue);
		}
	}

	/*
	*	Processes a single Assimp node.
	*/
	static void ProcessNode(aiNode *RESTRICT node, const aiScene *RESTRICT scene, const TreeVegetationModelCreationParameters::Axis upAxis, Vector2<float> *const RESTRICT heightRange) NOEXCEPT
	{
		//Process all meshes.
		for (uint32 i = 0; i < node->mNumMeshes; ++i)
		{
			ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene, upAxis, heightRange);
		}

		//Process all nodes.
		for (uint32 i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene, upAxis, heightRange);
		}
	}

	/*
	*	Processes a single Assimp mesh.
	*/
	static void ProcessMesh(aiMesh *RESTRICT mesh, const aiScene *RESTRICT scene, const TreeVegetationModelCreationParameters::Axis upAxis, const Vector2<float> &heightRange, DynamicArray<VegetationVertex> &vertices, DynamicArray<uint32> &indices, float &extent) NOEXCEPT
	{
		//Process the vertices.
		for (uint32 i = 0; i < mesh->mNumVertices; ++i)
		{
			float upValue;

			if (upAxis == TreeVegetationModelCreationParameters::Axis::X)
			{
				upValue = mesh->mVertices[i].x;
			}

			else if (upAxis == TreeVegetationModelCreationParameters::Axis::Y)
			{
				upValue = mesh->mVertices[i].y;
			}

			else
			{
				upValue = mesh->mVertices[i].z;
			}

			const float modulatorFactor{ heightRange._Y - heightRange._X == 0.0f ? 0.0f : CatalystBaseMath::Scale(upValue, heightRange._X, heightRange._Y, 0.0f, 1.0f) };

			vertices.EmplaceSlow(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z, mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y, modulatorFactor);

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
	static void ProcessNode(aiNode *RESTRICT node, const aiScene *RESTRICT scene, const TreeVegetationModelCreationParameters::Axis upAxis, const Vector2<float> &heightRange, DynamicArray<VegetationVertex> &vertices, DynamicArray<uint32> &indices, float &extent) NOEXCEPT
	{
		//Process all meshes.
		for (uint32 i = 0; i < node->mNumMeshes; ++i)
		{
			ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene, upAxis, heightRange, vertices, indices, extent);
		}

		//Process all nodes.
		for (uint32 i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene, upAxis, heightRange, vertices, indices, extent);
		}
	}

};