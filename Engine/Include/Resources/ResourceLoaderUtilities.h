#pragma once

//Core.
#include <Core/EngineCore.h>

namespace ResourceLoaderUtilities
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
		layerAlbedoData.UpsizeSlow(mipmapLevels);

		for (uint8 i = 0; i < mipmapLevels; ++i)
		{
			const uint64 textureSize{ (layerWidth >> i) * (layerHeight >> i) * 4 };

			layerAlbedoData[i].Reserve(textureSize);

			file.Read(layerAlbedoData[i].Data(), textureSize);
		}

		//Read the layer normal map.
		layerNormalMapData.UpsizeSlow(mipmapLevels);

		for (uint8 i = 0; i < mipmapLevels; ++i)
		{
			const uint64 textureSize{ (layerWidth >> i) * (layerHeight >> i) * 4 };

			layerNormalMapData[i].Reserve(textureSize);

			file.Read(layerNormalMapData[i].Data(), textureSize);
		}

		//Read the layer material properties.
		layerMaterialPropertiesData.UpsizeSlow(mipmapLevels);

		for (uint8 i = 0; i < mipmapLevels; ++i)
		{
			const uint64 textureSize{ (layerWidth >> i) * (layerHeight >> i) * 4 };

			layerMaterialPropertiesData[i].Reserve(textureSize);

			file.Read(layerMaterialPropertiesData[i].Data(), textureSize);
		}
	}
}