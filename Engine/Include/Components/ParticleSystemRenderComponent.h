#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class ParticleSystemRenderComponent final
{

public:

	//The random seed of the particle system.
	float particleSystemRandomSeed;

	//The starting time of the particle system.
	float particleSystemStartingTime;

	//The descriptor set.
	RenderDataTableHandle renderDataTable;

	//The instance count.
	uint32 instanceCount;

};