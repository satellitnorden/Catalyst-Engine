//Header file.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/Types/Entity.h>

/*
*	Defines an entity class with with one component.
*/
#define DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(ENTITY_CLASS, FIRST_COMPONENT)														\
DynamicArray<Entity *RESTRICT> ComponentManager::_ ## ENTITY_CLASS ## Entities;														\
DynamicArray<TransformComponent> ComponentManager::_ ## ENTITY_CLASS ## TransformComponents;										\
DynamicArray<FIRST_COMPONENT> ComponentManager::_ ## ENTITY_CLASS ## FIRST_COMPONENT ## s;											\
NO_DISCARD uint64 ComponentManager::GetNew ## ENTITY_CLASS ## ComponentsIndex(Entity *const RESTRICT entity) NOEXCEPT				\
{																																	\
	_ ## ENTITY_CLASS ## Entities.EmplaceSlow(entity);																				\
	_ ## ENTITY_CLASS ## TransformComponents.EmplaceSlow();																			\
	_ ## ENTITY_CLASS ## FIRST_COMPONENT ## s.EmplaceSlow();																		\
																																	\
	return _ ## ENTITY_CLASS ## Entities.LastIndex();																				\
}																																	\
RESTRICTED NO_DISCARD DynamicArray<Entity *RESTRICT> *const RESTRICT ComponentManager::Get ## ENTITY_CLASS ## Entities() NOEXCEPT	\
{																																	\
	return &_ ## ENTITY_CLASS ## Entities;																							\
}																																	\
NO_DISCARD uint64 ComponentManager::GetNumberOf ## ENTITY_CLASS ## Components() NOEXCEPT											\
{																																	\
	return _ ## ENTITY_CLASS ## Entities.Size();																					\
}																																	\
RESTRICTED NO_DISCARD TransformComponent *const RESTRICT ComponentManager::Get ## ENTITY_CLASS ## TransformComponents() NOEXCEPT	\
{																																	\
	return _ ## ENTITY_CLASS ## TransformComponents.Data();																			\
}																																	\
RESTRICTED NO_DISCARD FIRST_COMPONENT *const RESTRICT ComponentManager::Get ## ENTITY_CLASS ## FIRST_COMPONENT ## s() NOEXCEPT		\
{																																	\
	return _ ## ENTITY_CLASS ## FIRST_COMPONENT ## s.Data();																		\
}																																	\
void ComponentManager::Return ## ENTITY_CLASS ## ComponentsIndex(const uint64 componentsIndex) NOEXCEPT								\
{																																	\
	_ ## ENTITY_CLASS ## Entities.Back()->_ComponentsIndex = componentsIndex;														\
																																	\
	_ ## ENTITY_CLASS ## Entities.EraseAt(componentsIndex);																			\
	_ ## ENTITY_CLASS ## TransformComponents.EraseAt(componentsIndex);																\
	_ ## ENTITY_CLASS ## FIRST_COMPONENT ## s.EraseAt(componentsIndex);																\
}

