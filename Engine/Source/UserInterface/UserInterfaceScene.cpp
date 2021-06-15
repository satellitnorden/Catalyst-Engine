//Header file.
#include <UserInterface/UserInterfaceScene.h>

//Systems.
#include <Systems/ResourceSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/UserInterfaceSystem.h>

//User interface scene constants.
namespace UserInterfaceSceneConstants
{
	constexpr float32 PADDING{ 0.001'25f };
}

/*
*	Callback for when this user interface scene is activated.
*/
void UserInterfaceScene::OnActivated() NOEXCEPT
{
	//Define constants.
	constexpr float32 BUTTON_BORDER_OFFSET{ 0.002'25f };
	constexpr float32 CHECKBOX_BORDER_OFFSET{ 0.016'50f };

	//Set up default values.
	SetHorizontalSubdivision(33);
	SetVerticalSubdivision(33);

	_FontResource = ResourceSystem::Instance->GetFontResource(HashString("Catalyst_Engine_Default_Font"));

	_ButtonIdleMaterial.SetPrimaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 0.01f));
	_ButtonIdleMaterial.SetSecondaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
	_ButtonIdleMaterial.SetBorderOffset(BUTTON_BORDER_OFFSET);

	_ButtonHoveredMaterial.SetPrimaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 0.25f));
	_ButtonHoveredMaterial.SetSecondaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
	_ButtonHoveredMaterial.SetBorderOffset(BUTTON_BORDER_OFFSET);

	_ButtonPressedMaterial.SetPrimaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
	_ButtonPressedMaterial.SetSecondaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
	_ButtonPressedMaterial.SetBorderOffset(BUTTON_BORDER_OFFSET);

	_CheckboxUncheckedIdleMaterial.SetPrimaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 0.01f));
	_CheckboxUncheckedIdleMaterial.SetSecondaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
	_CheckboxUncheckedIdleMaterial.SetBorderOffset(CHECKBOX_BORDER_OFFSET);

	_CheckboxUncheckedHoveredMaterial.SetPrimaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 0.25f));
	_CheckboxUncheckedHoveredMaterial.SetSecondaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
	_CheckboxUncheckedHoveredMaterial.SetBorderOffset(CHECKBOX_BORDER_OFFSET);

	_CheckboxUncheckedPressedMaterial.SetPrimaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
	_CheckboxUncheckedPressedMaterial.SetSecondaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
	_CheckboxUncheckedPressedMaterial.SetBorderOffset(CHECKBOX_BORDER_OFFSET);

	_CheckboxCheckedIdleMaterial.SetPrimaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 0.25f));
	_CheckboxCheckedIdleMaterial.SetSecondaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
	_CheckboxCheckedIdleMaterial.SetBorderOffset(CHECKBOX_BORDER_OFFSET);

	_CheckboxCheckedHoveredMaterial.SetPrimaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 0.5f));
	_CheckboxCheckedHoveredMaterial.SetSecondaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
	_CheckboxCheckedHoveredMaterial.SetBorderOffset(CHECKBOX_BORDER_OFFSET);

	_CheckboxCheckedPressedMaterial.SetPrimaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
	_CheckboxCheckedPressedMaterial.SetSecondaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
	_CheckboxCheckedPressedMaterial.SetBorderOffset(CHECKBOX_BORDER_OFFSET);

	_ProgressBarBottomMaterial.SetPrimaryColor(Vector4<float32>(0.125f, 0.125f, 0.125f, 1.0f));
	_ProgressBarBottomMaterial.SetSecondaryColor(Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f));

	_ProgressBarTopMaterial.SetPrimaryColor(Vector4<float32>(0.5f, 0.5f, 0.5f, 1.0f));
	_ProgressBarTopMaterial.SetSecondaryColor(Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f));

	_TextScale = 0.015f;
	_TextSmoothingFactor = 0.2f;
}

/*
*	Callback for when this user interface scene is activated updated
*/
void UserInterfaceScene::Update() NOEXCEPT
{
	//Update buttons.
	UpdateButtons();
}

