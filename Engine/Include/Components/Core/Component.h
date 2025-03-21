#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/HashString.h>

//Components.
#include <Components/Core/ComponentEditableField.h>

//Concurrency.
#include <Concurrency/Spinlock.h>
#include <Concurrency/ScopedLock.h>

//Entities.
#include <Entities/Core/Entity.h>

//Math.
#include <Math/Geometry/Ray.h>

//Memory.
#include <Memory/PoolAllocator.h>

//Forward declarations.
class ComponentInitializationData;

/*
*	Component class definition.
*/
class Component
{

public:

	//The name.
	const char *RESTRICT _Name;

	//The identifier.
	HashString _Identifier;

	//The entity to instance mappings.
	DynamicArray<uint64> _EntityToInstanceMappings;

	//The instance to entity mappings.
	DynamicArray<Entity *RESTRICT> _InstanceToEntityMappings;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Component(const char *const RESTRICT name) NOEXCEPT
		:
		_Name(name),
		_Identifier(name)
	{

	}

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
	*	Returns the editable field data.
	*/
	template <typename TYPE>
	NO_DISCARD TYPE *const RESTRICT EditableFieldData(Entity *const RESTRICT entity, const ComponentEditableField &editable_field) NOEXCEPT
	{
		return static_cast<TYPE *const RESTRICT>(SubEditableFieldData(entity, editable_field));
	}

	/*
	*	Returns the editable fields.
	*/
	FORCE_INLINE NO_DISCARD const DynamicArray<ComponentEditableField> &EditableFields() const NOEXCEPT
	{
		return _EditableFields;
	}

	/*
	*	Callback for before an editable field change happens.	
	*/
	FORCE_INLINE virtual void PreEditableFieldChange(Entity *const RESTRICT entity, const ComponentEditableField &editable_field) NOEXCEPT
	{

	}

	/*
	*	Callback for after an editable field change happens.
	*/
	FORCE_INLINE virtual void PostEditableFieldChange(Entity *const RESTRICT entity, const ComponentEditableField &editable_field) NOEXCEPT
	{

	}

protected:

	/*
	*	Sub-editable field data function.
	*/
	virtual NO_DISCARD void *const RESTRICT SubEditableFieldData(Entity *const RESTRICT entity, const ComponentEditableField &editable_field) NOEXCEPT = 0;

	/*
	*	Adds an editable bool field.
	*/
	void AddEditableBoolField
	(
		const char *const RESTRICT name,
		const uint64 initialization_data_offset,
		const uint64 instance_data_offset
	) NOEXCEPT;

	/*
	*	Adds an editable color field.
	*/
	void AddEditableColorField
	(
		const char *const RESTRICT name,
		const uint64 initialization_data_offset,
		const uint64 instance_data_offset
	) NOEXCEPT;

	/*
	*	Adds an editable float field.
	*/
	void AddEditableFloatField
	(
		const char *const RESTRICT name,
		const uint64 initialization_data_offset,
		const uint64 instance_data_offset
	) NOEXCEPT;

	/*
	*	Adds an editable enumeration field.
	*/
	void AddEditableEnumerationField
	(
		const char *const RESTRICT name,
		const uint64 initialization_data_offset,
		const uint64 instance_data_offset
	) NOEXCEPT;

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
	*	Adds an editable hash string field.
	*/
	void AddEditableHashStringField
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
	*	Adds an editable uint32 field.
	*/
	void AddEditableUint32Field
	(
		const char *const RESTRICT name,
		const uint64 initialization_data_offset,
		const uint64 instance_data_offset
	) NOEXCEPT;

	/*
	*	Adds an editable uint64 field.
	*/
	void AddEditableUint64Field
	(
		const char *const RESTRICT name,
		const uint64 initialization_data_offset,
		const uint64 instance_data_offset
	) NOEXCEPT;

	/*
	*	Adds an editable vector 2 field.
	*/
	void AddEditableVector2Field
	(
		const char *const RESTRICT name,
		const uint64 initialization_data_offset,
		const uint64 instance_data_offset
	) NOEXCEPT;

