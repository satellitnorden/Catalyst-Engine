#if defined(AUDIO_PLUGIN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Input.
#include <Input/MouseState.h>

//Rendering.
#include <Rendering/Native/Resolution.h>

namespace Audio
{

	/*
	*	Class for handling the (child) window for a plugin editor.
	*	Handles creation/destruction/showing/hiding as well as handling window messages.
	*/
	class PluginWindow final
	{

	public:

		//Type aliases.
		using UpdateFunction = void(*)(void *const RESTRICT /*arguments*/);

		/*
		*	Sets the update function.
		*/
		void SetUpdateFunction(const UpdateFunction function, void *const RESTRICT arguments) NOEXCEPT;

		/*
		*	Initializes this plugin window.
		*/
		void Initialize(const Resolution resolution, void *const RESTRICT parent_window) NOEXCEPT;

		/*
		*	Shows this plugin window.
		*/
		void Show();

		/*
		*	Hides this plugin window.
		*/
		void Hide();

		/*
		*	Destroys this plugin window.
		*/
		void Destroy();

		/*
		*	Calls the update function.
		*/
		void CallUpdateFunction() NOEXCEPT;

		/*
		*	Returns the resolution.
		*/
		FORCE_INLINE NO_DISCARD Resolution GetResolution() const NOEXCEPT
		{
			return _Resolution;
		}

		/*
		*	Sets the scroll wheel step.
		*/
		FORCE_INLINE void SetScrollWheelStep(const int8 value) NOEXCEPT
		{
			_ScrollWheelStep = value;
		}

		/*
		*	Returns and resets the scroll wheel step.
		*/
		FORCE_INLINE NO_DISCARD int8 GetAndResetScrollWheelStep() NOEXCEPT
		{
			const int8 scroll_wheel_step{ _ScrollWheelStep };
			_ScrollWheelStep = 0;
			return scroll_wheel_step;
		}

		/*
		*	Returns the mouse state.
		*/
		NO_DISCARD const MouseState *const RESTRICT GetMouseState() NOEXCEPT;

		/*
		*	Renders to this window.
		*/
		void RenderTo(const uint32 *const RESTRICT data) NOEXCEPT;

	private:

		//The update function.
		UpdateFunction _UpdateFunction;

		//The update arguments.
		void *RESTRICT _UpdateArguments;

		//The resolution.
		Resolution _Resolution;

		//The window.
		void *RESTRICT _Window{ nullptr };

		//The mouse state.
		MouseState _MouseState;

		//The scroll wheel step.
		int8 _ScrollWheelStep{ 0 };

	};

}
#endif