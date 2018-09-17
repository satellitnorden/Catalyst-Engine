#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/ParticleMaterial.h>
#include <Rendering/Engine/ParticleSystemProperties.h>

class ParticleSystemInitializationData final
{

public:

	//The material.
	ParticleMaterial _Material;

	//The properties.
	ParticleSystemProperties _Properties;

	//The position.
	Vector3 _Position;

};