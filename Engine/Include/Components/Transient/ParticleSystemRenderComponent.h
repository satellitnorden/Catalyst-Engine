#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Core/MaterialResource.h>
#include <Resources/Core/ResourcePointer.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class ParticleSystemRenderComponent final
{

public:

	//The material resource.
	ResourcePointer<MaterialResource> _MaterialResource;

	//The transformations buffer.
	BufferHandle _TransformationsBuffer;

	//The number of instances.
	uint32 _NumberOfInstances;

};