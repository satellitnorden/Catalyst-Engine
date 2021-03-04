//Header file.
#include <UserInterface/UserInterfaceProgressBar.h>

//Systems.
#include <Systems/UserInterfaceSystem.h>

//User interface.
#include <UserInterface/ImageUserInterfaceElementDescription.h>

/*
*	Default constructor.
*/
UserInterfaceProgressBar::UserInterfaceProgressBar(	const Vector2<float32> initial_minimum,
													const Vector2<float32> initial_maximum,
													const UserInterfaceMaterial &initial_bottom_material,
													const UserInterfaceMaterial &initial_top_material) NOEXCEPT
{
	//Create the bottom and top element.
	{
		ImageUserInterfaceElementDescription description;

		description._Type = UserInterfaceElementType::IMAGE;
		description._Minimum = initial_minimum;
		description._Maximum = initial_maximum;
		description._Opacity = 1.0f;
		description._Material = initial_bottom_material;

		_BottomElement = static_cast<ImageUserInterfaceElement *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfaceElement(&description));
	}

	{
		ImageUserInterfaceElementDescription description;

		description._Type = UserInterfaceElementType::IMAGE;
		description._Minimum = initial_minimum;
		description._Maximum = Vector2<float32>(initial_minimum._X, initial_maximum._Y);
		description._Opacity = 1.0f;
		description._Material = initial_top_material;

		_TopElement = static_cast<ImageUserInterfaceElement *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfaceElement(&description));
	}
}

/*
*	Default destructor.
*/
UserInterfaceProgressBar::~UserInterfaceProgressBar() NOEXCEPT
{
	//Destroy the bottom/top elements.
	UserInterfaceSystem::Instance->DestroyUserInterfaceElement(_BottomElement);
	UserInterfaceSystem::Instance->DestroyUserInterfaceElement(_TopElement);
}

/*
*	Sets the current progress.
*/
void UserInterfaceProgressBar::SetCurrentProgress(const float32 value) NOEXCEPT
{
	//Set the current progress.
	_CurrentProgress = value;

	//Update the bounding box for the top element.
	_TopElement->_Maximum._X = CatalystBaseMath::LinearlyInterpolate(_BottomElement->_Minimum._X, _BottomElement->_Maximum._X, _CurrentProgress);
}