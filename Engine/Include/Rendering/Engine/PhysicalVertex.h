#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector2.h>
#include <Math/General/Vector3.h>

class PhysicalVertex final
{

public:

	//The position of this physical vertex.
	Vector3<float> _Position;

	//The normal of this physical vertex.
	Vector3<float> _Normal;

	//The tangent of this physical vertex.
	Vector3<float> _Tangent;

	//The texture coordinate of this physical vertex.
	Vector2<float> _TextureCoordinate;

	/*
	*	Default constructor - prohibited, must be constructed with the proper arguments.
	*/
	PhysicalVertex() NOEXCEPT = delete;

	/*
	*	Constructor taking the position, normal and the color of the physical vertex.
	*/
	PhysicalVertex(const Vector3<float> &newPosition, const Vector3<float> &newNormal, const Vector3<float> &newTangent, const Vector2<float> &newTextureCoordinate) NOEXCEPT
		:
		_Position(newPosition),
		_Normal(newNormal),
		_Tangent(newTangent),
		_TextureCoordinate(newTextureCoordinate)
	{

	}

	/*
	*	Constructor taking the position and the color of the physical vertex as separate values.
	*/
		PhysicalVertex(const float newPositionX, const float newPositionY, const float newPositionZ, const float newNormalX, const float newNormalY, const float newNormalZ, const float newTangentX, const float newTangentY, const float newTangentZ, const float newTextureCoordinateX, const float newTextureCoordinateY) NOEXCEPT
			:
			_Position(newPositionX, newPositionY, newPositionZ),
			_Normal(newNormalX, newNormalY, newNormalZ),
			_Tangent(newTangentX, newTangentY, newTangentZ),
			_TextureCoordinate(newTextureCoordinateX, newTextureCoordinateY)
		{

		}

};