//Header file.
#include <UserInterface/UserInterfaceScene.h>

//Components.
#include <Components/Components/WorldTransformComponent.h>

//Math.
#include <Math/Core/CatalystgeometryMath.h>

//Systems.
#if defined(CATALYST_EDITOR)
	#include <Systems/CatalystEditorSystem.h>
#endif
#include <Systems/CatalystEngineSystem.h>
#include <Systems/ContentSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/MemorySystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/UserInterfaceSystem.h>
#include <Systems/WorldSystem.h>

//User interface scene constants.
namespace UserInterfaceSceneConstants
{
	constexpr float32 PADDING{ 0.001'25f };
}

/*
*	Default destructor.
*/
UserInterfaceScene::~UserInterfaceScene()
{
	//Deactivate this user interface scene, if it is active.
	if (!CatalystEngineSystem::Instance->ShouldTerminate() && _IsActive)
	{
		OnDeactivated();
		_IsActive = false;
	}
}

/*
*	Callback for when this user interface scene is activated.
*/
void UserInterfaceScene::OnActivated() NOEXCEPT
{
	//Set up default values.
	SetHorizontalSubdivision(33);
	SetVerticalSubdivision(33);

	_Font = ContentSystem::Instance->GetAsset<FontAsset>(HashString("Default"));

	_ButtonIdleMaterial.SetColor(Vector4<float32>(0.25f, 0.25f, 0.25f, 0.5f));

	_ButtonHoveredMaterial.SetColor(Vector4<float32>(0.5f, 0.5f, 0.5f, 0.75f));

	_ButtonPressedMaterial.SetColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));

	_CheckboxUncheckedIdleMaterial.SetColor(Vector4<float32>(0.25f, 0.25f, 0.25f, 0.5f));

	_CheckboxUncheckedHoveredMaterial.SetColor(Vector4<float32>(0.5f, 0.5f, 0.5f, 0.75f));

	_CheckboxUncheckedPressedMaterial.SetColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));

	_CheckboxCheckedIdleMaterial.SetColor(Vector4<float32>(0.75f, 0.75f, 0.75f, 0.75f));

	_CheckboxCheckedHoveredMaterial.SetColor(Vector4<float32>(0.875f, 0.875f, 0.875f, 0.875f));

	_CheckboxCheckedPressedMaterial.SetColor(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));

	_ProgressBarBottomMaterial.SetColor(Vector4<float32>(0.125f, 0.125f, 0.125f, 0.5f));

	_ProgressBarTopMaterial.SetColor(Vector4<float32>(0.5f, 0.5f, 0.5f, 1.0f));

	_TextScale = 0.02'50f;
}

/*
*	Callback for when this user interface scene is activated updated
*/
void UserInterfaceScene::Update() NOEXCEPT
{
	//Update buttons.
	UpdateButtons();

	//Update the text inputs.
	UpdateTextInputs();
}

/*
*	Callback for when this user interface scene is deactivated.
*/
void UserInterfaceScene::OnDeactivated() NOEXCEPT
{
	//Clear the elements.
	_Elements.Clear();

	//Free all buttons.
	for (UserInterfaceButton *const RESTRICT button : _Buttons)
	{
		button->~UserInterfaceButton();
		MemorySystem::Instance->TypeFree<UserInterfaceButton>(button);
	}

	_Buttons.Clear();

	//Free all checkboxes.
	for (UserInterfaceCheckbox *const RESTRICT checkbox : _Checkboxes)
	{
		checkbox->~UserInterfaceCheckbox();
		MemorySystem::Instance->TypeFree<UserInterfaceCheckbox>(checkbox);
	}

	_Checkboxes.Clear();

	//Free all images.
	for (UserInterfaceImage *const RESTRICT image : _Images)
	{
		image->~UserInterfaceImage();
		MemorySystem::Instance->TypeFree<UserInterfaceImage>(image);
	}

	_Images.Clear();

	//Free all progress bars.
	for (UserInterfaceProgressBar *const RESTRICT progress_bar : _ProgressBars)
	{
		progress_bar->~UserInterfaceProgressBar();
		MemorySystem::Instance->TypeFree<UserInterfaceProgressBar>(progress_bar);
	}

	_ProgressBars.Clear();

	//Free all texts.
	for (UserInterfaceText *const RESTRICT text : _Texts)
	{
		text->~UserInterfaceText();
		MemorySystem::Instance->TypeFree<UserInterfaceText>(text);
	}

	_Texts.Clear();

	//Free all text inputs.
	for (UserInterfaceTextInput *const RESTRICT text_input : _TextInputs)
	{
		text_input->~UserInterfaceTextInput();
		MemorySystem::Instance->TypeFree<UserInterfaceTextInput>(text_input);
	}

	_TextInputs.Clear();

	//Clear the button interfaces.
	_ButtonInterfaces.Clear();
}

