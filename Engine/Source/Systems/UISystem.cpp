#if defined(USE_NEW_UI_SYSTEM)
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
	switch (phase)
	{
		case UpdatePhase::USER_INTERFACE:
		{
			UpdateUserInterface();

			break;
		}

		case UpdatePhase::PRE_RENDER:
		{
			UpdatePreRender();

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
*	Updates widgets.
*/
void UISystem::UpdateWidgets() NOEXCEPT
{
	//Cache the mouse data.
	const MouseState *const RESTRICT mouse_state{ InputSystem::Instance->GetMouseState(InputLayer::USER_INTERFACE) };
	const Vector2<float32> mouse_position{ Vector2<float32>(mouse_state->_CurrentX, mouse_state->_CurrentY) * UI::Constants::REFERENCE_RESOLUTION };
	const ButtonState mouse_button_state{ mouse_state->_Left };
	const int8 mouse_scroll_wheel_step{ mouse_state->_ScrollWheelStep };

	//Remember if we have already consumed input.
	bool have_consumed_click_input{ false };
	bool have_consumed_scroll_input{ false };

	//Iterate through all scenes.
	if (!_Scenes.Empty())
	{
		for (int64 scene_index{static_cast<int64>(_Scenes.Size() - 1)}; scene_index >= 0; --scene_index)
		{
			const UI::Scene *const RESTRICT scene{ _Scenes[scene_index] };

			//Iterate through all of this scene's widgets.
			const DynamicArray<UI::Widget *RESTRICT> &widgets{ scene->GetWidgets() };

			if (!widgets.Empty())
			{
				for (int64 widget_index{ static_cast<int64>(widgets.Size() - 1) }; widget_index >= 0; --widget_index)
				{
					UI::Widget *const RESTRICT widget{ widgets[widget_index] };

					if (!widget->IsEnabled())
					{
						continue;
					}

					//Update clickable interfaces.
					if (UI::ClickableInterface *const RESTRICT clickable_interface{ widget->GetClickableInterface() })
					{
						if (have_consumed_click_input)
						{
							clickable_interface->SetState(widget, UI::ClickableInterface::State::IDLE);
						}

						else
						{
							if (clickable_interface->_IsInside(widget, mouse_position))
							{
								if (mouse_button_state == ButtonState::PRESSED)
								{
									if (clickable_interface->GetState() == UI::ClickableInterface::State::HOVERED)
									{
										clickable_interface->SetState(widget, UI::ClickableInterface::State::PRESSED);
									}

									else
									{
										clickable_interface->SetState(widget, UI::ClickableInterface::State::HOVERED);
									}
								}

								else if (mouse_button_state == ButtonState::PRESSED_HELD)
								{
									//The clickable can only go from hovered to pressed if it was first hovered, and then actually pressed.
									if (clickable_interface->GetState() != UI::ClickableInterface::State::PRESSED)
									{
										clickable_interface->SetState(widget, UI::ClickableInterface::State::HOVERED);
									}
								}

								else
								{
									clickable_interface->SetState(widget, UI::ClickableInterface::State::HOVERED);
								}

								have_consumed_click_input = true;
							}

							else
							{
								clickable_interface->SetState(widget, UI::ClickableInterface::State::IDLE);
							}
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
*	Updates during the USER_INTERFACE phase.
*/
void UISystem::UpdateUserInterface() NOEXCEPT
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
	UI::BuildContext context;

	context._WidgetAllocator = &_WidgetAllocator;

	//Build all scenes.
	for (UI::Scene *const RESTRICT scene : _Scenes)
	{
		scene->Build(context);
	}

	//Clean the widget allocator.
	_WidgetAllocator.Clean();

	//Update the widgets.
	UpdateWidgets();
}

/*
*	Updates during the PRE_RENDER phase.
*/
void UISystem::UpdatePreRender() NOEXCEPT
{
	//Clear the render commands.
	_RenderCommands.Clear();

	//Set up the context.
	UI::RenderContext context;

	context._DeltaTime = CatalystEngineSystem::Instance->GetDeltaTime();
	context._RenderCommands = &_RenderCommands;

	//Render all scenes!
	for (UI::Scene *const RESTRICT scene : _Scenes)
	{
		scene->Render(context);
	}

	//Go through all render commands and make them ready for rendering.
	for (UI::RenderCommand &render_command : _RenderCommands)
	{
		//Flip position and transform into clip space.
		for (Vector4<float32> &position : render_command._Positions)
		{
			position._X /= UI::Constants::REFERENCE_RESOLUTION._X;
			position._Y /= UI::Constants::REFERENCE_RESOLUTION._Y;

			position._Y = 1.0f - position._Y;

			position._X = position._X * 2.0f - 1.0f;
			position._Y = position._Y * 2.0f - 1.0f;
		}
	}
}
#endif