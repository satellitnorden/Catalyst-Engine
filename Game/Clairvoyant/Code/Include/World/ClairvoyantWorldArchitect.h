#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/Updateable.h>
#include <Core/Pointers/UniquePointer.h>

//Clairvoyant.
#include <World/EnvironmentParameters.h>

//Forward declarations.
class AxisAlignedBoundingBox;
class Matrix4;

class ClairvoyantWorldArchitect final : public Updateable
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

	/*
	*	Updates the Clairvoyant world architect asynchronously during the logic update phase.
	*/
	bool LogicUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT override;

private:

	//Enumeration covering all environment phases.
	enum class EnvironmentPhase : uint8
	{
		Night,
		Morning,
		Day,
		Evening,

		NumberOfEnvironmentPhases
	};

	//The current time.
	float _CurrentTime{ 0.0f };

	//Container for all environment parameters.
	StaticArray<EnvironmentParameters, UNDERLYING(EnvironmentPhase::NumberOfEnvironmentPhases)> _EnvironmentParameters;

	/*
	*	Initializes the environment parameters.
	*/
	void InitializeEnvironmentParameters() NOEXCEPT;

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
	bool GenerateTransformation(const bool underwater, const bool snow, const bool onlySnow, const float height, const float dotModulator, const float minimumScale, const float maximumScale, const Vector3 &randomRotation, const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation) NOEXCEPT;

	/*
	*	Updates the environment.
	*/
	void UpdateEnvironment() NOEXCEPT;

	/*
	*	Blends between two environment parameters.
	*/
	void BlendEnvironmentParameters(const EnvironmentParameters &first, const EnvironmentParameters &second, const float alpha) NOEXCEPT;

};