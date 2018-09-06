#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Entity.
#include <Entities/Entity.h>

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
	*	Initializes this particle system entity.
	*/
	void Initialize(const ParticleMaterial &material, const ParticleSystemProperties &properties) NOEXCEPT;

protected:

	/*
	*	Returns the position of this entity.
	*/
	NO_DISCARD RESTRICTED Vector3 *const RESTRICT GetPositionInternal() NOEXCEPT final override;

};