#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>

//Components.
#include <Components/DirectionalLightComponent.h>
#include <Components/DynamicPhysicalRenderComponent.h>
#include <Components/DynamicOutlineRenderComponent.h>
#include <Components/FrustumCullingComponent.h>
#include <Components/ParticleSystemComponent.h>
#include <Components/ParticleSystemRenderComponent.h>
#include <Components/PointLightComponent.h>
#include <Components/SpotLightComponent.h>
#include <Components/TransformComponent.h>

/*
*	Declares an entity class with one component.
*/
#define DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(ENTITY_CLASS, FIRST_COMPONENT)												\
public:																														\
	static NO_DISCARD uint64 GetNew ## ENTITY_CLASS ## ComponentsIndex(Entity *const RESTRICT entity) NOEXCEPT;				\
	RESTRICTED static NO_DISCARD DynamicArray<Entity *RESTRICT> *const RESTRICT Get ## ENTITY_CLASS ## Entities() NOEXCEPT;	\
	static NO_DISCARD uint64 GetNumberOf ## ENTITY_CLASS ## Components() NOEXCEPT;											\
	RESTRICTED static NO_DISCARD FIRST_COMPONENT *const RESTRICT Get ## ENTITY_CLASS ## FIRST_COMPONENT ## s() NOEXCEPT;	\
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
	RESTRICTED static NO_DISCARD SECOND_COMPONENT *const RESTRICT Get ## ENTITY_CLASS ## SECOND_COMPONENT ## s() NOEXCEPT;	\
private:																													\
	static DynamicArray<SECOND_COMPONENT> _ ## ENTITY_CLASS ## SECOND_COMPONENT ## s;

/*
*	Declares an entity class with three components.
*/
#define DECLARE_ENTITY_CLASS_WITH_THREE_COMPONENTS(ENTITY_CLASS, FIRST_COMPONENT, SECOND_COMPONENT, THIRD_COMPONENT)		\
DECLARE_ENTITY_CLASS_WITH_TWO_COMPONENTS(ENTITY_CLASS, FIRST_COMPONENT, SECOND_COMPONENT)									\
public:																														\
	RESTRICTED static NO_DISCARD THIRD_COMPONENT *const RESTRICT Get ## ENTITY_CLASS ## THIRD_COMPONENT ## s() NOEXCEPT;	\
private:																													\
	static DynamicArray<THIRD_COMPONENT> _ ## ENTITY_CLASS ## THIRD_COMPONENT ## s;

/*
*	Declares an entity class with four components.
*/
#define DECLARE_ENTITY_CLASS_WITH_FOUR_COMPONENTS(ENTITY_CLASS, FIRST_COMPONENT, SECOND_COMPONENT, THIRD_COMPONENT, FOURTH_COMPONENT)	\
DECLARE_ENTITY_CLASS_WITH_THREE_COMPONENTS(ENTITY_CLASS, FIRST_COMPONENT, SECOND_COMPONENT, THIRD_COMPONENT)							\
public:																																	\
	RESTRICTED static NO_DISCARD FOURTH_COMPONENT *const RESTRICT Get ## ENTITY_CLASS ## FOURTH_COMPONENT ## s() NOEXCEPT;				\
private:																																\
	static DynamicArray<FOURTH_COMPONENT> _ ## ENTITY_CLASS ## FOURTH_COMPONENT ## s;

//Forward declarations.
class Entity;

class ComponentManager final
{

	DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(DirectionalLight, DirectionalLightComponent);
	DECLARE_ENTITY_CLASS_WITH_FOUR_COMPONENTS(DynamicPhysical, FrustumCullingComponent, DynamicOutlineRenderComponent, DynamicPhysicalRenderComponent, TransformComponent);
	DECLARE_ENTITY_CLASS_WITH_TWO_COMPONENTS(ParticleSystem, ParticleSystemComponent, ParticleSystemRenderComponent);
	DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(PointLight, PointLightComponent);
	DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(SpotLight, SpotLightComponent);

};