/*
*	Callback for when this user interface scene is deactivated.
*/
void UserInterfaceScene::OnDeactivated() NOEXCEPT
{
	//Free all buttons.
	for (UserInterfaceButton *const RESTRICT button : _Buttons)
	{
		delete button;
	}

	_Buttons.Clear();

	//Free all checkboxes.
	for (UserInterfaceCheckbox *const RESTRICT checkbox : _Checkboxes)
	{
		delete checkbox;
	}

	_Checkboxes.Clear();

	//Free all images.
	for (UserInterfaceImage *const RESTRICT image : _Images)
	{
		delete image;
	}

	_Images.Clear();

	//Free all progress bars.
	for (UserInterfaceProgressBar *const RESTRICT progress_bar : _ProgressBars)
	{
		delete progress_bar;
	}

	_ProgressBars.Clear();

	//Free all texts.
	for (UserInterfaceText *const RESTRICT text : _Texts)
	{
		delete text;
	}

	_Texts.Clear();
}

/*
*	Returns the user interface primitives for this scene.
*/
void UserInterfaceScene::RetrieveUserInterfacePrimitives(DynamicArray<const UserInterfacePrimitive *RESTRICT> *const RESTRICT output) NOEXCEPT
{
	//First of all, clear.
	output->Clear();

	//Retrieve all text primitives.
	for (UserInterfaceText *const RESTRICT text : _Texts)
	{
		text->RetrieveUserInterfacePrimitives(output);
	}
}

/*
*	Adds a button, using cells.
*/
RESTRICTED UserInterfaceButton *const RESTRICT UserInterfaceScene::AddButton(	const Vector2<uint32> &minimum_cell,
																				const Vector2<uint32> &maximum_cell,
																				const UserInterfaceButton::Callback start_pressed_callback,
																				UserInterfaceMaterial *const RESTRICT idle_material_override,
																				UserInterfaceMaterial *const RESTRICT hovered_material_override,
																				UserInterfaceMaterial *const RESTRICT pressed_material_override,
																				const char *const RESTRICT text) NOEXCEPT
{
	//Calculate the bounding box.
	Vector2<float32> minimum;
	Vector2<float32> maximum;

	CalculateBoundingBox(minimum_cell, maximum_cell, &minimum, &maximum);

	//Add the button.
	return AddButton(	minimum,
						maximum,
						start_pressed_callback,
						idle_material_override,
						hovered_material_override,
						pressed_material_override,
						text);
}

/*
*	Adds a button, raw coordinates.
*/
RESTRICTED UserInterfaceButton* const RESTRICT UserInterfaceScene::AddButton(	const Vector2<float32> &minimum,
																				const Vector2<float32> &maximum,
																				const UserInterfaceButton::Callback start_pressed_callback,
																				UserInterfaceMaterial *const RESTRICT idle_material_override,
																				UserInterfaceMaterial *const RESTRICT hovered_material_override,
																				UserInterfaceMaterial *const RESTRICT pressed_material_override,
																				const char *const RESTRICT text) NOEXCEPT
{
	//Allocate the button.
	UserInterfaceButton *const RESTRICT new_button{ new UserInterfaceButton(minimum,
																			maximum,
																			nullptr,
																			nullptr,
																			start_pressed_callback,
																			nullptr,
																			idle_material_override ? *idle_material_override : _ButtonIdleMaterial,
																			hovered_material_override ? *hovered_material_override : _ButtonHoveredMaterial,
																			pressed_material_override ? *pressed_material_override : _ButtonPressedMaterial,
																			text,
																			_FontResource,
																			_IsThreeDimensional) };

	//Add the button to the container.
	_Buttons.Emplace(new_button);

	//Add the button interface to the container.
	_ButtonInterfaces.Emplace(new_button);

	//Return the button.
	return new_button;
}

/*
*	Adds a checkbox, using cells.
*/
RESTRICTED UserInterfaceCheckbox *const RESTRICT UserInterfaceScene::AddCheckbox(	const Vector2<uint32> &minimum_cell,
																					const Vector2<uint32> &maximum_cell,
																					const UserInterfaceCheckbox::Callback start_pressed_callback,
																					UserInterfaceMaterial *const RESTRICT unchecked_idle_material_override,
																					UserInterfaceMaterial *const RESTRICT unchecked_hovered_material_override,
																					UserInterfaceMaterial *const RESTRICT unchecked_pressed_material_override,
																					UserInterfaceMaterial *const RESTRICT checked_idle_material_override,
																					UserInterfaceMaterial *const RESTRICT checked_hovered_material_override,
																					UserInterfaceMaterial *const RESTRICT checked_pressed_material_override,
																					const char *const RESTRICT text) NOEXCEPT
{
	//Calculate the bounding box.
	Vector2<float32> minimum;
	Vector2<float32> maximum;

	CalculateBoundingBox(minimum_cell, maximum_cell, &minimum, &maximum);

	//Add the checkbox.
	return AddCheckbox(	minimum,
						maximum,
						start_pressed_callback,
						unchecked_idle_material_override,
						unchecked_hovered_material_override,
						unchecked_pressed_material_override,
						checked_idle_material_override,
						checked_hovered_material_override,
						checked_pressed_material_override,
						text);
}


