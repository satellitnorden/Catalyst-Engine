//Header file.
#include <Systems/UISystem.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>

/*
*	Initializes the UI system.
*/
void UISystem::Initialize() NOEXCEPT
{
	//Set the reference resolution.
	{
		const Resolution resolution{ RenderingSystem::Instance->GetFullResolution() };
		SetReferenceResolution(Vector2<float32>(static_cast<float32>(resolution._Width), static_cast<float32>(resolution._Height)));
	}

	//Register the storage buffer.
	RenderingSystem::Instance->GetBufferManager()->RegisterStorageBuffer
	(
		HashString("UI"),
		sizeof(UI::RenderCommand) * 1'024,
		[](DynamicArray<byte> *const RESTRICT data, void *const RESTRICT)
		{
			if (!UISystem::Instance->_RenderCommands.Empty())
			{
				data->Resize<false>(UISystem::Instance->_RenderCommands.Size() * sizeof(UI::RenderCommand));
				Memory::Copy(data->Data(), UISystem::Instance->_RenderCommands.Data(), UISystem::Instance->_RenderCommands.Size() * sizeof(UI::RenderCommand));
			}
			
			else
			{
				data->Clear();
			}
		},
		nullptr
	);

	//Register the render input stream.
	RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
	(
		HashString("UI"),
		DynamicArray<VertexInputAttributeDescription>(),
		DynamicArray<VertexInputBindingDescription>(),
		0,
		[](void *const RESTRICT, RenderInputStream *const RESTRICT input_stream)
		{
			//Clear the entries.
			input_stream->_Entries.Clear();

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
			new_entry._InstanceCount = static_cast<uint32>(UISystem::Instance->_RenderCommands.Size());
		},
		RenderInputStream::Mode::DRAW_INSTANCED,
		this
	);
}

/*
*	Updates the user interface system.
*/
void UISystem::Update(const UpdatePhase phase) NOEXCEPT
{
	//Update. (:
	Update(CatalystEngineSystem::Instance->GetDeltaTime(), InputSystem::Instance->GetMouseState(InputLayer::USER_INTERFACE));
}

/*
*	Sets the reference resolution.
*/
void UISystem::SetReferenceResolution(const Vector2<float32> value) NOEXCEPT
{
	//Set the reference resolution.
	_ReferenceResolution = value;

	//Set it on all active scenes as well.
	for (UI::Scene *const RESTRICT scene : _Scenes)
	{
		scene->SetReferenceResolution(_ReferenceResolution);
	}
}

/*
*	Updates the UI system.
*/
void UISystem::Update(const float32 delta_time, const MouseState *const RESTRICT mouse_state) NOEXCEPT
{
	//Process the add scene requests.
	{
		Optional<UI::Scene *RESTRICT> add_scene_request{ _AddSceneRequests.Pop() };

		while (add_scene_request.Valid())
		{
			_Scenes.Emplace(add_scene_request.Get());

			add_scene_request = _AddSceneRequests.Pop();
		}
	}

	//Process the remove scene requests.
	{
		Optional<UI::Scene *RESTRICT> remove_scene_request{ _RemoveSceneRequests.Pop() };

		while (remove_scene_request.Valid())
		{
			UI::Scene *const RESTRICT scene{ remove_scene_request.Get() };

			_Scenes.Erase<true>(scene);
			delete scene;

			remove_scene_request = _RemoveSceneRequests.Pop();
		}
	}

	//Prepare all scenes for building.
	for (UI::Scene *const RESTRICT scene : _Scenes)
	{
		scene->PrepareBuild();
	}

	//Prepare the widget allocator.
	_WidgetAllocator.Prepare();

	//Set up the build context.
	UI::BuildContext build_context;

	build_context._DeltaTime = delta_time;
	build_context._WidgetAllocator = &_WidgetAllocator;

	//Build all scenes.
	for (UI::Scene *const RESTRICT scene : _Scenes)
	{
		scene->Build(build_context);
	}

	//Clean the widget allocator.
	_WidgetAllocator.Clean();

	//Update the widgets.
	UpdateWidgets(mouse_state);

	//Clear the render commands.
	_RenderCommands.Clear();

	//Set up the render context.
	UI::RenderContext render_context;

	render_context._DeltaTime = delta_time;
	render_context._RenderCommands = &_RenderCommands;

	//Render all scenes!
	for (UI::Scene *const RESTRICT scene : _Scenes)
	{
		scene->Render(render_context);
	}

	//Go through all render commands and make them ready for rendering.
	for (UI::RenderCommand &render_command : _RenderCommands)
	{
		//Flip position and transform into clip space.
		for (Vector4<float32> &position : render_command._Positions)
		{
			position._X /= _ReferenceResolution._X;
			position._Y /= _ReferenceResolution._Y;

			position._Y = 1.0f - position._Y;

			position._X = position._X * 2.0f - 1.0f;
			position._Y = position._Y * 2.0f - 1.0f;
		}
	}
}

