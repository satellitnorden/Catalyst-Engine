#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/ParticleSystemProperties.h>
#include <Rendering/Engine/RenderingCore.h>

class ParticleSystemComponent final
{

public:

	//The particle system properties.
	ParticleSystemProperties properties;

	//Handle to the uniform buffer that stores the properties.
	ConstantBufferHandle propertiesUniformBuffer;

};