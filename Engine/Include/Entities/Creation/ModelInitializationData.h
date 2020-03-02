#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Math.
#include <Math/General/Matrix.h>

//Physics.
#include <Physics/ModelCollisionData.h>
#include <Physics/ModelPhysicsSimulationData.h>
#include <Physics/PhysicsCore.h>

//Rendering.
#include <Rendering/Native/Material.h>
#include <Rendering/Native/Model.h>

class ModelInitializationData final : public EntityInitializationData
{

public:

	//The transform.
	Matrix4x4 _Transform{ MatrixConstants::IDENTITY };

	//The model.
	const Model *RESTRICT _Model{ nullptr };

	//The material indices.
	DynamicArray<uint32> _MaterialIndices;

	//The model collision data.
	ModelCollisionData _ModelCollisionData;

	//Denotes whether or not to simulate physics.
	bool _SimulatePhysics;

	//The model physics simulation data.
	ModelPhysicsSimulationData _ModelPhysicsSimulationData;

};