#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/Map.h>

//Math.
#include <Math/Geometry/Ray.h>

//Physics.
#include <Physics/ModelCollisionData.h>
#include <Physics/PhysicsCore.h>
#include <Physics/RaycastConfiguration.h>
#include <Physics/RaycastResult.h>

class ModelPhysicsSystem final
{

public:

	/*
	*	Updates the model physics system during the physics update phase.
	*/
	void PhysicsUpdate(const UpdateContext* const RESTRICT context) NOEXCEPT;

	/*
	*	Registers model collision data.
	*/
	void RegisterModelCollisionData(const uint64 entity_identifier, const ModelCollisionData& data) NOEXCEPT;

	/*
	*	Unregisters model collision data.
	*/
	void UnregisterModelCollisionData(const uint64 entity_identifier) NOEXCEPT;

	/*
	*	Casts a ray against models.
	*/
	void CastRayModels(const Ray& ray, const RaycastConfiguration& configuration, RaycastResult* const RESTRICT result) NOEXCEPT;

private:

	//Container for all model collision data.
	Map<uint64, ModelCollisionData> _ModelCollisionData;

};