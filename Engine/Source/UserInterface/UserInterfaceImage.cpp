//Header file.
#include <UserInterface/UserInterfaceImage.h>

//Systems.
#include <Systems/ResourceSystem.h>
#include <Systems/UserInterfaceSystem.h>

//User interface.
#include <UserInterface/ImageUserInterfaceElementDescription.h>
#include <UserInterface/TextUserInterfaceElementDescription.h>

/*
*	Default constructor.
*/
UserInterfaceImage::UserInterfaceImage(	const Vector2<float32> initial_minimum,
													const Vector2<float32> initial_maximum,
													const UserInterfaceMaterial &initial_material) NOEXCEPT
{
	//Create the element.
	{
		ImageUserInterfaceElementDescription description;

		description._Type = UserInterfaceElementType::IMAGE;
		description._Minimum = initial_minimum;
		description._Maximum = initial_maximum;
		description._Opacity = 1.0f;
		description._Material = initial_material;

		_Element = static_cast<ImageUserInterfaceElement *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfaceElement(&description));
	}
}

/*
*	Default destructor.
*/
UserInterfaceImage::~UserInterfaceImage() NOEXCEPT
{
	//Destroy the element.
	UserInterfaceSystem::Instance->DestroyUserInterfaceElement(_Element);
}