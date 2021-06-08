#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

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
						const float32 opacity) NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~UserInterfaceImage() NOEXCEPT;

	/*
	*	Sets the material.
	*/
	void SetMaterial(const UserInterfaceMaterial &value) NOEXCEPT;

private:

	//The primitive.
	ImageUserInterfacePrimitive *RESTRICT _Primitive;

};