/*
*	Returns the user interface primitives for this scene.
*/
void UserInterfaceScene::RetrieveUserInterfacePrimitives(DynamicArray<UserInterfacePrimitive *RESTRICT> *const RESTRICT output) NOEXCEPT
{
	//First of all, clear.
	output->Clear();

	//Retrieve all the primitives from the elements.
	for (UserInterfaceElement *const RESTRICT element : _Elements)
	{
		element->RetrieveUserInterfacePrimitives(output);
	}
}

/*
*	Adds a button, using cells.
*/
RESTRICTED UserInterfaceButton *const RESTRICT UserInterfaceScene::AddButtonByCell(	const Vector2<uint32> &minimum_cell,
																					const Vector2<uint32> &maximum_cell,
																					const UserInterfaceButton::Callback start_pressed_callback,
																					const UserInterfaceButton::Callback stop_pressed_callback,
																					const UserInterfaceButton::Callback start_hovered_callback,
																					const UserInterfaceButton::Callback stop_hovered_callback,
																					UserInterfaceMaterial *const RESTRICT idle_material_override,
																					UserInterfaceMaterial *const RESTRICT hovered_material_override,
																					UserInterfaceMaterial *const RESTRICT pressed_material_override,
																					const char *const RESTRICT text,
																					const float32 *const RESTRICT text_scale_override) NOEXCEPT
{
	//Calculate the bounding box.
	Vector2<float32> minimum;
	Vector2<float32> maximum;

	CalculateBoundingBox(minimum_cell, maximum_cell, &minimum, &maximum);

	//Add the button.
	return AddButtonByNormalizedCoordinate(	minimum,
											maximum,
											start_pressed_callback,
											stop_pressed_callback,
											start_hovered_callback,
											stop_hovered_callback,
											idle_material_override,
											hovered_material_override,
											pressed_material_override,
											text,
											text_scale_override);
}

/*
*	Adds a button, raw coordinates.
*/
RESTRICTED UserInterfaceButton* const RESTRICT UserInterfaceScene::AddButtonByNormalizedCoordinate(	const Vector2<float32> &minimum,
																									const Vector2<float32> &maximum,
																									const UserInterfaceButton::Callback start_pressed_callback,
																									const UserInterfaceButton::Callback stop_pressed_callback,
																									const UserInterfaceButton::Callback start_hovered_callback,
																									const UserInterfaceButton::Callback stop_hovered_callback,
																									UserInterfaceMaterial *const RESTRICT idle_material_override,
																									UserInterfaceMaterial *const RESTRICT hovered_material_override,
																									UserInterfaceMaterial *const RESTRICT pressed_material_override,
																									const char *const RESTRICT text,
																									const float32 *const RESTRICT text_scale_override) NOEXCEPT
{
	//Allocate the button.
	UserInterfaceButton *const RESTRICT new_button{ new (MemorySystem::Instance->TypeAllocate<UserInterfaceButton>()) UserInterfaceButton(minimum,
																			maximum,
																			start_pressed_callback,
																			stop_pressed_callback,
																			start_hovered_callback,
																			stop_hovered_callback,
																			idle_material_override ? *idle_material_override : _ButtonIdleMaterial,
																			hovered_material_override ? *hovered_material_override : _ButtonHoveredMaterial,
																			pressed_material_override ? *pressed_material_override : _ButtonPressedMaterial,
																			text,
																			_Font,
																			text_scale_override ? *text_scale_override : _TextScale) };

	//Add the button to the container.
	_Buttons.Emplace(new_button);

	//Add the button interface to the container.
	_ButtonInterfaces.Emplace(new_button);

	//Add the button to the elements container.
	_Elements.Emplace(new_button);

	//Set the parent.
	new_button->SetParent(this);

	//Return the button.
	return new_button;
}

