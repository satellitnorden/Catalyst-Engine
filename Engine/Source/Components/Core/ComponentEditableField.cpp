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
*	Serializes a component editable field to the given stream archive.
*/
void SerializeEditableField
(
	const ComponentEditableField &editable_field,
	Component *const RESTRICT component,
	Entity *const RESTRICT entity,
	StreamArchive *const RESTRICT stream_archive
) NOEXCEPT
{
	//Write the editable field identifier.
	stream_archive->Write(editable_field._Identifier);

	//Retrieve the data.
	const void *const RESTRICT data{ component->EditableFieldData<void>(entity, editable_field) };

	switch (editable_field._Type)
	{
		case ComponentEditableField::Type::MODEL_RESOURCE:
		{
			//Cast the data.
			const ResourcePointer<ModelResource> *const RESTRICT _data{ static_cast<const ResourcePointer<ModelResource> *const RESTRICT>(data) };

			//Can't really serialize a pointer, so serialize the resource identifier.
			stream_archive->Write((*_data)->_Header._ResourceIdentifier);

			break;
		}

		case ComponentEditableField::Type::WORLD_TRANSFORM:
		{
			//Cast the data.
			const WorldTransform *const RESTRICT _data{ static_cast<const WorldTransform *const RESTRICT>(data) };

			//This can just be copied directly.
			stream_archive->Write(*_data);

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