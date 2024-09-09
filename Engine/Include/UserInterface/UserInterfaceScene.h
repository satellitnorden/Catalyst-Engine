#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Entities.
#include <Entities/Core/Entity.h>

//Input.
#include <Input/InputCore.h>

//User interface.
#include <UserInterface/UserInterfaceButton.h>
#include <UserInterface/UserInterfaceCheckbox.h>
#include <UserInterface/UserInterfaceImage.h>
#include <UserInterface/UserInterfaceProgressBar.h>
#include <UserInterface/UserInterfaceText.h>
#include <UserInterface/UserInterfaceTextInput.h>

class UserInterfaceScene
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE UserInterfaceScene() NOEXCEPT
	{

	}

	/*
	*	Default destructor.
	*/
	virtual ~UserInterfaceScene();

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
	virtual void OnActivated() NOEXCEPT;

	/*
	*	Callback for when this user interface scene is activated updated
	*/
	virtual void Update() NOEXCEPT;

	/*
	*	Callback for when this user interface scene is deactivated.
	*/
	virtual void OnDeactivated() NOEXCEPT;

	/*
	*	Returns the name.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const char *const RESTRICT GetName() const NOEXCEPT
	{
		return _Name.Data();
	}

	/*
	*	Sets the name.
	*/
	FORCE_INLINE void SetName(const char *const RESTRICT value) NOEXCEPT
	{
		_Name = value;
	}

	/*
	*	Returns the identifier.
	*/
	FORCE_INLINE NO_DISCARD HashString GetIdentifier() const NOEXCEPT
	{
		return _Identifier;
	}

	/*
	*	Sets the identifier.
	*/
	FORCE_INLINE void SetIdentifier(const HashString value) NOEXCEPT
	{
		_Identifier = value;
	}

	/*
	*	Returns the entity.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD Entity *const RESTRICT GetEntity() NOEXCEPT
	{
		return _Entity;
	}

	/*
	*	Sets the entity.
	*/
	FORCE_INLINE void SetEntity(Entity *const RESTRICT value) NOEXCEPT
	{
		_Entity = value;
	}

	/*
	*	Returns the user interface primitives for this scene.
	*/
	void RetrieveUserInterfacePrimitives(DynamicArray<UserInterfacePrimitive *RESTRICT> *const RESTRICT output) NOEXCEPT;

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
	*	Sets the font.
	*/
	FORCE_INLINE void SetFont(const AssetPointer<FontAsset> value) NOEXCEPT
	{
		_Font = value;
	}

	/*
	*	Sets the button idle material.
	*/
	FORCE_INLINE void SetButtonIdleMaterial(const UserInterfaceMaterial &value) NOEXCEPT
	{
		_ButtonIdleMaterial = value;
	}

	/*
	*	Sets the button hovered material.
	*/
	FORCE_INLINE void SetButtonHoveredMaterial(const UserInterfaceMaterial &value) NOEXCEPT
	{
		_ButtonHoveredMaterial = value;
	}

	/*
	*	Sets the button pressed material.
	*/
	FORCE_INLINE void SetButtonPressedMaterial(const UserInterfaceMaterial &value) NOEXCEPT
	{
		_ButtonPressedMaterial = value;
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
	*	Sets the text scale.
	*/
	FORCE_INLINE void SetTextScale(const float32 value) NOEXCEPT
	{
		_TextScale = value;
	}

	/*
	*	Adds a button, using cells.
	*/
	RESTRICTED UserInterfaceButton *const RESTRICT AddButtonByCell(	const Vector2<uint32> &minimum_cell,
																	const Vector2<uint32> &maximum_cell,
																	const UserInterfaceButton::Callback start_pressed_callback,
																	const UserInterfaceButton::Callback stop_pressed_callback,
																	const UserInterfaceButton::Callback start_hovered_callback,
																	const UserInterfaceButton::Callback stop_hovered_callback,
																	UserInterfaceMaterial *const RESTRICT idle_material_override = nullptr,
																	UserInterfaceMaterial *const RESTRICT hovered_material_override = nullptr,
																	UserInterfaceMaterial *const RESTRICT pressed_material_override = nullptr,
																	const char *const RESTRICT text = "",
																	const float32 *const RESTRICT text_scale_override = nullptr) NOEXCEPT;

	/*
	*	Adds a button, raw coordinates.
	*/
	RESTRICTED UserInterfaceButton *const RESTRICT AddButtonByNormalizedCoordinate(	const Vector2<float32> &minimum,
																					const Vector2<float32> &maximum,
																					const UserInterfaceButton::Callback start_pressed_callback,
																					const UserInterfaceButton::Callback stop_pressed_callback,
																					const UserInterfaceButton::Callback start_hovered_callback,
																					const UserInterfaceButton::Callback stop_hovered_callback,
																					UserInterfaceMaterial *const RESTRICT idle_material_override = nullptr,
																					UserInterfaceMaterial *const RESTRICT hovered_material_override = nullptr,
																					UserInterfaceMaterial *const RESTRICT pressed_material_override = nullptr,
																					const char *const RESTRICT text = "",
																					const float32 *const RESTRICT text_scale_override = nullptr) NOEXCEPT;

	/*
	*	Removes a button.
	*/
	void RemoveButton(UserInterfaceButton *const RESTRICT button) NOEXCEPT;

	/*
	*	Adds a checkbox, using cells.
	*/
	RESTRICTED UserInterfaceCheckbox *const RESTRICT AddCheckboxByCell(	const Vector2<uint32> &minimum_cell,
																		const Vector2<uint32> &maximum_cell,
																		const bool initially_checked,
																		const UserInterfaceCheckbox::Callback start_pressed_callback,
																		UserInterfaceMaterial *const RESTRICT unchecked_idle_material_override = nullptr,
																		UserInterfaceMaterial *const RESTRICT unchecked_hovered_material_overrid = nullptr,
																		UserInterfaceMaterial *const RESTRICT unchecked_pressed_material_overrid = nullptr,
																		UserInterfaceMaterial *const RESTRICT checked_idle_material_overrid = nullptr,
																		UserInterfaceMaterial *const RESTRICT checked_hovered_material_override = nullptr,
																		UserInterfaceMaterial *const RESTRICT checked_pressed_material_override = nullptr,
																		const char *const RESTRICT text = "",
																		const float32 *const RESTRICT scale_override = nullptr,
																		const TextHorizontalAlignment horizontal_alignment = TextHorizontalAlignment::LEFT,
																		const TextVerticalAlignment vertical_alignment = TextVerticalAlignment::CENTER) NOEXCEPT;

	/*
	*	Adds a checkbox, using raw coordinates.
	*/
	RESTRICTED UserInterfaceCheckbox *const RESTRICT AddCheckboxByNormalizedCoordinate(	const Vector2<float32> &minimum,
																						const Vector2<float32> &maximum,
																						const bool initially_checked,
																						const UserInterfaceCheckbox::Callback start_pressed_callback,
																						UserInterfaceMaterial *const RESTRICT unchecked_idle_material_override = nullptr,
																						UserInterfaceMaterial *const RESTRICT unchecked_hovered_material_overrid = nullptr,
																						UserInterfaceMaterial *const RESTRICT unchecked_pressed_material_overrid = nullptr,
																						UserInterfaceMaterial *const RESTRICT checked_idle_material_overrid = nullptr,
																						UserInterfaceMaterial *const RESTRICT checked_hovered_material_override = nullptr,
																						UserInterfaceMaterial *const RESTRICT checked_pressed_material_override = nullptr,
																						const char *const RESTRICT text = "",
																						const float32 *const RESTRICT scale_override = nullptr,
																						const TextHorizontalAlignment horizontal_alignment = TextHorizontalAlignment::LEFT,
																						const TextVerticalAlignment vertical_alignment = TextVerticalAlignment::CENTER) NOEXCEPT;
	
	/*
	*	Removes a checkbox.
	*/
	void RemoveCheckbox(UserInterfaceCheckbox *const RESTRICT checkbox) NOEXCEPT;

	/*
	*	Adds an image, using cells.
	*/
	RESTRICTED UserInterfaceImage *const RESTRICT AddImageByCell(	const Vector2<uint32> &minimum_cell,
																	const Vector2<uint32> &maximum_cell,
																	const UserInterfaceMaterial &material,
																	const float32 opacity = 1.0f) NOEXCEPT;
	/*
	*	Adds an image, raw coordinates.
	*/
	RESTRICTED UserInterfaceImage *const RESTRICT AddImageByNormalizedCoordinate(	const Vector2<float32> &minimum,
																					const Vector2<float32> &maximum,
																					const UserInterfaceMaterial &material,
																					const float32 opacity = 1.0f) NOEXCEPT;

	/*
	*	Removes an image.
	*/
	void RemoveImage(UserInterfaceImage *const RESTRICT image) NOEXCEPT;

	/*
	*	Adds a progress bar, using cells.
	*/
	RESTRICTED UserInterfaceProgressBar *const RESTRICT AddProgressBarByCell(	const Vector2<uint32> &minimum_cell,
																				const Vector2<uint32> &maximum_cell,
																				const char *const RESTRICT text = "",
																				UserInterfaceMaterial *const RESTRICT bottom_material_override = nullptr,
																				UserInterfaceMaterial *const RESTRICT top_material_override = nullptr) NOEXCEPT;

	/*
	*	Adds a progress bar, using raw coordinates.
	*/
	RESTRICTED UserInterfaceProgressBar *const RESTRICT AddProgressBarByNormalizedCoordinate(	const Vector2<float32> &minimum,
																								const Vector2<float32> &maximum,
																								const char *const RESTRICT text = "",
																								UserInterfaceMaterial *const RESTRICT bottom_material_override = nullptr,
																								UserInterfaceMaterial *const RESTRICT top_material_override = nullptr) NOEXCEPT;

	/*
	*	Adds a text, using cells.
	*/
	RESTRICTED UserInterfaceText *const RESTRICT AddTextByCell(	const Vector2<uint32> &minimum_cell,
																const Vector2<uint32> &maximum_cell,
																const char *const RESTRICT text = "",
																const float32 *const RESTRICT scale_override = nullptr,
																const TextHorizontalAlignment horizontal_alignment = TextHorizontalAlignment::CENTER,
																const TextVerticalAlignment vertical_alignment = TextVerticalAlignment::CENTER) NOEXCEPT;

	/*
	*	Adds a text, using raw coordinates.
	*/
	RESTRICTED UserInterfaceText *const RESTRICT AddTextByNormalizedCoordinate(	const Vector2<float32> &minimum,
																				const Vector2<float32> &maximum,
																				const char *const RESTRICT text = "",
																				const float32 *const RESTRICT scale_override = nullptr,
																				const TextHorizontalAlignment horizontal_alignment = TextHorizontalAlignment::CENTER,
																				const TextVerticalAlignment vertical_alignment = TextVerticalAlignment::CENTER) NOEXCEPT;

	/*
	*	Removes a text.
	*/
	void RemoveText(UserInterfaceText *const RESTRICT text) NOEXCEPT;

	/*
	*	Adds a text input, using cells.
	*/
	RESTRICTED UserInterfaceTextInput *const RESTRICT AddTextInputByCell(	const Vector2<uint32> &minimum_cell,
																			const Vector2<uint32> &maximum_cell,
																			const char *const RESTRICT prompt_text,
																			const char *const RESTRICT text = "") NOEXCEPT;

	/*
	*	Adds a text input, using raw coordinates.
	*/
	RESTRICTED UserInterfaceTextInput *const RESTRICT AddTextInputByNormalizedCoordinate(	const Vector2<float32> &minimum,
																							const Vector2<float32> &maximum,
																							const char *const RESTRICT prompt_text,
																							const char *const RESTRICT text = "") NOEXCEPT;

	/*
	*	Removes a text input.
	*/
	void RemoveTextInput(UserInterfaceTextInput *const RESTRICT text_input) NOEXCEPT;

	/*
	*	Removes the given element.
	*/
	void Remove(UserInterfaceElement *const RESTRICT element) NOEXCEPT;

	/*
	*	Removes all user interface elements.
	*/
	void RemoveAll() NOEXCEPT;

	/*
	*	Returns the normalized coordinate center of the given cell.
	*/
	NO_DISCARD Vector2<float32> GetNormalizedCoordinateCenterOfCell(const Vector2<uint32> &cell) NOEXCEPT;

	/*
	*	Calculates the bounding box for the given minimum/maximum cell.
	*/
	void CalculateBoundingBox(	const Vector2<uint32> &minimum_cell,
								const Vector2<uint32> &maximum_cell,
								Vector2<float32> *const RESTRICT minimum,
								Vector2<float32> *const RESTRICT maximum) NOEXCEPT;

private:

	/*
	*	Button interface class definition.
	*/
	class ButtonInterface final
	{

	public:

		/*
		*	Default constructor.
		*/
		FORCE_INLINE ButtonInterface() NOEXCEPT
			:
			_State(State::NONE),
			_Element(nullptr)
		{

		}

		/*
		*	Constructor taking a user interface button.
		*/
		FORCE_INLINE ButtonInterface(UserInterfaceButton *const RESTRICT button) NOEXCEPT
			:
			_State(State::BUTTON),
			_Element(button)
		{

		}

		/*
		*	Constructor taking a user interface checkbox.
		*/
		FORCE_INLINE ButtonInterface(UserInterfaceCheckbox *const RESTRICT checkbox) NOEXCEPT
			:
			_State(State::CHECKBOX),
			_Element(checkbox)
		{

		}

		/*
		*	Bool operator overload.
		*/
		FORCE_INLINE operator bool() const NOEXCEPT
		{
			return _State != State::NONE;
		}

		/*
		*	Equality operator overload.
		*/
		FORCE_INLINE bool operator==(const ButtonInterface &other) const NOEXCEPT
		{
			return _Element == other._Element;
		}

		/*
		*	Inequality operator overload.
		*/
		FORCE_INLINE bool operator!=(const ButtonInterface &other) const NOEXCEPT
		{
			return _Element != other._Element;
		}

		/*
		*	Returns the minimum.
		*/
		FORCE_INLINE NO_DISCARD Vector2<float32> GetMinimum() const NOEXCEPT
		{
			switch (_State)
			{
				case State::NONE:
				{
					return Vector2<float32>(0.0f, 0.0f);
				}

				case State::BUTTON:
				{
					return static_cast<UserInterfaceButton *const RESTRICT>(_Element)->GetMinimum();
				}

				case State::CHECKBOX:
				{
					return static_cast<UserInterfaceCheckbox *const RESTRICT>(_Element)->GetButtonMinimum();
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					return Vector2<float32>(0.0f, 0.0f);
				}
			}
		}

		/*
		*	Returns the maximum.
		*/
		FORCE_INLINE NO_DISCARD Vector2<float32> GetMaximum() const NOEXCEPT
		{
			switch (_State)
			{
				case State::NONE:
				{
					return Vector2<float32>(0.0f, 0.0f);
				}

				case State::BUTTON:
				{
					return static_cast<UserInterfaceButton *const RESTRICT>(_Element)->GetMaximum();
				}

				case State::CHECKBOX:
				{
					return static_cast<UserInterfaceCheckbox *const RESTRICT>(_Element)->GetButtonMaximum();
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					return Vector2<float32>(0.0f, 0.0f);
				}
			}
		}

		/*
		*	Returns the current state.
		*/
		FORCE_INLINE NO_DISCARD UserInterfaceButtonState GetCurrentState() const NOEXCEPT
		{
			switch (_State)
			{
				case State::NONE:
				{
					return UserInterfaceButtonState::IDLE;
				}

				case State::BUTTON:
				{
					return static_cast<UserInterfaceButton *const RESTRICT>(_Element)->GetCurrentState();
				}

				case State::CHECKBOX:
				{
					return static_cast<UserInterfaceCheckbox *const RESTRICT>(_Element)->GetCurrentState();
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					return UserInterfaceButtonState::IDLE;
				}
			}
		}

		/*
		*	Sets the current state.
		*/
		FORCE_INLINE void SetCurrentState(const UserInterfaceButtonState value) NOEXCEPT
		{
			switch (_State)
			{
				case State::NONE:
				{
					//Do nothing.

					break;
				}

				case State::BUTTON:
				{
					static_cast<UserInterfaceButton *const RESTRICT>(_Element)->SetCurrentState(value);

					break;
				}

				case State::CHECKBOX:
				{
					static_cast<UserInterfaceCheckbox *const RESTRICT>(_Element)->SetCurrentState(value);

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}
		}

		/*
		*	On start hovered callback.
		*/
		FORCE_INLINE void OnStartHovered() NOEXCEPT
		{
			switch (_State)
			{
				case State::NONE:
				{
					//Do nothing.

					break;
				}

				case State::BUTTON:
				{
					static_cast<UserInterfaceButton *const RESTRICT>(_Element)->OnStartHovered();

					break;
				}

				case State::CHECKBOX:
				{
					static_cast<UserInterfaceCheckbox *const RESTRICT>(_Element)->OnStartHovered();

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}
		}

		/*
		*	On stop hovered callback.
		*/
		FORCE_INLINE void OnStopHovered() NOEXCEPT
		{
			switch (_State)
			{
				case State::NONE:
				{
					//Do nothing.

					break;
				}

				case State::BUTTON:
				{
					static_cast<UserInterfaceButton *const RESTRICT>(_Element)->OnStopHovered();

					break;
				}

				case State::CHECKBOX:
				{
					static_cast<UserInterfaceCheckbox *const RESTRICT>(_Element)->OnStopHovered();

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}
		}

		/*
		*	On start pressed callback.
		*/
		FORCE_INLINE void OnStartPressed() NOEXCEPT
		{
			switch (_State)
			{
				case State::NONE:
				{
					//Do nothing.

					break;
				}

				case State::BUTTON:
				{
					static_cast<UserInterfaceButton *const RESTRICT>(_Element)->OnStartPressed();

					break;
				}

				case State::CHECKBOX:
				{
					static_cast<UserInterfaceCheckbox *const RESTRICT>(_Element)->OnStartPressed();

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}
		}

		/*
		*	On stop pressed callback.
		*/
		FORCE_INLINE void OnStopPressed() NOEXCEPT
		{
			switch (_State)
			{
				case State::NONE:
				{
					//Do nothing.

					break;
				}

				case State::BUTTON:
				{
					static_cast<UserInterfaceButton *const RESTRICT>(_Element)->OnStopPressed();

					break;
				}

				case State::CHECKBOX:
				{
					static_cast<UserInterfaceCheckbox *const RESTRICT>(_Element)->OnStopPressed();

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}
		}

		/*
		*	Returns whether or not this button is gamepad selected.
		*/
		FORCE_INLINE NO_DISCARD bool GetGamepadSelected() const NOEXCEPT
		{
			switch (_State)
			{
				case State::NONE:
				{
					return false;
				}

				case State::BUTTON:
				{
					return static_cast<UserInterfaceButton *const RESTRICT>(_Element)->GetGamepadSelected();
				}

				case State::CHECKBOX:
				{
					return static_cast<UserInterfaceCheckbox *const RESTRICT>(_Element)->GetGamepadSelected();
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					return false;
				}
			}
		}

		/*
		*	Sets whether or not this button is gamepad selected.
		*/
		FORCE_INLINE void SetGamepadSelected(const bool value) NOEXCEPT
		{
			switch (_State)
			{
				case State::NONE:
				{
					//Do nothing.

					break;
				}

				case State::BUTTON:
				{
					static_cast<UserInterfaceButton *const RESTRICT>(_Element)->SetGamepadSelected(value);

					break;
				}

				case State::CHECKBOX:
				{
					static_cast<UserInterfaceCheckbox *const RESTRICT>(_Element)->SetGamepadSelected(value);

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}
		}

	private:

		//Enumeration covering all states.
		enum class State : uint8
		{
			NONE,
			BUTTON,
			CHECKBOX
		};

		//The state.
		State _State;

		//The underlying element.
		void *RESTRICT _Element;

	};

	/*
	*	Cursor state class definition.
	*/
	class CursorState final
	{

	public:

		//The input device type.
		InputDeviceType _InputDeviceType;

		//The position.
		Vector2<float32> _Position;

		//Denotes whether or not the cursor is pressed.
		bool _Pressed;

	};

	//The name.
	DynamicString _Name;

	//The identifier.
	HashString _Identifier;

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

	//The font.
	AssetPointer<FontAsset> _Font;

	//The button idle material.
	UserInterfaceMaterial _ButtonIdleMaterial;

	//The button hovered material.
	UserInterfaceMaterial _ButtonHoveredMaterial;

	//The button pressed material.
	UserInterfaceMaterial _ButtonPressedMaterial;

	//The checkbox unchecked idle material.
	UserInterfaceMaterial _CheckboxUncheckedIdleMaterial;

	//The checkbox unchecked hovered material.
	UserInterfaceMaterial _CheckboxUncheckedHoveredMaterial;

	//The checkbox unchecked pressed material.
	UserInterfaceMaterial _CheckboxUncheckedPressedMaterial;

	//The checkbox checked idle material.
	UserInterfaceMaterial _CheckboxCheckedIdleMaterial;

	//The checkbox checked hovered material.
	UserInterfaceMaterial _CheckboxCheckedHoveredMaterial;

	//The checkbox checked pressed material.
	UserInterfaceMaterial _CheckboxCheckedPressedMaterial;

	//The progress bar bottom material.
	UserInterfaceMaterial _ProgressBarBottomMaterial;

	//The progress bar top material.
	UserInterfaceMaterial _ProgressBarTopMaterial;

	//The text scale.
	float32 _TextScale;

	//Container for all the elements.
	DynamicArray<UserInterfaceElement *RESTRICT> _Elements;

	//The buttons.
	DynamicArray<UserInterfaceButton *RESTRICT> _Buttons;

	//The checkboxes.
	DynamicArray<UserInterfaceCheckbox *RESTRICT> _Checkboxes;

	//The images.
	DynamicArray<UserInterfaceImage *RESTRICT> _Images;

	//The progress bars.
	DynamicArray<UserInterfaceProgressBar *RESTRICT> _ProgressBars;

	//The texts.
	DynamicArray<UserInterfaceText *RESTRICT> _Texts;

	//The text inputs.
	DynamicArray<UserInterfaceTextInput *RESTRICT> _TextInputs;

	//The button interfaces.
	DynamicArray<ButtonInterface> _ButtonInterfaces;

	//The entity.
	Entity *RESTRICT _Entity{ nullptr };

	/*
	*	Updates buttons.
	*/
	void UpdateButtons() NOEXCEPT;

	/*
	*	Updates text inputs.
	*/
	void UpdateTextInputs() NOEXCEPT;

	/*
	*	Retrieves the cursor state.
	*/
	NO_DISCARD CursorState RetrieveCursorState() NOEXCEPT;

};