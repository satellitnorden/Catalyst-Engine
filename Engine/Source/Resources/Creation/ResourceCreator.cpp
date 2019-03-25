//Header file.
#include <Resources/Creation/ResourceCreator.h>

//Core.
#include <Core/Containers/DynamicArray.h>
#include <Core/General/BinaryFile.h>
#include <Core/General/DynamicString.h>
#include <Core/General/HashString.h>

//Resources
#include <Resources/Core/ResourcesCore.h>

//Rendering.
#include <Rendering/Engine/PhysicalVertex.h>

//Third party.
#include <ThirdParty/assimp/Importer.hpp>
#include <ThirdParty/assimp/scene.h>
#include <ThirdParty/assimp/postprocess.h>

/*
*	Creates a model.
*/
void ResourceCreator::CreateModel(const ModelCreationParameters &parameters) NOEXCEPT
{
	//What should the material be called?
	DynamicString fileName{ parameters._Output };
	fileName += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ fileName.CString() };

	//Write the resource type to the file.
	constexpr uint8 resourceType{ static_cast<uint8>(ResourceType::Model) };
	file.Write(&resourceType, sizeof(ResourceType));

	//Write the resource ID to the file.
	const HashString resourceID{ parameters._ID };
	file.Write(&resourceID, sizeof(HashString));

	DynamicArray<PhysicalVertex> vertices;
	DynamicArray<uint32> indices;
	float extent{ 0.0f };

	Assimp::Importer modelImporter;
	const aiScene *modelScene = modelImporter.ReadFile(parameters._File, aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

	//ProcessNode(modelScene->mRootNode, modelScene, vertices, indices, extent);

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