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
	DynamicArray<EntityIdentifier> _EntityToInstanceMappings;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Component(const HashString identifier) NOEXCEPT
		:
		_Identifier(identifier)
	{

	}

	/*
	*	Callback for when a new entity is create.
	*/
	FORCE_INLINE void OnEntityCreated(const EntityIdentifier entity) NOEXCEPT
	{
		for (uint64 i{ _EntityToInstanceMappings.Size() }; i <= entity; ++i)
		{
			_EntityToInstanceMappings.Emplace(UINT64_MAXIMUM);
		}
	}

	/*
	*	Returns if the given entity has this component.
	*/
	FORCE_INLINE NO_DISCARD bool Has(const EntityIdentifier entity) NOEXCEPT
	{
		return _EntityToInstanceMappings[entity] != UINT64_MAXIMUM;
	}

	/*
	*	Creates an instance.
	*/
	virtual void CreateInstance(const EntityIdentifier entity, void *const RESTRICT initialization_data) NOEXCEPT = 0;

	/*
	*	Destroys an instance.
	*/
	virtual void DestroyInstance(const EntityIdentifier entity) NOEXCEPT = 0;

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
*	Returns all components.
*/
NO_DISCARD DynamicArray<Component *RESTRICT> &AllComponents() NOEXCEPT;

/*
*	Declares a component.
*/
#define DECLARE_COMPONENT(COMPONENT_CLASS, GLOBAL_DATA_CLASS, INITIALIZATION_DATA_CLASS, INSTANCE_DATA_CLASS)									\
class ALIGN(8) COMPONENT_CLASS final : public Component																							\
{																																				\
public:																																			\
	DECLARE_SINGLETON(COMPONENT_CLASS);																											\
	static GLOBAL_DATA_CLASS GLOBAL_DATA;																										\
	FORCE_INLINE static INITIALIZATION_DATA_CLASS *const RESTRICT AllocateInitializationData() NOEXCEPT											\
	{																																			\
		SCOPED_LOCK(POOL_ALLOCATOR_LOCK);																										\
		INITIALIZATION_DATA_CLASS *const RESTRICT data{ static_cast<INITIALIZATION_DATA_CLASS *const RESTRICT>(POOL_ALLOCATOR.Allocate()) };	\
		Memory::Set(data, 0, sizeof(INITIALIZATION_DATA_CLASS));																				\
		return data;																															\
	}																																			\
	FORCE_INLINE static void FreeInitializationData(INITIALIZATION_DATA_CLASS *const RESTRICT data) NOEXCEPT									\
	{																																			\
		SCOPED_LOCK(POOL_ALLOCATOR_LOCK);																										\
		POOL_ALLOCATOR.Free(data);																												\
	}																																			\
	FORCE_INLINE COMPONENT_CLASS() NOEXCEPT																										\
		:																																		\
		Component(HashString(#COMPONENT_CLASS))																									\
	{																																			\
		AllComponents().Emplace(this);																											\
	}																																			\
	void CreateInstance(const EntityIdentifier entity, void *const RESTRICT initialization_data) NOEXCEPT override;								\
	void DestroyInstance(const EntityIdentifier entity) NOEXCEPT override;																		\
	FORCE_INLINE NO_DISCARD uint64 NumberOfInstances() const NOEXCEPT override																	\
	{																																			\
		return _InstanceData.Size();																											\
	}																																			\
	void GetUpdateConfiguration(ComponentUpdateConfiguration *const RESTRICT update_configuration) NOEXCEPT override;							\
	void Update(const UpdatePhase update_phase, const uint64 start_index, const uint64 end_index) NOEXCEPT override;							\
	DynamicArray<INSTANCE_DATA_CLASS> &InstanceData() NOEXCEPT																					\
	{																																			\
		return _InstanceData;																													\
	}																																			\
private:																																		\
	static Spinlock POOL_ALLOCATOR_LOCK;																										\
	static PoolAllocator<sizeof(INITIALIZATION_DATA_CLASS)> POOL_ALLOCATOR;																		\
	DynamicArray<INSTANCE_DATA_CLASS> _InstanceData;																							\
	DynamicArray<EntityIdentifier> _EntityIdentifiers;																							\
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
