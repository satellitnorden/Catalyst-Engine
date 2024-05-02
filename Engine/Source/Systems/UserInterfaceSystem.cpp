//Header file.
#include <Systems/UserInterfaceSystem.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/MemorySystem.h>

//User interface.
#include <UserInterface/ImageUserInterfacePrimitive.h>
#include <UserInterface/ImageUserInterfacePrimitiveDescription.h>
#include <UserInterface/TextUserInterfacePrimitive.h>
#include <UserInterface/TextUserInterfacePrimitiveDescription.h>
#include <UserInterface/UserInterfaceUtilities.h>

//Constants. These need to match UserInterface.storage_buffer_definition.
#define USER_INTERFACE_INSTANCE_FLAG_IMAGE (BIT(0))
#define USER_INTERFACE_INSTANCE_FLAG_TEXT (BIT(1))
#define USER_INTERFACE_INSTANCE_FLAG_TEXTURE (BIT(2))

/*
*	User interface push constant data class definition.
*/
class UserInterfacePushConstantData final
{

public:

	//The transformation.
	Matrix4x4 _Transformation;

};

/*
*	Initializes the user interface system.
*/
void UserInterfaceSystem::Initialize() NOEXCEPT
{
	//Register the storage buffer.
	RenderingSystem::Instance->GetBufferManager()->RegisterStorageBuffer
	(
		HashString("UserInterface"),
		sizeof(UserInterfaceInstance) * 64,
		[](DynamicArray<byte> *const RESTRICT data, void *const RESTRICT)
		{
			UserInterfaceSystem::Instance->StorageBufferUpdate(data);
		},
		nullptr
	);

	//Register the render input stream.
	RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
	(
		HashString("UserInterface"),
		DynamicArray<VertexInputAttributeDescription>(),
		DynamicArray<VertexInputBindingDescription>(),
		sizeof(UserInterfacePushConstantData),
		[](void *const RESTRICT, RenderInputStream *const RESTRICT input_stream)
		{
			UserInterfaceSystem::Instance->RenderInputStreamUpdate(input_stream);
		},
		RenderInputStream::Mode::DRAW_INSTANCED,
		this
	);

	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT arguments)
	{
		static_cast<UserInterfaceSystem *const RESTRICT>(arguments)->UserInterfaceUpdate();
	},
	this,
	UpdatePhase::USER_INTERFACE,
	UpdatePhase::PHYSICS,
	false,
	false);
}

/*
*	Terminates the user interface system.
*/
void UserInterfaceSystem::Terminate() NOEXCEPT
{
	//Deactivate all active user interface scenes.
	for (UserInterfaceScene* const RESTRICT scene : _ActiveUserInterfaceScenes)
	{
		scene->OnDeactivated();
	}
}

/*
*	Creates a user interface primitive.
*/
RESTRICTED NO_DISCARD UserInterfacePrimitive *const RESTRICT UserInterfaceSystem::CreateUserInterfacePrimitive(const UserInterfacePrimitiveDescription *const RESTRICT description) NOEXCEPT
{
	switch (description->_Type)
	{
		case UserInterfacePrimitiveType::IMAGE:
		{
			ImageUserInterfacePrimitive *const RESTRICT primitive{ new (MemorySystem::Instance->TypeAllocate<ImageUserInterfacePrimitive>()) ImageUserInterfacePrimitive() };
			const ImageUserInterfacePrimitiveDescription *const RESTRICT type_description{ static_cast<const ImageUserInterfacePrimitiveDescription *const RESTRICT>(description) };

			primitive->_Type = UserInterfacePrimitiveType::IMAGE;
			primitive->_Minimum = type_description->_Minimum;
			primitive->_Maximum = type_description->_Maximum;
			primitive->_Opacity = type_description->_Opacity;
			primitive->_Material = type_description->_Material;

			return primitive;
		}

		case UserInterfacePrimitiveType::TEXT:
		{
			TextUserInterfacePrimitive *const RESTRICT primitive{ new (MemorySystem::Instance->TypeAllocate<TextUserInterfacePrimitive>()) TextUserInterfacePrimitive() };
			const TextUserInterfacePrimitiveDescription *const RESTRICT type_description{ static_cast<const TextUserInterfacePrimitiveDescription *const RESTRICT>(description) };

			primitive->_Type = UserInterfacePrimitiveType::TEXT;
			primitive->_Minimum = type_description->_Minimum;
			primitive->_Maximum = type_description->_Maximum;
			primitive->_Opacity = type_description->_Opacity;
			primitive->_Font = type_description->_Font;
			primitive->_Scale = type_description->_Scale;
			primitive->_HorizontalAlignment = type_description->_HorizontalAlignment;
			primitive->_VerticalAlignment = type_description->_VerticalAlignment;
			primitive->_Opacity = type_description->_Opacity;
			primitive->_Text = std::move(type_description->_Text);

			return primitive;
		}

		default:
		{
			ASSERT(false, "Unhandled case!");

			return nullptr;
		}
	}
}

