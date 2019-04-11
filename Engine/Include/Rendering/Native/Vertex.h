#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Padding.h>

//Math.
#include <Math/General/Vector.h>
#include <Math/General/Matrix.h>

class Vertex final
{

public:

	//The position of this vertex.
	Vector3<float> _Position;

	//The normal of this vertex.
	Vector3<float> _Normal;

	//The tangent of this vertex.
	Vector3<float> _Tangent;

	//The texture coordinate of this vertex.
	Vector2<float> _TextureCoordinate;

	//Some padding. (:
	Padding<4> _Padding;

	/*
	*	Default constructor - prohibited, must be constructed with the proper arguments.
	*/
	Vertex() NOEXCEPT = delete;

	/*
	*	Constructor taking the position, normal and the color of this vertex.
	*/
	Vertex(	const Vector3<float> &newPosition,
			const Vector3<float> &newNormal,
			const Vector3<float> &newTangent,
			const Vector2<float> &newTextureCoordinate) NOEXCEPT
		:
		_Position(newPosition),
		_Normal(newNormal),
		_Tangent(newTangent),
		_TextureCoordinate(newTextureCoordinate)
	{

	}

	/*
	*	Constructor taking the position and the color of this vertex as separate values.
	*/
	Vertex(	const float newPositionX,
			const float newPositionY,
			const float newPositionZ,
			const float newNormalX,
			const float newNormalY,
			const float newNormalZ,
			const float newTangentX,
			const float newTangentY,
			const float newTangentZ,
			const float newTextureCoordinateX,
			const float newTextureCoordinateY) NOEXCEPT
		:
		_Position(newPositionX, newPositionY, newPositionZ),
		_Normal(newNormalX, newNormalY, newNormalZ),
		_Tangent(newTangentX, newTangentY, newTangentZ),
		_TextureCoordinate(newTextureCoordinateX, newTextureCoordinateY)
	{

	}

	/*
	*	Transforms this vertex.
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