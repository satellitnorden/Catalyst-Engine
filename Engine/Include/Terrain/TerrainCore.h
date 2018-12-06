#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector3.h>
#include <Math/Vector4.h>

//Terrain constants.
namespace TerrainConstants
{
	constexpr uint8 TERRAIN_QUAD_TREE_MAX_DEPTH{ 10 };
	constexpr uint32 TERRAIN_PATCH_RESOLUTION{ 65 };
	constexpr uint32 TERRAIN_HEIGHT_TEXTURE_RESOLUTION{ TERRAIN_PATCH_RESOLUTION };
#if defined(CATALYST_FINAL)
	constexpr uint32 TERRAIN_NORMAL_TEXTURE_RESOLUTION{ 256 };
#else
	constexpr uint32 TERRAIN_NORMAL_TEXTURE_RESOLUTION{ 64 };
#endif
	constexpr uint32 TERRAIN_MATERIAL_TEXTURE_RESOLUTION{ 64 };
	constexpr float TERRAIN_PATCH_SIZE{ 65'536.0f };
}

//Forward declarations.
class TerrainProperties;

//Type aliases.
using HeightFunction = void(*)(const TerrainProperties &properties, const Vector3<float> &worldPosition, float *const RESTRICT height);
using MaterialFunction = void(*)(const TerrainProperties &properties, const Vector3<float> &worldPosition, const float height, const Vector3<float> &normal, uint8 *const RESTRICT material);