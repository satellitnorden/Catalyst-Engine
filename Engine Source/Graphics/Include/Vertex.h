#pragma once

//Engine core.
#include <EngineCore.h>

//Math.
#include <Vector2.h>
#include <Vector3.h>

class Vertex final
{

public:

	//The position of this Vertex.
	Vector3 position;

	//The normal of this Vertex.
	Vector3 normal;

	//The tangent of this Vertex.
	Vector3 tangent;

	//The texture coordinate of this Vertex.
	Vector2 textureCoordinate;

	/*
	*	Default constructor - prohibited, must be constructed with the proper arguments.
	*/
	Vertex() NOEXCEPT = delete;

	/*
	*	Constructor taking the position, normal and the color of the Vertex.
	*/
	Vertex(const Vector3 &newPosition, const Vector3 &newNormal, const Vector3 &newTangent, const Vector2 &newTextureCoordinate) NOEXCEPT;

	/*
	*	Constructor taking the position and the color of the Vertex as separate values.
	*/
	Vertex(const float newPositionX, const float newPositionY, const float newPositionZ, const float newNormalX, const float newNormalY, const float newNormalZ, const float newTangentX, const float newTangentY, const float newTangentZ, const float newTextureCoordinateX, const float newTextureCoordinateY) NOEXCEPT;

};