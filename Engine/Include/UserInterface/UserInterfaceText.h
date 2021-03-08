#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//User interface.
#include <UserInterface/TextUserInterfaceElement.h>

class UserInterfaceText final
{

public:

	/*
	*	Default constructor.
	*/
	UserInterfaceText(	const Vector2<float32> initial_minimum,
						const Vector2<float32> initial_maximum,
						const char *const RESTRICT text) NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~UserInterfaceText() NOEXCEPT;

	/*
	*	Sets the text.
	*/
	void SetText(const char *const RESTRICT text) NOEXCEPT;

private:

	//The minimum.
	Vector2<float32> _Minimum;

	//The maximum.
	Vector2<float32> _Maximum;

	//The element.
	TextUserInterfaceElement *RESTRICT _Element;

};