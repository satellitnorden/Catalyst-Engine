#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/HashTable.h>
#include <Core/General/UpdateContext.h>

//Entities.
#include <Entities/Types/DynamicModelEntity.h>
#include <Entities/Types/StaticModelEntity.h>

//Math.
#include <Math/Geometry/Ray.h>

//Physics.
#include <Physics/ModelCollisionConfiguration.h>
#include <Physics/ModelPhysicsSimulationData.h>
#include <Physics/PhysicsCore.h>
#include <Physics/RaycastConfiguration.h>
#include <Physics/RaycastResult.h>

class ModelPhysicsSystem final
{

public:

	/*
	*	Registers dynamic model collision configuration.
	*/
	void RegisterDynamicModelCollisionConfiguration(DynamicModelEntity *const RESTRICT entity, const ModelCollisionConfiguration &configuration) NOEXCEPT;

	/*
	*	Registers static model collision configuration.
	*/
	void RegisterStaticModelCollisionConfiguration(StaticModelEntity *const RESTRICT entity, const ModelCollisionConfiguration &configuration) NOEXCEPT;

	/*
	*	Casts a ray against dynamic models.
	*/
	void CastRayDynamicModels(const Ray& ray, const RaycastConfiguration& configuration, RaycastResult* const RESTRICT result) NOEXCEPT;

	/*
	*	Casts a ray against static models.
	*/
	void CastRayStaticModels(const Ray& ray, const RaycastConfiguration& configuration, RaycastResult* const RESTRICT result) NOEXCEPT;

private:

	//Container for all dynamic model collision configurations.
	HashTable<DynamicModelEntity *RESTRICT, ModelCollisionConfiguration> _DynamicModelCollisionConfigurations;

	//Container for all static model collision configurations.
	HashTable<StaticModelEntity *RESTRICT, ModelCollisionConfiguration> _StaticModelCollisionConfigurations;

};