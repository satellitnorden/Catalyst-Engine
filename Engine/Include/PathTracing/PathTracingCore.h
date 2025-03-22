#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/Any.h>

//Math.
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Native/Vertex.h>

//Path Tracing constants.
namespace PathTracingConstants
{
	constexpr uint64 USER_DATA_SIZE{ 16 };
}

/*
*	Path tracing shading context class definition.
*/
class PathTracingShadingContext final
{

public:

	//The world position.
	Vector3<float32> _WorldPosition;

	//The geometry normal.
	Vector3<float32> _GeometryNormal;

	//The geometry tangent.
	Vector3<float32> _GeometryTangent;

	//The texture coordinate.
	Vector2<float32> _TextureCoordinate;

	//The user data.
	Any<PathTracingConstants::USER_DATA_SIZE> _UserData;

};

/*
*	Path tracing shading result class definition.
*/
class PathTracingShadingResult final
{

public:

	//The albedo.
	Vector3<float32> _Albedo;

	//The shading normal.
	Vector3<float32> _ShadingNormal;

	//The roughness.
	float32 _Roughness;

	//The metallic.
	float32 _Metallic;

	//The ambient occlusion.
	float32 _AmbientOcclusion;

	//The emissive.
	float32 _Emissive;

	//The thickness.
	float32 _Thickness;

};

/*
*	Path Tracing triangle class definition.
*	Holds indices into a vertex buffer, as well as some additional data to control how this triangle is rendered.
*/
class PathTracingTriangle final
{

public:

	//Type aliases.
	using DiscardFunction = bool(*)(const PathTracingShadingContext &context);
	using ShadingFunction = void(*)(const PathTracingShadingContext &context, PathTracingShadingResult *const RESTRICT result);

	//The indices.
	StaticArray<uint64, 3> _Indices;

	//The discard function. Can be nullptr if this triangle doesn't need one.
	DiscardFunction _DiscardFunction;

	//The shading function.
	ShadingFunction _ShadingFunction;

	//The user data.
	Any<PathTracingConstants::USER_DATA_SIZE> _UserData;

};