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
						const ResourcePointer<FontResource> font_resource,
						const float32 scale,
						const TextHorizontalAlignment horizontal_alignment,
						const TextVerticalAlignment vertical_alignment,
						const float32 smoothing_factor) NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~UserInterfaceText() NOEXCEPT;

	/*
	*	Sets the text.
	*/
	void SetText(const char *const RESTRICT text) NOEXCEPT;

	/*
	*	Sets the opacity.
	*/
	void SetOpacity(const float32 opacity) NOEXCEPT;

private:

	//The minimum.
	Vector2<float32> _Minimum;

	//The maximum.
	Vector2<float32> _Maximum;

	//The font resource.
	ResourcePointer<FontResource> _FontResource;

	//The scale.
	float32 _Scale;

	//The horizontal alignment.
	TextHorizontalAlignment _HorizontalAlignment;

	//The vertical alignment.
	TextVerticalAlignment _VerticalAlignment;

	//The smoothing factor.
	float32 _SmoothingFactor;

	//The primitive.
	TextUserInterfacePrimitive *RESTRICT _Primitive;

};