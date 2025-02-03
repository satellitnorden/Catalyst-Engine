#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/General/Vector.h>

//User interface.
#include <UserInterface/TextUserInterfacePrimitive.h>
#include <UserInterface/UserInterfaceElement.h>

class UserInterfaceText final : public UserInterfaceElement
{

public:

	/*
	*	Default constructor.
	*/
	UserInterfaceText(	const Vector2<float32> initial_minimum,
						const Vector2<float32> initial_maximum,
						const char *const RESTRICT text,
						const AssetPointer<FontAsset> font,
						const float32 scale,
						const TextHorizontalAlignment horizontal_alignment,
						const TextVerticalAlignment vertical_alignment) NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~UserInterfaceText() NOEXCEPT;

	/*
	*	Sets the position.
	*/
	void SetPosition(const Vector2<float32> &position) NOEXCEPT;

	/*
	*	Sets the text.
	*/
	void SetText(const char *const RESTRICT text) NOEXCEPT;

	/*
	*	Sets the color.
	*/
	void SetColor(const Vector3<float32> &color) NOEXCEPT;

	/*
	*	Sets the opacity.
	*/
	void SetOpacity(const float32 opacity) NOEXCEPT;

	/*
	*	Sets the text bounds.
	*/
	void SetTextBounds(const AxisAlignedBoundingBox2D &text_bounds) NOEXCEPT;

	/*
	*	Returns the user interface primitives.
	*/
	void RetrieveUserInterfacePrimitives(DynamicArray<UserInterfacePrimitive *RESTRICT> *const RESTRICT output) const NOEXCEPT override;

private:

	//The minimum.
	Vector2<float32> _Minimum;

	//The maximum.
	Vector2<float32> _Maximum;

	//The font.
	AssetPointer<FontAsset> _Font;

	//The scale.
	float32 _Scale;

	//The horizontal alignment.
	TextHorizontalAlignment _HorizontalAlignment;

	//The vertical alignment.
	TextVerticalAlignment _VerticalAlignment;

	//The text bounds.
	AxisAlignedBoundingBox2D _TextBounds;

	//The primitive.
	TextUserInterfacePrimitive *RESTRICT _Primitive{ nullptr };

};