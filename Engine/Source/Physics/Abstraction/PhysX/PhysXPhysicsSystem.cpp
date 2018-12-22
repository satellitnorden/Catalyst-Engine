#if defined(CATALYST_PHYSICS_PHYSX)
//Header file.
#include <Physics/Abstraction/PhysX/PhysXPhysicsSystem.h>

//Math.
#include <Math/CatalystPhysicsMath.h>

//Physics.
#include <Physics/Abstraction/PhysX/PhysXAbstractionUtilities.h>

//Singleton definition.
DEFINE_SINGLETON(PhysXPhysicsSystem);

/*
*	Initializes the PhysX physics system.
*/
void PhysXPhysicsSystem::InitializeSystem() NOEXCEPT
{
	//Create the foundation.
	_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, _AllocatorCallback, _ErrorCallback);

	//Create the tolerances scale.
	physx::PxTolerancesScale tolerancesScale;

	tolerancesScale.length = 100.0f;
	tolerancesScale.speed = 981.0f;

	//Create the physics.
	_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_Foundation, tolerancesScale, false, nullptr);

	//Create the scene description.
	physx::PxSceneDesc sceneDescription{ tolerancesScale };

	CreateSceneDescription(&sceneDescription);

	//Create the scene.
	_Scene = _Physics->createScene(sceneDescription);
}

/*
*	Updates the PhysX physics system synchronously.
*/
void PhysXPhysicsSystem::UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Simulate the scene.
	_Scene->simulate(context->_DeltaTime);

	//Fetch the results.
	_Scene->fetchResults(true);
}

/*
*	Releases the PhysX physics system.
*/
void PhysXPhysicsSystem::ReleaseSystem() NOEXCEPT
{
	//Release the scene.
	_Scene->release();

	//Release the physics.
	_Physics->release();

	//Release the foundation.
	_Foundation->release();
}

/*
*	Creates the scene description.
*/
void PhysXPhysicsSystem::CreateSceneDescription(physx::PxSceneDesc *const RESTRICT sceneDescription) NOEXCEPT
{
	sceneDescription->gravity = PhysXAbstractionUtilities::ToPhysXVector(CatalystPhysicsMathConstants::GRAVITATIONAL_DIRECTION * CatalystPhysicsMathConstants::GRAVITATIONAL_SPEED);
	sceneDescription->simulationEventCallback = nullptr;
	sceneDescription->contactModifyCallback = nullptr;
	sceneDescription->ccdContactModifyCallback = nullptr;
	sceneDescription->filterShaderData = nullptr;
	sceneDescription->filterShaderDataSize = 0;
	sceneDescription->filterShader = PhysxFilterShader;
	sceneDescription->filterCallback = nullptr;
	sceneDescription->kineKineFilteringMode = physx::PxPairFilteringMode::eDEFAULT;
	sceneDescription->staticKineFilteringMode = physx::PxPairFilteringMode::eDEFAULT;
	sceneDescription->broadPhaseType = physx::PxBroadPhaseType::eABP;
	sceneDescription->broadPhaseCallback = nullptr;
	sceneDescription->limits.maxNbActors = UINT8_MAX;
	sceneDescription->limits.maxNbBodies = UINT8_MAX;
	sceneDescription->limits.maxNbStaticShapes = UINT8_MAX;
	sceneDescription->limits.maxNbDynamicShapes = UINT8_MAX;
	sceneDescription->limits.maxNbAggregates = UINT8_MAX;
	sceneDescription->limits.maxNbConstraints = UINT8_MAX;
	sceneDescription->limits.maxNbRegions = UINT8_MAX;
	sceneDescription->limits.maxNbBroadPhaseOverlaps = UINT8_MAX;
	sceneDescription->frictionType = physx::PxFrictionType::ePATCH;
	sceneDescription->solverType = physx::PxSolverType::eTGS;
	sceneDescription->bounceThresholdVelocity = 0.2f * 100.0f;
	sceneDescription->frictionOffsetThreshold = 0.4f * 100.0f;
	sceneDescription->ccdMaxSeparation = 0.4f * 100.0f;
	sceneDescription->solverOffsetSlop = 0.0f;
	sceneDescription->flags = physx::PxSceneFlag::eENABLE_PCM;
	sceneDescription->cpuDispatcher = &_CPUDistpatcher;
	sceneDescription->gpuDispatcher = nullptr;
	sceneDescription->staticStructure = physx::PxPruningStructureType::eDYNAMIC_AABB_TREE;
	sceneDescription->dynamicStructure = physx::PxPruningStructureType::eDYNAMIC_AABB_TREE;
	sceneDescription->dynamicTreeRebuildRateHint = 100;
	sceneDescription->sceneQueryUpdateMode = physx::PxSceneQueryUpdateMode::eBUILD_ENABLED_COMMIT_ENABLED;
	sceneDescription->userData = nullptr;
	sceneDescription->solverBatchSize = 128;
	sceneDescription->nbContactDataBlocks = 0;
	sceneDescription->maxNbContactDataBlocks = 65'536;
	sceneDescription->maxBiasCoefficient = PX_MAX_F32;
	sceneDescription->contactReportStreamBufferSize = 8'192;
	sceneDescription->ccdMaxPasses = 1;
	sceneDescription->wakeCounterResetValue = 0.4f;
	sceneDescription->sanityBounds = physx::PxBounds3(physx::PxVec3(-PX_MAX_BOUNDS_EXTENTS), physx::PxVec3(PX_MAX_BOUNDS_EXTENTS));
	sceneDescription->gpuDynamicsConfig = physx::PxgDynamicsMemoryConfig();
	sceneDescription->gpuMaxNumPartitions = 8;
	sceneDescription->gpuComputeVersion = 0;
}
#endif