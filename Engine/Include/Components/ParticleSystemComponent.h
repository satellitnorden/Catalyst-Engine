#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/ParticleSystemProperties.h>
#include <Rendering/Engine/RenderingCore.h>

class ParticleSystemComponent final
{

public:

	//The axis-aligned bounding box.
	AxisAlignedBoundingBox _AxisAlignedBoundingBox;

	//The particle system properties.
	ParticleSystemProperties _Properties;

	//Handle to the uniform buffer that stores the properties.
	UniformBufferHandle _PropertiesUniformBuffer;

};