/*
*	Removes a button.
*/
void UserInterfaceScene::RemoveButton(UserInterfaceButton *const RESTRICT button) NOEXCEPT
{
	_Buttons.Erase<false>(button);
	_ButtonInterfaces.Erase<false>(button);
	_Elements.Erase<true>(button);

	button->~UserInterfaceButton();
	MemorySystem::Instance->TypeFree<UserInterfaceButton>(button);
}

/*
*	Adds a checkbox, using cells.
*/
RESTRICTED UserInterfaceCheckbox *const RESTRICT UserInterfaceScene::AddCheckboxByCell(	const Vector2<uint32> &minimum_cell,
																						const Vector2<uint32> &maximum_cell,
																						const bool initially_checked,
																						const UserInterfaceCheckbox::Callback start_pressed_callback,
																						UserInterfaceMaterial *const RESTRICT unchecked_idle_material_override,
																						UserInterfaceMaterial *const RESTRICT unchecked_hovered_material_override,
																						UserInterfaceMaterial *const RESTRICT unchecked_pressed_material_override,
																						UserInterfaceMaterial *const RESTRICT checked_idle_material_override,
																						UserInterfaceMaterial *const RESTRICT checked_hovered_material_override,
																						UserInterfaceMaterial *const RESTRICT checked_pressed_material_override,
																						const char *const RESTRICT text,
																						const float32 *const RESTRICT scale_override,
																						const TextHorizontalAlignment horizontal_alignment,
																						const TextVerticalAlignment vertical_alignment) NOEXCEPT
{
	//Calculate the bounding box.
	Vector2<float32> minimum;
	Vector2<float32> maximum;

	CalculateBoundingBox(minimum_cell, maximum_cell, &minimum, &maximum);

	//Add the checkbox.
	return AddCheckboxByNormalizedCoordinate(	minimum,
												maximum,
												initially_checked,
												start_pressed_callback,
												unchecked_idle_material_override,
												unchecked_hovered_material_override,
												unchecked_pressed_material_override,
												checked_idle_material_override,
												checked_hovered_material_override,
												checked_pressed_material_override,
												text,
												scale_override,
												horizontal_alignment,
												vertical_alignment);
}


/*
*	Adds a checkbox, using raw coordinates.
*/
RESTRICTED UserInterfaceCheckbox* const RESTRICT UserInterfaceScene::AddCheckboxByNormalizedCoordinate(	const Vector2<float32> &minimum,
																										const Vector2<float32> &maximum,
																										const bool initially_checked,
																										const UserInterfaceCheckbox::Callback start_pressed_callback,
																										UserInterfaceMaterial *const RESTRICT unchecked_idle_material_override,
																										UserInterfaceMaterial *const RESTRICT unchecked_hovered_material_override,
																										UserInterfaceMaterial *const RESTRICT unchecked_pressed_material_override,
																										UserInterfaceMaterial *const RESTRICT checked_idle_material_override,
																										UserInterfaceMaterial *const RESTRICT checked_hovered_material_override,
																										UserInterfaceMaterial *const RESTRICT checked_pressed_material_override,
																										const char *const RESTRICT text,
																										const float32 *const RESTRICT scale_override,
																										const TextHorizontalAlignment horizontal_alignment,
																										const TextVerticalAlignment vertical_alignment) NOEXCEPT
{
	//Allocate the checkbox.
	UserInterfaceCheckbox *const RESTRICT new_checkbox
	{ 
		new (MemorySystem::Instance->TypeAllocate<UserInterfaceCheckbox>()) UserInterfaceCheckbox
		(
			minimum,
			maximum,
			initially_checked,
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
			scale_override ? *scale_override : _TextScale,
			horizontal_alignment,
			vertical_alignment,
			_Font
		) 
	};

	//Add the checkbox to the container.
	_Checkboxes.Emplace(new_checkbox);

	//Add the button interface to the container.
	_ButtonInterfaces.Emplace(new_checkbox);

	//Add the checkbox to the elements container.
	_Elements.Emplace(new_checkbox);

	//Set the parent.
	new_checkbox->SetParent(this);

	//Return the checkbox.
	return new_checkbox;
}

