#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

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

};