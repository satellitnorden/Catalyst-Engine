#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//User interface.
#include <UserInterface/TextUserInterfacePrimitive.h>

class UserInterfaceText final
{

public:

	/*
	*	Default constructor.
	*/
	UserInterfaceText(	const Vector2<float32> initial_minimum,
						const Vector2<float32> initial_maximum,
						const char *const RESTRICT text,
						const TextHorizontalAlignment horizontal_alignment) NOEXCEPT;

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

	//The primitive.
	TextUserInterfacePrimitive *RESTRICT _Primitive;

	//The horizontal alignment.
	TextHorizontalAlignment _HorizontalAlignment;

};