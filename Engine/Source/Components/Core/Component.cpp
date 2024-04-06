//Header file.
#include <Components/Core/Component.h>

/*
*	Adds an editable float field.
*/
void Component::AddEditableFloatField
(
	const char *const RESTRICT name,
	const uint64 initialization_data_offset,
	const uint64 instance_data_offset
) NOEXCEPT
{
	ComponentEditableField editable_field;

	editable_field._Name = name;
	editable_field._Identifier = HashString(name);
	editable_field._Type = ComponentEditableField::Type::FLOAT;
	editable_field._InitializationDataOffset = initialization_data_offset;
	editable_field._InstanceDataOffset = instance_data_offset;

	_EditableFields.Emplace(editable_field);
}

/*
*	Adds an editable enumeration field.
*/
void Component::AddEditableEnumerationField
(
	const char *const RESTRICT name,
	const uint64 initialization_data_offset,
	const uint64 instance_data_offset
) NOEXCEPT
{
	ComponentEditableField editable_field;

	editable_field._Name = name;
	editable_field._Identifier = HashString(name);
	editable_field._Type = ComponentEditableField::Type::ENUMERATION;
	editable_field._InitializationDataOffset = initialization_data_offset;
	editable_field._InstanceDataOffset = instance_data_offset;

	_EditableFields.Emplace(editable_field);
}

/*
*	Adds an editable euler angles field.
*/
void Component::AddEditableEulerAnglesField
(
	const char *const RESTRICT name,
	const uint64 initialization_data_offset,
	const uint64 instance_data_offset
) NOEXCEPT
{
	ComponentEditableField editable_field;

	editable_field._Name = name;
	editable_field._Identifier = HashString(name);
	editable_field._Type = ComponentEditableField::Type::EULER_ANGLES;
	editable_field._InitializationDataOffset = initialization_data_offset;
	editable_field._InstanceDataOffset = instance_data_offset;

	_EditableFields.Emplace(editable_field);
}

/*
*	Adds an editable hash string field.
*/
void Component::AddEditableHashStringField
(
	const char *const RESTRICT name,
	const uint64 initialization_data_offset,
	const uint64 instance_data_offset
) NOEXCEPT
{
	ComponentEditableField editable_field;

	editable_field._Name = name;
	editable_field._Identifier = HashString(name);
	editable_field._Type = ComponentEditableField::Type::HASH_STRING;
	editable_field._InitializationDataOffset = initialization_data_offset;
	editable_field._InstanceDataOffset = instance_data_offset;

	_EditableFields.Emplace(editable_field);
}

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
	editable_field._Identifier = HashString(name);
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
	editable_field._Identifier = HashString(name);
	editable_field._Type = ComponentEditableField::Type::MODEL_ASSET;
	editable_field._InitializationDataOffset = initialization_data_offset;
	editable_field._InstanceDataOffset = instance_data_offset;

	_EditableFields.Emplace(editable_field);
}

/*
*	Adds an editable uint32 field.
*/
void Component::AddEditableUint32Field
(
	const char *const RESTRICT name,
	const uint64 initialization_data_offset,
	const uint64 instance_data_offset
) NOEXCEPT
{
	ComponentEditableField editable_field;

	editable_field._Name = name;
	editable_field._Identifier = HashString(name);
	editable_field._Type = ComponentEditableField::Type::UINT32;
	editable_field._InitializationDataOffset = initialization_data_offset;
	editable_field._InstanceDataOffset = instance_data_offset;

	_EditableFields.Emplace(editable_field);
}

/*
*	Adds an editable world vector 2 field.
*/
void Component::AddEditableVector2Field
(
	const char *const RESTRICT name,
	const uint64 initialization_data_offset,
	const uint64 instance_data_offset
) NOEXCEPT
{
	ComponentEditableField editable_field;

	editable_field._Name = name;
	editable_field._Identifier = HashString(name);
	editable_field._Type = ComponentEditableField::Type::VECTOR2;
	editable_field._InitializationDataOffset = initialization_data_offset;
	editable_field._InstanceDataOffset = instance_data_offset;

	_EditableFields.Emplace(editable_field);
}

/*
*	Adds an editable vector 3 field.
*/
void Component::AddEditableVector3Field
(
	const char *const RESTRICT name,
	const uint64 initialization_data_offset,
	const uint64 instance_data_offset
) NOEXCEPT
{
	ComponentEditableField editable_field;

	editable_field._Name = name;
	editable_field._Identifier = HashString(name);
	editable_field._Type = ComponentEditableField::Type::VECTOR3;
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
	editable_field._Identifier = HashString(name);
	editable_field._Type = ComponentEditableField::Type::WORLD_TRANSFORM;
	editable_field._InitializationDataOffset = initialization_data_offset;
	editable_field._InstanceDataOffset = instance_data_offset;

	_EditableFields.Emplace(editable_field);
}