#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Components.
#include <Components/CameraComponent.h>
#include <Components/DirectionalLightComponent.h>
#include <Components/DynamicPhysicalRenderComponent.h>
#include <Components/DynamicOutlineRenderComponent.h>
#include <Components/FrustumCullingComponent.h>
#include <Components/InstancedPhysicalRenderComponent.h>
#include <Components/ParticleSystemComponent.h>
#include <Components/ParticleSystemRenderComponent.h>
#include <Components/PointLightComponent.h>
#include <Components/SpotLightComponent.h>
#include <Components/StaticPhysicalRenderComponent.h>
#include <Components/TerrainComponent.h>
#include <Components/TerrainRenderComponent.h>
#include <Components/TransformComponent.h>
#include <Components/VegetationComponent.h>
#include <Components/VegetationCullingComponent.h>

/*
*	Declares an entity class with one component.
*/
#define DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(ENTITY_CLASS, FIRST_COMPONENT)												\
public:																														\
	NO_DISCARD static uint64 GetNumberOf ## ENTITY_CLASS ## Components() NOEXCEPT;											\
	NO_DISCARD static uint64 GetNew ## ENTITY_CLASS ## ComponentsIndex(Entity *const RESTRICT entity) NOEXCEPT;				\
	NO_DISCARD RESTRICTED static FIRST_COMPONENT *const RESTRICT Get ## ENTITY_CLASS ## FIRST_COMPONENT ## s() NOEXCEPT;	\
	static void Return ## ENTITY_CLASS ## ComponentsIndex(const uint64 componentsIndex) NOEXCEPT;							\
private:																													\
	static DynamicArray<Entity *RESTRICT> _ ## ENTITY_CLASS ## Entities;													\
	static DynamicArray<FIRST_COMPONENT> _ ## ENTITY_CLASS ## FIRST_COMPONENT ## s;

/*
*	Declares an entity class with two components.
*/
#define DECLARE_ENTITY_CLASS_WITH_TWO_COMPONENTS(ENTITY_CLASS, FIRST_COMPONENT, SECOND_COMPONENT)							\
DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(ENTITY_CLASS, FIRST_COMPONENT)														\
public:																														\
	NO_DISCARD RESTRICTED static SECOND_COMPONENT *const RESTRICT Get ## ENTITY_CLASS ## SECOND_COMPONENT ## s() NOEXCEPT;	\
private:																													\
	static DynamicArray<SECOND_COMPONENT> _ ## ENTITY_CLASS ## SECOND_COMPONENT ## s;

/*
*	Declares an entity class with three components.
*/
#define DECLARE_ENTITY_CLASS_WITH_THREE_COMPONENTS(ENTITY_CLASS, FIRST_COMPONENT, SECOND_COMPONENT, THIRD_COMPONENT)		\
DECLARE_ENTITY_CLASS_WITH_TWO_COMPONENTS(ENTITY_CLASS, FIRST_COMPONENT, SECOND_COMPONENT)									\
public:																														\
	NO_DISCARD RESTRICTED static THIRD_COMPONENT *const RESTRICT Get ## ENTITY_CLASS ## THIRD_COMPONENT ## s() NOEXCEPT;	\
private:																													\
	static DynamicArray<THIRD_COMPONENT> _ ## ENTITY_CLASS ## THIRD_COMPONENT ## s;

/*
*	Declares an entity class with four components.
*/
#define DECLARE_ENTITY_CLASS_WITH_FOUR_COMPONENTS(ENTITY_CLASS, FIRST_COMPONENT, SECOND_COMPONENT, THIRD_COMPONENT, FOURTH_COMPONENT)	\
DECLARE_ENTITY_CLASS_WITH_THREE_COMPONENTS(ENTITY_CLASS, FIRST_COMPONENT, SECOND_COMPONENT, THIRD_COMPONENT)							\
public:																																	\
	NO_DISCARD RESTRICTED static FOURTH_COMPONENT *const RESTRICT Get ## ENTITY_CLASS ## FOURTH_COMPONENT ## s() NOEXCEPT;				\
private:																																\
	static DynamicArray<FOURTH_COMPONENT> _ ## ENTITY_CLASS ## FOURTH_COMPONENT ## s;

//Forward declarations.
class Entity;

class ComponentManager final
{

	DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(Camera, CameraComponent);
	DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(DirectionalLight, DirectionalLightComponent);
	DECLARE_ENTITY_CLASS_WITH_FOUR_COMPONENTS(DynamicPhysical, FrustumCullingComponent, DynamicOutlineRenderComponent, DynamicPhysicalRenderComponent, TransformComponent);
	DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(InstancedPhysical, InstancedPhysicalRenderComponent);
	DECLARE_ENTITY_CLASS_WITH_THREE_COMPONENTS(StaticPhysical, FrustumCullingComponent, StaticPhysicalRenderComponent, TransformComponent);
	DECLARE_ENTITY_CLASS_WITH_TWO_COMPONENTS(ParticleSystem, ParticleSystemComponent, ParticleSystemRenderComponent);
	DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(PointLight, PointLightComponent);
	DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(SpotLight, SpotLightComponent);
	DECLARE_ENTITY_CLASS_WITH_THREE_COMPONENTS(Terrain, TerrainComponent, FrustumCullingComponent, TerrainRenderComponent);
	DECLARE_ENTITY_CLASS_WITH_TWO_COMPONENTS(Vegetation, VegetationComponent, VegetationCullingComponent);

};