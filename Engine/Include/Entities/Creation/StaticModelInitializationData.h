#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Math.
#include <Math/General/Matrix.h>

//Physics.
#include <Physics/ModelCollisionConfiguration.h>

//Resources.
#include <Resources/Core/ModelResource.h>
#include <Resources/Core/ResourcePointer.h>

class StaticModelInitializationData final : public EntityInitializationData
{

public:

	//The transform.
	Matrix4x4 _Transform{ MatrixConstants::IDENTITY };

	//The model resource.
	ResourcePointer<ModelResource> _ModelResource;

	//The material indices.
	DynamicArray<uint32> _MaterialIndices;

	//The model collision configuration.
	ModelCollisionConfiguration _ModelCollisionConfiguration;

};