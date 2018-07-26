#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/ParticleSystemProperties.h>
#include <Rendering/Engine Layer/RenderingCore.h>

class ParticleSystemComponent final
{

public:

	//The particle system properties.
	ParticleSystemProperties properties;

	//Handle to the uniform buffer that stores the properties.
	ConstantBufferHandle propertiesUniformBuffer;

};