/*
*	Adds a checkbox, using raw coordinates.
*/
RESTRICTED UserInterfaceCheckbox* const RESTRICT UserInterfaceScene::AddCheckbox(	const Vector2<float32> &minimum,
																					const Vector2<float32> &maximum,
																					const UserInterfaceCheckbox::Callback start_pressed_callback,
																					UserInterfaceMaterial *const RESTRICT unchecked_idle_material_override,
																					UserInterfaceMaterial *const RESTRICT unchecked_hovered_material_override,
																					UserInterfaceMaterial *const RESTRICT unchecked_pressed_material_override,
																					UserInterfaceMaterial *const RESTRICT checked_idle_material_override,
																					UserInterfaceMaterial *const RESTRICT checked_hovered_material_override,
																					UserInterfaceMaterial *const RESTRICT checked_pressed_material_override,
																					const char *const RESTRICT text) NOEXCEPT
{
	//Allocate the checkbox.
	UserInterfaceCheckbox *const RESTRICT new_checkbox{ new UserInterfaceCheckbox(	minimum,
																					maximum,
																					nullptr,
																					nullptr,
																					start_pressed_callback,
																					nullptr,
																					unchecked_idle_material_override ? *unchecked_idle_material_override : _CheckboxUncheckedIdleMaterial,
																					unchecked_hovered_material_override ? *unchecked_hovered_material_override : _CheckboxUncheckedHoveredMaterial,
																					unchecked_pressed_material_override ? *unchecked_pressed_material_override : _CheckboxUncheckedPressedMaterial,
																					checked_idle_material_override ? *checked_idle_material_override : _CheckboxCheckedIdleMaterial,
																					checked_hovered_material_override ? *checked_hovered_material_override : _CheckboxCheckedHoveredMaterial,
																					checked_pressed_material_override ? *checked_pressed_material_override : _CheckboxCheckedPressedMaterial,
																					text,
																					_FontResource,
																					_IsThreeDimensional) };

	//Add the checkbox to the container.
	_Checkboxes.Emplace(new_checkbox);

	//Add the button interface to the container.
	_ButtonInterfaces.Emplace(new_checkbox);

	//Return the checkbox.
	return new_checkbox;
}

/*
*	Adds an image, using cells.
*/
RESTRICTED UserInterfaceImage *const RESTRICT UserInterfaceScene::AddImage(	const Vector2<uint32> &minimum_cell,
																			const Vector2<uint32> &maximum_cell,
																			const UserInterfaceMaterial &material,
																			const float32 opacity) NOEXCEPT
{
	//Calculate the bounding box.
	Vector2<float32> minimum;
	Vector2<float32> maximum;

	CalculateBoundingBox(minimum_cell, maximum_cell, &minimum, &maximum);

	//Add the image.
	return AddImage(minimum,
					maximum,
					material,
					opacity);
}
/*
*	Adds an image, raw coordinates.
*/
RESTRICTED UserInterfaceImage* const RESTRICT UserInterfaceScene::AddImage(	const Vector2<float32> &minimum,
																			const Vector2<float32> &maximum,
																			const UserInterfaceMaterial &material,
																			const float32 opacity) NOEXCEPT
{
	//Allocate the image.
	UserInterfaceImage *const RESTRICT new_image{ new UserInterfaceImage(	minimum,
																			maximum,
																			material,
																			opacity,
																			_IsThreeDimensional) };

	//Add the image to the container.
	_Images.Emplace(new_image);

	//Return the image.
	return new_image;
}

