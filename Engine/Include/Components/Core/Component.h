#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/HashString.h>

//Concurrency.
#include <Concurrency/Spinlock.h>
#include <Concurrency/ScopedLock.h>

//Entities.
#include <Entities/Core/EntityCore.h>

//Memory.
#include <Memory/PoolAllocator.h>

//Forward declarations.
class ComponentInitializationData;

/*
*	Component update configuration class definition.
*/
class ComponentUpdateConfiguration final
{

public:

	//The update phase mask.
	UpdatePhase _UpdatePhaseMask{ static_cast<UpdatePhase>(0) };

	//The batch size.
	uint64 _BatchSize;

};

/*
*	Component class definition.
*/
class Component
{

public:

	//The identifier.
	HashString _Identifier;

	//The entity to instance mappings.
	DynamicArray<uint64> _EntityToInstanceMappings;

	//The instance to entity mappings.
	DynamicArray<EntityIdentifier> _InstanceToEntityMappings;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Component(const HashString identifier) NOEXCEPT
		:
		_Identifier(identifier)
	{

	}

	/*
	*	Returns if this component needs pre-processing.
	*/
	virtual NO_DISCARD bool NeedsPreProcessing() const NOEXCEPT = 0;

	/*
	*	Preprocessed initialization data an instance.
	*/
	virtual void PreProcess(ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT = 0;

	/*
	*	Runs before an instance is created.
	*/
	FORCE_INLINE void PreCreateInstance(const EntityIdentifier entity) NOEXCEPT
	{
		for (uint64 i{ _EntityToInstanceMappings.Size() }; i <= entity; ++i)
		{
			_EntityToInstanceMappings.Emplace(UINT64_MAXIMUM);
		}

		_EntityToInstanceMappings[entity] = _InstanceToEntityMappings.Size();
		_InstanceToEntityMappings.Emplace(entity);
	}

	/*
	*	Creates an instance.
	*/
	virtual void CreateInstance(const EntityIdentifier entity, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT = 0;

	/*
	*	Runs after all components have created their instance for the given entity.
	*	Useful if there is some setup needed involving multiple components.
	*/
	virtual void PostCreateInstance(const EntityIdentifier entity) NOEXCEPT = 0;

	/*
	*	Destroys an instance.
	*/
	virtual void DestroyInstance(const EntityIdentifier entity) NOEXCEPT = 0;

	/*
	*	Returns if the given entity has this component.
	*/
	FORCE_INLINE NO_DISCARD bool Has(const EntityIdentifier entity) NOEXCEPT
	{
		return entity < _EntityToInstanceMappings.Size() && _EntityToInstanceMappings[entity] != UINT64_MAXIMUM;
	}

	/*
	*	Returns the instance index for the given entity.
	*/
	FORCE_INLINE NO_DISCARD uint64 EntityToInstance(const EntityIdentifier entity) const NOEXCEPT
	{
		return _EntityToInstanceMappings[entity];
	}

	/*
	*	Returns the entity index for the given instance index.
	*/
	FORCE_INLINE NO_DISCARD EntityIdentifier InstanceToEntity(const uint64 instance_index) const NOEXCEPT
	{
		return _InstanceToEntityMappings[instance_index];
	}

	/*
	*	Returns the number of instances.
	*/
	virtual NO_DISCARD uint64 NumberOfInstances() const NOEXCEPT = 0;

	/*
	*	Returns the update configuration.
	*/
	virtual void GetUpdateConfiguration(ComponentUpdateConfiguration *const RESTRICT update_configuration) NOEXCEPT = 0;

	/*
	*	Updates this component.
	*/
	virtual void Update(const UpdatePhase update_phase, const uint64 start_index, const uint64 end_index) NOEXCEPT = 0;

};

/*
*	Component initialization data class definition.
*/
class ComponentInitializationData
{

public:

	//The component.
	Component *RESTRICT _Component;

};

/*
*	Returns all components.
*/
NO_DISCARD const DynamicArray<Component *RESTRICT> &AllComponents() NOEXCEPT;

/*
*	Adds a component to all component.
*/
void AddComponentToAllComponents(Component *const RESTRICT component) NOEXCEPT;

/*
*	Declares a component.
*/
#define DECLARE_COMPONENT(COMPONENT_CLASS, GLOBAL_DATA_CLASS, INITIALIZATION_DATA_CLASS, INSTANCE_DATA_CLASS)									\
static_assert(std::is_convertible<INITIALIZATION_DATA_CLASS*, ComponentInitializationData*>::value, "Incorrect inheritance");					\
class ALIGN(8) COMPONENT_CLASS final : public Component																							\
{																																				\
public:																																			\
	DECLARE_SINGLETON(COMPONENT_CLASS);																											\
	static GLOBAL_DATA_CLASS GLOBAL_DATA;																										\
	FORCE_INLINE INITIALIZATION_DATA_CLASS *const RESTRICT AllocateInitializationData() NOEXCEPT												\
	{																																			\
		SCOPED_LOCK(POOL_ALLOCATOR_LOCK);																										\
		INITIALIZATION_DATA_CLASS *const RESTRICT data{ static_cast<INITIALIZATION_DATA_CLASS *const RESTRICT>(POOL_ALLOCATOR.Allocate()) };	\
		Memory::Set(data, 0, sizeof(INITIALIZATION_DATA_CLASS));																				\
		data->_Component = COMPONENT_CLASS::Instance.Get();																						\
		return data;																															\
	}																																			\
	FORCE_INLINE void FreeInitializationData(INITIALIZATION_DATA_CLASS *const RESTRICT data) NOEXCEPT											\
	{																																			\
		SCOPED_LOCK(POOL_ALLOCATOR_LOCK);																										\
		POOL_ALLOCATOR.Free(data);																												\
	}																																			\
	FORCE_INLINE COMPONENT_CLASS() NOEXCEPT																										\
		:																																		\
		Component(HashString(#COMPONENT_CLASS))																									\
	{																																			\
		AddComponentToAllComponents(this);																										\
	}																																			\
	NO_DISCARD bool NeedsPreProcessing() const NOEXCEPT override;																				\
	void PreProcess(ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT override;											\
	void CreateInstance(const EntityIdentifier entity, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT override;		\
	void PostCreateInstance(const EntityIdentifier entity) NOEXCEPT override;																	\
	void DestroyInstance(const EntityIdentifier entity) NOEXCEPT override;																		\
	FORCE_INLINE NO_DISCARD uint64 NumberOfInstances() const NOEXCEPT override																	\
	{																																			\
		return _InstanceData.Size();																											\
	}																																			\
	FORCE_INLINE NO_DISCARD DynamicArray<INSTANCE_DATA_CLASS> &InstanceData() NOEXCEPT															\
	{																																			\
		return _InstanceData;																													\
	}																																			\
	FORCE_INLINE NO_DISCARD INSTANCE_DATA_CLASS &InstanceData(const EntityIdentifier entity) NOEXCEPT											\
	{																																			\
		return _InstanceData[EntityToInstance(entity)];																							\
	}																																			\
	void GetUpdateConfiguration(ComponentUpdateConfiguration *const RESTRICT update_configuration) NOEXCEPT override;							\
	void Update(const UpdatePhase update_phase, const uint64 start_index, const uint64 end_index) NOEXCEPT override;							\
	FORCE_INLINE void RemoveInstance(const EntityIdentifier entity) NOEXCEPT																	\
	{																																			\
		const uint64 instance_index{ _EntityToInstanceMappings[entity] };																		\
		if (instance_index == _InstanceData.LastIndex())																						\
		{																																		\
			_InstanceData.Pop();																												\
			_InstanceToEntityMappings.Pop();																									\
			_EntityToInstanceMappings[entity] = UINT64_MAXIMUM;																					\
		}																																		\
		else																																	\
		{																																		\
			const EntityIdentifier moved_entity_identifier{ _InstanceToEntityMappings.Back() };													\
			_InstanceData.EraseAt<false>(instance_index);																						\
			_InstanceToEntityMappings.EraseAt<false>(instance_index);																			\
			_EntityToInstanceMappings[entity] = UINT64_MAXIMUM;																					\
			_EntityToInstanceMappings[moved_entity_identifier] = instance_index;																\
		}																																		\
	}																																			\
private:																																		\
	static Spinlock POOL_ALLOCATOR_LOCK;																										\
	static PoolAllocator<sizeof(INITIALIZATION_DATA_CLASS)> POOL_ALLOCATOR;																		\
	DynamicArray<INSTANCE_DATA_CLASS> _InstanceData;																							\
};																																				\

/*
*	Defines a component.
*	This needs to be in ONE, and only ONE .cpp file.
*/
#define DEFINE_COMPONENT(COMPONENT_CLASS, GLOBAL_DATA_CLASS, INITIALIZATION_DATA_CLASS, INSTANCE_CLASS)	\
DEFINE_SINGLETON(COMPONENT_CLASS);																		\
GLOBAL_DATA_CLASS COMPONENT_CLASS::GLOBAL_DATA;															\
Spinlock COMPONENT_CLASS::POOL_ALLOCATOR_LOCK;															\
PoolAllocator<sizeof(INITIALIZATION_DATA_CLASS)> COMPONENT_CLASS::POOL_ALLOCATOR;						\
