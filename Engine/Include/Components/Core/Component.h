#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/HashString.h>

//Components.
#include <Components/Core/ComponentEditableField.h>

//Concurrency.
#include <Concurrency/Spinlock.h>
#include <Concurrency/ScopedLock.h>

//Entities.
#include <Entities/Core/Entity.h>

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

	//Enumeration covering all modes.
	enum class Mode : uint8
	{
		/*
		*	Will update instances in batches of '_BatchSize'.
		*/
		BATCH,

		/*
		*	Will update each instance in parallel, based on how many sub instances each instance has.
		*/
		SUB_INSTANCE
	};

	//The update phase mask.
	UpdatePhase _UpdatePhaseMask{ static_cast<UpdatePhase>(0) };

	//The mode.
	Mode _Mode;

	/*
	*	The batch size.
	*	Only used then '_Mode' is set to Mode::BATCH.
	*/
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
	DynamicArray<Entity *RESTRICT> _InstanceToEntityMappings;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Component(const HashString identifier) NOEXCEPT
		:
		_Identifier(identifier)
	{

	}

	/*
	*	Initializes this component.
	*/
	virtual void Initialize() NOEXCEPT = 0;

	/*
	*	Post-Initializes this component.
	*/
	virtual void PostInitialize() NOEXCEPT = 0;

	/*
	*	Terminates this component.
	*/
	virtual void Terminate() NOEXCEPT = 0;

	/*
	*	Allocations initialization data.
	*/
	virtual NO_DISCARD ComponentInitializationData *const RESTRICT AllocateInitializationData() NOEXCEPT = 0;

	/*
	*	Sets default values for initialization data.
	*/
	FORCE_INLINE virtual void DefaultInitializationData(ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
	{

	}

	/*
	*	Frees initialization data.
	*/
	virtual void FreeInitializationData(ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT = 0;

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
	FORCE_INLINE void PreCreateInstance(Entity *const RESTRICT entity) NOEXCEPT
	{
		for (uint64 i{ _EntityToInstanceMappings.Size() }; i <= entity->_EntityIdentifier; ++i)
		{
			_EntityToInstanceMappings.Emplace(UINT64_MAXIMUM);
		}

		_EntityToInstanceMappings[entity->_EntityIdentifier] = _InstanceToEntityMappings.Size();
		_InstanceToEntityMappings.Emplace(entity);
	}

	/*
	*	Creates an instance.
	*/
	virtual void CreateInstance(Entity *const RESTRICT entity, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT = 0;

	/*
	*	Runs after all components have created their instance for the given entity.
	*	Useful if there is some setup needed involving multiple components.
	*/
	virtual void PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT = 0;

	/*
	*	Destroys an instance.
	*/
	virtual void DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT = 0;

	/*
	*	Returns if the given entity has this component.
	*/
	FORCE_INLINE NO_DISCARD bool Has(Entity *const RESTRICT entity) const NOEXCEPT
	{
		return entity->_EntityIdentifier < _EntityToInstanceMappings.Size() && _EntityToInstanceMappings[entity->_EntityIdentifier] != UINT64_MAXIMUM;
	}

	/*
	*	Returns the instance index for the given entity.
	*/
	FORCE_INLINE NO_DISCARD uint64 EntityToInstance(Entity *const RESTRICT entity) const NOEXCEPT
	{
		return _EntityToInstanceMappings[entity->_EntityIdentifier];
	}

	/*
	*	Returns the entity index for the given instance index.
	*/
	FORCE_INLINE NO_DISCARD Entity *const RESTRICT InstanceToEntity(const uint64 instance_index) const NOEXCEPT
	{
		return _InstanceToEntityMappings[instance_index];
	}

	/*
	*	Returns the number of instances.
	*/
	virtual NO_DISCARD uint64 NumberOfInstances() const NOEXCEPT = 0;

	/*
	*	Returns the number of sub-instances for the given instance.
	*/
	virtual NO_DISCARD uint64 NumberOfSubInstances(const uint64 instance_index) const NOEXCEPT = 0;

	/*
	*	Returns the update configuration.
	*/
	virtual void GetUpdateConfiguration(ComponentUpdateConfiguration *const RESTRICT update_configuration) NOEXCEPT = 0;

	/*
	*	Runs before the given update phase.
	*/
	virtual void PreUpdate(const UpdatePhase update_phase) NOEXCEPT = 0;

	/*
	*	Updates this component.
	*/
	virtual void Update
	(
		const UpdatePhase update_phase,
		const uint64 start_instance_index,
		const uint64 end_instance_index,
		const uint64 sub_instance_index
	) NOEXCEPT = 0;

	/*
	*	Runs after the given update phase.
	*/
	virtual void PostUpdate(const UpdatePhase update_phase) NOEXCEPT = 0;

	/*
	*	Returns the editable field data.
	*/
	template <typename TYPE>
	NO_DISCARD TYPE *const RESTRICT EditableFieldData(Entity *const RESTRICT entity, const ComponentEditableField &editable_field) NOEXCEPT
	{
		return static_cast<TYPE *const RESTRICT>(SubEditableFieldData(entity, editable_field));
	}

	/*
	*	Returns the name of this component.
	*/
	virtual NO_DISCARD const char *const RESTRICT Name() const NOEXCEPT = 0;

	/*
	*	Returns the editable fields.
	*/
	FORCE_INLINE NO_DISCARD const DynamicArray<ComponentEditableField> &EditableFields() const NOEXCEPT
	{
		return _EditableFields;
	}

protected:

	/*
	*	Sub-editable field data function.
	*/
	virtual NO_DISCARD void *const RESTRICT SubEditableFieldData(Entity *const RESTRICT entity, const ComponentEditableField &editable_field) NOEXCEPT = 0;

	/*
	*	Adds an editable euler angles field.
	*/
	void AddEditableEulerAnglesField
	(
		const char *const RESTRICT name,
		const uint64 initialization_data_offset,
		const uint64 instance_data_offset
	) NOEXCEPT;

	/*
	*	Adds an editable asset resource field.
	*/
	void AddEditableMaterialAssetField
	(
		const char *const RESTRICT name,
		const uint64 initialization_data_offset,
		const uint64 instance_data_offset
	) NOEXCEPT;

	/*
	*	Adds an editable model asset field.
	*/
	void AddEditableModelAssetField
	(
		const char *const RESTRICT name,
		const uint64 initialization_data_offset,
		const uint64 instance_data_offset
	) NOEXCEPT;

	/*
	*	Adds an editable world transform field.
	*/
	void AddEditableWorldTransformField
	(
		const char *const RESTRICT name,
		const uint64 initialization_data_offset,
		const uint64 instance_data_offset
	) NOEXCEPT;

private:

	//The editable fields.
	DynamicArray<ComponentEditableField> _EditableFields;

};

/*
*	Component initialization data class definition.
*/
class ComponentInitializationData
{

public:

	//The component.
	Component *RESTRICT _Component;

	//The next component initialization data.
	ComponentInitializationData *RESTRICT _NextComponentInitializationData;

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
#define DECLARE_COMPONENT(COMPONENT_CLASS, INITIALIZATION_DATA_CLASS, INSTANCE_DATA_CLASS, ...)									\
static_assert(std::is_convertible<INITIALIZATION_DATA_CLASS*, ComponentInitializationData*>::value, "Incorrect inheritance");					\
class ALIGN(8) COMPONENT_CLASS final : public Component																							\
{																																				\
public:																																			\
	DECLARE_SINGLETON(COMPONENT_CLASS);																											\
	void Initialize() NOEXCEPT override;																										\
	void PostInitialize() NOEXCEPT override;																									\
	void Terminate() NOEXCEPT override;																											\
	FORCE_INLINE INITIALIZATION_DATA_CLASS *const RESTRICT AllocateDerivedInitializationData() NOEXCEPT											\
	{																																			\
		SCOPED_LOCK(POOL_ALLOCATOR_LOCK);																										\
		INITIALIZATION_DATA_CLASS *const RESTRICT data{ static_cast<INITIALIZATION_DATA_CLASS *const RESTRICT>(POOL_ALLOCATOR.Allocate()) };	\
		Memory::Set(data, 0, sizeof(INITIALIZATION_DATA_CLASS));																				\
		DefaultInitializationData(data);																										\
		data->_Component = COMPONENT_CLASS::Instance.Get();																						\
		return data;																															\
	}																																			\
	FORCE_INLINE NO_DISCARD ComponentInitializationData *const RESTRICT AllocateInitializationData() NOEXCEPT override							\
	{																																			\
		return AllocateDerivedInitializationData();																								\
	}																																			\
	FORCE_INLINE void FreeInitializationData(ComponentInitializationData *const RESTRICT data) NOEXCEPT override								\
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
	void CreateInstance(Entity *const RESTRICT entity, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT override;		\
	void PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT override;																	\
	void DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT override;																		\
	FORCE_INLINE NO_DISCARD uint64 NumberOfInstances() const NOEXCEPT override																	\
	{																																			\
		return _InstanceData.Size();																											\
	}																																			\
	NO_DISCARD uint64 NumberOfSubInstances(const uint64 instance_index) const NOEXCEPT override;												\
	FORCE_INLINE NO_DISCARD DynamicArray<INSTANCE_DATA_CLASS> &InstanceData() NOEXCEPT															\
	{																																			\
		return _InstanceData;																													\
	}																																			\
	FORCE_INLINE NO_DISCARD INSTANCE_DATA_CLASS &InstanceData(Entity *const RESTRICT entity) NOEXCEPT											\
	{																																			\
		return _InstanceData[EntityToInstance(entity)];																							\
	}																																			\
	void GetUpdateConfiguration(ComponentUpdateConfiguration *const RESTRICT update_configuration) NOEXCEPT override;							\
	void PreUpdate(const UpdatePhase update_phase) NOEXCEPT override;																			\
	void Update																																	\
	(																																			\
		const UpdatePhase update_phase,																											\
		const uint64 start_instance_index,																										\
		const uint64 end_instance_index,																										\
		const uint64 sub_instance_index																											\
	) NOEXCEPT override;																														\
	void PostUpdate(const UpdatePhase update_phase) NOEXCEPT override;																			\
	FORCE_INLINE void RemoveInstance(Entity *const RESTRICT entity) NOEXCEPT																	\
	{																																			\
		const uint64 instance_index{ _EntityToInstanceMappings[entity->_EntityIdentifier] };													\
		if (instance_index == _InstanceData.LastIndex())																						\
		{																																		\
			_InstanceData.Pop();																												\
			_InstanceToEntityMappings.Pop();																									\
			_EntityToInstanceMappings[entity->_EntityIdentifier] = UINT64_MAXIMUM;																\
		}																																		\
		else																																	\
		{																																		\
			Entity *const RESTRICT moved_entity{ _InstanceToEntityMappings.Back() };															\
			_InstanceData.EraseAt<false>(instance_index);																						\
			_InstanceToEntityMappings.EraseAt<false>(instance_index);																			\
			_EntityToInstanceMappings[entity->_EntityIdentifier] = UINT64_MAXIMUM;																\
			_EntityToInstanceMappings[moved_entity->_EntityIdentifier] = instance_index;														\
		}																																		\
	}																																			\
	FORCE_INLINE NO_DISCARD void *const RESTRICT SubEditableFieldData																			\
	(																																			\
		Entity *const RESTRICT entity,																											\
		const ComponentEditableField &editable_field																							\
	) NOEXCEPT override																															\
	{																																			\
		INSTANCE_DATA_CLASS &instance_data{ InstanceData(entity) };																				\
		return AdvancePointer(&instance_data, editable_field._InstanceDataOffset);																\
	}																																			\
	FORCE_INLINE NO_DISCARD const char *const RESTRICT Name() const NOEXCEPT override															\
	{																																			\
		return #COMPONENT_CLASS;																												\
	}																																			\
private:																																		\
	Spinlock POOL_ALLOCATOR_LOCK;																												\
	PoolAllocator<sizeof(INITIALIZATION_DATA_CLASS)> POOL_ALLOCATOR;																			\
	DynamicArray<INSTANCE_DATA_CLASS> _InstanceData;																							\
																																				\
__VA_ARGS__																																		\
};																																				\

/*
*	Defines a component.
*	This needs to be in ONE, and only ONE .cpp file.
*/
#define DEFINE_COMPONENT(COMPONENT_CLASS, INITIALIZATION_DATA_CLASS, INSTANCE_CLASS)	\
DEFINE_SINGLETON(COMPONENT_CLASS);														\
