//Header file.
#include <PhysicalVertex.h>

/*
*	Constructor taking the position and the color of the PhysicalVertex.
*/
PhysicalVertex::PhysicalVertex(const Vector3 &newPosition, const Vector3 &newNormal, const Vector3 &newTangent, const Vector2 &newTextureCoordinate) NOEXCEPT
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
PhysicalVertex::PhysicalVertex(const float newPositionX, const float newPositionY, const float newPositionZ, const float newNormalX, const float newNormalY, const float newNormalZ, const float newTangentX, const float newTangentY, const float newTangentZ, const float newTextureCoordinateX, const float newTextureCoordinateY) NOEXCEPT
	:
	position(newPositionX, newPositionY, newPositionZ),
	normal(newNormalX, newNormalY, newNormalZ),
	tangent(newTangentX, newTangentY, newTangentZ),
	textureCoordinate(newTextureCoordinateX, newTextureCoordinateY)
{

}