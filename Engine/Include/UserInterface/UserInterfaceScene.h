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
		_HorizontalSubdivisionReciprocal = 1.0f / static_cast<float32>(_HorizontalSubdivision);
	}

	/*
	*	Sets the vertical subdivision.
	*/
	FORCE_INLINE void SetVerticalSubdivision(const uint32 value) NOEXCEPT
	{
		_VerticalSubdivision = value;
		_VerticalSubdivisionReciprocal = 1.0f / static_cast<float32>(_VerticalSubdivision);
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
	*	Adds a button.
	*/
	void AddButton(	const Vector2<uint32> &minimum_cell,
					const Vector2<uint32> &maximum_cell,
					const ButtonUserInterfaceElementCallback start_pressed_callback,
					const char *const RESTRICT text,
					UserInterfaceElement *RESTRICT *const RESTRICT button_element = nullptr,
					UserInterfaceElement *RESTRICT *const RESTRICT text_element = nullptr) NOEXCEPT;

	/*
	*	Adds a progress bar.
	*/
	RESTRICTED UserInterfaceProgressBar *const RESTRICT AddProgressBar(	const Vector2<uint32> &minimum_cell,
																		const Vector2<uint32> &maximum_cell,
																		const char *const RESTRICT text = "",
																		UserInterfaceMaterial *const RESTRICT bottom_material_override = nullptr,
																		UserInterfaceMaterial *const RESTRICT top_material_override = nullptr) NOEXCEPT;

private:

	//Denotes whether or not this scene is active.
	bool _IsActive{ false };

	//The horizontal subdivision.
	uint32 _HorizontalSubdivision;

	//The horizontal subdivision reciprocal.
	float32 _HorizontalSubdivisionReciprocal;

	//The vertical subdivision.
	uint32 _VerticalSubdivision;

	//The vertical subdivision reciprocal.
	float32 _VerticalSubdivisionReciprocal;

	//The progress bar bottom material.
	UserInterfaceMaterial _ProgressBarBottomMaterial;

	//The progress bar top material.
	UserInterfaceMaterial _ProgressBarTopMaterial;

	//The progress bars.
	DynamicArray<UserInterfaceProgressBar *const RESTRICT> _ProgressBars;

	//Container for all the user interface elements.
	DynamicArray<UserInterfaceElement *RESTRICT> _UserInterfaceElements;

	/*
	*	Calculates the bounding box for the given minimum/maximum cell.
	*/
	void CalculateBoundingBox(	const Vector2<uint32> &minimum_cell,
								const Vector2<uint32> &maximum_cell,
								Vector2<float32> *const RESTRICT minimum,
								Vector2<float32> *const RESTRICT maximum) NOEXCEPT;

};