/*
*	Defines an entity class with with two components.
*/
#define DEFINE_ENTITY_CLASS_WITH_TWO_COMPONENTS(ENTITY_CLASS, FIRST_COMPONENT, SECOND_COMPONENT)									\
DynamicArray<Entity *RESTRICT> ComponentManager::_ ## ENTITY_CLASS ## Entities;														\
DynamicArray<TransformComponent> ComponentManager::_ ## ENTITY_CLASS ## TransformComponents;										\
DynamicArray<FIRST_COMPONENT> ComponentManager::_ ## ENTITY_CLASS ## FIRST_COMPONENT ## s;											\
DynamicArray<SECOND_COMPONENT> ComponentManager::_ ## ENTITY_CLASS ## SECOND_COMPONENT ## s;										\
NO_DISCARD uint64 ComponentManager::GetNew ## ENTITY_CLASS ## ComponentsIndex(Entity *const RESTRICT entity) NOEXCEPT				\
{																																	\
	_ ## ENTITY_CLASS ## Entities.EmplaceSlow(entity);																				\
	_ ## ENTITY_CLASS ## TransformComponents.EmplaceSlow();																			\
	_ ## ENTITY_CLASS ## FIRST_COMPONENT ## s.EmplaceSlow();																		\
	_ ## ENTITY_CLASS ## SECOND_COMPONENT ## s.EmplaceSlow();																		\
																																	\
	return _ ## ENTITY_CLASS ## Entities.LastIndex();																				\
}																																	\
RESTRICTED NO_DISCARD DynamicArray<Entity *RESTRICT> *const RESTRICT ComponentManager::Get ## ENTITY_CLASS ## Entities() NOEXCEPT	\
{																																	\
	return &_ ## ENTITY_CLASS ## Entities;																							\
}																																	\
NO_DISCARD uint64 ComponentManager::GetNumberOf ## ENTITY_CLASS ## Components() NOEXCEPT											\
{																																	\
	return _ ## ENTITY_CLASS ## Entities.Size();																					\
}																																	\
RESTRICTED NO_DISCARD TransformComponent *const RESTRICT ComponentManager::Get ## ENTITY_CLASS ## TransformComponents() NOEXCEPT	\
{																																	\
	return _ ## ENTITY_CLASS ## TransformComponents.Data();																			\
}																																	\
RESTRICTED NO_DISCARD FIRST_COMPONENT *const RESTRICT ComponentManager::Get ## ENTITY_CLASS ## FIRST_COMPONENT ## s() NOEXCEPT		\
{																																	\
	return _ ## ENTITY_CLASS ## FIRST_COMPONENT ## s.Data();																		\
}																																	\
RESTRICTED NO_DISCARD SECOND_COMPONENT *const RESTRICT ComponentManager::Get ## ENTITY_CLASS ## SECOND_COMPONENT ## s() NOEXCEPT	\
{																																	\
	return _ ## ENTITY_CLASS ## SECOND_COMPONENT ## s.Data();																		\
}																																	\
void ComponentManager::Return ## ENTITY_CLASS ## ComponentsIndex(const uint64 componentsIndex) NOEXCEPT								\
{																																	\
	_ ## ENTITY_CLASS ## Entities.Back()->_ComponentsIndex = componentsIndex;														\
																																	\
	_ ## ENTITY_CLASS ## Entities.EraseAt(componentsIndex);																			\
	_ ## ENTITY_CLASS ## TransformComponents.EraseAt(componentsIndex);																\
	_ ## ENTITY_CLASS ## FIRST_COMPONENT ## s.EraseAt(componentsIndex);																\
	_ ## ENTITY_CLASS ## SECOND_COMPONENT ## s.EraseAt(componentsIndex);															\
}

