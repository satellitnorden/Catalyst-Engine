//Header file.
#include <Components/Core/ComponentEditableField.h>

//Components.
#include <Components/Core/Component.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/MaterialAsset.h>
#include <Content/Assets/ModelAsset.h>

//Systems.
#include <Systems/ContentSystem.h>

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
		case ComponentEditableField::Type::MATERIAL_ASSET:
		{
			AssetPointer<MaterialAsset> *const RESTRICT destination{ (AssetPointer<MaterialAsset> *const RESTRICT)AdvancePointer(initialization_data, editable_field._InitializationDataOffset) };
			const AssetPointer<MaterialAsset> *const RESTRICT source{ static_cast<const AssetPointer<MaterialAsset> *const RESTRICT>(data) };

			(*destination) = (*source);

			break;
		}

		case ComponentEditableField::Type::MODEL_ASSET:
		{
			AssetPointer<ModelAsset> *const RESTRICT destination{ (AssetPointer<ModelAsset> *const RESTRICT)AdvancePointer(initialization_data, editable_field._InitializationDataOffset) };
			const AssetPointer<ModelAsset> *const RESTRICT source{ static_cast<const AssetPointer<ModelAsset> *const RESTRICT>(data) };

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
	stream_archive->Write(&editable_field._Identifier, sizeof(uint64));

	//Retrieve the data.
	const void *const RESTRICT data{ component->EditableFieldData<void>(entity, editable_field) };

	switch (editable_field._Type)
	{
		case ComponentEditableField::Type::MATERIAL_ASSET:
		{
			//Cast the data.
			const AssetPointer<MaterialAsset> *const RESTRICT _data{ static_cast<const AssetPointer<MaterialAsset> *const RESTRICT>(data) };

			//Can't really serialize a pointer, so serialize the resource identifier.
			stream_archive->Write(&(*_data)->_Header._AssetIdentifier, sizeof(HashString));

			break;
		}

		case ComponentEditableField::Type::MODEL_ASSET:
		{
			//Cast the data.
			const AssetPointer<ModelAsset> *const RESTRICT _data{ static_cast<const AssetPointer<ModelAsset> *const RESTRICT>(data) };

			//Can't really serialize a pointer, so serialize the resource identifier.
			stream_archive->Write(&(*_data)->_Header._AssetIdentifier, sizeof(HashString));

			break;
		}

		case ComponentEditableField::Type::WORLD_TRANSFORM:
		{
			//Cast the data.
			const WorldTransform *const RESTRICT _data{ static_cast<const WorldTransform *const RESTRICT>(data) };

			//This can just be copied directly.
			stream_archive->Write(_data, sizeof(WorldTransform));

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
NO_DISCARD uint64 DeserializeEditableField
(
	const ComponentEditableField &editable_field,
	const void *const RESTRICT data,
	ComponentInitializationData *const RESTRICT initialization_data
) NOEXCEPT
{	
	//Retrieve the data.
	void *const RESTRICT _data{ AdvancePointer(initialization_data, editable_field._InitializationDataOffset) };

	switch (editable_field._Type)
	{
		case ComponentEditableField::Type::MATERIAL_ASSET:
		{
			//Cast the data.
			AssetPointer<MaterialAsset> *const RESTRICT destination{ static_cast<AssetPointer<MaterialAsset> *const RESTRICT>(_data) };
			const HashString *const RESTRICT source{ static_cast<const HashString *const RESTRICT>(data) };

			//Write the data.
			(*destination) = ContentSystem::Instance->GetAsset<MaterialAsset>(*source);

			//Return the advance.
			return sizeof(HashString);
		}

		case ComponentEditableField::Type::MODEL_ASSET:
		{
			//Cast the data.
			AssetPointer<ModelAsset> *const RESTRICT destination{ static_cast<AssetPointer<ModelAsset> *const RESTRICT>(_data) };
			const HashString *const RESTRICT source{ static_cast<const HashString *const RESTRICT>(data) };

			//Write the data.
			(*destination) = ContentSystem::Instance->GetAsset<ModelAsset>(*source);

			//Return the advance.
			return sizeof(HashString);
		}

		case ComponentEditableField::Type::WORLD_TRANSFORM:
		{
			//Cast the data.
			WorldTransform *const RESTRICT destination{ static_cast<WorldTransform *const RESTRICT>(_data) };
			const WorldTransform *const RESTRICT source{ static_cast<const WorldTransform *const RESTRICT>(data) };

			//Write the data.
			*destination = *source;

			//Return the advance.
			return sizeof(WorldTransform);
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			return 0;
		}
	}
}