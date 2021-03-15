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
*	Default constructor.
*/
UserInterfaceScene::UserInterfaceScene() NOEXCEPT
{
	//Set up default values.
	SetHorizontalSubdivision(33);
	SetVerticalSubdivision(33);

	_ButtonIdleMaterial.SetPrimaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 0.01f));
	_ButtonIdleMaterial.SetSecondaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
	_ButtonIdleMaterial.SetBorderOffset(0.0025f);

	_ButtonHoveredMaterial.SetPrimaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 0.25f));
	_ButtonHoveredMaterial.SetSecondaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
	_ButtonHoveredMaterial.SetBorderOffset(0.0025f);

	_ButtonPressedMaterial.SetPrimaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
	_ButtonPressedMaterial.SetSecondaryColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
	_ButtonPressedMaterial.SetBorderOffset(0.0025f);

	_ProgressBarBottomMaterial.SetPrimaryColor(Vector4<float32>(0.125f, 0.125f, 0.125f, 1.0f));
	_ProgressBarBottomMaterial.SetSecondaryColor(Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f));

	_ProgressBarTopMaterial.SetPrimaryColor(Vector4<float32>(0.5f, 0.5f, 0.5f, 1.0f));
	_ProgressBarTopMaterial.SetSecondaryColor(Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f));
}

/*
*	Callback for when this user interface scene is activated updated
*/
void UserInterfaceScene::Update() NOEXCEPT
{
	//Update the buttons.
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
*	Adds a button.
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
																			text) };

	//Add the button to the container.
	_Buttons.Emplace(new_button);

	//Return the button.
	return new_button;
}

/*
*	Adds an image.
*/
RESTRICTED UserInterfaceImage *const RESTRICT UserInterfaceScene::AddImage(	const Vector2<uint32> &minimum_cell,
																			const Vector2<uint32> &maximum_cell,
																			const UserInterfaceMaterial &material) NOEXCEPT
{
	//Calculate the bounding box.
	Vector2<float32> minimum;
	Vector2<float32> maximum;

	CalculateBoundingBox(minimum_cell, maximum_cell, &minimum, &maximum);

	//Allocate the image.
	UserInterfaceImage *const RESTRICT new_image{ new UserInterfaceImage(	minimum,
																			maximum,
																			material) };

	//Add the image to the container.
	_Images.Emplace(new_image);

	//Return the image.
	return new_image;
}

