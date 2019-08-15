#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Padding.h>

//Math.
#include <Math/General/Vector.h>

class AnimatedVertex final
{

public:

	//The position of this vertex.
	Vector3<float> _Position;

	//The normal of this vertex.
	Vector3<float> _Normal;

	//The tangent of this vertex.
	Vector3<float> _Tangent;

	//The joint indices of this vertex.
	Vector3<int32> _JointIndices;

	//The joint weights of this vertex.
	Vector3<float> _JointWeights;

	//The texture coordinate of this vertex.
	Vector2<float> _TextureCoordinate;

	//Some padding. (:
	Padding<4> _Padding;

	/*
	*	Default constructor - prohibited, must be constructed with the proper arguments.
	*/
	AnimatedVertex() NOEXCEPT = delete;

	/*
	*	Constructor taking the position, normal and the color of this vertex.
	*/
	AnimatedVertex(	const Vector3<float> &initial_position,
					const Vector3<float> &initial_normal,
					const Vector3<float> &initial_tangent,
					const Vector3<int32> &initial_joint_indices,
					const Vector3<float> &initial_joint_weights,
					const Vector2<float> &initial_texture_coordinate) NOEXCEPT
		:
		_Position(initial_position),
		_Normal(initial_normal),
		_Tangent(initial_tangent),
		_JointIndices(initial_joint_indices),
		_JointWeights(initial_joint_weights),
		_TextureCoordinate(initial_texture_coordinate)
	{

	}

};