	/*
	*	Adds an editable vector 3 field.
	*/
	void AddEditableVector3Field
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

	////////////////////////////////////////////////////////
	// The below functions are common for all components! //
	////////////////////////////////////////////////////////

public:

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

	//////////////////////////////////////////////////////////////////////////////////////////
	// The below functions are automatically added to a component with CATALYST_COMPONENT()! //
	//////////////////////////////////////////////////////////////////////////////////////////

public:

	/*
	*	Returns the number of instances.
	*/
	virtual NO_DISCARD uint64 NumberOfInstances() const NOEXCEPT = 0;

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
*	"Manager" class for all components.
*/
class Components final
{

public:

	/*
	*	Returns the number of components.
	*/
	static NO_DISCARD uint64 Size() NOEXCEPT;

	/*
	*	Returns the component at the given index.
	*/
	static NO_DISCARD Component *const RESTRICT At(const uint64 index) NOEXCEPT;

	/*
	*	Initializes components.
	*/
	static void Initialize() NOEXCEPT;

	/*
	*	Post-initializes components.
	*/
	static void PostInitialize() NOEXCEPT;

	/*
	*	Allocates initialization data from the given component.
	*/
	static NO_DISCARD ComponentInitializationData *const RESTRICT AllocateInitializationData(Component *const RESTRICT component) NOEXCEPT;