/*
*	Removes a checkbox.
*/
void UserInterfaceScene::RemoveCheckbox(UserInterfaceCheckbox *const RESTRICT checkbox) NOEXCEPT
{
	_Checkboxes.Erase<false>(checkbox);
	_Elements.Erase<true>(checkbox);

	checkbox->~UserInterfaceCheckbox();
	MemorySystem::Instance->TypeFree<UserInterfaceCheckbox>(checkbox);
}

/*
*	Adds an image, using cells.
*/
RESTRICTED UserInterfaceImage *const RESTRICT UserInterfaceScene::AddImageByCell(	const Vector2<uint32> &minimum_cell,
																					const Vector2<uint32> &maximum_cell,
																					const UserInterfaceMaterial &material,
																					const float32 opacity) NOEXCEPT
{
	//Calculate the bounding box.
	Vector2<float32> minimum;
	Vector2<float32> maximum;

	CalculateBoundingBox(minimum_cell, maximum_cell, &minimum, &maximum);

	//Add the image.
	return AddImageByNormalizedCoordinate(	minimum,
											maximum,
											material,
											opacity);
}
/*
*	Adds an image, raw coordinates.
*/
RESTRICTED UserInterfaceImage* const RESTRICT UserInterfaceScene::AddImageByNormalizedCoordinate(	const Vector2<float32> &minimum,
																									const Vector2<float32> &maximum,
																									const UserInterfaceMaterial &material,
																									const float32 opacity) NOEXCEPT
{
	//Allocate the image.
	UserInterfaceImage *const RESTRICT new_image{ new (MemorySystem::Instance->TypeAllocate<UserInterfaceImage>()) UserInterfaceImage(	minimum,
																			maximum,
																			material,
																			opacity) };

	//Add the image to the container.
	_Images.Emplace(new_image);

	//Add the image to the elements container.
	_Elements.Emplace(new_image);

	//Set the parent.
	new_image->SetParent(this);

	//Return the image.
	return new_image;
}

/*
*	Removes an image.
*/
void UserInterfaceScene::RemoveImage(UserInterfaceImage *const RESTRICT image) NOEXCEPT
{
	_Images.Erase<false>(image);
	_Elements.Erase<true>(image);

	image->~UserInterfaceImage();
	MemorySystem::Instance->TypeFree<UserInterfaceImage>(image);
}

/*
*	Adds a progress bar, using cells.
*/
RESTRICTED UserInterfaceProgressBar *const RESTRICT UserInterfaceScene::AddProgressBarByCell(	const Vector2<uint32> &minimum_cell,
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
	return AddProgressBarByNormalizedCoordinate(minimum,
												maximum,
												text,
												bottom_material_override,
												top_material_override);
}

/*
*	Adds a progress bar, using raw coordinates.
*/
RESTRICTED UserInterfaceProgressBar* const RESTRICT UserInterfaceScene::AddProgressBarByNormalizedCoordinate(	const Vector2<float32> &minimum,
																												const Vector2<float32> &maximum,
																												const char *const RESTRICT text,
																												UserInterfaceMaterial *const RESTRICT bottom_material_override,
																												UserInterfaceMaterial *const RESTRICT top_material_override) NOEXCEPT
{
	//Allocate the progress bar.
	UserInterfaceProgressBar* const RESTRICT new_progress_bar{ new (MemorySystem::Instance->TypeAllocate<UserInterfaceProgressBar>()) UserInterfaceProgressBar(minimum,
																							maximum,
																							bottom_material_override ? *bottom_material_override : _ProgressBarBottomMaterial,
																							top_material_override ? *top_material_override : _ProgressBarTopMaterial,
																							text,
																							_Font) };

	//Add the progress bar to the container.
	_ProgressBars.Emplace(new_progress_bar);

	//Add the progress bar to the elements container.
	_Elements.Emplace(new_progress_bar);

	//Set the parent.
	new_progress_bar->SetParent(this);

	//Return the progress bar.
	return new_progress_bar;
}

