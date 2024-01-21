#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Components.
#include <Components/Transient/AnimatedModelComponent.h>
#include <Components/Transient/InstancedStaticModelComponent.h>
#include <Components/Transient/LightComponent.h>
#include <Components/Transient/ParticleSystemComponent.h>
#include <Components/Transient/ParticleSystemRenderComponent.h>
#include <Components/Transient/TerrainComponent.h>
#include <Components/Transient/UserInterfaceComponent.h>

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

//Forward declarations.
class Entity;

class ComponentManager final
{

public:

	DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(AnimatedModel, AnimatedModelComponent);
	DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(InstancedStaticModel, InstancedStaticModelComponent);
	DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(Light, LightComponent);
	DECLARE_ENTITY_CLASS_WITH_TWO_COMPONENTS(ParticleSystem, ParticleSystemComponent, ParticleSystemRenderComponent);
	DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(Terrain, TerrainComponent);
	DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(UserInterface, UserInterfaceComponent);

};