/*
*	Updates widgets.
*/
void UISystem::UpdateWidgets(const MouseState *const RESTRICT mouse_state) NOEXCEPT
{
	//Cache the mouse data.
	const Vector2<float32> mouse_position{ Vector2<float32>(mouse_state->_CurrentX, mouse_state->_CurrentY) * _ReferenceResolution };
	const Vector2<float32> mouse_delta{ Vector2<float32>(mouse_state->_DeltaX, mouse_state->_DeltaY) * _ReferenceResolution };
	const ButtonState mouse_button_state{ mouse_state->_Left };
	const int8 mouse_scroll_wheel_step{ mouse_state->_ScrollWheelStep };

	//Remember if we have already consumed input.
	bool have_consumed_mouse_input{ false };
	bool have_consumed_scroll_input{ false };

	//Iterate through all scenes.
	if (!_Scenes.Empty())
	{
		for (int64 scene_index{ static_cast<int64>(_Scenes.Size() - 1) }; scene_index >= 0; --scene_index)
		{
			const UI::Scene *const RESTRICT scene{ _Scenes[scene_index] };

			//Iterate through all of this scene's widgets.
			const DynamicArray<UI::Widget *RESTRICT> &widgets{ scene->GetWidgets() };

			if (!widgets.Empty())
			{
				for (int64 widget_index{ static_cast<int64>(widgets.Size() - 1) }; widget_index >= 0; --widget_index)
				{
					UI::Widget *const RESTRICT widget{ widgets[widget_index] };

					for (UI::ClickableInterface *clickable_interface : widget->GetClickableInterfaces())
					{
						clickable_interface->SetMousePosition(mouse_position);
						clickable_interface->SetMouseDelta(mouse_delta);
					}

					if (!widget->IsEnabled())
					{
						continue;
					}

					//Update clickable interfaces.
					ArrayProxy<UI::ClickableInterface *const RESTRICT> clickable_interfaces{ widget->GetClickableInterfaces() };

					if (!clickable_interfaces.Empty())
					{
						for (int64 clickable_interface_index{ static_cast<int64>(clickable_interfaces.Size() - 1) }; clickable_interface_index >= 0; --clickable_interface_index)
						{
							UI::ClickableInterface *const RESTRICT clickable_interface{ clickable_interfaces[clickable_interface_index] };

							const bool is_inside{ clickable_interface->_IsInside(widget, clickable_interface, mouse_position) && !AnyWidgetBlockingPosition(mouse_position, widget) };
							have_consumed_mouse_input = UpdateClickableInterfaceState(widget, clickable_interface, mouse_button_state, is_inside, have_consumed_mouse_input);
						}
					}

					//Update scrollable interfaces.
					if (!have_consumed_scroll_input && mouse_scroll_wheel_step != 0)
					{
						if (UI::ScrollableInterface *const RESTRICT scrollable_interface{ widget->GetScrollableInterface() })
						{
							if (scrollable_interface->_IsInside(widget, mouse_position))
							{ 
								if (mouse_scroll_wheel_step < 0)
								{
									scrollable_interface->OnScrollDown();
								}

								else if (mouse_scroll_wheel_step > 0)
								{
									scrollable_interface->OnScrollUp();
								}

								have_consumed_scroll_input = true;
							}
						}
					}
				}
			}
		}
	}
	
}

/*
*	Returns if any widget is blocking the given position for the given widget.
*/
NO_DISCARD bool UISystem::AnyWidgetBlockingPosition(const Vector2<float32> position, const UI::Widget *const RESTRICT widget) NOEXCEPT
{
	for (int64 scene_index{ static_cast<int64>(_Scenes.Size() - 1) }; scene_index >= 0; --scene_index)
	{
		const UI::Scene *const RESTRICT scene{ _Scenes[scene_index] };

		//Iterate through all of this scene's widgets.
		const DynamicArray<UI::Widget *RESTRICT> &widgets{ scene->GetWidgets() };

		if (!widgets.Empty())
		{
			for (int64 widget_index{ static_cast<int64>(widgets.Size() - 1) }; widget_index >= 0; --widget_index)
			{
				UI::Widget *const RESTRICT _widget{ widgets[widget_index] };

				if (_widget == widget)
				{
					return false;
				}

				if (_widget->IsEnabled() && _widget->GetAxisAlignedBoundingBox().IsInside(position))
				{
					return true;
				}
			}
		}
	}

	return false;
}

