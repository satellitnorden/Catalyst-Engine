//Header file.
#include <Vertex.h>

/*
*	Constructor taking the position and the color of the Vertex.
*/
Vertex::Vertex(const Vector3 &newPosition, const Vector3 &newNormal, const Vector3 &newTangent, const Vector2 &newTextureCoordinate) CATALYST_NOEXCEPT
	:
	position(newPosition),
	normal(newNormal),
	tangent(newTangent),
	textureCoordinate(newTextureCoordinate)
{

}

/*
*	Constructor taking the position and the color of the Vertex as separate values.
*/
Vertex::Vertex(const float newPositionX, const float newPositionY, const float newPositionZ, const float newNormalX, const float newNormalY, const float newNormalZ, const float newTangentX, const float newTangentY, const float newTangentZ, const float newTextureCoordinateX, const float newTextureCoordinateY) CATALYST_NOEXCEPT
	:
	position(newPositionX, newPositionY, newPositionZ),
	normal(newNormalX, newNormalY, newNormalZ),
	tangent(newTangentX, newTangentY, newTangentZ),
	textureCoordinate(newTextureCoordinateX, newTextureCoordinateY)
{

}