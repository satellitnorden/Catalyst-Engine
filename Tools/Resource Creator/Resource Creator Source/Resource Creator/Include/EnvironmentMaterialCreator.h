#pragma once

//Engine core.
#include <EngineCore.h>

//Resources
#include <ResourcesCore.h>

//Systems.
#include <RenderingSystem.h>

//Third party libraries.
#include <stb_image.h>
//#include <stb_image_resize.h>

class EnvironmentMaterialCreator final
{

public:

	/*
	*	Creates a environment material resource file.
	*/
	static void CreateEnvironmentMaterial(const int32 argumentCount, char *RESTRICT arguments[]) noexcept
	{
		//What should the material be called?
		DynamicString fileName{ arguments[2] };
		fileName += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> file{ fileName.CString() };

		//Write the resource type to the file.
		constexpr ResourceType resourceType{ ResourceType::EnvironmentMaterial };
		file.Write(&resourceType, sizeof(ResourceType));

		//Write the resource ID to the file.
		const uint64 resourceID{ std::strtoull(arguments[3], nullptr, 0) };
		file.Write(&resourceID, sizeof(uint64));

		//Load the albedo.
		int32 width, height, numberOfChannels;

		float *const RESTRICT data{ stbi_loadf(arguments[4], &width, &height, &numberOfChannels, 0) };

		//Construct the environment material.
		DynamicArray<float> diffuseData;
		DynamicArray<float> diffuseIrradianceData;

		RenderingSystem::Instance->ConstructEnvironmentMaterial(data, width, height, numberOfChannels, diffuseData, diffuseIrradianceData);

		//Free the texture data.
		stbi_image_free(data);

		//Close the file.
		file.Close();
	}

};