/*
*	Adds a progress bar, using cells.
*/
RESTRICTED UserInterfaceProgressBar *const RESTRICT UserInterfaceScene::AddProgressBar(	const Vector2<uint32> &minimum_cell,
																						const Vector2<uint32> &maximum_cell,
																						const char *const RESTRICT text,
																						UserInterfaceMaterial *const RESTRICT bottom_material_override,
																						UserInterfaceMaterial *const RESTRICT top_material_override) NOEXCEPT
{
	//Calculate the bounding box.
	Vector2<float32> minimum;
	Vector2<float32> maximum;

	CalculateBoundingBox(minimum_cell, maximum_cell, &minimum, &maximum);

	//Add the progress bar.
	return AddProgressBar(	minimum,
							maximum,
							text,
							bottom_material_override,
							top_material_override);
}

/*
*	Adds a progress bar, using raw coordinates.
*/
RESTRICTED UserInterfaceProgressBar* const RESTRICT UserInterfaceScene::AddProgressBar(	const Vector2<float32> &minimum,
																						const Vector2<float32> &maximum,
																						const char *const RESTRICT text,
																						UserInterfaceMaterial *const RESTRICT bottom_material_override,
																						UserInterfaceMaterial *const RESTRICT top_material_override) NOEXCEPT
{
	//Allocate the progress bar.
	UserInterfaceProgressBar* const RESTRICT new_progress_bar{ new UserInterfaceProgressBar(minimum,
																							maximum,
																							bottom_material_override ? *bottom_material_override : _ProgressBarBottomMaterial,
																							top_material_override ? *top_material_override : _ProgressBarTopMaterial,
																							text,
																							_IsThreeDimensional) };

	//Add the progress bar to the container.
	_ProgressBars.Emplace(new_progress_bar);

	//Return the progress bar.
	return new_progress_bar;
}

/*
*	Adds a text, using cells.
*/
RESTRICTED UserInterfaceText *const RESTRICT UserInterfaceScene::AddText(	const Vector2<uint32> &minimum_cell,
																			const Vector2<uint32> &maximum_cell,
																			const char *const RESTRICT text,
																			const float32 *const RESTRICT scale_override,
																			const TextHorizontalAlignment horizontal_alignment,
																			const TextVerticalAlignment vertical_alignment,
																			const float32 *const RESTRICT smoothing_factor_override) NOEXCEPT
{
	//Calculate the bounding box.
	Vector2<float32> minimum;
	Vector2<float32> maximum;

	CalculateBoundingBox(minimum_cell, maximum_cell, &minimum, &maximum);

	//Add the text.
	return AddText(	minimum,
					maximum,
					text,
					scale_override,
					horizontal_alignment,
					vertical_alignment,
					smoothing_factor_override);
}

/*
*	Adds a text, using raw coordinates.
*/
RESTRICTED UserInterfaceText* const RESTRICT UserInterfaceScene::AddText(	const Vector2<float32> &minimum,
																			const Vector2<float32> &maximum,
																			const char *const RESTRICT text,
																			const float32 *const RESTRICT scale_override,
																			const TextHorizontalAlignment horizontal_alignment,
																			const TextVerticalAlignment vertical_alignment,
																			const float32 *const RESTRICT smoothing_factor_override) NOEXCEPT
{
	//Allocate the text.
	UserInterfaceText* const RESTRICT new_text{ new UserInterfaceText(	minimum,
																		maximum,
																		text,
																		_FontResource,
																		scale_override ? *scale_override : _TextScale,
																		horizontal_alignment,
																		vertical_alignment,
																		smoothing_factor_override ? *smoothing_factor_override : _TextSmoothingFactor,
																		_IsThreeDimensional) };

	//Add the text to the container.
	_Texts.Emplace(new_text);

	//Return the text.
	return new_text;
}

/*
*	Calculates the bounding box for the given minimum/maximum cell.
*/
void UserInterfaceScene::CalculateBoundingBox(	const Vector2<uint32>& minimum_cell,
												const Vector2<uint32>& maximum_cell,
												Vector2<float32>* const RESTRICT minimum,
												Vector2<float32>* const RESTRICT maximum) NOEXCEPT
{
	minimum->_X = static_cast<float32>(minimum_cell._X) * _HorizontalSubdivisionReciprocal + UserInterfaceSceneConstants::PADDING;
	minimum->_Y = static_cast<float32>(minimum_cell._Y) * _VerticalSubdivisionReciprocal + UserInterfaceSceneConstants::PADDING;

	maximum->_X = static_cast<float32>(maximum_cell._X + 1) * _HorizontalSubdivisionReciprocal - UserInterfaceSceneConstants::PADDING;
	maximum->_Y = static_cast<float32>(maximum_cell._Y + 1) * _VerticalSubdivisionReciprocal - UserInterfaceSceneConstants::PADDING;
}

