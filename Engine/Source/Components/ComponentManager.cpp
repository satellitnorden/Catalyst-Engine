//Header file.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Types/Entity.h>

/*
*	Defines an entity class with with one component.
*/
#define DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(ENTITY_CLASS, FIRST_COMPONENT)														\
DynamicArray<Entity *RESTRICT> ComponentManager::_ ## ENTITY_CLASS ## Entities;														\
DynamicArray<FIRST_COMPONENT> ComponentManager::_ ## ENTITY_CLASS ## FIRST_COMPONENT ## s;											\
NO_DISCARD uint64 ComponentManager::GetNew ## ENTITY_CLASS ## ComponentsIndex(Entity *const RESTRICT entity) NOEXCEPT				\
{																																	\
	_ ## ENTITY_CLASS ## Entities.Emplace(entity);																					\
	_ ## ENTITY_CLASS ## FIRST_COMPONENT ## s.Emplace();																			\
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
RESTRICTED NO_DISCARD FIRST_COMPONENT *const RESTRICT ComponentManager::Get ## ENTITY_CLASS ## FIRST_COMPONENT ## s() NOEXCEPT		\
{																																	\
	return _ ## ENTITY_CLASS ## FIRST_COMPONENT ## s.Data();																		\
}																																	\
void ComponentManager::Return ## ENTITY_CLASS ## ComponentsIndex(const uint64 componentsIndex) NOEXCEPT								\
{																																	\
	_ ## ENTITY_CLASS ## Entities.Back()->_ComponentsIndex = componentsIndex;														\
																																	\
	_ ## ENTITY_CLASS ## Entities.EraseAt<false>(componentsIndex);																	\
	_ ## ENTITY_CLASS ## FIRST_COMPONENT ## s.EraseAt<false>(componentsIndex);														\
}

/*
*	Defines an entity class with with two components.
*/
#define DEFINE_ENTITY_CLASS_WITH_TWO_COMPONENTS(ENTITY_CLASS, FIRST_COMPONENT, SECOND_COMPONENT)									\
DynamicArray<Entity *RESTRICT> ComponentManager::_ ## ENTITY_CLASS ## Entities;														\
DynamicArray<FIRST_COMPONENT> ComponentManager::_ ## ENTITY_CLASS ## FIRST_COMPONENT ## s;											\
DynamicArray<SECOND_COMPONENT> ComponentManager::_ ## ENTITY_CLASS ## SECOND_COMPONENT ## s;										\
NO_DISCARD uint64 ComponentManager::GetNew ## ENTITY_CLASS ## ComponentsIndex(Entity *const RESTRICT entity) NOEXCEPT				\
{																																	\
	_ ## ENTITY_CLASS ## Entities.Emplace(entity);																					\
	_ ## ENTITY_CLASS ## FIRST_COMPONENT ## s.Emplace();																			\
	_ ## ENTITY_CLASS ## SECOND_COMPONENT ## s.Emplace();																			\
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
	_ ## ENTITY_CLASS ## Entities.EraseAt<false>(componentsIndex);																	\
	_ ## ENTITY_CLASS ## FIRST_COMPONENT ## s.EraseAt<false>(componentsIndex);														\
	_ ## ENTITY_CLASS ## SECOND_COMPONENT ## s.EraseAt<false>(componentsIndex);														\
}

DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(AnimatedModel, AnimatedModelComponent);
DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(DynamicModel, DynamicModelComponent);
DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(Grass, GrassComponent);
DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(InstancedImpostor, InstancedImpostorComponent);
DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(InstancedStaticModel, InstancedStaticModelComponent);
DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(Light, LightComponent);
DEFINE_ENTITY_CLASS_WITH_TWO_COMPONENTS(ParticleSystem, ParticleSystemComponent, ParticleSystemRenderComponent);
DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(Terrain, TerrainComponent);
DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(UserInterface, UserInterfaceComponent);