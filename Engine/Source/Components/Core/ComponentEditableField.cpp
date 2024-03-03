//Header file.
#include <Components/Core/ComponentEditableField.h>

//Components.
#include <Components/Core/Component.h>

//Resources.
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/ModelResource.h>

//World.
#include <World/Core/WorldTransform.h>

/*
*	Applies an editable field to initialization data.
*/
void ApplyEditableFieldToInitializationData
(
	const ComponentEditableField &editable_field,
	const void *const RESTRICT data,
	ComponentInitializationData *const RESTRICT initialization_data
) NOEXCEPT
{
	switch (editable_field._Type)
	{
		case ComponentEditableField::Type::MODEL_RESOURCE:
		{
			ResourcePointer<ModelResource> *const RESTRICT destination{ (ResourcePointer<ModelResource> *const RESTRICT)AdvancePointer(initialization_data, editable_field._InitializationDataOffset) };
			const ResourcePointer<ModelResource> *const RESTRICT source{ static_cast<const ResourcePointer<ModelResource> *const RESTRICT>(data) };

			(*destination) = (*source);

			break;
		}

		case ComponentEditableField::Type::WORLD_TRANSFORM:
		{
			const void *const RESTRICT destination{ AdvancePointer(initialization_data, editable_field._InitializationDataOffset) };
			const WorldTransform *const RESTRICT source{ static_cast<const WorldTransform *const RESTRICT>(data) };

			Memory::Copy(destination, source, sizeof(WorldTransform));

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
}

/*
*	Applies an editable field to an entity.
*/
void ApplyEditableFieldToEntity
(
	const ComponentEditableField &editable_field,
	const Component *const RESTRICT component,
	const Entity *const RESTRICT entity,
	const void *const RESTRICT data
) NOEXCEPT
{

}

/*
*	Serializes a component editable field to the given file.
*/
void SerializeEditableField
(
	const ComponentEditableField &editable_field,
	const Component *const RESTRICT component,
	const Entity *const RESTRICT entity,
	BinaryFile<BinaryFileMode::OUT>* const RESTRICT file
) NOEXCEPT
{

}

/*
*	Deserializes a component editable field to the given initialization data.
*/
void DeserializeEditableField
(
	const ComponentEditableField &editable_field,
	const void *const RESTRICT data,
	ComponentInitializationData *const RESTRICT initialization_data
) NOEXCEPT
{

}