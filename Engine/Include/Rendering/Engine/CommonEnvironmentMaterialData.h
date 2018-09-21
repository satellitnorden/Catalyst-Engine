#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Resources.
#include <Resources/EnvironmentMaterialData.h>

namespace CommonEnvironmentMaterialData
{

	/*
	*	Generates day environment material data.
	*/
	static void GetDayEnvironmentMaterialData(EnvironmentMaterialData *const RESTRICT data) NOEXCEPT
	{
		constexpr Vector3 BOTTOM_COLOR{ 0.25f, 0.25f, 0.25f };
		constexpr Vector3 TOP_COLOR{ 0.0f, 0.75f, 1.0f };

		data->_DiffuseResolution = 2;
		data->_DiffuseData.Reserve(16 * 6);
		data->_DiffuseIrradianceResolution = 2;
		data->_DiffuseIrradianceData.Reserve(16 * 6);

		for (uint8 i = 0; i < 6; ++i)
		{
			if (i == 2)
			{
				data->_DiffuseData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);
			}

			else if (i == 3)
			{
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);
			}

			else
			{
				data->_DiffuseData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);
			}
		}
	}

	/*
	*	Generates night environment material data.
	*/
	static void GetNightEnvironmentMaterialData(EnvironmentMaterialData *const RESTRICT data) NOEXCEPT
	{
		constexpr Vector3 BOTTOM_COLOR{ 0.025f, 0.025f, 0.025f };
		constexpr Vector3 TOP_COLOR{ 0.0f, 0.75f / 64.0f, 1.0f / 64.0f };

		data->_DiffuseResolution = 2;
		data->_DiffuseData.Reserve(16 * 6);
		data->_DiffuseIrradianceResolution = 2;
		data->_DiffuseIrradianceData.Reserve(16 * 6);

		for (uint8 i = 0; i < 6; ++i)
		{
			if (i == 2)
			{
				data->_DiffuseData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);
			}

			else if (i == 3)
			{
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);
			}

			else
			{
				data->_DiffuseData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(TOP_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);

				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._X);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Y);
				data->_DiffuseIrradianceData.EmplaceFast(BOTTOM_COLOR._Z);
				data->_DiffuseIrradianceData.EmplaceFast(1.0f);
			}
		}
	}

}