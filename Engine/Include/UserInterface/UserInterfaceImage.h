#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/General/Vector.h>

//User interface.
#include <UserInterface/ImageUserInterfacePrimitive.h>

class UserInterfaceImage final
{

public:

	/*
	*	Default constructor.
	*/
	UserInterfaceImage(	const Vector2<float32> initial_minimum,
						const Vector2<float32> initial_maximum,
						const UserInterfaceMaterial &initial_material,
						const float32 opacity,
						const bool is_three_dimensional) NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~UserInterfaceImage() NOEXCEPT;

	/*
	*	Sets the material.
	*/
	void SetMaterial(const UserInterfaceMaterial &value) NOEXCEPT;

	/*
	*	Returns the user interface primitives.
	*/
	void RetrieveUserInterfacePrimitives(DynamicArray<const UserInterfacePrimitive *RESTRICT> *const RESTRICT output) const NOEXCEPT;

private:

	//The primitive.
	ImageUserInterfacePrimitive *RESTRICT _Primitive;

	//Denotes whether or not this image is three dimensional.
	bool _IsThreeDimensional{ false };

};