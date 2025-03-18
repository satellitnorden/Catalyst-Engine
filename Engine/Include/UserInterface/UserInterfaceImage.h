#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/General/Vector.h>

//User interface.
#include <UserInterface/ImageUserInterfacePrimitive.h>
#include <UserInterface/UserInterfaceElement.h>

class UserInterfaceImage final : public UserInterfaceElement
{

public:

	/*
	*	Default constructor.
	*/
	UserInterfaceImage(	const Vector2<float32> initial_minimum,
						const Vector2<float32> initial_maximum,
						const UserInterfaceMaterial &initial_material,
						const float32 opacity) NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~UserInterfaceImage() NOEXCEPT;

	/*
	*	Sets the minimum.
	*/
	void SetMinimum(const Vector2<float32> &minimum) NOEXCEPT;

	/*
	*	Sets the maximum.
	*/
	void SetMaximum(const Vector2<float32> &maximum) NOEXCEPT;

	/*
	*	Sets the position.
	*/
	void SetPosition(const Vector2<float32> &position) NOEXCEPT;

	/*
	*	Sets the half extents.
	*/
	void SetHalfExtents(const Vector2<float32> &half_extents) NOEXCEPT;

	/*
	*	Corrects for aspect ratio.
	*/
	void CorrectForAspectRatio() NOEXCEPT;

	/*
	*	Sets the material.
	*/
	void SetMaterial(const UserInterfaceMaterial &value) NOEXCEPT;

	/*
	*	Sets the color.
	*/
	void SetColor(const Vector3<float32> &value) NOEXCEPT;

	/*
	*	Sets the opacity.
	*/
	void SetOpacity(const float32 value) NOEXCEPT;

	/*
	*	Returns the user interface primitives.
	*/
	void RetrieveUserInterfacePrimitives(DynamicArray<UserInterfacePrimitive *RESTRICT> *const RESTRICT output) const NOEXCEPT override;

private:

	//The primitive.
	ImageUserInterfacePrimitive *RESTRICT _Primitive;

};