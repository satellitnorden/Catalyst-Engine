//Header file.
#include <UserInterface/UserInterfaceImage.h>

//Systems.
#include <Systems/ResourceSystem.h>
#include <Systems/UserInterfaceSystem.h>

//User interface.
#include <UserInterface/ImageUserInterfacePrimitiveDescription.h>

/*
*	Default constructor.
*/
UserInterfaceImage::UserInterfaceImage(	const Vector2<float32> initial_minimum,
										const Vector2<float32> initial_maximum,
										const UserInterfaceMaterial &initial_material,
										const float32 opacity,
										const bool is_three_dimensional) NOEXCEPT
{
	//Remember whether or not this image is three dimensional.
	_IsThreeDimensional = is_three_dimensional;

	//Create the primitive.
	{
		ImageUserInterfacePrimitiveDescription description;

		description._Type = UserInterfacePrimitiveType::IMAGE;
		description._Minimum = initial_minimum;
		description._Maximum = initial_maximum;
		description._Opacity = opacity;
		description._Material = initial_material;

		_Primitive = static_cast<ImageUserInterfacePrimitive *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfacePrimitive(&description, _IsThreeDimensional));
	}
}

/*
*	Default destructor.
*/
UserInterfaceImage::~UserInterfaceImage() NOEXCEPT
{
	//Destroy the primitive.
	UserInterfaceSystem::Instance->DestroyUserInterfacePrimitive(_Primitive);
}

/*
*	Sets the material.
*/
void UserInterfaceImage::SetMaterial(const UserInterfaceMaterial &value) NOEXCEPT
{
	_Primitive->_Material = value;
}

/*
*	Returns the user interface primitives.
*/
void UserInterfaceImage::RetrieveUserInterfacePrimitives(DynamicArray<const UserInterfacePrimitive *RESTRICT> *const RESTRICT output) const NOEXCEPT
{
	output->Emplace(_Primitive);
}