#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

namespace UI
{

	/*
	*	Scrollable interface class.
	*	Widgets can store this and return it in 'Widget::GetScrollableInterface()' to be able to be receive scroll events.
	*/
	class ScrollableInterface final
	{

	public:

		//Type aliases.
		using Callback = void(*)(void *const RESTRICT /*user_data*/);

		//Should return if the given position is inside the given widget.
		bool (*_IsInside)(const class Widget *const RESTRICT widget, const Vector2<float32> position);

		/*
		*	Default constructor.
		*/
		ScrollableInterface() NOEXCEPT;

		/*
		*	Sets the user data.
		*/
		FORCE_INLINE ScrollableInterface *const RESTRICT SetUserData(void *const RESTRICT user_data) NOEXCEPT
		{
			_UserData = user_data;
			return this;
		}

		/*
		*	Sets the on scroll down callback.
		*/
		FORCE_INLINE ScrollableInterface *const RESTRICT SetOnScrollDownCallback(const Callback callback) NOEXCEPT
		{
			_OnScrollDownCallback = callback;
			return this;
		}

		/*
		*	Sets the on scroll up callback.
		*/
		FORCE_INLINE ScrollableInterface *const RESTRICT SetOnScrollUpCallback(const Callback callback) NOEXCEPT
		{
			_OnScrollUpCallback = callback;
			return this;
		}

		/*
		*	Called on scroll down.
		*/
		FORCE_INLINE void OnScrollDown() NOEXCEPT
		{
			if (_OnScrollDownCallback)
			{
				_OnScrollDownCallback(_UserData);
			}
		}

		/*
		*	Called on scroll up.
		*/
		FORCE_INLINE void OnScrollUp() NOEXCEPT
		{
			if (_OnScrollUpCallback)
			{
				_OnScrollUpCallback(_UserData);
			}
		}

	private:

		//The user data.
		void *RESTRICT _UserData{ nullptr };

		//Callback for when the user scrolled down.
		Callback _OnScrollDownCallback{ nullptr };

		//Callback for when the user scrolled up.
		Callback _OnScrollUpCallback{ nullptr };

	};

}