/*
*	Defines an entity class with with three components.
*/
#define DEFINE_ENTITY_CLASS_WITH_THREE_COMPONENTS(ENTITY_CLASS, FIRST_COMPONENT, SECOND_COMPONENT, THIRD_COMPONENT)					\
DynamicArray<Entity *RESTRICT> ComponentManager::_ ## ENTITY_CLASS ## Entities;														\
DynamicArray<TransformComponent> ComponentManager::_ ## ENTITY_CLASS ## TransformComponents;										\
DynamicArray<FIRST_COMPONENT> ComponentManager::_ ## ENTITY_CLASS ## FIRST_COMPONENT ## s;											\
DynamicArray<SECOND_COMPONENT> ComponentManager::_ ## ENTITY_CLASS ## SECOND_COMPONENT ## s;										\
DynamicArray<THIRD_COMPONENT> ComponentManager::_ ## ENTITY_CLASS ## THIRD_COMPONENT ## s;											\
NO_DISCARD uint64 ComponentManager::GetNew ## ENTITY_CLASS ## ComponentsIndex(Entity *const RESTRICT entity) NOEXCEPT				\
{																																	\
	_ ## ENTITY_CLASS ## Entities.EmplaceSlow(entity);																				\
	_ ## ENTITY_CLASS ## TransformComponents.EmplaceSlow();																			\
	_ ## ENTITY_CLASS ## FIRST_COMPONENT ## s.EmplaceSlow();																		\
	_ ## ENTITY_CLASS ## SECOND_COMPONENT ## s.EmplaceSlow();																		\
	_ ## ENTITY_CLASS ## THIRD_COMPONENT ## s.EmplaceSlow();																		\
																																	\
	return _ ## ENTITY_CLASS ## Entities.LastIndex();																				\
}																																	\
RESTRICTED NO_DISCARD DynamicArray<Entity *RESTRICT> *const RESTRICT ComponentManager::Get ## ENTITY_CLASS ## Entities() NOEXCEPT	\
{																																	\
	return &_ ## ENTITY_CLASS ## Entities;																							\
}																																	\
NO_DISCARD uint64 ComponentManager::GetNumberOf ## ENTITY_CLASS ## Components() NOEXCEPT											\
{																																	\
	return _ ## ENTITY_CLASS ## Entities.Size();																					\
}																																	\
RESTRICTED NO_DISCARD TransformComponent *const RESTRICT ComponentManager::Get ## ENTITY_CLASS ## TransformComponents() NOEXCEPT	\
{																																	\
	return _ ## ENTITY_CLASS ## TransformComponents.Data();																			\
}																																	\
RESTRICTED NO_DISCARD FIRST_COMPONENT *const RESTRICT ComponentManager::Get ## ENTITY_CLASS ## FIRST_COMPONENT ## s() NOEXCEPT		\
{																																	\
	return _ ## ENTITY_CLASS ## FIRST_COMPONENT ## s.Data();																		\
}																																	\
RESTRICTED NO_DISCARD SECOND_COMPONENT *const RESTRICT ComponentManager::Get ## ENTITY_CLASS ## SECOND_COMPONENT ## s() NOEXCEPT	\
{																																	\
	return _ ## ENTITY_CLASS ## SECOND_COMPONENT ## s.Data();																		\
}																																	\
RESTRICTED NO_DISCARD THIRD_COMPONENT *const RESTRICT ComponentManager::Get ## ENTITY_CLASS ## THIRD_COMPONENT ## s() NOEXCEPT		\
{																																	\
	return _ ## ENTITY_CLASS ## THIRD_COMPONENT ## s.Data();																		\
}																																	\
void ComponentManager::Return ## ENTITY_CLASS ## ComponentsIndex(const uint64 componentsIndex) NOEXCEPT								\
{																																	\
	_ ## ENTITY_CLASS ## Entities.Back()->_ComponentsIndex = componentsIndex;														\
																																	\
	_ ## ENTITY_CLASS ## Entities.EraseAt(componentsIndex);																			\
	_ ## ENTITY_CLASS ## TransformComponents.EraseAt(componentsIndex);																\
	_ ## ENTITY_CLASS ## FIRST_COMPONENT ## s.EraseAt(componentsIndex);																\
	_ ## ENTITY_CLASS ## SECOND_COMPONENT ## s.EraseAt(componentsIndex);															\
	_ ## ENTITY_CLASS ## THIRD_COMPONENT ## s.EraseAt(componentsIndex);																\
}

DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(Collision, CollisionComponent);
DEFINE_ENTITY_CLASS_WITH_THREE_COMPONENTS(DynamicPhysical, FrustumCullingComponent, DynamicOutlineRenderComponent, DynamicPhysicalRenderComponent);
DEFINE_ENTITY_CLASS_WITH_TWO_COMPONENTS(ParticleSystem, ParticleSystemComponent, ParticleSystemRenderComponent);
DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(Physics, PhysicsComponent);
DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(PointLight, PointLightComponent);
DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(Sound, SoundComponent);
DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(SpotLight, SpotLightComponent);