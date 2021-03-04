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
	constexpr Resolution BASE_RESOLUTION{ 3'840, 2'160 };
	constexpr uint32 PADDING{ 16 };
	constexpr uint32 COLUMN_HEIGHT{ 64 };
	constexpr Vector4<float32> IDLE_BUTTON_PRIMARY_COLOR{ 1.0f, 1.0f, 1.0f, 0.01f };
	constexpr Vector4<float32> IDLE_BUTTON_CHECKED_PRIMARY_COLOR{ 1.0f, 1.0f, 1.0f, 0.5f };
	constexpr Vector4<float32> HOVERED_BUTTON_PRIMARY_COLOR{ 1.0f, 1.0f, 1.0f, 0.25f };
	constexpr Vector4<float32> HOVERED_BUTTON_CHECKED_PRIMARY_COLOR{ 1.0f, 1.0f, 1.0f, 0.75f };
	constexpr Vector4<float32> PRESSED_BUTTON_PRIMARY_COLOR{ 1.0f, 1.0f, 1.0f, 1.0f };
	constexpr Vector4<float32> IDLE_BUTTON_SECONDARY_COLOR{ 1.0f, 1.0f, 1.0f, 1.0f };
	constexpr Vector4<float32> HOVERED_BUTTON_SECONDARY_COLOR{ 1.0f, 1.0f, 1.0f, 1.0f };
	constexpr Vector4<float32> PRESSED_BUTTON_SECONDARY_COLOR{ 1.0f, 1.0f, 1.0f, 1.0f };
	constexpr float32 BUTTON_BORDER_OFFSET{ 0.0025f };
	constexpr float32 CHECKBOX_BORDER_OFFSET{ 0.0175f };
}

/*
*	Default constructor.
*/
UserInterfaceScene::UserInterfaceScene() NOEXCEPT
{
	//Set up default values.
	_HorizontalSubdivision = 32;
	_VerticalSubdivision = 16;

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
*	Adds a button on the given row/column.
*/
void UserInterfaceScene::AddButton(	const uint32 vertical_index,
									const uint32 horizontal_index,
									const uint32 number_of_elements_vertically,
									const ButtonUserInterfaceElementCallback start_pressed_callback,
									const char *const RESTRICT text,
									UserInterfaceElement *RESTRICT *const RESTRICT button_element,
									UserInterfaceElement *RESTRICT *const RESTRICT text_element) NOEXCEPT
{
	//Calculate the bounding box for the element.
	Vector2<float32> minimum;
	Vector2<float32> maximum;

	CalculateBoundingBox(vertical_index, horizontal_index, number_of_elements_vertically, &minimum, &maximum);

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
RESTRICTED UserInterfaceProgressBar *const RESTRICT UserInterfaceScene::AddProgressBar(	const uint32 vertical_index,
																						const uint32 horizontal_index,
																						const uint32 number_of_elements_vertically) NOEXCEPT
{
	//Calculate the bounding box.
	Vector2<float32> minimum;
	Vector2<float32> maximum;

	CalculateBoundingBox(vertical_index, horizontal_index, number_of_elements_vertically, &minimum, &maximum);

	//Allocate the progress bar.
	UserInterfaceProgressBar* const RESTRICT new_progress_bar{ new UserInterfaceProgressBar(minimum,
		maximum,
																							_ProgressBarBottomMaterial,
																							_ProgressBarTopMaterial) };

	//Add the progress bar to the container.
	_ProgressBars.Emplace(new_progress_bar);

	//Return the progress bar.
	return new_progress_bar;
}

/*
*	Calculates the bounding box for the given row/column.
*/
void UserInterfaceScene::CalculateBoundingBox(	const Vector2<uint32> &in_minimum,
												const Vector2<uint32> &in_maximum,
												Vector2<float32> *const RESTRICT out_minimum,
												Vector2<float32> *const RESTRICT out_maximum) NOEXCEPT
{

}

/*
*	Calculates the bounding box for the given row/column.
*/
void UserInterfaceScene::CalculateBoundingBox(	const uint32 vertical_index,
												const uint32 horizontal_index,
												const uint32 number_of_elements_vertically,
												Vector2<float32> *const RESTRICT minimum,
												Vector2<float32> *const RESTRICT maximum) NOEXCEPT
{
	//Calculate the width for each element on the row.
	const uint32 element_width{ ((UserInterfaceSceneConstants::BASE_RESOLUTION._Width - UserInterfaceSceneConstants::PADDING) / number_of_elements_vertically) - UserInterfaceSceneConstants::PADDING };

	//Calculate the minimum.
	Vector2<uint32> integer_minimum;

	integer_minimum._X = UserInterfaceSceneConstants::PADDING + ((element_width + UserInterfaceSceneConstants::PADDING) * horizontal_index);
	integer_minimum._Y = UserInterfaceSceneConstants::BASE_RESOLUTION._Height - UserInterfaceSceneConstants::PADDING - UserInterfaceSceneConstants::COLUMN_HEIGHT - ((UserInterfaceSceneConstants::PADDING + UserInterfaceSceneConstants::COLUMN_HEIGHT) * vertical_index);

	//Calculate the maximum.
	Vector2<uint32> integer_maximum;

	integer_maximum._X = integer_minimum._X + element_width;
	integer_maximum._Y = integer_minimum._Y + UserInterfaceSceneConstants::COLUMN_HEIGHT;

	*minimum = UserInterfaceUtilities::CalculateNormalizedCoordinate(integer_minimum, UserInterfaceSceneConstants::BASE_RESOLUTION);
	*maximum = UserInterfaceUtilities::CalculateNormalizedCoordinate(integer_maximum, UserInterfaceSceneConstants::BASE_RESOLUTION);
}