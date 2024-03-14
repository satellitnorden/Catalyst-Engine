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