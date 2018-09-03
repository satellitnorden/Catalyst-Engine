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
	ParticleSystemProperties _Properties;

	//Handle to the uniform buffer that stores the properties.
	ConstantBufferHandle _PropertiesUniformBuffer;

};