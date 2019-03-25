#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//Resources.
#include <Resources/Data/ParticleMaterialData.h>

namespace CommonParticleMaterialData
{

	/*
	*	Generates white circle particle material data.
	*/
	static void GetWhiteCircleParticleMaterialData(ParticleMaterialData *const RESTRICT data) NOEXCEPT
	{
		constexpr uint8 RESOLUTION{ 32 };
		constexpr Vector2<float> MIDDLE_POINT{ 0.5f, 0.5f };

		data->_MipmapLevels = 1;
		data->_Width = RESOLUTION;
		data->_Height = RESOLUTION;

		data->_AlbedoData.UpsizeSlow(1);
		data->_AlbedoData[0].Reserve(RESOLUTION * RESOLUTION * 4);

		for (uint8 i = 0; i < RESOLUTION; ++i)
		{
			for (uint8 j = 0; j < RESOLUTION; ++j)
			{
				const Vector2<float> point{ static_cast<float>(i) / static_cast<float>(RESOLUTION - 1), static_cast<float>(j) / static_cast<float>(RESOLUTION - 1) };
				const float distance{ Vector2<float>::Length(MIDDLE_POINT - point) * 2.0f };
				const float alpha{ CatalystBaseMath::SmoothStep<2>(CatalystBaseMath::Maximum<float>(1.0f - distance, 0.0f)) };

				data->_AlbedoData[0].EmplaceFast(255);
				data->_AlbedoData[0].EmplaceFast(255);
				data->_AlbedoData[0].EmplaceFast(255);
				data->_AlbedoData[0].EmplaceFast(static_cast<byte>(static_cast<float>(255) * alpha));
			}
		}
	}
}