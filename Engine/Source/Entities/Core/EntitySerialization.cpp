//Header file.
#include <Entities/Core/EntitySerialization.h>

//Core.
#include <Core/General/Enumeration.h>

//Components.
#include <Components/Components/WorldTransformComponent.h>

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

#if defined(CATALYST_EDITOR)
	/*
	*	Serializes the given entity to the given JSON object with the given name.
	*/
	void SerializeToJSON
	(
		const EditorEntityData &editor_entity_data,
		nlohmann::json &JSON,
		Entity *const RESTRICT entity
	) NOEXCEPT
	{
		nlohmann::json &components{ JSON["Components"] };

		for (uint64 component_index{ 0 }; component_index < Components::Size(); ++component_index)
		{
			//Cache the component.
			Component *const RESTRICT component{ Components::At(component_index) };

			if (!component->Has(entity))
			{
				continue;
			}

			nlohmann::json &component_entry{ components[component->_Name] };

			for (const ComponentEditableField &editable_field : component->EditableFields())
			{
				switch (editable_field._Type)
				{
					case ComponentEditableField::Type::COLOR:
					{
						const Vector3<float32> *const RESTRICT data{ component->EditableFieldData<Vector3<float32>>(entity, editable_field) };

						nlohmann::json &color_entry{ component_entry[editable_field._Name] };

						color_entry["R"] = data->_R;
						color_entry["G"] = data->_G;
						color_entry["B"] = data->_B;

						break;
					}

					case ComponentEditableField::Type::FLOAT:
					{
						const float32 *const RESTRICT data{ component->EditableFieldData<float32>(entity, editable_field) };

						component_entry[editable_field._Name] = *data;

						break;
					}

					case ComponentEditableField::Type::ENUMERATION:
					{
						const Enumeration *const RESTRICT data{ component->EditableFieldData<Enumeration>(entity, editable_field) };

						component_entry[editable_field._Name] = data->ToString();

						break;
					}

					case ComponentEditableField::Type::EULER_ANGLES:
					{
						const EulerAngles *const RESTRICT data{ component->EditableFieldData<EulerAngles>(entity, editable_field) };

						nlohmann::json &rotation_entry{ component_entry[editable_field._Name] };

						//Always store in degrees as this might be manually manipulated via text.
						rotation_entry["Roll"] = BaseMath::RadiansToDegrees(data->_Roll);
						rotation_entry["Yaw"] = BaseMath::RadiansToDegrees(data->_Yaw);
						rotation_entry["Pitch"] = BaseMath::RadiansToDegrees(data->_Pitch);

						break;
					}

					case ComponentEditableField::Type::HASH_STRING:
					{
						//Find the hash string data.
						const EditorEntityData::HashStringData *RESTRICT hash_string_data{ nullptr };

						for (const EditorEntityData::HashStringData &_hash_string_data : editor_entity_data._HashStringData)
						{
							if (_hash_string_data._ComponentIdentifier == component->_Identifier
								&& _hash_string_data._EditableFieldIdentifier == editable_field._Identifier)
							{
								hash_string_data = &_hash_string_data;

								break;
							}
						}

						ASSERT(hash_string_data, "Couldn't find hash string data!");

						component_entry[editable_field._Name] = hash_string_data->_String.Data();

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

					case ComponentEditableField::Type::UINT32:
					{
						const uint32 *const RESTRICT data{ component->EditableFieldData<uint32>(entity, editable_field) };

						component_entry[editable_field._Name] = *data;

						break;
					}

					case ComponentEditableField::Type::UINT64:
					{
						const uint64 *const RESTRICT data{ component->EditableFieldData<uint64>(entity, editable_field) };

						component_entry[editable_field._Name] = *data;

						break;
					}

					case ComponentEditableField::Type::VECTOR2:
					{
						const Vector2<float32> *const RESTRICT data{ component->EditableFieldData<Vector2<float32>>(entity, editable_field) };

						nlohmann::json &vector_2_entry{ component_entry[editable_field._Name] };

						vector_2_entry["X"] = data->_X;
						vector_2_entry["Y"] = data->_Y;

						break;
					}

					case ComponentEditableField::Type::VECTOR3:
					{
						const Vector3<float32> *const RESTRICT data{ component->EditableFieldData<Vector3<float32>>(entity, editable_field) };

						nlohmann::json& position_entry{ component_entry[editable_field._Name] };

						position_entry["X"] = data->_X;
						position_entry["Y"] = data->_Y;
						position_entry["Z"] = data->_Z;

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
							const Vector3<float32> scale{ data->GetScale() };

							nlohmann::json &scale_entry{ world_transform_entry["Scale"] };

							scale_entry["X"] = scale._X;
							scale_entry["Y"] = scale._Y;
							scale_entry["Z"] = scale._Z;
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
#endif

	/*
	*	Serializes an entity from the given JSON object to the give stream archive.
	*/
	void SerializeToStreamArchive
	(
		const nlohmann::json &JSON,
		StreamArchive *const RESTRICT stream_archive
	) NOEXCEPT
	{
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

			for (uint64 component_index{ 0 }; component_index < Components::Size(); ++component_index)
			{
				//Cache the component.
				Component *const RESTRICT _component{ Components::At(component_index) };

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
							case ComponentEditableField::Type::COLOR:
							{
								Vector3<float32> color;

								color._X = editable_field_entry["R"];
								color._Y = editable_field_entry["G"];
								color._Z = editable_field_entry["B"];

								stream_archive->Write(&color, sizeof(Vector3<float32>));

								break;
							}

							case ComponentEditableField::Type::FLOAT:
							{
								const float32 value{ editable_field_entry.get<float32>() };
								
								stream_archive->Write(&value, sizeof(float32));

								break;
							}

							case ComponentEditableField::Type::ENUMERATION:
							{
								const std::string enumeration_value{ editable_field_entry.get<std::string>() };

								const uint64 hash{ HashAlgorithms::MurmurHash64(enumeration_value.data(), enumeration_value.length()) };

								stream_archive->Write(&hash, sizeof(hash));

								break;
							}

							case ComponentEditableField::Type::EULER_ANGLES:
							{
								EulerAngles euler_angles;

								euler_angles._Roll = BaseMath::DegreesToRadians(editable_field_entry["Roll"]);
								euler_angles._Yaw = BaseMath::DegreesToRadians(editable_field_entry["Yaw"]);
								euler_angles._Pitch = BaseMath::DegreesToRadians(editable_field_entry["Pitch"]);

								stream_archive->Write(&euler_angles, sizeof(EulerAngles));

								break;
							}

							case ComponentEditableField::Type::HASH_STRING:
							{
								const HashString value{ editable_field_entry.get<std::string>().c_str()};

								stream_archive->Write(&value, sizeof(HashString));

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

							case ComponentEditableField::Type::UINT32:
							{
								const uint32 value{ editable_field_entry.get<uint32>() };

								stream_archive->Write(&value, sizeof(uint32));

								break;
							}

							case ComponentEditableField::Type::UINT64:
							{
								const uint64 value{ editable_field_entry.get<uint64>() };

								stream_archive->Write(&value, sizeof(uint64));

								break;
							}

							case ComponentEditableField::Type::VECTOR2:
							{
								Vector2<float32> value;

								value._X = editable_field_entry["X"];
								value._Y = editable_field_entry["Y"];

								stream_archive->Write(&value, sizeof(Vector2<float32>));

								break;
							}

							case ComponentEditableField::Type::VECTOR3:
							{
								Vector3<float32> position;

								position._X = editable_field_entry["X"];
								position._Y = editable_field_entry["Y"];
								position._Z = editable_field_entry["Z"];

								stream_archive->Write(&position, sizeof(Vector3<float32>));

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

								Vector3<float32> scale;

								{
									const nlohmann::json &scale_entry{ editable_field_entry["Scale"] };

									scale._X = scale_entry["X"];
									scale._Y = scale_entry["Y"];
									scale._Z = scale_entry["Z"];
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
	*	Takes an optional world transform to apply to spawned entities.
	*/
	NO_DISCARD Entity *const RESTRICT DeserializeFromStreamArchive
	(
		const StreamArchive &stream_archive,
		uint64 *const RESTRICT stream_archive_position,
		const WorldTransform *const RESTRICT world_transform,
		const DeserializeFunction deserialize_function,
		void *const RESTRICT deserialize_user_data
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

			for (uint64 component_index{ 0 }; component_index < Components::Size(); ++component_index)
			{
				//Cache the component.
				Component *const RESTRICT _component{ Components::At(component_index) };

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
			ComponentInitializationData *const RESTRICT component_configuration{ Components::AllocateInitializationData(component) };
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
					case ComponentEditableField::Type::COLOR:
					{
						Vector3<float32> data;
						stream_archive.Read(&data, sizeof(Vector3<float32>), stream_archive_position);

						Memory::Copy(AdvancePointer(component_configuration, editable_field->_InitializationDataOffset), &data, sizeof(Vector3<float32>));

						break;
					}

					case ComponentEditableField::Type::FLOAT:
					{
						float32 data;
						stream_archive.Read(&data, sizeof(float32), stream_archive_position);

						float32 *const RESTRICT value{ static_cast<float32 *const RESTRICT>(static_cast<void *const RESTRICT>(AdvancePointer(component_configuration, editable_field->_InitializationDataOffset))) };

						*value = data;

						break;
					}

					case ComponentEditableField::Type::ENUMERATION:
					{
						uint64 data;
						stream_archive.Read(&data, sizeof(uint64), stream_archive_position);

						Enumeration *const RESTRICT enumeration{ static_cast<Enumeration *const RESTRICT>(static_cast<void *const RESTRICT>(AdvancePointer(component_configuration, editable_field->_InitializationDataOffset))) };

						enumeration->SetFromHash(data);

						break;
					}

					case ComponentEditableField::Type::EULER_ANGLES:
					{
						EulerAngles data;
						stream_archive.Read(&data, sizeof(EulerAngles), stream_archive_position);

						Memory::Copy(AdvancePointer(component_configuration, editable_field->_InitializationDataOffset), &data, sizeof(EulerAngles));

						break;
					}

					case ComponentEditableField::Type::HASH_STRING:
					{
						HashString data;
						stream_archive.Read(&data, sizeof(HashString), stream_archive_position);

						Memory::Copy(AdvancePointer(component_configuration, editable_field->_InitializationDataOffset), &data, sizeof(HashString));

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

					case ComponentEditableField::Type::UINT32:
					{
						uint32 data;
						stream_archive.Read(&data, sizeof(uint32), stream_archive_position);

						uint32 *const RESTRICT value{ static_cast<uint32 *const RESTRICT>(static_cast<void* const RESTRICT>(AdvancePointer(component_configuration, editable_field->_InitializationDataOffset))) };

						*value = data;

						break;
					}

					case ComponentEditableField::Type::UINT64:
					{
						uint64 data;
						stream_archive.Read(&data, sizeof(uint64), stream_archive_position);

						uint64 *const RESTRICT value{ static_cast<uint64 *const RESTRICT>(static_cast<void *const RESTRICT>(AdvancePointer(component_configuration, editable_field->_InitializationDataOffset))) };

						*value = data;

						break;
					}

					case ComponentEditableField::Type::VECTOR2:
					{
						Vector2<float32> data;
						stream_archive.Read(&data, sizeof(Vector2<float32>), stream_archive_position);

						Memory::Copy(AdvancePointer(component_configuration, editable_field->_InitializationDataOffset), &data, sizeof(Vector2<float32>));

						break;
					}

					case ComponentEditableField::Type::VECTOR3:
					{
						Vector3<float32> data;
						stream_archive.Read(&data, sizeof(Vector3<float32>), stream_archive_position);

						Memory::Copy(AdvancePointer(component_configuration, editable_field->_InitializationDataOffset), &data, sizeof(Vector3<float32>));

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

			//Apply the supplied world transform.
			if (world_transform)
			{
				if (component_identifier == WorldTransformComponent::Instance->_Identifier)
				{
					//Cache the world transform component configuration.
					WorldTransformInitializationData *const RESTRICT world_transform_component_configuration{ static_cast<WorldTransformInitializationData *const RESTRICT>(component_configuration) };

					//Create a transformation matrix.
					const Matrix4x4 transformation_matrix{ world_transform->ToLocalMatrix4x4() };

					//Apply the cell.
					world_transform_component_configuration->_WorldTransform.SetCell(world_transform_component_configuration->_WorldTransform.GetCell() + world_transform->GetCell());
					
					//Apply the position.
					{
						Vector3<float32> local_position{ world_transform_component_configuration->_WorldTransform.GetLocalPosition() };
						local_position.Rotate(world_transform->GetRotation().ToEulerAngles());
						local_position += world_transform->GetLocalPosition();
						world_transform_component_configuration->_WorldTransform.SetLocalPosition(local_position);
					}

					//Apply rotation.
					{
						Quaternion local_rotation{ world_transform_component_configuration->_WorldTransform.GetRotation() };
						local_rotation = world_transform->GetRotation() * local_rotation;
						world_transform_component_configuration->_WorldTransform.SetRotation(local_rotation);
					}

					//Apply scale.
					world_transform_component_configuration->_WorldTransform.SetScale(world_transform_component_configuration->_WorldTransform.GetScale() * world_transform->GetScale());
				}
			}

			//Call the deserialize function, if one is applied.
			if (deserialize_function)
			{
				deserialize_function(component_configuration, deserialize_user_data);
			}
		}

		//Create the entity!
		return EntitySystem::Instance->CreateEntity(ArrayProxy<ComponentInitializationData *RESTRICT>(component_configurations.Data(), current_component_configuration));
	}

}