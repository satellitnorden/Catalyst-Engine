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
	constexpr Resolution BASE_RESOLUTION{ 1'920, 1'080 };
	constexpr uint32 PADDING{ 8 };
	constexpr uint32 COLUMN_HEIGHT{ 32 };
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
*	Callback for when this user interface scene is deactivated.
*/
void UserInterfaceScene::OnDeactivated() NOEXCEPT
{
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
	Vector2<uint32> minimum;
	Vector2<uint32> maximum;

	CalculateBoundingBox(vertical_index, horizontal_index, number_of_elements_vertically, &minimum, &maximum);

	//Add the button.
	{
		ButtonUserInterfaceElementDescription description;

		description._Type = UserInterfaceElementType::BUTTON;
		description._Minimum = UserInterfaceUtilities::CalculateNormalizedCoordinate(minimum, UserInterfaceSceneConstants::BASE_RESOLUTION);
		description._Maximum = UserInterfaceUtilities::CalculateNormalizedCoordinate(maximum, UserInterfaceSceneConstants::BASE_RESOLUTION);
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
		description._Minimum = UserInterfaceUtilities::CalculateNormalizedCoordinate(minimum, UserInterfaceSceneConstants::BASE_RESOLUTION);
		description._Maximum = UserInterfaceUtilities::CalculateNormalizedCoordinate(maximum, UserInterfaceSceneConstants::BASE_RESOLUTION);
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
*	Calculates the bounding box for the given row/column.
*/
void UserInterfaceScene::CalculateBoundingBox(	const uint32 vertical_index,
												const uint32 horizontal_index,
												const uint32 number_of_elements_vertically,
												Vector2<uint32> *const RESTRICT minimum,
												Vector2<uint32> *const RESTRICT maximum) NOEXCEPT
{
	//Calculate the width for each element on the row.
	const uint32 element_width{ ((UserInterfaceSceneConstants::BASE_RESOLUTION._Width - UserInterfaceSceneConstants::PADDING) / number_of_elements_vertically) - UserInterfaceSceneConstants::PADDING };

	//Calculate the minimum.
	minimum->_X = UserInterfaceSceneConstants::PADDING + ((element_width + UserInterfaceSceneConstants::PADDING) * horizontal_index);
	minimum->_Y = UserInterfaceSceneConstants::BASE_RESOLUTION._Height - UserInterfaceSceneConstants::PADDING - UserInterfaceSceneConstants::COLUMN_HEIGHT - ((UserInterfaceSceneConstants::PADDING + UserInterfaceSceneConstants::COLUMN_HEIGHT) * vertical_index);

	//Calculate the maximum.
	maximum->_X = minimum->_X + element_width;
	maximum->_Y = minimum->_Y + UserInterfaceSceneConstants::COLUMN_HEIGHT;
}