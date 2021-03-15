#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//User interface.
#include <UserInterface/ImageUserInterfaceElement.h>

class UserInterfaceImage final
{

public:

	/*
	*	Default constructor.
	*/
	UserInterfaceImage(	const Vector2<float32> initial_minimum,
						const Vector2<float32> initial_maximum,
						const UserInterfaceMaterial &initial_material) NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~UserInterfaceImage() NOEXCEPT;

private:

	//The element.
	ImageUserInterfaceElement *RESTRICT _Element;

};