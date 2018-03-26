#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Math.
#include <Math/Vector2.h>

//Rendering.
#include <Rendering/Engine Layer/CPUTexture4.h>

//Resources
#include <Resources/ResourcesCore.h>

//Third party libraries.
#include <stb_image.h>
#include <stb_image_resize.h>

//Environment material creator constants.
namespace EnvironmentMaterialCreatorConstants
{
	constexpr Vector2 INVERSE_ATAN{ 0.1591f, 0.3183f };
}

class EnvironmentMaterialCreator final
{

public:

	/*
	*	Creates an environment material resource file.
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
		const ResourceID resourceID{ std::strtoull(arguments[3], nullptr, 0) };
		file.Write(&resourceID, sizeof(ResourceID));

		//Load the texture.
		int32 width, height, numberOfChannels;
		float *const RESTRICT data{ stbi_loadf(arguments[4], &width, &height, &numberOfChannels, STBI_rgb_alpha) };

		//Wrap the data into a cpu texture for easier manipulation.
		CPUTexture4 hdrTexture{ static_cast<uint64>(width), static_cast<uint64>(height) };

		//Copy the data into the cpu texture.
		MemoryUtilities::CopyMemory(hdrTexture.Data(), data, width * height);

		//Get the resolution of the output texture(s).
		const uint64 outputResolution{ std::strtoull(arguments[5], nullptr, 0) };

		//Write the output resolution to the file.
		file.Write(&outputResolution, sizeof(uint64));

		//Create the albedo output textures.
		StaticArray<CPUTexture4, 6> albedoOutputTextures
		{
			CPUTexture4(outputResolution),
			CPUTexture4(outputResolution),
			CPUTexture4(outputResolution),
			CPUTexture4(outputResolution),
			CPUTexture4(outputResolution),
			CPUTexture4(outputResolution)
		};

		for (uint8 i = 0; i < 6; ++i)
		{
			for (uint64 j = 0; j < outputResolution; ++j)
			{
				for (uint64 k = 0; k < outputResolution; ++k)
				{
					Vector2 textureCoordinate{ static_cast<float>(j) / static_cast<float>(outputResolution), static_cast<float>(k) / static_cast<float>(outputResolution) };
					textureCoordinate *= EnvironmentMaterialCreatorConstants::INVERSE_ATAN;
					textureCoordinate += 0.5f;

					albedoOutputTextures[i].At(j, k) = hdrTexture.At(textureCoordinate);
				}
			}
		}
		
		//Write the albedo to the file.
		for (uint8 i = 0; i < 6; ++i)
		{
			file.Write(albedoOutputTextures[i].Data(), outputResolution * outputResolution);
		}

		//Close the file.
		file.Close();
	}

};