/*
*	Adds a progress bar.
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

	//Allocate the progress bar.
	UserInterfaceProgressBar* const RESTRICT new_progress_bar{ new UserInterfaceProgressBar(minimum,
																							maximum,
																							bottom_material_override ? *bottom_material_override : _ProgressBarBottomMaterial,
																							top_material_override ? *top_material_override : _ProgressBarTopMaterial,
																							text) };

	//Add the progress bar to the container.
	_ProgressBars.Emplace(new_progress_bar);

	//Return the progress bar.
	return new_progress_bar;
}

/*
*	Adds a text.
*/
RESTRICTED UserInterfaceText *const RESTRICT UserInterfaceScene::AddText(	const Vector2<uint32>& minimum_cell,
																			const Vector2<uint32> &maximum_cell,
																			const char *const RESTRICT text,
																			const TextHorizontalAlignment horizontal_alignment) NOEXCEPT
{
	//Calculate the bounding box.
	Vector2<float32> minimum;
	Vector2<float32> maximum;

	CalculateBoundingBox(minimum_cell, maximum_cell, &minimum, &maximum);

	//Allocate the text.
	UserInterfaceText* const RESTRICT new_text{ new UserInterfaceText(	minimum,
																		maximum,
																		text,
																		horizontal_alignment) };

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
	//If there's no buttons to update, just quit.
	if (_Buttons.Empty())
	{
		return;
	}

	//Cache the input data.
	const GamepadState *const RESTRICT gamepad_state{ InputSystem::Instance->GetGamepadState() };
	const Vector2<float32> mouse_position{ InputSystem::Instance->GetMouseState()->_CurrentX, InputSystem::Instance->GetMouseState()->_CurrentY };
	const bool mouse_pressed{ InputSystem::Instance->GetMouseState()->_Left == ButtonState::PRESSED  };

	//Update which button is gamepad selected.
	if (InputSystem::Instance->GetLastUpdatedInputDeviceType() == InputDeviceType::GAMEPAD)
	{
		//First of all, find the first and the currently gamepad selected button.
		UserInterfaceButton *RESTRICT first_button{ nullptr };
		UserInterfaceButton *RESTRICT currently_gamepad_selected_button{ nullptr };

		//Update the state of all buttons and call the callbacks if necessary.
		for (UserInterfaceButton *RESTRICT button : _Buttons)
		{
			//Remember the first button.
			first_button = first_button ? first_button : button;

			//Remember the currently gamepad selected button.
			if (button->GetGamepadSelected())
			{
				currently_gamepad_selected_button = button;

				break;
			}
		}

		//If there's no currently gamepad selected button, pick the first one found.
		if (!currently_gamepad_selected_button && first_button)
		{
			first_button->SetGamepadSelected(true);
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
				UserInterfaceButton *RESTRICT currently_gamepad_selected_button{ nullptr };

				//Update the state of all button user interface primitives and call the callbacks if necessary.
				for (UserInterfaceButton *const RESTRICT button : _Buttons)
				{
					//Remember the currently gamepad selected button.
					if (button->GetGamepadSelected())
					{
						currently_gamepad_selected_button = button;

						break;
					}
				}

				//If there are no buttons at all, just return.
				if (currently_gamepad_selected_button)
				{
					//Calculate the center of the currently gamepad selected button.
					const Vector2<float32> currently_gamepad_selected_button_center{ currently_gamepad_selected_button->GetMinimum() + ((currently_gamepad_selected_button->GetMaximum() - currently_gamepad_selected_button->GetMinimum()) * 0.5f) };

					//Find another button that is close and in the general direction.
					UserInterfaceButton *RESTRICT new_gamepad_selected_button{ nullptr };
					float32 shortest_distance{ FLOAT_MAXIMUM };

					for (UserInterfaceButton *const RESTRICT button : _Buttons)
					{
						//Is this element a button, and not the currently selected gamepad button?
						if (button != currently_gamepad_selected_button)
						{
							//Calculate the center of this button.
							const Vector2<float32> button_center{ button->GetMinimum() + ((button->GetMaximum() - button->GetMinimum()) * 0.5f) };

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
						currently_gamepad_selected_button->SetGamepadSelected(false);
						new_gamepad_selected_button->SetGamepadSelected(true);
					}
				}
			}
		}
	}

	//Update the state of all button user interface elements and call the callbacks if necessary.
	for (UserInterfaceButton *const RESTRICT button : _Buttons)
	{
		//Apply different logic based on which input device type was updated last.
		switch (InputSystem::Instance->GetLastUpdatedInputDeviceType())
		{
			case InputDeviceType::GAMEPAD:
			{
				//Mutate the state and call callbacks.
				switch (button->GetCurrentState())
				{
					case UserInterfaceButton::State::IDLE:
					{
						if (button->GetGamepadSelected())
						{
							if (gamepad_state->_A == ButtonState::PRESSED)
							{
								button->OnStartPressed();
								button->SetCurrentState(UserInterfaceButton::State::PRESSED);
							}

							else
							{
								button->OnStartHovered();
								button->SetCurrentState(UserInterfaceButton::State::HOVERED);
							}
						}

						break;
					}

					case UserInterfaceButton::State::HOVERED:
					{
						if (button->GetGamepadSelected())
						{
							if (gamepad_state->_A == ButtonState::PRESSED)
							{
								button->OnStartPressed();
								button->SetCurrentState(UserInterfaceButton::State::PRESSED);
							}
						}

						else
						{
							button->SetCurrentState(UserInterfaceButton::State::IDLE);
						}

						break;
					}

					case UserInterfaceButton::State::PRESSED:
					{
						if (button->GetGamepadSelected())
						{
							if (gamepad_state->_A == ButtonState::RELEASED)
							{
								button->OnStopPressed();
								button->SetCurrentState(UserInterfaceButton::State::HOVERED);
							}
						}

						else
						{
							button->SetCurrentState(UserInterfaceButton::State::IDLE);
						}

						break;
					}
					}

					break;
				}

				case InputDeviceType::KEYBOARD:
				case InputDeviceType::MOUSE:
				{
					//Determine if the mouse is inside the element's bounding box.
					const bool is_inside{	mouse_position._X >= button->GetMinimum()._X
											&& mouse_position._X <= button->GetMaximum()._X
											&& mouse_position._Y >= button->GetMinimum()._Y
											&& mouse_position._Y <= button->GetMaximum()._Y };

					//Mutate the state and call callbacks.
					switch (button->GetCurrentState())
					{
						case UserInterfaceButton::State::IDLE:
						{
							if (is_inside)
							{
								if (mouse_pressed)
								{
									button->OnStartPressed();
									button->SetCurrentState(UserInterfaceButton::State::PRESSED);
								}

								else
								{
									button->OnStartHovered();
									button->SetCurrentState(UserInterfaceButton::State::HOVERED);
								}
							}

							break;
						}

						case UserInterfaceButton::State::HOVERED:
						{
							if (is_inside)
							{
								if (mouse_pressed)
								{
									button->OnStartPressed();
									button->SetCurrentState(UserInterfaceButton::State::PRESSED);
								}
							}

							else
							{
								button->OnStopHovered();
								button->SetCurrentState(UserInterfaceButton::State::IDLE);
							}

							break;
						}

						case UserInterfaceButton::State::PRESSED:
						{
							if (is_inside)
							{
								if (!mouse_pressed)
								{
									button->OnStopPressed();
									button->SetCurrentState(UserInterfaceButton::State::HOVERED);
								}
							}

							else
							{
								button->OnStopPressed();
								button->SetCurrentState(UserInterfaceButton::State::IDLE);
							}

							break;
						}
					}

				break;
			}
		}
	}
}