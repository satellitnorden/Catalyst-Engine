#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class ParticleSystemRenderComponent final
{

public:

	//The visibility.
	VisibilityFlag _Visibility;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The instance count.
	uint32 _InstanceCount;

	//The world position.
	Vector3 _WorldPosition;

	//The random seed of the particle system.
	float _ParticleSystemRandomSeed;

	//The starting time of the particle system.
	float _ParticleSystemStartingTime;

};