#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

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
#if defined(CATALYST_FINAL)
	constexpr StaticArray<uint32, TERRAIN_QUAD_TREE_MAX_DEPTH + 1> TERRAIN_NORMAL_TEXTURE_RESOLUTIONS
	{
		static_cast<uint32>(512),
		static_cast<uint32>(256),
		static_cast<uint32>(128),
		static_cast<uint32>(64),
		static_cast<uint32>(64),
		static_cast<uint32>(64),
		static_cast<uint32>(64),
		static_cast<uint32>(64),
		static_cast<uint32>(64),
		static_cast<uint32>(64),
		static_cast<uint32>(64)
};
#else
	constexpr StaticArray<uint32, TERRAIN_QUAD_TREE_MAX_DEPTH + 1> TERRAIN_NORMAL_TEXTURE_RESOLUTIONS
	{
		static_cast<uint32>(64),
		static_cast<uint32>(64),
		static_cast<uint32>(64),
		static_cast<uint32>(64),
		static_cast<uint32>(64),
		static_cast<uint32>(64),
		static_cast<uint32>(64),
		static_cast<uint32>(64),
		static_cast<uint32>(64),
		static_cast<uint32>(64),
		static_cast<uint32>(64)
	};
#endif
#if defined(CATALYST_FINAL)
	constexpr StaticArray<uint32, TERRAIN_QUAD_TREE_MAX_DEPTH + 1> TERRAIN_MATERIAL_TEXTURE_RESOLUTIONS
	{
		static_cast<uint32>(1'024),
		static_cast<uint32>(512),
		static_cast<uint32>(256),
		static_cast<uint32>(128),
		static_cast<uint32>(64),
		static_cast<uint32>(32),
		static_cast<uint32>(32),
		static_cast<uint32>(32),
		static_cast<uint32>(32),
		static_cast<uint32>(32),
		static_cast<uint32>(32)
	};
#else
	constexpr StaticArray<uint32, TERRAIN_QUAD_TREE_MAX_DEPTH + 1> TERRAIN_MATERIAL_TEXTURE_RESOLUTIONS
	{
		static_cast<uint32>(32),
		static_cast<uint32>(32),
		static_cast<uint32>(32),
		static_cast<uint32>(32),
		static_cast<uint32>(32),
		static_cast<uint32>(32),
		static_cast<uint32>(32),
		static_cast<uint32>(32),
		static_cast<uint32>(32),
		static_cast<uint32>(32),
		static_cast<uint32>(32)
	};
#endif
	constexpr float TERRAIN_PATCH_SIZE{ 65'536.0f };
}

//Forward declarations.
class TerrainProperties;

//Type aliases.
using HeightFunction = void(*)(const TerrainProperties &properties, const Vector3<float> &worldPosition, float *const RESTRICT height);
using MaterialFunction = void(*)(const TerrainProperties &properties, const Vector3<float> &worldPosition, const float height, const Vector3<float> &normal, uint8 *const RESTRICT material);