/*
*	Updates the state of the given clickable interface with the given parameters.
*	Returns if the mouse input was consumed.
*/
NO_DISCARD bool UISystem::UpdateClickableInterfaceState
(
	UI::Widget *const RESTRICT widget,
	UI::ClickableInterface *const RESTRICT clickable_interface,
	const ButtonState mouse_button_state,
	const bool is_inside,
	bool mouse_input_consumed
) NOEXCEPT
{
	if (mouse_input_consumed)
	{
		clickable_interface->SetState(widget, UI::ClickableInterface::State::IDLE);

		return mouse_input_consumed;
	}

	switch (clickable_interface->GetState())
	{
		case UI::ClickableInterface::State::IDLE:
		{
			switch (mouse_button_state)
			{
				case ButtonState::PRESSED:
				{
					if (is_inside)
					{
						clickable_interface->SetState(widget, UI::ClickableInterface::State::PRESSED);
					}

					else
					{
						//Do nothing. (:
					}

					break;
				}

				case ButtonState::PRESSED_HELD:
				{
					if (is_inside)
					{
						//Do nothing. (:
					}

					else
					{
						//Do nothing. (:
					}

					break;
				}

				case ButtonState::RELEASED:
				{
					if (is_inside)
					{
						clickable_interface->SetState(widget, UI::ClickableInterface::State::HOVERED);

						mouse_input_consumed = true;
					}

					else
					{
						//Do nothing. (:
					}

					break;
				}

				case ButtonState::RELEASED_HELD:
				{
					if (is_inside)
					{
						clickable_interface->SetState(widget, UI::ClickableInterface::State::HOVERED);

						mouse_input_consumed = true;
					}

					else
					{
						//Do nothing. (:
					}

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

		case UI::ClickableInterface::State::HOVERED:
		{
			switch (mouse_button_state)
			{
				case ButtonState::PRESSED:
				{
					if (is_inside)
					{
						clickable_interface->SetState(widget, UI::ClickableInterface::State::PRESSED);

						mouse_input_consumed = true;
					}

					else
					{
						clickable_interface->SetState(widget, UI::ClickableInterface::State::IDLE);
					}

					break;
				}

				case ButtonState::PRESSED_HELD:
				{
					if (is_inside)
					{
						//Do nothing. (:
					}

					else
					{
						clickable_interface->SetState(widget, UI::ClickableInterface::State::IDLE);
					}

					break;
				}

				case ButtonState::RELEASED:
				{
					if (is_inside)
					{
						//Do nothing. (:
					}

					else
					{
						clickable_interface->SetState(widget, UI::ClickableInterface::State::IDLE);
					}

					break;
				}

				case ButtonState::RELEASED_HELD:
				{
					if (is_inside)
					{
						mouse_input_consumed = true;
					}

					else
					{
						clickable_interface->SetState(widget, UI::ClickableInterface::State::IDLE);
					}

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

		case UI::ClickableInterface::State::PRESSED:
		{
			switch (mouse_button_state)
			{
				case ButtonState::PRESSED:
				{
					if (is_inside)
					{
						//Do nothing. (:
					}

					else
					{
						//Do nothing. (:
					}

					break;
				}

				case ButtonState::PRESSED_HELD:
				{
					if (is_inside)
					{
						//Do nothing. (:
					}

					else
					{
						//Do nothing. (:
					}

					break;
				}

				case ButtonState::RELEASED:
				{
					if (is_inside)
					{
						clickable_interface->SetState(widget, UI::ClickableInterface::State::HOVERED);

						mouse_input_consumed = true;
					}

					else
					{
						clickable_interface->SetState(widget, UI::ClickableInterface::State::IDLE);
					}

					break;
				}

				case ButtonState::RELEASED_HELD:
				{
					if (is_inside)
					{
						clickable_interface->SetState(widget, UI::ClickableInterface::State::HOVERED);
					}

					else
					{
						clickable_interface->SetState(widget, UI::ClickableInterface::State::IDLE);
					}

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

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	return mouse_input_consumed;
}