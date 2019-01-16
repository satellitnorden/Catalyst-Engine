#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector2.h>
#include <Math/Vector3.h>

class VegetationVertex final
{

public:

	//The position of this vegetation vertex.
	Vector3<float> _Position;

	//The normal of this vegetation vertex.
	Vector3<float> _Normal;

	//The tangent of this vegetation vertex.
	Vector3<float> _Tangent;

	//The texture coordinate of this vegetation vertex.
	Vector2<float> _TextureCoordinate;

	//The modulator factor of this vegetation vertex.
	float _ModulatorFactor;

	/*
	*	Default constructor - prohibited, must be constructed with the proper arguments.
	*/
	VegetationVertex() NOEXCEPT = delete;

	/*
	*	Constructor taking all values as arguments.
	*/
	VegetationVertex(	const Vector3<float> &initialPosition,
							const Vector3<float> &initialNormal,
							const Vector3<float> &initialTangent,
							const Vector2<float> &initialTextureCoordinate,
							const float initialModulatorFactor) NOEXCEPT
		:
		_Position(initialPosition),
		_Normal(initialNormal),
		_Tangent(initialTangent),
		_TextureCoordinate(initialTextureCoordinate),
		_ModulatorFactor(initialModulatorFactor)
	{

	}

	/*
	*	Constructor taking all values as separate arguments.
	*/
	VegetationVertex(	const float initialPositionX,
						const float initialPositionY,
						const float initialPositionZ,
						const float initialNormalX,
						const float initialNormalY,
						const float initialNormalZ,
						const float initialTangentX,
						const float initialTangentY,
						const float initialTangentZ,
						const float initialTextureCoordinateX,
						const float initialTextureCoordinateY,
						const float initialModulatorFactor) NOEXCEPT
		:
		_Position(initialPositionX, initialPositionY, initialPositionZ),
		_Normal(initialNormalX, initialNormalY, initialNormalZ),
		_Tangent(initialTangentX, initialTangentY, initialTangentZ),
		_TextureCoordinate(initialTextureCoordinateX, initialTextureCoordinateY),
		_ModulatorFactor(initialModulatorFactor)
	{

	}

};