	/*
	*	Defaults initialization data for the given component.
	*/
	static void DefaultInitializationData(Component *const RESTRICT component, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT;

	/*
	*	Frees initialization data for the given component.
	*/
	static void FreeInitializationData(Component *const RESTRICT component, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT;

	/*
	*	Returns if the given component needs pre processing.
	*/
	static NO_DISCARD bool ShouldPreProcess(Component *const RESTRICT component) NOEXCEPT;

	/*
	*	Pre processed the initialization data for the given component.
	*/
	static void PreProcess(Component *const RESTRICT component, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT;

	/*
	*	Creates an instance and calls 'CreateInstance' for the given component.
	*/
	static void CreateInstance(Component *const RESTRICT component, Entity *const RESTRICT entity, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT;

	/*
	*	Calls 'PostCreateInstance' for the given component with the given entity.
	*/
	static void PostCreateInstance(Component *const RESTRICT component, Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Updates components during the specific update phase.
	*/
	static void Update(const UpdatePhase update_phase) NOEXCEPT;

	/*
	*	Terminates components.
	*/
	static void Terminate() NOEXCEPT;

	/*
	*	Given a ray and a collection of entities, tries to select an entity and returns the selected entity, or nullptr of none was selected.
	*/
	static NO_DISCARD Entity *const RESTRICT Select(const Ray &ray, const ArrayProxy<Entity *RESTRICT> entities) NOEXCEPT;

	/*
	*	Let's components know that the game is starting.
	*/
	static void StartGame() NOEXCEPT;

	/*
	*	Gathers path tracing triangles.
	*/
	static void GatherPathTracingTriangles(DynamicArray<class Vertex> *const RESTRICT vertices, DynamicArray<class PathTracingTriangle> *const RESTRICT triangles) NOEXCEPT;

};

/*
*	Put this in your component declaration and implement it to receive an "Initialize()" call.
*/
#define COMPONENT_INITIALIZE()		\
public:								\
	void Initialize() NOEXCEPT;

/*
*	Put this in your component declaration and implement it to receive a "PostInitialize()" call.
*/
#define COMPONENT_POST_INITIALIZE()	\
public:								\
	void PostInitialize() NOEXCEPT;

/*
*	Put this in your component declaration and implement it to receive a "DefaultInitializationData()" call.
*/
#define COMPONENT_DEFAULT_INITIALIZATION_DATA(INITIALIZATION_DATA_CLASS)									\
public:																										\
	void DefaultInitializationData(INITIALIZATION_DATA_CLASS *const RESTRICT initialization_data) NOEXCEPT;

/*
*	Put this in your component declaration and implement it to receive a "PreProcess()" call.
*/
#define COMPONENT_PRE_PROCESS(INITIALIZATION_DATA_CLASS)									\
public:																						\
	void PreProcess(INITIALIZATION_DATA_CLASS *const RESTRICT initialization_data) NOEXCEPT;

/*
*	Put this in your component declaration and implement it to receive a "PostCreatInstance()" call.
*	This function runs after all components have created their instance for the given entity.
*	Useful if there is some setup needed involving multiple components.
*/
#define COMPONENT_POST_CREATE_INSTANCE()							\
public:																\
	void PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT;

/*
*	Put this in your component declaration and implement it to receive a "SerialUpdate()" call during the specified update phase.
*	Can optionally add "Before(X)" or "After(X)" to control ordering of updates.
*/
#define COMPONENT_SERIAL_UPDATE(UPDATE_PHASE, ...)				\
public:															\
	void SerialUpdate(const UpdatePhase update_phase) NOEXCEPT;

/*
*	Put this in your component declaration and implement it to receive a "ParallelBatchUpdate()" call during the specified update phase.
*	Needs to specify the batch size.
*	Can optionally add "Before(X)" or "After(X)" to control ordering of updates.
*/
#define COMPONENT_PARALLEL_BATCH_UPDATE(UPDATE_PHASE, BATCH_SIZE, ...)																		\
public:																																		\
	void ParallelBatchUpdate(const UpdatePhase update_phase, const uint64 start_instance_index, const uint64 end_instance_index) NOEXCEPT;

/*
*	Put this in your component declaration and implement it to receive a "ParallelSubInstanceUpdate()" call during the specified update phase.
*	Can optionally add "Before(X)" or "After(X)" to control ordering of updates.
*/
#define COMPONENT_PARALLEL_SUB_INSTANCE_UPDATE(UPDATE_PHASE, ...)																			\
public:																																		\
	NO_DISCARD uint64 NumberOfSubInstances(const uint64 instance_index) const NOEXCEPT;														\
	void ParallelSubInstanceUpdate(const UpdatePhase update_phase, const uint64 instance_index, const uint64 sub_instance_index) NOEXCEPT;

/*
*	Put this in your component declaration and implement it to receive an "PostUpdate()" call.
*/
#define COMPONENT_POST_UPDATE(UPDATE_PHASE)						\
public:															\
	void PostUpdate(const UpdatePhase update_phase) NOEXCEPT;

/*
*	Put this in your component declaration and implement it to receive a "Terminate()" call.
*/
#define COMPONENT_TERMINATE()	\
public:							\
	void Terminate() NOEXCEPT;

/*
*	Put this in your component declaration and implement the "Select()" function for entities with this component to be able to be selected.
*	Should return whether or not this entity was hit, and if so, update the 'hit_distance' argument.
*/
#define COMPONENT_SELECT()																									\
public:																														\
	NO_DISCARD bool Select(const Ray &ray, Entity *const RESTRICT entity, float32 *const RESTRICT hit_distance) NOEXCEPT;

/*
*	Put this in your component declaration and implement the "StartGame()" function for this component to get a callback when the game starts.
*/
#define COMPONENT_START_GAME()	\
public:							\
	void StartGame() NOEXCEPT;

/*
*	Put this in your component declaration and implement the "GatherPathTracingTriangles()" function
*	for this component to get a callback when path tracing triangles are being gathered.
*	Should include "PathTracing/PathTracingCore.h" in the .cpp file.
*/
#define COMPONENT_GATHER_PATH_TRACING_TRIANGLES()																														\
public:																																									\
	void GatherPathTracingTriangles(DynamicArray<class Vertex> *const RESTRICT vertices, DynamicArray<class PathTracingTriangle> *const RESTRICT triangles) NOEXCEPT;

/*
*	Declares a Catalyst component.
*	Put this within the class.
*	This will define an "Instance" static variable within the class that is automatically allocated by the engine.
*	This will also delete copy constructors, making this component a singleton.
*
*	Can optionally add arguments, see above which ones is supported.
*/
#define CATALYST_COMPONENT(X, ...)																																			\
public:																																										\
	static X##Component *RESTRICT Instance;																																	\
	X##Component(const X##Component &other) = delete;																														\
	X##Component(X##Component &&other) = delete;																															\
	FORCE_INLINE X##Component() NOEXCEPT																																	\
		:																																									\
		Component(#X "Component")																																			\
	{																																										\
	}																																										\
	FORCE_INLINE X##InitializationData *const RESTRICT AllocateInitializationData() NOEXCEPT																				\
	{																																										\
		SCOPED_LOCK(POOL_ALLOCATOR_LOCK);																																	\
		X##InitializationData *const RESTRICT data{ new (POOL_ALLOCATOR.Allocate()) X##InitializationData() };																\
		Components::DefaultInitializationData(this, data);																													\
		data->_Component = Instance;																																		\
		return data;																																						\
	}																																										\
	FORCE_INLINE void FreeInitializationData(ComponentInitializationData *const RESTRICT data) NOEXCEPT																		\
	{																																										\
		SCOPED_LOCK(POOL_ALLOCATOR_LOCK);																																	\
		POOL_ALLOCATOR.Free(data);																																			\
	}																																										\
	void CreateInstance(Entity *const RESTRICT entity, X##InitializationData *const RESTRICT initialization_data, X##InstanceData *const RESTRICT instance_data) NOEXCEPT;	\
	void DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT override;																									\
	FORCE_INLINE NO_DISCARD uint64 NumberOfInstances() const NOEXCEPT override																								\
	{																																										\
		return _InstanceData.Size();																																		\
	}																																										\
	FORCE_INLINE NO_DISCARD DynamicArray<X##InstanceData> &InstanceData() NOEXCEPT																							\
	{																																										\
		return _InstanceData;																																				\
	}																																										\
	FORCE_INLINE NO_DISCARD X##InstanceData &InstanceData(Entity *const RESTRICT entity) NOEXCEPT																			\
	{																																										\
		return _InstanceData[EntityToInstance(entity)];																														\
	}																																										\
	FORCE_INLINE void RemoveInstance(Entity *const RESTRICT entity) NOEXCEPT																								\
	{																																										\
		const uint64 instance_index{ _EntityToInstanceMappings[entity->_EntityIdentifier] };																				\
		if (instance_index == _InstanceData.LastIndex())																													\
		{																																									\
			_InstanceData.Pop();																																			\
			_InstanceToEntityMappings.Pop();																																\
			_EntityToInstanceMappings[entity->_EntityIdentifier] = UINT64_MAXIMUM;																							\
		}																																									\
		else																																								\
		{																																									\
			Entity *const RESTRICT moved_entity{ _InstanceToEntityMappings.Back() };																						\
			_InstanceData.EraseAt<false>(instance_index);																													\
			_InstanceToEntityMappings.EraseAt<false>(instance_index);																										\
			_EntityToInstanceMappings[entity->_EntityIdentifier] = UINT64_MAXIMUM;																							\
			_EntityToInstanceMappings[moved_entity->_EntityIdentifier] = instance_index;																					\
		}																																									\
	}																																										\
	FORCE_INLINE NO_DISCARD void *const RESTRICT SubEditableFieldData																										\
	(																																										\
		Entity *const RESTRICT entity,																																		\
		const ComponentEditableField &editable_field																														\
	) NOEXCEPT override																																						\
	{																																										\
		X##InstanceData &instance_data{ InstanceData(entity) };																												\
		return AdvancePointer(&instance_data, editable_field._InstanceDataOffset);																							\
	}																																										\
private:																																									\
	friend class Components;																																				\
	Spinlock POOL_ALLOCATOR_LOCK;																																			\
	PoolAllocator<sizeof(X##InitializationData)> POOL_ALLOCATOR;																											\
	DynamicArray<X##InstanceData> _InstanceData;																															\
																																											\
	__VA_ARGS__																																								\