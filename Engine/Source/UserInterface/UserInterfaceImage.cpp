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
	//Set the type.
	SetType(UserInterfaceElementType::IMAGE);

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

	_Primitive->CorrectForAspectRatio();
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
*	Sets the position.
*/
void UserInterfaceImage::SetPosition(const Vector2<float32>& position) NOEXCEPT
{
	const Vector2<float32> half_extents{ (_Primitive->_Maximum - _Primitive->_Minimum) * 0.5f };

	_Primitive->_Minimum = position - half_extents;
	_Primitive->_Maximum = position + half_extents;
}

/*
*	Sets the half extents.
*/
void UserInterfaceImage::SetHalfExtents(const Vector2<float32> &half_extents) NOEXCEPT
{
	const Vector2<float32> center{ CatalystBaseMath::LinearlyInterpolate(_Primitive->_Minimum, _Primitive->_Maximum, 0.5f) };

	_Primitive->_Minimum = center - half_extents;
	_Primitive->_Maximum = center + half_extents;

	_Primitive->CorrectForAspectRatio();
}

/*
*	Sets the material.
*/
void UserInterfaceImage::SetMaterial(const UserInterfaceMaterial &value) NOEXCEPT
{
	_Primitive->_Material = value;
}

/*
*	Sets the opacity.
*/
void UserInterfaceImage::SetOpacity(const float32 value) NOEXCEPT
{
	_Primitive->_Opacity = value;
}

/*
*	Returns the user interface primitives.
*/
void UserInterfaceImage::RetrieveUserInterfacePrimitives(DynamicArray<const UserInterfacePrimitive *RESTRICT> *const RESTRICT output) const NOEXCEPT
{
	output->Emplace(_Primitive);
}