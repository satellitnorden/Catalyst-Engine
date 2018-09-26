#if defined(CATALYST_ENABLE_OCEAN)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Resources.
#include <Resources/OceanMaterialData.h>

namespace CommonOceanMaterialData
{

	/*
	*	Generates ocean ocean material data.
	*/
	static void GetOceanOceanMaterialData(OceanMaterialData *const RESTRICT data) NOEXCEPT
	{
		constexpr uint8 RESOLUTION{ 64 };

		data->_MipmapLevels = 1;
		data->_Width = RESOLUTION;
		data->_Height = RESOLUTION;
		data->_NormalMapData.UpsizeSlow(1);
		data->_NormalMapData[0].Reserve(RESOLUTION * RESOLUTION * 4);

		for (uint16 i = 0; i < RESOLUTION * RESOLUTION; ++i)
		{
			data->_NormalMapData[0].EmplaceFast(static_cast<uint8>(CatalystBaseMath::RandomIntegerInRange<uint16>(126, 128)));
			data->_NormalMapData[0].EmplaceFast(static_cast<uint8>(CatalystBaseMath::RandomIntegerInRange<uint16>(126, 128)));
			data->_NormalMapData[0].EmplaceFast(255);
			data->_NormalMapData[0].EmplaceFast(255);
		}
	}

}
#endif