//Header file.
#include <UserInterface/UserInterfaceScene.h>

//Systems.
#include <Systems/ResourceSystem.h>
#include <Systems/UserInterfaceSystem.h>

//User interface.
#include <UserInterface/ButtonUserInterfaceElementDescription.h>
#include <UserInterface/TextUserInterfaceElementDescription.h>
#include <UserInterface/UserInterfaceUtilities.h>

//User interface scene constants.
namespace UserInterfaceSceneConstants
{
	constexpr float32 PADDING{ 0.001'25f };
	constexpr Vector4<float32> IDLE_BUTTON_PRIMARY_COLOR{ 1.0f, 1.0f, 1.0f, 0.01f };
	constexpr Vector4<float32> IDLE_BUTTON_CHECKED_PRIMARY_COLOR{ 1.0f, 1.0f, 1.0f, 0.5f };
	constexpr Vector4<float32> HOVERED_BUTTON_PRIMARY_COLOR{ 1.0f, 1.0f, 1.0f, 0.25f };
	constexpr Vector4<float32> HOVERED_BUTTON_CHECKED_PRIMARY_COLOR{ 1.0f, 1.0f, 1.0f, 0.75f };
	constexpr Vector4<float32> PRESSED_BUTTON_PRIMARY_COLOR{ 1.0f, 1.0f, 1.0f, 1.0f };
	constexpr Vector4<float32> IDLE_BUTTON_SECONDARY_COLOR{ 1.0f, 1.0f, 1.0f, 1.0f };
	constexpr Vector4<float32> HOVERED_BUTTON_SECONDARY_COLOR{ 1.0f, 1.0f, 1.0f, 1.0f };
	constexpr Vector4<float32> PRESSED_BUTTON_SECONDARY_COLOR{ 1.0f, 1.0f, 1.0f, 1.0f };
	constexpr float32 BUTTON_BORDER_OFFSET{ 0.0025f };
}

/*
*	Default constructor.
*/
UserInterfaceScene::UserInterfaceScene() NOEXCEPT
{
	//Set up default values.
	SetHorizontalSubdivision(33);
	SetVerticalSubdivision(33);

	_ProgressBarBottomMaterial.SetPrimaryColor(Vector4<float32>(0.125f, 0.125f, 0.125f, 1.0f));
	_ProgressBarBottomMaterial.SetSecondaryColor(Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f));

	_ProgressBarTopMaterial.SetPrimaryColor(Vector4<float32>(0.5f, 0.5f, 0.5f, 1.0f));
	_ProgressBarTopMaterial.SetSecondaryColor(Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f));
}

/*
*	Callback for when this user interface scene is deactivated.
*/
void UserInterfaceScene::OnDeactivated() NOEXCEPT
{
	//Free all progress bars.
	for (UserInterfaceProgressBar *const RESTRICT progress_bar : _ProgressBars)
	{
		delete progress_bar;
	}

	_ProgressBars.Clear();

	//Destroy all elements.
	for (UserInterfaceElement *const RESTRICT element : _UserInterfaceElements)
	{
		UserInterfaceSystem::Instance->DestroyUserInterfaceElement(element);
	}
}

/*
*	Adds a button.
*/
void UserInterfaceScene::AddButton(	const Vector2<uint32> &minimum_cell,
									const Vector2<uint32> &maximum_cell,
									const ButtonUserInterfaceElementCallback start_pressed_callback,
									const char *const RESTRICT text,
									UserInterfaceElement *RESTRICT *const RESTRICT button_element,
									UserInterfaceElement *RESTRICT *const RESTRICT text_element) NOEXCEPT
{
	//Calculate the bounding box for the element.
	Vector2<float32> minimum;
	Vector2<float32> maximum;

	CalculateBoundingBox(minimum_cell, maximum_cell, &minimum, &maximum);

	//Add the button.
	{
		ButtonUserInterfaceElementDescription description;

		description._Type = UserInterfaceElementType::BUTTON;
		description._Minimum = minimum;
		description._Maximum = maximum;
		description._Opacity = 1.0f;
		description._StartHoveredCallback = nullptr;
		description._StopHoveredCallback = nullptr;
		description._StartPressedCallback = start_pressed_callback;
		description._StopPressedCallback = nullptr;
		description._IdleMaterial.SetPrimaryColor(UserInterfaceSceneConstants::IDLE_BUTTON_PRIMARY_COLOR);
		description._IdleMaterial.SetSecondaryColor(UserInterfaceSceneConstants::IDLE_BUTTON_SECONDARY_COLOR);
		description._IdleMaterial.SetBorderOffset(UserInterfaceSceneConstants::BUTTON_BORDER_OFFSET);
		description._HoveredMaterial.SetPrimaryColor(UserInterfaceSceneConstants::HOVERED_BUTTON_PRIMARY_COLOR);
		description._HoveredMaterial.SetSecondaryColor(UserInterfaceSceneConstants::HOVERED_BUTTON_SECONDARY_COLOR);
		description._HoveredMaterial.SetBorderOffset(UserInterfaceSceneConstants::BUTTON_BORDER_OFFSET);
		description._PressedMaterial.SetPrimaryColor(UserInterfaceSceneConstants::PRESSED_BUTTON_PRIMARY_COLOR);
		description._PressedMaterial.SetSecondaryColor(UserInterfaceSceneConstants::PRESSED_BUTTON_SECONDARY_COLOR);
		description._PressedMaterial.SetBorderOffset(UserInterfaceSceneConstants::BUTTON_BORDER_OFFSET);

		UserInterfaceElement* const RESTRICT element{ UserInterfaceSystem::Instance->CreateUserInterfaceElement(&description) };

		if (button_element)
		{
			*button_element = element;
		}

		_UserInterfaceElements.Emplace(element);
	}

	//Add the text.
	{
		TextUserInterfaceElementDescription description;

		description._Type = UserInterfaceElementType::TEXT;
		description._Minimum = minimum;
		description._Maximum = maximum;
		description._Opacity = 1.0f;
		description._FontResource = ResourceSystem::Instance->GetFontResource(HashString("Catalyst_Engine_Default_Font"));
		description._Scale = 0.015f;
		description._HorizontalAlignment = TextHorizontalAlignment::CENTER;
		description._VerticalAlignment = TextVerticalAlignment::CENTER;
		description._TextSmoothingFactor = 0.2f; //0.025f step.
		description._Text = text;

		UserInterfaceElement* const RESTRICT element{ UserInterfaceSystem::Instance->CreateUserInterfaceElement(&description) };

		if (text_element)
		{
			*text_element = element;
		}

		_UserInterfaceElements.Emplace(element);
	}
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