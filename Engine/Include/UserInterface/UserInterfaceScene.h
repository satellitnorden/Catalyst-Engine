#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//User interface.
#include <UserInterface/UserInterfaceElement.h>
#include <UserInterface/UserInterfaceProgressBar.h>

class UserInterfaceScene
{

public:

	/*
	*	Default constructor.
	*/
	UserInterfaceScene() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	FORCE_INLINE virtual ~UserInterfaceScene() = default;

	/*
	*	Returns whether or not this scene is active.
	*/
	FORCE_INLINE NO_DISCARD bool GetIsActive() const NOEXCEPT
	{
		return _IsActive;
	}

	/*
	*	Sets whether or not this scene is active.
	*/
	FORCE_INLINE void SetIsActive(const bool value) NOEXCEPT
	{
		_IsActive = value;
	}

	/*
	*	Callback for when this user interface scene is activated.
	*/
	FORCE_INLINE virtual void OnActivated() NOEXCEPT
	{

	}

	/*
	*	Callback for when this user interface scene is activated updated
	*/
	FORCE_INLINE virtual void Update() NOEXCEPT
	{

	}

	/*
	*	Callback for when this user interface scene is deactivated.
	*/
	virtual void OnDeactivated() NOEXCEPT;

protected:

	/*
	*	Sets the horizontal subdivision.
	*/
	FORCE_INLINE void SetHorizontalSubdivision(const uint32 value) NOEXCEPT
	{
		_HorizontalSubdivision = value;
	}

	/*
	*	Sets the vertical subdivision.
	*/
	FORCE_INLINE void SetVerticalSubdivision(const uint32 value) NOEXCEPT
	{
		_VerticalSubdivision = value;
	}

	/*
	*	Sets the progress bar bottom material.
	*/
	FORCE_INLINE void SetProgressBarBottomMaterial(const UserInterfaceMaterial &value) NOEXCEPT
	{
		_ProgressBarBottomMaterial = value;
	}

	/*
	*	Sets the progress bar top material.
	*/
	FORCE_INLINE void SetProgressBarTopMaterial(const UserInterfaceMaterial &value) NOEXCEPT
	{
		_ProgressBarTopMaterial = value;
	}

	/*
	*	Adds a button on the given row/column.
	*/
	void AddButton(	const uint32 vertical_index,
					const uint32 horizontal_index,
					const uint32 number_of_elements_vertically,
					const ButtonUserInterfaceElementCallback start_pressed_callback,
					const char *const RESTRICT text,
					UserInterfaceElement *RESTRICT *const RESTRICT button_element = nullptr,
					UserInterfaceElement *RESTRICT *const RESTRICT text_element = nullptr) NOEXCEPT;

	/*
	*	Adds a progress bar.
	*/
	RESTRICTED UserInterfaceProgressBar *const RESTRICT AddProgressBar(	const uint32 vertical_index,
																		const uint32 horizontal_index,
																		const uint32 number_of_elements_vertically) NOEXCEPT;

private:

	//Denotes whether or not this scene is active.
	bool _IsActive{ false };

	//The horizontal subdivision.
	uint32 _HorizontalSubdivision;

	//The vertical subdivision.
	uint32 _VerticalSubdivision;

	//The progress bar bottom material.
	UserInterfaceMaterial _ProgressBarBottomMaterial;

	//The progress bar top material.
	UserInterfaceMaterial _ProgressBarTopMaterial;

	//The progress bars.
	DynamicArray<UserInterfaceProgressBar *const RESTRICT> _ProgressBars;

	//Container for all the user interface elements.
	DynamicArray<UserInterfaceElement *RESTRICT> _UserInterfaceElements;

	/*
	*	Calculates the bounding box for the given row/column.
	*/
	void CalculateBoundingBox(	const Vector2<uint32> &in_minimum,
								const Vector2<uint32> &in_maximum,
								Vector2<float32> *const RESTRICT out_minimum,
								Vector2<float32> *const RESTRICT out_maximum) NOEXCEPT;

	/*
	*	Calculates the bounding box for the given row/column.
	*/
	void CalculateBoundingBox(	const uint32 vertical_index,
								const uint32 horizontal_index,
								const uint32 number_of_elements_vertically,
								Vector2<float32> *const RESTRICT minimum,
								Vector2<float32> *const RESTRICT maximum) NOEXCEPT;

};