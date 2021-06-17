#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/General/Vector.h>

//User interface.
#include <UserInterface/ImageUserInterfacePrimitive.h>
#include <UserInterface/TextUserInterfacePrimitive.h>

class UserInterfaceProgressBar final
{

public:

	/*
	*	Default constructor.
	*/
	UserInterfaceProgressBar(	const Vector2<float32> initial_minimum,
								const Vector2<float32> initial_maximum,
								const UserInterfaceMaterial &initial_bottom_material,
								const UserInterfaceMaterial &initial_top_material,
								const char *const RESTRICT text,
								const bool is_three_dimensional) NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~UserInterfaceProgressBar() NOEXCEPT;

	/*
	*	Returns the current progress.
	*/
	FORCE_INLINE NO_DISCARD float32 GetCurrentProgress() const NOEXCEPT
	{
		return _CurrentProgress;
	}

	/*
	*	Sets the current progress.
	*/
	void SetCurrentProgress(const float32 value) NOEXCEPT;

	/*
	*	Sets the text.
	*/
	void SetText(const char *const RESTRICT text) NOEXCEPT;

	/*
	*	Returns the user interface primitives.
	*/
	void RetrieveUserInterfacePrimitives(DynamicArray<const UserInterfacePrimitive *RESTRICT> *const RESTRICT output) const NOEXCEPT;

private:

	//The bottom primitive.
	ImageUserInterfacePrimitive *RESTRICT _BottomPrimitive;

	//The top primitive.
	ImageUserInterfacePrimitive *RESTRICT _TopPrimitive;

	//The text primitive.
	TextUserInterfacePrimitive *RESTRICT _TextPrimitive{ nullptr };

	//The current progress.
	float32 _CurrentProgress{ 0.0f };
	
	//Denotes whether or not this progress bar is three dimensional.
	bool _IsThreeDimensional{ false };

};