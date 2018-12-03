#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector2.h>

//Resources.
#include <Resources/ParticleMaterialData.h>

namespace CommonParticleMaterialData
{

	/*
	*	Generates white circle particle material data.
	*/
	static void GetWhiteCircleParticleMaterialData(ParticleMaterialData *const RESTRICT data) NOEXCEPT
	{
		constexpr uint8 RESOLUTION{ 7 };
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
				const float distance{ Vector2<float>::LengthSquared(MIDDLE_POINT - point) * 2.0f };
				float alpha{ 1.0f - distance };
				alpha *= alpha;
				alpha *= alpha;

				data->_AlbedoData[0].EmplaceFast(255);
				data->_AlbedoData[0].EmplaceFast(255);
				data->_AlbedoData[0].EmplaceFast(255);
				data->_AlbedoData[0].EmplaceFast(static_cast<byte>(static_cast<float>(255) * alpha));
			}
		}
	}
}