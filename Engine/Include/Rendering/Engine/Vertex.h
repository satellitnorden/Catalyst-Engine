#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

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

};