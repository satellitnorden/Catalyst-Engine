#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/HashString.h>

//Resources
#include <Resources/ResourcesCore.h>

//Third party libraries.
#include <stb_image.h>

class TerrainMaterialCreator final
{

public:

	class TerrainMaterialCreationParameters final
	{

	public:

		//The output file path.
		const char *RESTRICT _Output;

		//The resource id.
		const char *RESTRICT _ID;

		//The albedo file path.
		const char *RESTRICT _AlbedoFile;

		//The normal map file path.
		const char *RESTRICT _NormalMapFile;

		//The roughness file path.
		const char *RESTRICT _RoughnessFile;

		//The metallic file path.
		const char *RESTRICT _MetallicFile;

		//The ambient occlusion file path.
		const char *RESTRICT _AmbientOcclusionFile;

		//The displacement file path.
		const char *RESTRICT _DisplacementFile;

	};

	/*
	*	Creates a terrain material resource file.
	*/
	static void CreateTerrainMaterial(const TerrainMaterialCreationParameters &parameters) noexcept
	{
		//What should the material be called?
		DynamicString fileName{ parameters._Output };
		fileName += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> file{ fileName.CString() };

		//Write the resource type to the file.
		constexpr ResourceType resourceType{ ResourceType::TerrainMaterial };
		file.Write(&resourceType, sizeof(ResourceType));

		//Write the resource ID to the file.
		const HashString resourceID{ parameters._ID };
		file.Write(&resourceID, sizeof(HashString));

		//Load the albedo.
		int32 width, height, numberOfChannels;
		byte *RESTRICT data{ stbi_load(parameters._AlbedoFile, &width, &height, &numberOfChannels, STBI_rgb_alpha) };

		const uint32 uWidth{ static_cast<uint32>(width) };
		const uint32 uHeight{ static_cast<uint32>(height) };

		//Write the width and height of the albedo into the file, to be read into uint32's.
		file.Write(&uWidth, sizeof(uint32));
		file.Write(&uHeight, sizeof(uint32));

		//Calculate the texture size.
		const uint64 textureSize{ uWidth * uHeight };
		const uint64 fullTextureSize{ uWidth * uHeight * 4 };

		//Write the albedo to the file.
		file.Write(data, fullTextureSize);

		//Free the layer albedo data.
		stbi_image_free(data);

		//Load the normal map.
		data = stbi_load(parameters._NormalMapFile, &width, &height, &numberOfChannels, STBI_rgb_alpha);

		//Write the normal map to the file.
		file.Write(data, fullTextureSize);

		//Free the normal map data.
		stbi_image_free(data);

		//Load the roughness, metallic, ambient occlusion and displacement data.
		byte *RESTRICT roughnessData = parameters._RoughnessFile ? stbi_load(parameters._RoughnessFile, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr;
		byte *RESTRICT metallicData = parameters._MetallicFile ? stbi_load(parameters._MetallicFile, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr;
		byte *RESTRICT ambientOcclusionData = parameters._AmbientOcclusionFile ? stbi_load(parameters._AmbientOcclusionFile, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr;
		byte *RESTRICT displacementData = parameters._DisplacementFile ? stbi_load(parameters._DisplacementFile, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr;

		//Write the roughness, metallic, ambient occlusion and Displacement data to the file.
		constexpr byte defaultRoughness{ 255 };
		constexpr byte defaultMetallic{ 0 };
		constexpr byte defaultAmbientOcclusion{ 255 };
		constexpr byte defaultDisplacement{ 255 };

		for (uint64 j = 0; j < textureSize; ++j)
		{
			file.Write(roughnessData ? &roughnessData[j * 4] : &defaultRoughness, sizeof(byte));
			file.Write(metallicData ? &metallicData[j * 4] : &defaultMetallic, sizeof(byte));
			file.Write(ambientOcclusionData ? &ambientOcclusionData[j * 4] : &defaultAmbientOcclusion, sizeof(byte));
			file.Write(displacementData ? &displacementData[j * 4] : &defaultDisplacement, sizeof(byte));
		}

		//Free the roughness, metallic, ambient occlusion and displacement data.
		stbi_image_free(roughnessData);
		stbi_image_free(metallicData);
		stbi_image_free(ambientOcclusionData);
		stbi_image_free(displacementData);

		//Close the file.
		file.Close();
	}

};