#pragma once

//Core.
#include <Core/General/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class ParticleSystemRenderComponent final
{

public:

	//The random seed of the particle system.
	float particleSystemRandomSeed;

	//The starting time of the particle system.
	float particleSystemStartingTime;

	//The render data table.
	RenderDataTableHandle renderDataTable;

	//The instance count.
	uint32 instanceCount;

};