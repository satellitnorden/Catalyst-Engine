//Header file.
#include <Systems/LevelSystem.h>

//Components.
#include <Components/Core/Component.h>

//Systems.
#include <Systems/EntitySystem.h>

//Singleton definition.
DEFINE_SINGLETON(LevelSystem);

/*
*	Spawns a level.
*/
void LevelSystem::SpawnLevel
(
	const WorldTransform &world_transform,
	const ResourcePointer<LevelResource> level_resource,
	DynamicArray<LevelEntry> *const RESTRICT level_entries
) NOEXCEPT
{
	//Add all level entries.
	level_entries->Reserve(level_resource->_LevelEntries.Size());

	//Set up the component configurations.
	DynamicArray<ComponentInitializationData *RESTRICT> component_configurations;

	for (const LevelResource::LevelEntry &level_entry : level_resource->_LevelEntries)
	{
		//Add a new level entry.
		level_entries->Emplace();
		LevelEntry &new_level_entry{ level_entries->Back() };

		//Set the name.
		new_level_entry._Name = level_entry._Name;

		//Set up the component configurations.
		component_configurations.Clear();

		for (const LevelResource::LevelEntry::ComponentEntry &component_entry : level_entry._ComponentEntries)
		{
			//Create the component configuration.
			ComponentInitializationData *RESTRICT component_configuration{ component_entry._Component->AllocateInitializationData() };

			//Deserialize all editable fields.
			uint64 field_data_position{ component_entry._FieldDataPosition };

			for (uint64 editable_field_index{ 0 }; editable_field_index < component_entry._NumberOfEditableFields; ++editable_field_index)
			{
				//Read the editable field identifier.
				const uint64 editable_field_identifier{ *((const uint64 *const RESTRICT)level_resource->_StreamArchive.Read(field_data_position)) };
				field_data_position += sizeof(uint64);

				//Find the editable field.
				const ComponentEditableField *RESTRICT editable_field{ nullptr };

				for (const ComponentEditableField &_editable_field : component_entry._Component->EditableFields())
				{
					if (editable_field_identifier == _editable_field._Identifier)
					{
						editable_field = &_editable_field;

						break;
					}
				}

				//Might have been removed?
				if (!editable_field)
				{
					break;
				}

				//Deserialize the data.
				field_data_position += DeserializeEditableField
				(
					*editable_field,
					level_resource->_StreamArchive.Read(field_data_position),
					component_configuration
				);
			}

			//Add the component configuration.
			component_configurations.Emplace(component_configuration);
		}

		//Create the entity!
		new_level_entry._Entity = EntitySystem::Instance->CreateEntity(ArrayProxy<ComponentInitializationData *RESTRICT>(component_configurations));
	}
}