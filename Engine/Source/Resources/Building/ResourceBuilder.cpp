//Header file.
#include <Resources/Building/ResourceBuilder.h>

//Core.
#include <Core/Containers/DynamicArray.h>
#include <Core/General/BinaryFile.h>
#include <Core/General/DynamicString.h>
#include <Core/General/HashString.h>

//Resources
#include <Resources/Building/AssimpBuilder.h>
#include <Resources/Core/ResourcesCore.h>

//Rendering.
#include <Rendering/Engine/Vertex.h>

/*
*	Builds a resource collection.
*/
void ResourceBuilder::BuildResourceCollection(const ResourceCollectionBuildParameters &parameters) NOEXCEPT
{
	//What should the collection be called?
	DynamicString fileName{ parameters._Output };
	fileName += ".crc";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ fileName.CString() };

	//Write the number of resources in the resource collection.
	const uint64 numberOfResources{ parameters._Resources.Size() };
	file.Write(&numberOfResources, sizeof(uint64));

	//Slam all resources into the resource collection.
	for (const char *RESTRICT resource : parameters._Resources)
	{
		//Open the resource file.
		BinaryFile<IOMode::In> resourceFile{ resource };

		//Get the size of the resource file.
		const uint64 resourceFileSize{ resourceFile.Size() };

		//Read the data in the resource file.
		void *RESTRICT resourceFileData = MemoryUtilities::AllocateMemory(resourceFileSize);
		resourceFile.Read(resourceFileData, resourceFileSize);

		//Write the resource file data to the resource collection file.
		file.Write(resourceFileData, resourceFileSize);

		//Free the resource file data.
		MemoryUtilities::FreeMemory(resourceFileData);

		//Close the resource file.
		resourceFile.Close();
	}

	//Close the file.
	file.Close();
}

/*
*	Builds a model.
*/
void ResourceBuilder::BuildModel(const ModelBuildParameters &parameters) NOEXCEPT
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

	//Build the model.
	DynamicArray<Vertex> vertices;
	DynamicArray<uint32> indices;
	float extent{ 0.0f };

	AssimpBuilder::BuildModel(parameters._File, &vertices, &indices, &extent);

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

	//Close the file.
	file.Close();
}