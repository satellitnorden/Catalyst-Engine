#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Data/PhysicalMaterialData.h>

class CommonPhysicalMaterialData final
{

public:

	/*
	*	Generates black physical material data.
	*/
	static void GetBlackPhysicalMaterialData(PhysicalMaterialData *const RESTRICT data) NOEXCEPT
	{
		data->_MipmapLevels = 1;
		data->_Width = 1;
		data->_Height = 1;
		data->_AlbedoData.UpsizeSlow(1);
		data->_AlbedoData[0].Reserve(4);
		data->_AlbedoData[0].EmplaceFast(0);
		data->_AlbedoData[0].EmplaceFast(0);
		data->_AlbedoData[0].EmplaceFast(0);
		data->_AlbedoData[0].EmplaceFast(255);
		data->_NormalMapData.UpsizeSlow(1);
		data->_NormalMapData[0].Reserve(4);
		data->_NormalMapData[0].EmplaceFast(127);
		data->_NormalMapData[0].EmplaceFast(127);
		data->_NormalMapData[0].EmplaceFast(255);
		data->_NormalMapData[0].EmplaceFast(255);
		data->_MaterialPropertiesData.UpsizeSlow(1);
		data->_MaterialPropertiesData[0].Reserve(4);
		data->_MaterialPropertiesData[0].EmplaceFast(DEFAULT_ROUGHNESS);
		data->_MaterialPropertiesData[0].EmplaceFast(DEFAULT_METALLIC);
		data->_MaterialPropertiesData[0].EmplaceFast(DEFAULT_AMBIENT_OCCLUSION);
		data->_MaterialPropertiesData[0].EmplaceFast(0);
	}

	/*
	*	Generates red physical material data.
	*/
	static void GetRedPhysicalMaterialData(PhysicalMaterialData *const RESTRICT data) NOEXCEPT
	{
		data->_MipmapLevels = 1;
		data->_Width = 1;
		data->_Height = 1;
		data->_AlbedoData.UpsizeSlow(1);
		data->_AlbedoData[0].Reserve(4);
		data->_AlbedoData[0].EmplaceFast(255);
		data->_AlbedoData[0].EmplaceFast(0);
		data->_AlbedoData[0].EmplaceFast(0);
		data->_AlbedoData[0].EmplaceFast(255);
		data->_NormalMapData.UpsizeSlow(1);
		data->_NormalMapData[0].Reserve(4);
		data->_NormalMapData[0].EmplaceFast(127);
		data->_NormalMapData[0].EmplaceFast(127);
		data->_NormalMapData[0].EmplaceFast(255);
		data->_NormalMapData[0].EmplaceFast(255);
		data->_MaterialPropertiesData.UpsizeSlow(1);
		data->_MaterialPropertiesData[0].Reserve(4);
		data->_MaterialPropertiesData[0].EmplaceFast(DEFAULT_ROUGHNESS);
		data->_MaterialPropertiesData[0].EmplaceFast(DEFAULT_METALLIC);
		data->_MaterialPropertiesData[0].EmplaceFast(DEFAULT_AMBIENT_OCCLUSION);
		data->_MaterialPropertiesData[0].EmplaceFast(0);
	}

	/*
	*	Generates teal physical material data.
	*/
	static void GetTealPhysicalMaterialData(PhysicalMaterialData *const RESTRICT data) NOEXCEPT
	{
		data->_MipmapLevels = 1;
		data->_Width = 1;
		data->_Height = 1;
		data->_AlbedoData.UpsizeSlow(1);
		data->_AlbedoData[0].Reserve(4);
		data->_AlbedoData[0].EmplaceFast(0);
		data->_AlbedoData[0].EmplaceFast(255);
		data->_AlbedoData[0].EmplaceFast(255);
		data->_AlbedoData[0].EmplaceFast(255);
		data->_NormalMapData.UpsizeSlow(1);
		data->_NormalMapData[0].Reserve(4);
		data->_NormalMapData[0].EmplaceFast(127);
		data->_NormalMapData[0].EmplaceFast(127);
		data->_NormalMapData[0].EmplaceFast(255);
		data->_NormalMapData[0].EmplaceFast(255);
		data->_MaterialPropertiesData.UpsizeSlow(1);
		data->_MaterialPropertiesData[0].Reserve(4);
		data->_MaterialPropertiesData[0].EmplaceFast(DEFAULT_ROUGHNESS);
		data->_MaterialPropertiesData[0].EmplaceFast(DEFAULT_METALLIC);
		data->_MaterialPropertiesData[0].EmplaceFast(DEFAULT_AMBIENT_OCCLUSION);
		data->_MaterialPropertiesData[0].EmplaceFast(0);
	}

private:

	//The default roughness.
	constexpr static byte DEFAULT_ROUGHNESS{ 255 };

	//The default metallic.
	constexpr static byte DEFAULT_METALLIC{ 0 };

	//The default ambient occlusion.
	constexpr static byte DEFAULT_AMBIENT_OCCLUSION{ 255 };

};