/*
*	Adds a text, using cells.
*/
RESTRICTED UserInterfaceText *const RESTRICT UserInterfaceScene::AddTextByCell(	const Vector2<uint32> &minimum_cell,
																				const Vector2<uint32> &maximum_cell,
																				const char *const RESTRICT text,
																				const float32 *const RESTRICT scale_override,
																				const TextHorizontalAlignment horizontal_alignment,
																				const TextVerticalAlignment vertical_alignment) NOEXCEPT
{
	//Calculate the bounding box.
	Vector2<float32> minimum;
	Vector2<float32> maximum;

	CalculateBoundingBox(minimum_cell, maximum_cell, &minimum, &maximum);

	//Add the text.
	return AddTextByNormalizedCoordinate(	minimum,
											maximum,
											text,
											scale_override,
											horizontal_alignment,
											vertical_alignment);
}

/*
*	Adds a text, using raw coordinates.
*/
RESTRICTED UserInterfaceText* const RESTRICT UserInterfaceScene::AddTextByNormalizedCoordinate(	const Vector2<float32> &minimum,
																								const Vector2<float32> &maximum,
																								const char *const RESTRICT text,
																								const float32 *const RESTRICT scale_override,
																								const TextHorizontalAlignment horizontal_alignment,
																								const TextVerticalAlignment vertical_alignment) NOEXCEPT
{
	//Allocate the text.
	UserInterfaceText* const RESTRICT new_text{ new (MemorySystem::Instance->TypeAllocate<UserInterfaceText>()) UserInterfaceText(	minimum,
																		maximum,
																		text,
																		_Font,
																		scale_override ? *scale_override : _TextScale,
																		horizontal_alignment,
																		vertical_alignment) };

	//Add the text to the container.
	_Texts.Emplace(new_text);

	//Add the text to the elements container.
	_Elements.Emplace(new_text);

	//Set the parent.
	new_text->SetParent(this);

	//Return the text.
	return new_text;
}

/*
*	Removes a text.
*/
void UserInterfaceScene::RemoveText(UserInterfaceText *const RESTRICT text) NOEXCEPT
{
	_Texts.Erase<false>(text);
	_Elements.Erase<true>(text);

	text->~UserInterfaceText();
	MemorySystem::Instance->TypeFree<UserInterfaceText>(text);
}

/*
*	Adds a text input, using cells.
*/
RESTRICTED UserInterfaceTextInput *const RESTRICT UserInterfaceScene::AddTextInputByCell(	const Vector2<uint32> &minimum_cell,
																							const Vector2<uint32> &maximum_cell,
																							const char *const RESTRICT prompt_text,
																							const char *const RESTRICT text) NOEXCEPT
{
	//Calculate the bounding box.
	Vector2<float32> minimum;
	Vector2<float32> maximum;

	CalculateBoundingBox(minimum_cell, maximum_cell, &minimum, &maximum);

	//Add the text input.
	return AddTextInputByNormalizedCoordinate(	minimum,
												maximum,
												prompt_text,
												text);
}

/*
*	Adds a text input, using raw coordinates.
*/
RESTRICTED UserInterfaceTextInput *const RESTRICT UserInterfaceScene::AddTextInputByNormalizedCoordinate(	const Vector2<float32> &minimum,
																											const Vector2<float32> &maximum,
																											const char *const RESTRICT prompt_text,
																											const char *const RESTRICT text) NOEXCEPT
{
	//Allocate the text input.
	UserInterfaceTextInput *const RESTRICT new_text_input
	{
		new (MemorySystem::Instance->TypeAllocate<UserInterfaceTextInput>()) UserInterfaceTextInput
		(
			minimum,
			maximum,
			_ButtonIdleMaterial,
			_Font,
			_TextScale,
			prompt_text,
			text
		)
	};

	//Add the text input to the container.
	_TextInputs.Emplace(new_text_input);

	//Add the text input to the elements container.
	_Elements.Emplace(new_text_input);

	//Set the parent.
	new_text_input->SetParent(this);

	//Return the text input.
	return new_text_input;
}

