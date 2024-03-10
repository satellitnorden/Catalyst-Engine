//Header file.
#include <Components/Core/Component.h>

/*
*	Adds an editable material asset field.
*/
void Component::AddEditableMaterialAssetField
(
	const char *const RESTRICT name,
	const uint64 initialization_data_offset,
	const uint64 instance_data_offset
) NOEXCEPT
{
	ComponentEditableField editable_field;

	editable_field._Name = name;
	editable_field._Identifier = HashAlgorithms::MurmurHash64(name, StringUtilities::StringLength(name));
	editable_field._Type = ComponentEditableField::Type::MATERIAL_ASSET;
	editable_field._InitializationDataOffset = initialization_data_offset;
	editable_field._InstanceDataOffset = instance_data_offset;

	_EditableFields.Emplace(editable_field);
}

/*
*	Adds an editable model asset field.
*/
void Component::AddEditableModelAssetField
(
	const char *const RESTRICT name,
	const uint64 initialization_data_offset,
	const uint64 instance_data_offset
) NOEXCEPT
{
	ComponentEditableField editable_field;

	editable_field._Name = name;
	editable_field._Identifier = HashAlgorithms::MurmurHash64(name, StringUtilities::StringLength(name));
	editable_field._Type = ComponentEditableField::Type::MODEL_ASSET;
	editable_field._InitializationDataOffset = initialization_data_offset;
	editable_field._InstanceDataOffset = instance_data_offset;

	_EditableFields.Emplace(editable_field);
}

/*
*	Adds an editable world transform field.
*/
void Component::AddEditableWorldTransformField
(
	const char *const RESTRICT name,
	const uint64 initialization_data_offset,
	const uint64 instance_data_offset
) NOEXCEPT
{
	ComponentEditableField editable_field;

	editable_field._Name = name;
	editable_field._Identifier = HashAlgorithms::MurmurHash64(name, StringUtilities::StringLength(name));
	editable_field._Type = ComponentEditableField::Type::WORLD_TRANSFORM;
	editable_field._InitializationDataOffset = initialization_data_offset;
	editable_field._InstanceDataOffset = instance_data_offset;

	_EditableFields.Emplace(editable_field);
}

/*
*	Returns the lock for all components.
*/
NO_DISCARD Spinlock &_AllComponentsLock() NOEXCEPT
{
	static Spinlock ALL_COMPONENTS_LOCK;

	return ALL_COMPONENTS_LOCK;
}

/*
*	Returns all components.
*/
NO_DISCARD DynamicArray<Component *RESTRICT> &_AllComponents() NOEXCEPT
{
	static DynamicArray<Component *RESTRICT> ALL_COMPONENTS;

	return ALL_COMPONENTS;
}

/*
*	Returns all components.
*/
NO_DISCARD const DynamicArray<Component *RESTRICT> &AllComponents() NOEXCEPT
{
	return _AllComponents();
}

/*
*	Adds a component to all component.
*/
void AddComponentToAllComponents(Component* const RESTRICT component) NOEXCEPT
{
	SCOPED_LOCK(_AllComponentsLock());
	_AllComponents().Emplace(component);
}