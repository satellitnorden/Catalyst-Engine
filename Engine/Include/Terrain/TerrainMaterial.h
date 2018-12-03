#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector4.h>

//Rendering.
#include <Rendering/Engine/CPUTexture2D.h>

class TerrainMaterial final
{

public:

	//The albedo.
	CPUTexture2D<Vector4<byte>> _Albedo;

	//The normal map.
	CPUTexture2D<Vector4<byte>> _NormalMap;

	//The material properties.
	CPUTexture2D<Vector4<byte>> _MaterialProperties;

};