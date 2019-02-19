#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class ParticleSystemRenderComponent final
{

public:

	//The particle system properties.
	ParticleSystemProperties _Properties;

	//The visibility.
	VisibilityFlag _Visibility;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The instance count.
	uint32 _InstanceCount;

	//The random seed of the particle system.
	float _ParticleSystemRandomSeed;

	//The starting time of the particle system.
	float _ParticleSystemStartingTime;

};