#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/BinaryFile.h>
#include <Core/General/DynamicString.h>

//Resources
#include <Resources/ResourcesCore.h>

namespace ResourceCollectionCreator
{

	class ResourceCollectionCreationParameters final
	{

	public:

		//The output file path.
		const char *RESTRICT _Output;

		//File paths to all resources to put in the resource collection.
		DynamicArray<const char *RESTRICT> _Resources;

	};


	/*
	*	Creates a resource collection file.
	*/
	static void CreateResourceCollection(const ResourceCollectionCreationParameters &parameters) noexcept
	{
		//What should the collection be called?
		DynamicString fileName{ parameters._Output };
		fileName += ".crc";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> resourceCollectionFile{ fileName.CString() };

		//Write the number of resources in the resource collection.
		const uint64 numberOfResources{ parameters._Resources.Size() };
		resourceCollectionFile.Write(&numberOfResources, sizeof(uint64));

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
			resourceCollectionFile.Write(resourceFileData, resourceFileSize);

			//Free the resource file data.
			MemoryUtilities::FreeMemory(resourceFileData);

			//Close the resource file.
			resourceFile.Close();
		}

		//Close the file.
		resourceCollectionFile.Close();
	}

}