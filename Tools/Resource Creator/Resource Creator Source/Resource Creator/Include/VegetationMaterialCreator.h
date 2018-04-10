#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>
#include <Engine Core/HashString.h>

//Resources
#include <Resources/ResourcesCore.h>

//Third party libraries.
#include <stb_image.h>
#include <stb_image_resize.h>

class VegetationMaterialCreator final
{

public:

	/*
	*	Creates a vegetation material resource file.
	*/
	static void CreateVegetationMaterial(const int32 argumentCount, char *RESTRICT arguments[]) noexcept
	{
		//What should the material be called?
		DynamicString fileName{ arguments[2] };
		fileName += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> file{ fileName.CString() };

		//Write the resource type to the file.
		constexpr ResourceType resourceType{ ResourceType::VegetationMaterial };
		file.Write(&resourceType, sizeof(ResourceType));

		//Write the resource ID to the file.
		const HashString resourceID{ arguments[3] };
		file.Write(&resourceID, sizeof(HashString));

		//Load the albedo.
		int32 width, height, numberOfChannels;
		byte *RESTRICT data{ stbi_load(arguments[4], &width, &height, &numberOfChannels, STBI_rgb_alpha) };

		//Write the width and height of the albedo into the file, to be read into uint32's.
		file.Write(&width, sizeof(int32));
		file.Write(&height, sizeof(int32));

		//Write the albedo to the file.
		uint64 textureSize{ static_cast<uint64>(width * height * 4) };

		file.Write(data, textureSize);

		//Free the layer albedo data.
		stbi_image_free(data);

		//Close the file.
		file.Close();
	}

};