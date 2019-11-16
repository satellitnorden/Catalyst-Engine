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
	Vector3<float> _Position;

	//The normal of this vertex.
	Vector3<float> _Normal;

	//The tangent of this vertex.
	Vector3<float> _Tangent;

	//The bone indices of this vertex.
	Vector3<uint32> _BoneIndices;

	//The bone weights of this vertex.
	Vector3<float> _BoneWeights;

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
					const Vector3<uint32> &initial_bone_indices,
					const Vector3<float> &initial_bone_weights,
					const Vector2<float> &initial_texture_coordinate) NOEXCEPT
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
	FORCE_INLINE constexpr void Transform(const Matrix4 &transformation, const float textureCoordinateRotation) NOEXCEPT
	{
		//Transform the position.
		const Vector4<float> transformedPosition{ transformation * Vector4<float>(_Position, 1.0f) };

		_Position._X = transformedPosition._X;
		_Position._Y = transformedPosition._Y;
		_Position._Z = transformedPosition._Z;

		//Transform the normal.
		const Vector4<float> transformedNormal{ transformation * Vector4<float>(_Normal, 0.0f) };

		_Normal._X = transformedNormal._X;
		_Normal._Y = transformedNormal._Y;
		_Normal._Z = transformedNormal._Z;

		//Transform the tangent.
		const Vector4<float> transformedTangent{ transformation * Vector4<float>(_Tangent, 0.0f) };

		_Tangent._X = transformedTangent._X;
		_Tangent._Y = transformedTangent._Y;
		_Tangent._Z = transformedTangent._Z;

		//Rotate the texture coordinate.
		_TextureCoordinate.Rotate(textureCoordinateRotation);
	}

};