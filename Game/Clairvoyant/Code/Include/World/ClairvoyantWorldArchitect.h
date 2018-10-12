#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Pointers/UniquePointer.h>

//Forward declarations.
class AxisAlignedBoundingBox;
class Matrix4;
class Vector3;

class ClairvoyantWorldArchitect final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ClairvoyantWorldArchitect);

	/*
	*	Default constructor.
	*/
	ClairvoyantWorldArchitect() NOEXCEPT { }

	/*
	*	Initializes the Clairvoyant world architect.
	*/
	void Initialize() NOEXCEPT;

private:

	/*
	*	Initializes the particles.
	*/
	void InitializeParticles();

	/*
	*	Initializes the vegetation.
	*/
	void InitializeVegetation();

	/*
	*	Generates a transformation.
	*/
	bool GenerateTransformation(const bool underwater, const bool snow, const float height, const float dotModulator, const float minimumScale, const float maximumScale, const Vector3 &randomRotation, const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation) NOEXCEPT;

};