/*
*	Destroys a user interface primitive.
*/
void UserInterfaceSystem::DestroyUserInterfacePrimitive(UserInterfacePrimitive *const RESTRICT primitive) NOEXCEPT
{
	switch (primitive->_Type)
	{
		case UserInterfacePrimitiveType::IMAGE:
		{
			MemorySystem::Instance->TypeFree<ImageUserInterfacePrimitive>(static_cast<ImageUserInterfacePrimitive *const RESTRICT>(primitive));

			break;
		}

		case UserInterfacePrimitiveType::TEXT:
		{
			MemorySystem::Instance->TypeFree<TextUserInterfacePrimitive>(static_cast<TextUserInterfacePrimitive *const RESTRICT>(primitive));

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
*	Updates the user interface system during the user interface update phase.
*/
void UserInterfaceSystem::UserInterfaceUpdate() NOEXCEPT
{
	//Process the destruction queue.
	while (UserInterfaceScene *const RESTRICT *const RESTRICT scene{ _DestructionQueue.Pop() })
	{
		//Deactivate this scene if is active.
		if ((*scene)->GetIsActive())
		{
			(*scene)->OnDeactivated();

			_ActiveUserInterfaceScenes.Erase<false>(*scene);

			(*scene)->SetIsActive(false);
		}
	}

	//Process the deactivation queue.
	while (UserInterfaceScene *const RESTRICT *const RESTRICT scene{ _DeactivationQueue.Pop() })
	{
		if ((*scene)->GetIsActive())
		{
			(*scene)->OnDeactivated();

			_ActiveUserInterfaceScenes.Erase<false>(*scene);

			(*scene)->SetIsActive(false);
		}
	}

	//Process the activation queue.
	while (UserInterfaceScene *const RESTRICT *const RESTRICT scene{ _ActivationQueue.Pop() })
	{
		if (!(*scene)->GetIsActive())
		{
			_ActiveUserInterfaceScenes.Emplace(*scene);

			(*scene)->OnActivated();

			(*scene)->SetIsActive(true);
		}
	}

	//Update all the current scenes.
	for (UserInterfaceScene *const RESTRICT scene : _ActiveUserInterfaceScenes)
	{
		scene->Update();
	}
}

/*
*	The storage buffer update.
*/
void UserInterfaceSystem::StorageBufferUpdate(DynamicArray<byte> *const RESTRICT data) NOEXCEPT
{
	//Calculate the aspect ratio reciprocal.
	const float32 aspect_ratio_reciprocal{ 1.0f / RenderingSystem::Instance->GetFullAspectRatio() };

	//Gather all user interface primitives.
	_UserInterfacePrimitives.Clear();

	for (UserInterfaceScene *const RESTRICT scene : _ActiveUserInterfaceScenes)
	{
		scene->RetrieveUserInterfacePrimitives(&_UserInterfacePrimitives);
	}

	//Gather all instances.
	_Instances.Clear();

	for (const UserInterfacePrimitive *const RESTRICT primitive : _UserInterfacePrimitives)
	{
		//Don't include if this primitive isn't visible.
		if (primitive->_Opacity <= 0.0f)
		{
			continue;
		}

		//Treat differently depending on the type.
		switch (primitive->_Type)
		{
			case UserInterfacePrimitiveType::IMAGE:
			{
				//Cache the type primitive.
				const ImageUserInterfacePrimitive *const RESTRICT image_primitive{ static_cast<const ImageUserInterfacePrimitive *const RESTRICT>(primitive) };

				//Add the instance.
				_Instances.Emplace();
				UserInterfaceInstance &instance{ _Instances.Back() };

				instance._Positions[0] = Vector4<float32>(primitive->_Minimum._X, primitive->_Minimum._Y, 0.0f, 1.0f);
				instance._Positions[1] = Vector4<float32>(primitive->_Minimum._X, primitive->_Maximum._Y, 0.0f, 1.0f);
				instance._Positions[2] = Vector4<float32>(primitive->_Maximum._X, primitive->_Maximum._Y, 0.0f, 1.0f);
				instance._Positions[3] = Vector4<float32>(primitive->_Maximum._X, primitive->_Minimum._Y, 0.0f, 1.0f);

				instance._TextureCoordinates[0] = Vector2<float32>(0.0f, 0.0f);
				instance._TextureCoordinates[1] = Vector2<float32>(0.0f, 1.0f);
				instance._TextureCoordinates[2] = Vector2<float32>(1.0f, 1.0f);
				instance._TextureCoordinates[3] = Vector2<float32>(1.0f, 0.0f);

				//Transform into clip space.
				for (uint8 i{ 0 }; i < 4; ++i)
				{
					instance._Positions[i]._Y = 1.0f - instance._Positions[i]._Y;
					instance._Positions[i]._X = instance._Positions[i]._X * 2.0f - 1.0f;
					instance._Positions[i]._Y = instance._Positions[i]._Y * 2.0f - 1.0f;
				}

				//Set the flags.
				instance._Flags = 0;

				instance._Flags |= USER_INTERFACE_INSTANCE_FLAG_IMAGE;

				if (image_primitive->_Material.IsTexture())
				{
					instance._Flags |= USER_INTERFACE_INSTANCE_FLAG_TEXTURE;
				}

				//Set the color/texture.
				instance._ColorOrTexture = image_primitive->_Material.GetValue();

				//Set the color/opacity.
				instance._ColorOpacity = *Color(Vector4<float32>(1.0f, 1.0f, 1.0f, primitive->_Opacity)).Data();

				break;
			}

			case UserInterfacePrimitiveType::TEXT:
			{
				//Cache the type primitive.
				const TextUserInterfacePrimitive *const RESTRICT text_primitive{ static_cast<const TextUserInterfacePrimitive *const RESTRICT>(primitive) };

				//Calculate the aligned minimum/maximum.
				Vector2<float32> aligned_minimum;
				Vector2<float32> aligned_maximum;

				UserInterfaceUtilities::CalculateAlignedBoundingBox
				(
					text_primitive->_Minimum,
					text_primitive->_Maximum,
					text_primitive->_Font,
					text_primitive->_Text,
					text_primitive->_Scale,
					text_primitive->_HorizontalAlignment,
					text_primitive->_VerticalAlignment,
					&aligned_minimum,
					&aligned_maximum
				);

				//Gather all characters.
				float32 current_offset{ 0.0f };

				for (uint64 i{ 0 }, length{ text_primitive->_Text.Length() }; i < length; ++i)
				{
					//Cache the chartacter.
					const char character{ text_primitive->_Text[i] };

					//Only draw if it's a valid character.
					if (character < 0)
					{
						continue;
					}

					if (character == '\n')
					{
						continue;
					}

					//Add the instance.
					_Instances.Emplace();
					UserInterfaceInstance &instance{ _Instances.Back() };

					//Calculate the bounds.
					Vector2<float32> bounds_minimum;
					Vector2<float32> bounds_maximum;

					bounds_minimum._X = aligned_minimum._X + current_offset;
					bounds_minimum._Y = aligned_minimum._Y;
					bounds_maximum._X = aligned_minimum._X + current_offset + text_primitive->_Font->_CharacterDescriptions[character]._Size._X * text_primitive->_Scale * aspect_ratio_reciprocal;
					bounds_maximum._Y = aligned_minimum._Y + text_primitive->_Font->_CharacterDescriptions[character]._Size._Y * text_primitive->_Scale;

					bounds_minimum._X += text_primitive->_Font->_CharacterDescriptions[character]._Offset._X * text_primitive->_Scale * aspect_ratio_reciprocal;
					bounds_maximum._X += text_primitive->_Font->_CharacterDescriptions[character]._Offset._X * text_primitive->_Scale * aspect_ratio_reciprocal;

					bounds_minimum._Y += text_primitive->_Font->_CharacterDescriptions[character]._Offset._Y * text_primitive->_Scale;
					bounds_maximum._Y += text_primitive->_Font->_CharacterDescriptions[character]._Offset._Y * text_primitive->_Scale;

					//Set the positions.
					instance._Positions[0] = Vector4<float32>(bounds_minimum._X, bounds_minimum._Y, 0.0f, 1.0f);
					instance._Positions[1] = Vector4<float32>(bounds_minimum._X, bounds_maximum._Y, 0.0f, 1.0f);
					instance._Positions[2] = Vector4<float32>(bounds_maximum._X, bounds_maximum._Y, 0.0f, 1.0f);
					instance._Positions[3] = Vector4<float32>(bounds_maximum._X, bounds_minimum._Y, 0.0f, 1.0f);

					//Transform into clip space.
					for (uint8 i{ 0 }; i < 4; ++i)
					{
						instance._Positions[i]._Y = 1.0f - instance._Positions[i]._Y;
						instance._Positions[i]._X = instance._Positions[i]._X * 2.0f - 1.0f;
						instance._Positions[i]._Y = instance._Positions[i]._Y * 2.0f - 1.0f;
					}

					//Calculate the texture bounds.
					Vector2<float32> texture_bounds_minimum;
					Vector2<float32> texture_bounds_maximum;

					texture_bounds_minimum = text_primitive->_Font->_CharacterDescriptions[character]._TextureBounds._Minimum;
					texture_bounds_maximum = text_primitive->_Font->_CharacterDescriptions[character]._TextureBounds._Maximum;

					//Set the texture coordinates.
					instance._TextureCoordinates[0] = Vector2<float32>(texture_bounds_minimum._X, texture_bounds_minimum._Y);
					instance._TextureCoordinates[1] = Vector2<float32>(texture_bounds_minimum._X, texture_bounds_maximum._Y);
					instance._TextureCoordinates[2] = Vector2<float32>(texture_bounds_maximum._X, texture_bounds_maximum._Y);
					instance._TextureCoordinates[3] = Vector2<float32>(texture_bounds_maximum._X, texture_bounds_minimum._Y);

					//Set the flags.
					instance._Flags = 0;

					instance._Flags |= USER_INTERFACE_INSTANCE_FLAG_TEXT;
					instance._Flags |= USER_INTERFACE_INSTANCE_FLAG_TEXTURE;

					//Set the color/texture.
					instance._ColorOrTexture = text_primitive->_Font->_MasterTextureIndex;

					//Set the color/opacity.
					instance._ColorOpacity = *Color(Vector4<float32>(1.0f, 1.0f, 1.0f, primitive->_Opacity)).Data();

					//Update the current offset.
					current_offset += text_primitive->_Font->_CharacterDescriptions[character]._Advance * text_primitive->_Scale * aspect_ratio_reciprocal;
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

	data->Resize<false>(_Instances.Size() * sizeof(UserInterfaceInstance));
	Memory::Copy(data->Data(), _Instances.Data(), _Instances.Size() * sizeof(UserInterfaceInstance));
}

/*
*	The render input stream update.
*/
void UserInterfaceSystem::RenderInputStreamUpdate(RenderInputStream *const RESTRICT input_stream) NOEXCEPT
{
	//Clear the entries.
	input_stream->_Entries.Clear();

	//Clear the push constant data memory.
	input_stream->_PushConstantDataMemory.Clear();

	//Add a new entry.
	input_stream->_Entries.Emplace();
	RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

	new_entry._PushConstantDataOffset = 0;
	new_entry._VertexBuffer = EMPTY_HANDLE;
	new_entry._IndexBuffer = EMPTY_HANDLE;
	new_entry._IndexBufferOffset = 0;
	new_entry._InstanceBuffer = EMPTY_HANDLE;
	new_entry._VertexCount = 4;
	new_entry._IndexCount = 0;
	new_entry._InstanceCount = static_cast<uint32>(_Instances.Size());

	//Set up the push constant data.
	UserInterfacePushConstantData push_constant_data;

	push_constant_data._Transformation = Matrix4x4();

	for (uint64 i{ 0 }; i < sizeof(UserInterfacePushConstantData); ++i)
	{
		input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
	}
}