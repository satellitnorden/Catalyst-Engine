#pragma once

//Core.
#include <Core/EngineCore.h>

//Resources
#include <Resources/ResourcesCore.h>

class ResourceCollectionCreator final
{

public:

	/*
	*	Creates a resource collection file.
	*/
	static void CreateResourceCollection(const int32 argumentCount, const char *const RESTRICT arguments[]) noexcept
	{
		//What should the collection be called?
		DynamicString fileName{ arguments[0] };
		fileName += ".crc";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> resourceCollectionFile{ fileName.CString() };

		//Write the number of resources in the resource collection.
		const uint64 numberOfResources{ static_cast<uint64>(argumentCount - 1) };
		resourceCollectionFile.Write(&numberOfResources, sizeof(uint64));

		//Slam all resources into the resource collection.
		for (int32 i = 1; i < argumentCount; ++i)
		{
			//Open the resource file.
			BinaryFile<IOMode::In> resourceFile{ arguments[i] };

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

};