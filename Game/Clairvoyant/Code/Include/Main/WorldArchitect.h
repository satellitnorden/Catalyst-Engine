#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Pointers/UniquePointer.h>

//Forward declarations.
class AxisAlignedBoundingBox;
class Matrix4;
class Vector3;

class WorldArchitect final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(WorldArchitect);

	/*
	*	Default constructor.
	*/
	WorldArchitect() NOEXCEPT { }

	/*
	*	Initializes the world architect.
	*/
	void Initialize() NOEXCEPT;

private:

	/*
	*	Initializes the vegetation.
	*/
	void InitializeVegetation();

	/*
	*	Generates a transformation.
	*/
	bool GenerateTransformation(const bool underwater, const float height, const float dotModulator, const float minimumScale, const float maximumScale, const Vector3 &randomRotation, const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation) NOEXCEPT;

};