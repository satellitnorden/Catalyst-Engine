#pragma once

//Engine core.
#include <EngineCore.h>

namespace AssetLoaderUtilities
{

	/*
	*	Loads a single terrain layer data.
	*/
	void LoadTerrainLayerData(BinaryFile<IOMode::In> &file, uint32 &layerWidth, uint32 &layerHeight, const uint8 mipmapLevels, DynamicArray<DynamicArray<byte>> &layerAlbedoData, DynamicArray<DynamicArray<byte>> &layerNormalMapData, DynamicArray<DynamicArray<byte>> &layerMaterialPropertiesData) NOEXCEPT
	{
		//Read the width of the layer.
		file.Read(&layerWidth, sizeof(uint32));

		//Read the height of the layer.
		file.Read(&layerHeight, sizeof(uint32));

		//Read the layer albedo.
		layerAlbedoData.Resize(mipmapLevels);

		const uint64 textureSize{ layerWidth * layerHeight * 4 };

		for (uint8 i = 0; i < mipmapLevels; ++i)
		{
			layerAlbedoData[i].Reserve(textureSize >> i);

			file.Read(layerAlbedoData[i].Data(), textureSize >> i);
		}

		//Read the layer normal map.
		layerNormalMapData.Resize(mipmapLevels);

		for (uint8 i = 0; i < mipmapLevels; ++i)
		{
			layerNormalMapData[i].Reserve(textureSize >> i);

			file.Read(layerNormalMapData[i].Data(), textureSize >> i);
		}

		//Read the layer material properties.
		layerMaterialPropertiesData.Resize(mipmapLevels);

		for (uint8 i = 0; i < mipmapLevels; ++i)
		{
			layerMaterialPropertiesData[i].Reserve(textureSize >> i);

			file.Read(layerMaterialPropertiesData[i].Data(), textureSize >> i);
		}
	}
}