#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Resources.
#include <Resources/Data/OceanMaterialData.h>

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
		data->_NormalData.UpsizeSlow(1);
		data->_NormalData[0].Reserve(RESOLUTION * RESOLUTION * 4);

		for (uint16 i = 0; i < RESOLUTION * RESOLUTION; ++i)
		{
			data->_NormalData[0].EmplaceFast(static_cast<uint8>(CatalystRandomMath::RandomIntegerInRange<uint16>(126, 128)));
			data->_NormalData[0].EmplaceFast(static_cast<uint8>(CatalystRandomMath::RandomIntegerInRange<uint16>(126, 128)));
			data->_NormalData[0].EmplaceFast(255);
			data->_NormalData[0].EmplaceFast(255);
		}

		data->_FoamData.UpsizeSlow(1);
		data->_FoamData[0].Reserve(RESOLUTION * RESOLUTION * 4);

		for (uint16 i = 0; i < RESOLUTION * RESOLUTION; ++i)
		{
			data->_FoamData[0].EmplaceFast(255);
			data->_FoamData[0].EmplaceFast(255);
			data->_FoamData[0].EmplaceFast(255);
			data->_FoamData[0].EmplaceFast(255);
		}
	}

}