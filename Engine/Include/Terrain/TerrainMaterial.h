#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Padding.h>

//Math.
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Engine/CPUTexture2D.h>

class TerrainMaterial final
{

public:

	//The albedo texture index.
	int32 _AlbedoTextureIndex;

	//The normal map texture index.
	int32 _NormalMapTextureIndex;

	//The material properties texture index.
	int32 _MaterialPropertiesTextureIndex;

	//The texture scaling factor.
	float _TextureScalingFactor;

};