/*
*	Removes a text input.
*/
void UserInterfaceScene::RemoveTextInput(UserInterfaceTextInput *const RESTRICT text_input) NOEXCEPT
{
	_TextInputs.Erase<false>(text_input);
	_Elements.Erase<true>(text_input);

	text_input->~UserInterfaceTextInput();
	MemorySystem::Instance->TypeFree<UserInterfaceTextInput>(text_input);
}

/*
*	Removes all user interface elements.
*/
void UserInterfaceScene::RemoveAll() NOEXCEPT
{
	DynamicArray<UserInterfaceElement *RESTRICT> elements{ _Elements };

	for (UserInterfaceElement *const RESTRICT element : elements)
	{
		switch (element->GetType())
		{
			case UserInterfaceElementType::BUTTON:
			{
				RemoveButton(static_cast<UserInterfaceButton *const RESTRICT>(element));

				break;
			}

			case UserInterfaceElementType::CHECKBOX:
			{
				RemoveCheckbox(static_cast<UserInterfaceCheckbox *const RESTRICT>(element));

				break;
			}

			case UserInterfaceElementType::IMAGE:
			{
				RemoveImage(static_cast<UserInterfaceImage *const RESTRICT>(element));

				break;
			}

			/*
			case UserInterfaceElementType::PROGRESS_BAR:
			{
				RemoveProgressBar(static_cast<UserInterfaceProgressBar *const RESTRICT>(element));

				break;
			}
			*/

			case UserInterfaceElementType::TEXT:
			{
				RemoveText(static_cast<UserInterfaceText *const RESTRICT>(element));

				break;
			}

			case UserInterfaceElementType::TEXT_INPUT:
			{
				RemoveTextInput(static_cast<UserInterfaceTextInput *const RESTRICT>(element));

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

/*
*	Returns the normalized coordinate center of the given cell.
*/
NO_DISCARD Vector2<float32> UserInterfaceScene::GetNormalizedCoordinateCenterOfCell(const Vector2<uint32> &cell) NOEXCEPT
{
	const Vector2<float32> minimum{ static_cast<float32>(cell._X) * _HorizontalSubdivisionReciprocal,
									static_cast<float32>(cell._Y) * _VerticalSubdivisionReciprocal };

	const Vector2<float32> maximum{ static_cast<float32>(cell._X + 1) * _HorizontalSubdivisionReciprocal,
									static_cast<float32>(cell._Y + 1) * _VerticalSubdivisionReciprocal };

	return BaseMath::LinearlyInterpolate(minimum, maximum, 0.5f);
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
#if defined(CATALYST_EDITOR)
	if (!CatalystEditorSystem::Instance->IsInGame())
	{
		return;
	}
#endif

	//If there's no button interface to update, just quit.
	if (_ButtonInterfaces.Empty())
	{
		return;
	}

	//Cache the input data.
	const GamepadState *const RESTRICT gamepad_state{ InputSystem::Instance->GetGamepadState(InputLayer::USER_INTERFACE) };

	//Retrieve the cursor state.
	const CursorState cursor_state{ RetrieveCursorState() };

	//Update which button is gamepad selected.
	if (cursor_state._InputDeviceType == InputDeviceType::GAMEPAD)
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
					float32 shortest_distance{ FLOAT32_MAXIMUM };

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
		switch (cursor_state._InputDeviceType)
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
					const bool is_inside{	cursor_state._Position._X >= button.GetMinimum()._X
											&& cursor_state._Position._X <= button.GetMaximum()._X
											&& cursor_state._Position._Y >= button.GetMinimum()._Y
											&& cursor_state._Position._Y <= button.GetMaximum()._Y };

					//Mutate the state and call callbacks.
					switch (button.GetCurrentState())
					{
						case UserInterfaceButtonState::IDLE:
						{
							if (is_inside)
							{
								if (cursor_state._Pressed)
								{
									button.OnStartPressed();
									button.SetCurrentState(UserInterfaceButtonState::PRESSED);
								}

								else if (cursor_state._InputDeviceType != InputDeviceType::TOUCH)
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
								if (cursor_state._Pressed)
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
								if (!cursor_state._Pressed)
								{
									button.OnStopPressed();

									if (cursor_state._InputDeviceType != InputDeviceType::TOUCH)
									{
										button.SetCurrentState(UserInterfaceButtonState::HOVERED);
									}

									else
									{
										button.SetCurrentState(UserInterfaceButtonState::IDLE);
									}
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

/*
*	Updates text inputs.
*/
void UserInterfaceScene::UpdateTextInputs() NOEXCEPT
{
	//Update all text inputs.
	for (UserInterfaceTextInput *const RESTRICT text_input : _TextInputs)
	{
		text_input->Update();
	}
}

/*
*	Retrieves the cursor state.
*/
NO_DISCARD UserInterfaceScene::CursorState UserInterfaceScene::RetrieveCursorState() NOEXCEPT
{
	CursorState output;

	output._InputDeviceType = InputSystem::Instance->GetLastUpdatedInputDeviceType();
	output._Position = Vector2<float32>(0.0f, 0.0f);
	output._Pressed = false;

	if (_Entity)
	{
		//Cache the world transform.
		const WorldTransform &world_transform{ WorldTransformComponent::Instance->InstanceData(_Entity)._CurrentWorldTransform };

		//Construct the ray.
		Ray ray;

		ray.SetOrigin(RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetLocalPosition());
		ray.SetDirection(RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetForwardVector());

		//Construct the plane.
		Plane plane;

		plane._Position = world_transform.GetRelativePosition(WorldSystem::Instance->GetCurrentWorldGridCell());
		plane._Normal = CatalystCoordinateSpacesUtilities::RotatedWorldBackwardVector(world_transform.GetRotation().ToEulerAngles());

		//Intersect the plane!
		float32 intersection_distance;
		
		if (CatalystGeometryMath::RayPlaneIntersection(ray, plane, &intersection_distance))
		{
			//Calculate the world position.
			Vector3<float32> world_position{ ray._Origin + ray._Direction * intersection_distance };

			//Undo the translation.
			world_position -= world_transform.GetRelativePosition(WorldSystem::Instance->GetCurrentWorldGridCell());

			//Undo the rotation.
			world_position.Rotate(-world_transform.GetRotation().ToEulerAngles());

			//Undo the scale.
			world_position._X /= world_transform.GetScale()._X;
			world_position._Y /= world_transform.GetScale()._Y;

			//Shift the world position into the [0.0f, 1.0f] range.
			world_position._X += 0.5f;
			world_position._Y += 0.5f;

			//This is now the cursor position!
			output._Position = Vector2<float32>(world_position._X, world_position._Y);

			//Determine if the cursor is pressed.
			switch (output._InputDeviceType)
			{
				case InputDeviceType::KEYBOARD:
				case InputDeviceType::MOUSE:
				{
					output._Pressed = InputSystem::Instance->GetMouseState(InputLayer::GAME)->_Left == ButtonState::PRESSED;

					break;
				}

				case InputDeviceType::TOUCH:
				{
					output._Pressed = InputSystem::Instance->GetTouchState(InputLayer::GAME)->_FingerStates[0]._ButtonState == ButtonState::PRESSED;

					break;
				}
			}
		}
	}

	else
	{
		switch (output._InputDeviceType)
		{
			case InputDeviceType::KEYBOARD:
			case InputDeviceType::MOUSE:
			{
				output._Position = Vector2<float32>(InputSystem::Instance->GetMouseState(InputLayer::USER_INTERFACE)->_CurrentX, InputSystem::Instance->GetMouseState(InputLayer::USER_INTERFACE)->_CurrentY);
				output._Pressed = InputSystem::Instance->GetMouseState(InputLayer::USER_INTERFACE)->_Left == ButtonState::PRESSED;

				break;
			}

			case InputDeviceType::TOUCH:
			{
				output._Position = Vector2<float32>(InputSystem::Instance->GetTouchState(InputLayer::USER_INTERFACE)->_FingerStates[0]._CurrentX, InputSystem::Instance->GetTouchState(InputLayer::USER_INTERFACE)->_FingerStates[0]._CurrentY);
				output._Pressed = InputSystem::Instance->GetTouchState(InputLayer::USER_INTERFACE)->_FingerStates[0]._ButtonState == ButtonState::PRESSED;

				break;
			}
		}
	}

	return output;
}