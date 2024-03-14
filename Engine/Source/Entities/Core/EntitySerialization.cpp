//Header file.
#include <Entities/Core/EntitySerialization.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/MaterialAsset.h>
#include <Content/Assets/ModelAsset.h>

//Math.
#include <Math/General/EulerAngles.h>

//Systems.
#include <Systems/ContentSystem.h>
#include <Systems/EntitySystem.h>

//World.
#include <World/Core/WorldTransform.h>

namespace EntitySerialization
{

	/*
	*	Serializes the given entity to the given JSON object with the given name.
	*/
	void SerializeToJSON
	(
		nlohmann::json &JSON,
		Entity *const RESTRICT entity
	) NOEXCEPT
	{
		nlohmann::json &components{ JSON["Components"] };

		for (Component *const RESTRICT component : AllComponents())
		{
			if (!component->Has(entity) || component->EditableFields().Empty())
			{
				continue;
			}

			nlohmann::json &component_entry{ components[component->Name()] };

			for (const ComponentEditableField &editable_field : component->EditableFields())
			{
				switch (editable_field._Type)
				{
					case ComponentEditableField::Type::EULER_ANGLES:
					{
						const EulerAngles *const RESTRICT data{ component->EditableFieldData<EulerAngles>(entity, editable_field) };

						nlohmann::json &rotation_entry{ component_entry[editable_field._Name] };

						//Always store in degrees as this might be manually manipulated via text.
						rotation_entry["Roll"] = CatalystBaseMath::RadiansToDegrees(data->_Roll);
						rotation_entry["Yaw"] = CatalystBaseMath::RadiansToDegrees(data->_Yaw);
						rotation_entry["Pitch"] = CatalystBaseMath::RadiansToDegrees(data->_Pitch);

						break;
					}

					case ComponentEditableField::Type::MATERIAL_ASSET:
					{
						const AssetPointer<MaterialAsset> *const RESTRICT data{ component->EditableFieldData<AssetPointer<MaterialAsset>>(entity, editable_field) };

						component_entry[editable_field._Name] = (*data)->_Header._AssetName.Data();

						break;
					}

					case ComponentEditableField::Type::MODEL_ASSET:
					{
						const AssetPointer<ModelAsset> *const RESTRICT data{ component->EditableFieldData<AssetPointer<ModelAsset>>(entity, editable_field) };

						component_entry[editable_field._Name] = (*data)->_Header._AssetName.Data();

						break;
					}

					case ComponentEditableField::Type::WORLD_TRANSFORM:
					{
						const WorldTransform *const RESTRICT data{ component->EditableFieldData<WorldTransform>(entity, editable_field) };

						nlohmann::json &world_transform_entry{ component_entry[editable_field._Name] };

						{
							const Vector3<float32> position{ data->GetAbsolutePosition() };

							nlohmann::json &position_entry{ world_transform_entry["Position"] };

							position_entry["X"] = position._X;
							position_entry["Y"] = position._Y;
							position_entry["Z"] = position._Z;
						}

						{
							const Quaternion rotation{ data->GetRotation() };

							nlohmann::json &rotation_entry{ world_transform_entry["Rotation"] };

							rotation_entry["X"] = rotation._X;
							rotation_entry["Y"] = rotation._Y;
							rotation_entry["Z"] = rotation._Z;
							rotation_entry["W"] = rotation._W;
						}

						{
							const float32 scale{ data->GetScale() };

							nlohmann::json &scale_entry{ world_transform_entry["Scale"] };

							scale_entry = scale;
						}

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
				}
			}
		}
	}

	/*
	*	Serializes an entity from the given JSON object to the give stream archive.
	*/
	void SerializeToStreamArchive
	(
		const nlohmann::json &JSON,
		const char *const RESTRICT name,
		StreamArchive *const RESTRICT stream_archive
	) NOEXCEPT
	{
		//Write the name.
		const StaticString<64> _name{ name };
		stream_archive->Write(_name.Data(), _name.Size());

		//Cache the components object.
		const nlohmann::json &components{ JSON["Components"] };

		//Write the number of components.
		const uint64 number_of_components{ components.size() };
		stream_archive->Write(&number_of_components, sizeof(uint64));

		for (auto component_iterator{ components.begin() }; component_iterator != components.end(); ++component_iterator)
		{
			//Cache the component entry.
			const nlohmann::json &component_entry{ *component_iterator };

			//Calculate the component identifier.
			const HashString component_identifier{ component_iterator.key().c_str() };

			//Write the component identifier.
			stream_archive->Write(&component_identifier, sizeof(HashString));

			//Retrieve the component.
			const Component *RESTRICT component{ nullptr };

			for (const Component *const RESTRICT _component : AllComponents())
			{
				if (_component->_Identifier == component_identifier)
				{
					component = _component;

					break;
				}
			}

			//Skip if this component was not found.
			if (!component)
			{
				ASSERT(false, "Invalid case!");

				continue;
			}

			//Write the number of editable fields.
			const uint64 number_of_editable_fields{ component_entry.size() };
			stream_archive->Write(&number_of_editable_fields, sizeof(uint64));

			//Iterate over all editable fields.
			for (auto editable_field_iterator{ component_entry.begin() }; editable_field_iterator != component_entry.end(); ++editable_field_iterator)
			{
				//Cache the editable field entry.
				const nlohmann::json &editable_field_entry{ *editable_field_iterator };

				//Calculate the editable field identifier.
				const HashString editable_field_identifier{ editable_field_iterator.key().c_str() };

				//Write the editable field identifier.
				stream_archive->Write(&editable_field_identifier, sizeof(HashString));

				//Find the editable field.
				for (const ComponentEditableField &editable_field : component->EditableFields())
				{
					if (editable_field._Identifier == editable_field_identifier)
					{
						switch (editable_field._Type)
						{
							case ComponentEditableField::Type::EULER_ANGLES:
							{
								EulerAngles euler_angles;

								euler_angles._Roll = CatalystBaseMath::DegreesToRadians(editable_field_entry["Roll"]);
								euler_angles._Yaw = CatalystBaseMath::DegreesToRadians(editable_field_entry["Yaw"]);
								euler_angles._Pitch = CatalystBaseMath::DegreesToRadians(editable_field_entry["Pitch"]);

								stream_archive->Write(&euler_angles, sizeof(EulerAngles));

								break;
							}

							case ComponentEditableField::Type::MATERIAL_ASSET:
							{
								const HashString asset_identifier{ editable_field_entry.get<std::string>().c_str() };

								stream_archive->Write(&asset_identifier, sizeof(HashString));

								break;
							}

							case ComponentEditableField::Type::MODEL_ASSET:
							{
								const HashString asset_identifier{ editable_field_entry.get<std::string>().c_str() };

								stream_archive->Write(&asset_identifier, sizeof(HashString));

								break;
							}

							case ComponentEditableField::Type::WORLD_TRANSFORM:
							{
								Vector3<float32> position;

								{
									const nlohmann::json &position_entry{ editable_field_entry["Position"] };

									position._X = position_entry["X"];
									position._Y = position_entry["Y"];
									position._Z = position_entry["Z"];
								}

								Quaternion rotation;

								{
									const nlohmann::json &rotation_entry{ editable_field_entry["Rotation"] };

									rotation._X = rotation_entry["X"];
									rotation._Y = rotation_entry["Y"];
									rotation._Z = rotation_entry["Z"];
									rotation._W = rotation_entry["W"];
								}

								float32 scale;

								{
									scale = editable_field_entry["Scale"];
								}

								WorldTransform world_transform;

								world_transform.SetAbsolutePosition(position);
								world_transform.SetRotation(rotation);
								world_transform.SetScale(scale);

								stream_archive->Write(&world_transform, sizeof(WorldTransform));

								break;
							}

							default:
							{
								ASSERT(false, "Invalid case!");

								break;
							}
						}

						break;
					}
				}
			}
		}
	}

	/*
	*	Deserializes an entity from the given stream archive.
	*/
	NO_DISCARD Entity *const RESTRICT DeserializeFromStreamArchive
	(
		const StreamArchive &stream_archive,
		uint64 *const RESTRICT stream_archive_position
	) NOEXCEPT
	{
		//Define constants.
		constexpr uint64 MAXIMUM_NUMBER_OF_COMPONENTS{ 8 };

		//Set up the component configurations.
		StaticArray<ComponentInitializationData *RESTRICT, MAXIMUM_NUMBER_OF_COMPONENTS> component_configurations;

		//Read the number of components.
		uint64 number_of_components;
		stream_archive.Read(&number_of_components, sizeof(uint64), stream_archive_position);

		ASSERT(number_of_components <= MAXIMUM_NUMBER_OF_COMPONENTS, "Too many components! Increase number. (:");

		uint64 current_component_configuration{ 0 };

		for (uint64 component_index{ 0 }; component_index < number_of_components; ++component_index)
		{
			//Read the component identifier.
			HashString component_identifier;
			stream_archive.Read(&component_identifier, sizeof(HashString), stream_archive_position);

			//Find the component.
			Component *RESTRICT component{ nullptr };

			for (Component *const RESTRICT _component : AllComponents())
			{
				if (_component->_Identifier == component_identifier)
				{
					component = _component;

					break;
				}
			}

			if (!component)
			{
				ASSERT(false, "Couldn't find component!");

				continue;
			}

			//Allocate the initialization data.
			ComponentInitializationData *const RESTRICT component_configuration{ component->AllocateInitializationData() };
			component_configurations[current_component_configuration++] = component_configuration;

			//Apply all editable fields.
			uint64 number_of_editable_fields;
			stream_archive.Read(&number_of_editable_fields, sizeof(uint64), stream_archive_position);

			for (uint64 editable_field_index{ 0 }; editable_field_index < number_of_editable_fields; ++editable_field_index)
			{
				//Read the editable field identifier.
				HashString editable_field_identifier;
				stream_archive.Read(&editable_field_identifier, sizeof(HashString), stream_archive_position);

				//Find the editable field.
				const ComponentEditableField *RESTRICT editable_field{ nullptr };

				for (const ComponentEditableField &_editable_field : component->EditableFields())
				{
					if (_editable_field._Identifier == editable_field_identifier)
					{
						editable_field = &_editable_field;

						break;
					}
				}

				if (!editable_field)
				{
					ASSERT(false, "Couldn't find editable field!");

					break;
				}

				switch (editable_field->_Type)
				{
					case ComponentEditableField::Type::EULER_ANGLES:
					{
						EulerAngles data;
						stream_archive.Read(&data, sizeof(EulerAngles), stream_archive_position);

						Memory::Copy(AdvancePointer(component_configuration, editable_field->_InitializationDataOffset), &data, sizeof(EulerAngles));

						break;
					}

					case ComponentEditableField::Type::MATERIAL_ASSET:
					{
						HashString data;
						stream_archive.Read(&data, sizeof(HashString), stream_archive_position);

						*reinterpret_cast<AssetPointer<MaterialAsset> *const RESTRICT>(AdvancePointer(component_configuration, editable_field->_InitializationDataOffset)) = ContentSystem::Instance->GetAsset<MaterialAsset>(data);

						break;
					}

					case ComponentEditableField::Type::MODEL_ASSET:
					{
						HashString data;
						stream_archive.Read(&data, sizeof(HashString), stream_archive_position);

						*reinterpret_cast<AssetPointer<ModelAsset> *const RESTRICT>(AdvancePointer(component_configuration, editable_field->_InitializationDataOffset)) = ContentSystem::Instance->GetAsset<ModelAsset>(data);

						break;
					}

					case ComponentEditableField::Type::WORLD_TRANSFORM:
					{
						WorldTransform data;
						stream_archive.Read(&data, sizeof(WorldTransform), stream_archive_position);

						Memory::Copy(AdvancePointer(component_configuration, editable_field->_InitializationDataOffset), &data, sizeof(WorldTransform));

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
				}
			}
		}

		//Create the entity!
		return EntitySystem::Instance->CreateEntity(ArrayProxy<ComponentInitializationData *RESTRICT>(component_configurations.Data(), current_component_configuration));
	}

}