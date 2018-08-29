#pragma once

//Core.
#include <Core/General/EngineCore.h>

//Math.
#include <Math/Vector2.h>
#include <Math/Vector3.h>

class PhysicalVertex final
{

public:

	//The position of this PhysicalVertex.
	Vector3 position;

	//The normal of this PhysicalVertex.
	Vector3 normal;

	//The tangent of this PhysicalVertex.
	Vector3 tangent;

	//The texture coordinate of this PhysicalVertex.
	Vector2 textureCoordinate;

	/*
	*	Default constructor - prohibited, must be constructed with the proper arguments.
	*/
	PhysicalVertex() NOEXCEPT = delete;

	/*
	*	Constructor taking the position, normal and the color of the PhysicalVertex.
	*/
	PhysicalVertex(const Vector3 &newPosition, const Vector3 &newNormal, const Vector3 &newTangent, const Vector2 &newTextureCoordinate) NOEXCEPT
		:
		position(newPosition),
		normal(newNormal),
		tangent(newTangent),
		textureCoordinate(newTextureCoordinate)
	{

	}

	/*
	*	Constructor taking the position and the color of the PhysicalVertex as separate values.
	*/
		PhysicalVertex(const float newPositionX, const float newPositionY, const float newPositionZ, const float newNormalX, const float newNormalY, const float newNormalZ, const float newTangentX, const float newTangentY, const float newTangentZ, const float newTextureCoordinateX, const float newTextureCoordinateY) NOEXCEPT
			:
			position(newPositionX, newPositionY, newPositionZ),
			normal(newNormalX, newNormalY, newNormalZ),
			tangent(newTangentX, newTangentY, newTangentZ),
			textureCoordinate(newTextureCoordinateX, newTextureCoordinateY)
		{

		}

};