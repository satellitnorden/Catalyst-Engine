#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class TreeVegetationMaterialData final
{

public:

	//The number of mipmap levels for the crown mask texture.
	uint8 _CrownMaskMipmapLevels;

	//The crown mask width.
	uint32 _CrownMaskWidth;

	//The crown mask height.
	uint32 _CrownMaskHeight;

	//The crown mask data.
	DynamicArray<DynamicArray<byte>> _CrownMaskData;

	//The number of mipmap levels for the crown.
	uint8 _CrownMipmapLevels;

	//The crown width.
	uint32 _CrownWidth;

	//The crown height.
	uint32 _CrownHeight;

	//The crown albedo data.
	DynamicArray<DynamicArray<byte>> _CrownAlbedoData;

	//The crown normal map data.
	DynamicArray<DynamicArray<byte>> _CrownNormalMapData;

	//The number of mipmap levels for the impostor mask texture.
	uint8 _ImpostorMaskMipmapLevels;

	//The impostor mask width.
	uint32 _ImpostorMaskWidth;

	//The impostor mask height.
	uint32 _ImpostorMaskHeight;

	//The impostor mask data.
	DynamicArray<DynamicArray<byte>> _ImpostorMaskData;

	//The number of mipmap levels for the impostor.
	uint8 _ImpostorMipmapLevels;

	//The impostor width.
	uint32 _ImpostorWidth;

	//The impostor height.
	uint32 _ImpostorHeight;

	//The impostor albedo data.
	DynamicArray<DynamicArray<byte>> _ImpostorAlbedoData;

	//The number of mipmap levels for the trunk.
	uint8 _TrunkMipmapLevels;

	//The trunk width.
	uint32 _TrunkWidth;

	//The trunk height.
	uint32 _TrunkHeight;

	//The trunk albedo data.
	DynamicArray<DynamicArray<byte>> _TrunkAlbedoData;

	//The trunk normal map data.
	DynamicArray<DynamicArray<byte>> _TrunkNormalMapData;

	//The trunk material properties data.
	DynamicArray<DynamicArray<byte>> _TrunkMaterialPropertiesData;

};