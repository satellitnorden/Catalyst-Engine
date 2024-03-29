#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Padding.h>

//Math.
#include <Math/General/Matrix.h>

class AnimatedVertex final
{

public:

	//The position of this vertex.
	Vector3<float32> _Position;

	//The normal of this vertex.
	Vector3<float32> _Normal;

	//The tangent of this vertex.
	Vector3<float32> _Tangent;

	//The bone indices of this vertex.
	Vector4<uint32> _BoneIndices;

	//The bone weights of this vertex.
	Vector4<float32> _BoneWeights;

	//The texture coordinate of this vertex.
	Vector2<float32> _TextureCoordinate;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE AnimatedVertex() NOEXCEPT
	{

	}

	/*
	*	Constructor taking the position, normal and the color of this vertex.
	*/
	FORCE_INLINE AnimatedVertex(const Vector3<float32> &initial_position,
								const Vector3<float32> &initial_normal,
								const Vector3<float32> &initial_tangent,
								const Vector3<uint32> &initial_bone_indices,
								const Vector3<float32> &initial_bone_weights,
								const Vector2<float32> &initial_texture_coordinate) NOEXCEPT
		:
		_Position(initial_position),
		_Normal(initial_normal),
		_Tangent(initial_tangent),
		_BoneIndices(initial_bone_indices),
		_BoneWeights(initial_bone_weights),
		_TextureCoordinate(initial_texture_coordinate)
	{

	}

	/*
	*	Transforms this animated vertex.
	*/
	void Transform(const Matrix4x4 &transformation, const float32 texture_coordinate_rotation) NOEXCEPT;

};