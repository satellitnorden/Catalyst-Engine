#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Rendering.
#include <Rendering/Engine/ParticleMaterial.h>
#include <Rendering/Engine/ParticleSystemProperties.h>

class ParticleSystemInitializationData final : public EntityInitializationData
{

public:

	//The material.
	ParticleMaterial _Material;

	//The properties.
	ParticleSystemProperties _ParticleSystemProperties;

	//The position.
	Vector3 _Position;

};