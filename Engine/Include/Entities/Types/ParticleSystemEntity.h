#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entity.
#include <Entities/Types/Entity.h>

//Rendering.
#include <Rendering/Engine/ParticleMaterial.h>
#include <Rendering/Engine/ParticleSystemProperties.h>

class ParticleSystemEntity : public Entity
{

public:

	/*
	*	Default constructor.
	*/
	ParticleSystemEntity() NOEXCEPT;

	/*
	*	Initializes this entity.
	*/
	void Initialize(const EntityInitializationData *const RESTRICT data) NOEXCEPT
	{

	}

	/*
*	Terminates this entity.
*/
	void Terminate() NOEXCEPT
	{

	}

protected:

	/*
	*	Returns whether or not this entity should automatically terminate.
	*/
	bool ShouldAutomaticallyTerminate() const NOEXCEPT final override;

	/*
	*	Returns the position of this entity.
	*/
	RESTRICTED NO_DISCARD Vector3<float> *const RESTRICT GetPositionInternal() NOEXCEPT final override;

};