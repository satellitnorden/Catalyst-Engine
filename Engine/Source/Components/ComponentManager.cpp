//Header file.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/Entity.h>

/*
*	Defines an entity class with with one component.
*/
#define DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(ENTITY_CLASS, FIRST_COMPONENT)													\
DynamicArray<Entity *RESTRICT> ComponentManager::ENTITY_CLASS ## Entities;														\
DynamicArray<FIRST_COMPONENT> ComponentManager::ENTITY_CLASS ## FIRST_COMPONENT ## s;											\
NO_DISCARD uint64 ComponentManager::GetNumberOf ## ENTITY_CLASS ## Components() NOEXCEPT										\
{																																\
	return ENTITY_CLASS ## Entities.Size();																						\
}																																\
NO_DISCARD uint64 ComponentManager::GetNew ## ENTITY_CLASS ## ComponentsIndex(Entity *const RESTRICT entity) NOEXCEPT			\
{																																\
	ENTITY_CLASS ## Entities.EmplaceSlow(entity);																				\
	ENTITY_CLASS ## FIRST_COMPONENT ## s.EmplaceSlow();																							\
																																\
	return ENTITY_CLASS ## Entities.LastIndex();																				\
}																																\
NO_DISCARD RESTRICTED FIRST_COMPONENT *const RESTRICT ComponentManager::Get ## ENTITY_CLASS ## FIRST_COMPONENT ## s() NOEXCEPT	\
{																																\
	return ENTITY_CLASS ## FIRST_COMPONENT ## s.Data();																			\
}																																\
void ComponentManager::Return ## ENTITY_CLASS ## ComponentsIndex(const uint64 componentsIndex) NOEXCEPT							\
{																																\
	ENTITY_CLASS ## Entities.Back()->SetComponentsIndex(componentsIndex);														\
																																\
	ENTITY_CLASS ## Entities.Erase(componentsIndex);																			\
	ENTITY_CLASS ## FIRST_COMPONENT ## s.Erase(componentsIndex);																\
}


/*
*	Defines an entity class with with two components.
*/
#define DEFINE_ENTITY_CLASS_WITH_TWO_COMPONENTS(ENTITY_CLASS, FIRST_COMPONENT, SECOND_COMPONENT)									\
DynamicArray<Entity *RESTRICT> ComponentManager::ENTITY_CLASS ## Entities;															\
DynamicArray<FIRST_COMPONENT> ComponentManager::ENTITY_CLASS ## FIRST_COMPONENT ## s;												\
DynamicArray<SECOND_COMPONENT> ComponentManager::ENTITY_CLASS ## SECOND_COMPONENT ## s;												\
NO_DISCARD uint64 ComponentManager::GetNumberOf ## ENTITY_CLASS ## Components() NOEXCEPT											\
{																																	\
	return ENTITY_CLASS ## Entities.Size();																							\
}																																	\
NO_DISCARD uint64 ComponentManager::GetNew ## ENTITY_CLASS ## ComponentsIndex(Entity *const RESTRICT entity) NOEXCEPT				\
{																																	\
	ENTITY_CLASS ## Entities.EmplaceSlow(entity);																					\
	ENTITY_CLASS ## FIRST_COMPONENT ## s.EmplaceSlow();																				\
	ENTITY_CLASS ## SECOND_COMPONENT ## s.EmplaceSlow();																			\
																																	\
	return ENTITY_CLASS ## Entities.LastIndex();																					\
}																																	\
NO_DISCARD RESTRICTED FIRST_COMPONENT *const RESTRICT ComponentManager::Get ## ENTITY_CLASS ## FIRST_COMPONENT ## s() NOEXCEPT		\
{																																	\
	return ENTITY_CLASS ## FIRST_COMPONENT ## s.Data();																				\
}																																	\
NO_DISCARD RESTRICTED SECOND_COMPONENT *const RESTRICT ComponentManager::Get ## ENTITY_CLASS ## SECOND_COMPONENT ## s() NOEXCEPT	\
{																																	\
	return ENTITY_CLASS ## SECOND_COMPONENT ## s.Data();																			\
}																																	\
void ComponentManager::Return ## ENTITY_CLASS ## ComponentsIndex(const uint64 componentsIndex) NOEXCEPT								\
{																																	\
	ENTITY_CLASS ## Entities.Back()->SetComponentsIndex(componentsIndex);															\
																																	\
	ENTITY_CLASS ## Entities.Erase(componentsIndex);																				\
	ENTITY_CLASS ## FIRST_COMPONENT ## s.Erase(componentsIndex);																	\
	ENTITY_CLASS ## SECOND_COMPONENT ## s.Erase(componentsIndex);																	\
}

