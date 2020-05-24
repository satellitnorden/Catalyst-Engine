#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/Map.h>
#include <Core/General/UpdateContext.h>

//Math.
#include <Math/Geometry/Ray.h>

//Physics.
#include <Physics/ModelCollisionData.h>
#include <Physics/ModelPhysicsSimulationData.h>
#include <Physics/PhysicsCore.h>
#include <Physics/RaycastConfiguration.h>
#include <Physics/RaycastResult.h>

class ModelPhysicsSystem final
{

public:

	/*
	*	Registers dynamic model collision data.
	*/
	void RegisterDynamicModelCollisionData(const uint64 entity_identifier, const ModelCollisionData &data) NOEXCEPT;

	/*
	*	Unregisters dynamic model collision data.
	*/
	void UnregisterDynamicModelCollisionData(const uint64 entity_identifier) NOEXCEPT;

	/*
	*	Registers static model collision data.
	*/
	void RegisterStaticModelCollisionData(const uint64 entity_identifier, const ModelCollisionData& data) NOEXCEPT;

	/*
	*	Unregisters static model collision data.
	*/
	void UnregisterStaticModelCollisionData(const uint64 entity_identifier) NOEXCEPT;

	/*
	*	Casts a ray against dynamic models.
	*/
	void CastRayDynamicModels(const Ray& ray, const RaycastConfiguration& configuration, RaycastResult* const RESTRICT result) NOEXCEPT;

	/*
	*	Casts a ray against static models.
	*/
	void CastRayStaticModels(const Ray& ray, const RaycastConfiguration& configuration, RaycastResult* const RESTRICT result) NOEXCEPT;

	/*
	*	Returns the static model collision data.
	*/
	FORCE_INLINE NO_DISCARD const Map<uint64, ModelCollisionData> &GetStaticModelCollisionData() const NOEXCEPT
	{
		return _StaticModelCollisionData;
	}

private:

	//Container for all dynamic model collision data.
	Map<uint64, ModelCollisionData> _DynamicModelCollisionData;

	//Container for all static model collision data.
	Map<uint64, ModelCollisionData> _StaticModelCollisionData;

};