/*
*	Updates buttons.
*/
void UserInterfaceScene::UpdateButtons() NOEXCEPT
{
	//If there's no button interface to update, just quit.
	if (_ButtonInterfaces.Empty())
	{
		return;
	}

	//Cache the input data.
	const GamepadState *const RESTRICT gamepad_state{ InputSystem::Instance->GetGamepadState() };

	Vector2<float32> mouse_position;
	bool mouse_pressed;

	//Cheat a little bit and treat touch platforms the same as if they were a mouse.
	if (InputSystem::Instance->GetLastUpdatedInputDeviceType() == InputDeviceType::TOUCH)
	{
		mouse_position = Vector2<float32>(InputSystem::Instance->GetTouchState()->_CurrentX, InputSystem::Instance->GetTouchState()->_CurrentY);
		mouse_pressed = InputSystem::Instance->GetTouchState()->_ButtonState == ButtonState::PRESSED;
	}

	else
	{
		mouse_position = Vector2<float32>(InputSystem::Instance->GetMouseState()->_CurrentX, InputSystem::Instance->GetMouseState()->_CurrentY);
		mouse_pressed = InputSystem::Instance->GetMouseState()->_Left == ButtonState::PRESSED;
	}

	//Update which button is gamepad selected.
	if (InputSystem::Instance->GetLastUpdatedInputDeviceType() == InputDeviceType::GAMEPAD)
	{
		//First of all, find the first and the currently gamepad selected button.
		ButtonInterface first_button;
		ButtonInterface currently_gamepad_selected_button;

		//Update the state of all buttons and call the callbacks if necessary.
		for (ButtonInterface &button : _ButtonInterfaces)
		{
			//Remember the first button.
			first_button = first_button ? first_button : button;

			//Remember the currently gamepad selected button.
			if (button.GetGamepadSelected())
			{
				currently_gamepad_selected_button = button;

				break;
			}
		}

		//If there's no currently gamepad selected button, pick the first one found.
		if (!currently_gamepad_selected_button && first_button)
		{
			first_button.SetGamepadSelected(true);
		}

		//Choose a new gamepad selected button.
		{
			Vector2<float32> direction{ 0.0f, 0.0f };

			if (gamepad_state->_DpadUp == ButtonState::PRESSED)
			{
				direction = Vector2<float32>(0.0f, 1.0f);
			}

			else if (gamepad_state->_DpadDown == ButtonState::PRESSED)
			{
				direction = Vector2<float32>(0.0f, -1.0f);
			}

			else if (gamepad_state->_DpadLeft == ButtonState::PRESSED)
			{
				direction = Vector2<float32>(-1.0f, 0.0f);
			}

			else if (gamepad_state->_DpadRight == ButtonState::PRESSED)
			{
				direction = Vector2<float32>(1.0f, 0.0f);
			}

			if (!direction.IsZero())
			{
				//First of all, find the currently gamepad selected button.
				ButtonInterface currently_gamepad_selected_button;

				//Update the state of all button user interface primitives and call the callbacks if necessary.
				for (ButtonInterface &button : _ButtonInterfaces)
				{
					//Remember the currently gamepad selected button.
					if (button.GetGamepadSelected())
					{
						currently_gamepad_selected_button = button;

						break;
					}
				}

				//If there are no buttons at all, just return.
				if (currently_gamepad_selected_button)
				{
					//Calculate the center of the currently gamepad selected button.
					const Vector2<float32> currently_gamepad_selected_button_center{ currently_gamepad_selected_button.GetMinimum() + ((currently_gamepad_selected_button.GetMaximum() - currently_gamepad_selected_button.GetMinimum()) * 0.5f) };

					//Find another button that is close and in the general direction.
					ButtonInterface new_gamepad_selected_button;
					float32 shortest_distance{ FLOAT_MAXIMUM };

					for (ButtonInterface &button : _ButtonInterfaces)
					{
						//Is this element a button, and not the currently selected gamepad button?
						if (button != currently_gamepad_selected_button)
						{
							//Calculate the center of this button.
							const Vector2<float32> button_center{ button.GetMinimum() + ((button.GetMaximum() - button.GetMinimum()) * 0.5f) };

							//Calculate the distance to ths button.
							const float32 distance_to_button{ Vector2<float32>::Length(button_center - currently_gamepad_selected_button_center) };

							//Calculate the direction to this button.
							const Vector2<float32> direction_to_button{ (button_center - currently_gamepad_selected_button_center) / distance_to_button };

							//Test the angle.
							if (Vector2<float32>::DotProduct(direction, direction_to_button) >= 0.5f)
							{
								//Test the distance.
								if (shortest_distance > distance_to_button)
								{
									new_gamepad_selected_button = button;
									shortest_distance = distance_to_button;
								}

							}
						}
					}

					//Is there a new gamepad selected button?
					if (new_gamepad_selected_button)
					{
						currently_gamepad_selected_button.SetGamepadSelected(false);
						new_gamepad_selected_button.SetGamepadSelected(true);
					}
				}
			}
		}
	}

	//Update the state of all button user interface elements and call the callbacks if necessary.
	for (ButtonInterface &button : _ButtonInterfaces)
	{
		//Apply different logic based on which input device type was updated last.
		switch (InputSystem::Instance->GetLastUpdatedInputDeviceType())
		{
			case InputDeviceType::GAMEPAD:
			{
				//Mutate the state and call callbacks.
				switch (button.GetCurrentState())
				{
					case UserInterfaceButtonState::IDLE:
					{
						if (button.GetGamepadSelected())
						{
							if (gamepad_state->_A == ButtonState::PRESSED)
							{
								button.OnStartPressed();
								button.SetCurrentState(UserInterfaceButtonState::PRESSED);
							}

							else
							{
								button.OnStartHovered();
								button.SetCurrentState(UserInterfaceButtonState::HOVERED);
							}
						}

						break;
					}

					case UserInterfaceButtonState::HOVERED:
					{
						if (button.GetGamepadSelected())
						{
							if (gamepad_state->_A == ButtonState::PRESSED)
							{
								button.OnStartPressed();
								button.SetCurrentState(UserInterfaceButtonState::PRESSED);
							}
						}

						else
						{
							button.SetCurrentState(UserInterfaceButtonState::IDLE);
						}

						break;
					}

					case UserInterfaceButtonState::PRESSED:
					{
						if (button.GetGamepadSelected())
						{
							if (gamepad_state->_A == ButtonState::RELEASED)
							{
								button.OnStopPressed();
								button.SetCurrentState(UserInterfaceButtonState::HOVERED);
							}
						}

						else
						{
							button.SetCurrentState(UserInterfaceButtonState::IDLE);
						}

						break;
					}
					}

					break;
				}

				case InputDeviceType::KEYBOARD:
				case InputDeviceType::MOUSE:
				case InputDeviceType::TOUCH: //Touch input here is interpreted as mouse input, so cheat a little. (:
				{
					//Determine if the mouse is inside the element's bounding box.
					const bool is_inside{	mouse_position._X >= button.GetMinimum()._X
											&& mouse_position._X <= button.GetMaximum()._X
											&& mouse_position._Y >= button.GetMinimum()._Y
											&& mouse_position._Y <= button.GetMaximum()._Y };

					//Mutate the state and call callbacks.
					switch (button.GetCurrentState())
					{
						case UserInterfaceButtonState::IDLE:
						{
							if (is_inside)
							{
								if (mouse_pressed)
								{
									button.OnStartPressed();
									button.SetCurrentState(UserInterfaceButtonState::PRESSED);
								}

								else
								{
									button.OnStartHovered();
									button.SetCurrentState(UserInterfaceButtonState::HOVERED);
								}
							}

							break;
						}

						case UserInterfaceButtonState::HOVERED:
						{
							if (is_inside)
							{
								if (mouse_pressed)
								{
									button.OnStartPressed();
									button.SetCurrentState(UserInterfaceButtonState::PRESSED);
								}
							}

							else
							{
								button.OnStopHovered();
								button.SetCurrentState(UserInterfaceButtonState::IDLE);
							}

							break;
						}

						case UserInterfaceButtonState::PRESSED:
						{
							if (is_inside)
							{
								if (!mouse_pressed)
								{
									button.OnStopPressed();
									button.SetCurrentState(UserInterfaceButtonState::HOVERED);
								}
							}

							else
							{
								button.OnStopPressed();
								button.SetCurrentState(UserInterfaceButtonState::IDLE);
							}

							break;
						}
					}

				break;
			}
		}
	}
}