/*
*	Defines an entity class with with three components.
*/
#define DEFINE_ENTITY_CLASS_WITH_THREE_COMPONENTS(ENTITY_CLASS, FIRST_COMPONENT, SECOND_COMPONENT, THIRD_COMPONENT)					\
DynamicArray<Entity *RESTRICT> ComponentManager::ENTITY_CLASS ## Entities;															\
DynamicArray<FIRST_COMPONENT> ComponentManager::ENTITY_CLASS ## FIRST_COMPONENT ## s;												\
DynamicArray<SECOND_COMPONENT> ComponentManager::ENTITY_CLASS ## SECOND_COMPONENT ## s;												\
DynamicArray<THIRD_COMPONENT> ComponentManager::ENTITY_CLASS ## THIRD_COMPONENT ## s;												\
NO_DISCARD uint64 ComponentManager::GetNumberOf ## ENTITY_CLASS ## Components() NOEXCEPT											\
{																																	\
	return ENTITY_CLASS ## Entities.Size();																							\
}																																	\
NO_DISCARD uint64 ComponentManager::GetNew ## ENTITY_CLASS ## ComponentsIndex(Entity *const RESTRICT entity) NOEXCEPT				\
{																																	\
	ENTITY_CLASS ## Entities.EmplaceSlow(entity);																					\
	ENTITY_CLASS ## FIRST_COMPONENT ## s.EmplaceSlow();																				\
	ENTITY_CLASS ## SECOND_COMPONENT ## s.EmplaceSlow();																			\
	ENTITY_CLASS ## THIRD_COMPONENT ## s.EmplaceSlow();																				\
																																	\
	return ENTITY_CLASS ## Entities.LastIndex();																					\
}																																	\
NO_DISCARD RESTRICTED FIRST_COMPONENT *const RESTRICT ComponentManager::Get ## ENTITY_CLASS ## FIRST_COMPONENT ## s() NOEXCEPT		\
{																																	\
	return ENTITY_CLASS ## FIRST_COMPONENT ## s.Data();																				\
}																																	\
NO_DISCARD RESTRICTED SECOND_COMPONENT *const RESTRICT ComponentManager::Get ## ENTITY_CLASS ## SECOND_COMPONENT ## s() NOEXCEPT	\
{																																	\
	return ENTITY_CLASS ## SECOND_COMPONENT ## s.Data();																			\
}																																	\
NO_DISCARD RESTRICTED THIRD_COMPONENT *const RESTRICT ComponentManager::Get ## ENTITY_CLASS ## THIRD_COMPONENT ## s() NOEXCEPT		\
{																																	\
	return ENTITY_CLASS ## THIRD_COMPONENT ## s.Data();																				\
}																																	\
void ComponentManager::Return ## ENTITY_CLASS ## ComponentsIndex(const uint64 componentsIndex) NOEXCEPT								\
{																																	\
	ENTITY_CLASS ## Entities.Back()->SetComponentsIndex(componentsIndex);															\
																																	\
	ENTITY_CLASS ## Entities.Erase(componentsIndex);																				\
	ENTITY_CLASS ## FIRST_COMPONENT ## s.Erase(componentsIndex);																	\
	ENTITY_CLASS ## SECOND_COMPONENT ## s.Erase(componentsIndex);																	\
	ENTITY_CLASS ## THIRD_COMPONENT ## s.Erase(componentsIndex);																	\
}

DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(Camera, CameraComponent);
DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(DirectionalLight, DirectionalLightComponent);
DEFINE_ENTITY_CLASS_WITH_THREE_COMPONENTS(DynamicPhysical, FrustumCullingComponent, DynamicPhysicalRenderComponent, TransformComponent);
DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(InstancedPhysical, InstancedPhysicalRenderComponent);
DEFINE_ENTITY_CLASS_WITH_THREE_COMPONENTS(StaticPhysical, FrustumCullingComponent, StaticPhysicalRenderComponent, TransformComponent);
DEFINE_ENTITY_CLASS_WITH_TWO_COMPONENTS(ParticleSystem, ParticleSystemComponent, ParticleSystemRenderComponent);
DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(PointLight, PointLightComponent);
DEFINE_ENTITY_CLASS_WITH_ONE_COMPONENT(SpotLight, SpotLightComponent);
DEFINE_ENTITY_CLASS_WITH_THREE_COMPONENTS(Terrain, TerrainComponent, FrustumCullingComponent, TerrainRenderComponent);
DEFINE_ENTITY_CLASS_WITH_TWO_COMPONENTS(Vegetation, VegetationComponent, VegetationCullingComponent);