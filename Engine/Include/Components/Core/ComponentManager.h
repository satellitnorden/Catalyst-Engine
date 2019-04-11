#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Components.
#include <Components/Singleton/CatalystEngineComponent.h>
#include <Components/Singleton/InputComponent.h>
#include <Components/Transient/CollisionComponent.h>
#include <Components/Transient/FrustumCullingComponent.h>
#include <Components/Transient/PhysicsComponent.h>
#include <Components/Transient/PointLightComponent.h>
#include <Components/Transient/SoundComponent.h>
#include <Components/Transient/SpotLightComponent.h>
#include <Components/Transient/StaticModelComponent.h>
#include <Components/Transient/TransformComponent.h>

/*
*	Declares an entity class with one component.
*/
#define DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(ENTITY_CLASS, FIRST_COMPONENT)												\
public:																														\
	static NO_DISCARD uint64 GetNew ## ENTITY_CLASS ## ComponentsIndex(Entity *const RESTRICT entity) NOEXCEPT;				\
	RESTRICTED static NO_DISCARD DynamicArray<Entity *RESTRICT> *const RESTRICT Get ## ENTITY_CLASS ## Entities() NOEXCEPT;	\
	static NO_DISCARD uint64 GetNumberOf ## ENTITY_CLASS ## Components() NOEXCEPT;											\
	RESTRICTED static NO_DISCARD TransformComponent *const RESTRICT Get ## ENTITY_CLASS ## TransformComponents() NOEXCEPT;	\
	RESTRICTED static NO_DISCARD FIRST_COMPONENT *const RESTRICT Get ## ENTITY_CLASS ## FIRST_COMPONENT ## s() NOEXCEPT;	\
	static void Return ## ENTITY_CLASS ## ComponentsIndex(const uint64 componentsIndex) NOEXCEPT;							\
private:																													\
	static DynamicArray<Entity *RESTRICT> _ ## ENTITY_CLASS ## Entities;													\
	static DynamicArray<TransformComponent> _ ## ENTITY_CLASS ## TransformComponents;										\
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

//Forward declarations.
class Entity;

class ComponentManager final
{

public:

	/*
	*	Returns the singleton component of the given type for read access.
	*/
	template <typename TYPE>
	RESTRICTED static NO_DISCARD const TYPE *const RESTRICT ReadSingletonComponent() NOEXCEPT
	{
		VerifyReadAccess<TYPE>();

		return RetrieveSingletonComponent<TYPE>();
	}

	/*
	*	Returns the singleton component of the given type for write access.
	*/
	template <typename TYPE>
	RESTRICTED static NO_DISCARD TYPE *const RESTRICT WriteSingletonComponent() NOEXCEPT
	{
		VerifyWriteAccess<TYPE>();

		return RetrieveSingletonComponent<TYPE>();
	}

	DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(Collision, CollisionComponent);
	DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(Physics, PhysicsComponent);
	DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(PointLight, PointLightComponent);
	DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(Sound, SoundComponent);
	DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(SpotLight, SpotLightComponent);
	DECLARE_ENTITY_CLASS_WITH_ONE_COMPONENT(StaticModel, StaticModelComponent);

private:

	/*
	*	Returns the singleton component of the given type.
	*/
	template <typename TYPE>
	RESTRICTED static NO_DISCARD TYPE *const RESTRICT RetrieveSingletonComponent() NOEXCEPT
	{
		static DestructorPointer<TYPE> component{ DestructorPointer<TYPE>(new (Memory::GlobalLinearAllocator()->Allocate(sizeof(TYPE))) TYPE()) };

		return component.Get();
	}

};