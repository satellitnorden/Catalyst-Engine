#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

namespace UI
{

	/*
	*	Clickable interface class.
	*	Widgets can store this and return it in 'Widget::GetClickableInterface()' to be able to be clicked.
	*/
	class ClickableInterface final
	{

	public:

		//Type aliases.
		using Callback = void(*)(void *const RESTRICT /*user_data*/);

		//Enumeration covering all states.
		enum class State : uint8
		{
			IDLE,
			HOVERED,
			PRESSED
		};

		//Should return if the given position is inside the given widget.
		bool (*_IsInside)(const class Widget *const RESTRICT widget, const Vector2<float32> position);

		//Callback for when the state changes.
		void (*_OnStateChanged)(class Widget *const RESTRICT widget, const State previous_state, const State new_state) { nullptr };

		/*
		*	Default constructor.
		*/
		ClickableInterface() NOEXCEPT;

		/*
		*	Returns the state.
		*/
		FORCE_INLINE NO_DISCARD State GetState() const NOEXCEPT
		{
			return _State;
		}

		/*
		*	Sets the state.
		*/
		FORCE_INLINE void SetState(class Widget *const RESTRICT widget, const State state) NOEXCEPT
		{
			if (_State != state)
			{
				const State previous_state{ _State };
				const State new_state{ state };

				if (_OnStateChanged)
				{
					_OnStateChanged(widget, previous_state, new_state);
				}

				if (previous_state == State::PRESSED && _OnEndPressCallback)
				{
					_OnEndPressCallback(_UserData);
				}

				_State = state;
			}
		}

		/*
		*	Sets the user data.
		*/
		FORCE_INLINE ClickableInterface *const RESTRICT SetUserData(void* const RESTRICT user_data) NOEXCEPT
		{
			_UserData = user_data;
			return this;
		}

		/*
		*	Sets the on end press callback.
		*/
		FORCE_INLINE ClickableInterface *const RESTRICT SetOnEndPressCallback(const Callback callback) NOEXCEPT
		{
			_OnEndPressCallback = callback;
			return this;
		}

	private:

		//The state.
		State _State;

		//The user data.
		void *RESTRICT _UserData{ nullptr };

		//The on start press callback.
		Callback _OnEndPressCallback{ nullptr };

	};

}