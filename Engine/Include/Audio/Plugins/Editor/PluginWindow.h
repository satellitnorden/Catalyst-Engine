#if defined(AUDIO_PLUGIN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Input.
#include <Input/KeyboardState.h>
#include <Input/MouseState.h>

//Platform.
#include <Platform/Core/WindowManager.h>

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
		*	Returns whether or not this window supports OpenGL.
		*/
		FORCE_INLINE NO_DISCARD bool GetSupportsOpenGL() const NOEXCEPT
		{
			return _SupportsOpenGL;
		}

		/*
		*	Sets whether or not this window supports OpenGL.
		*/
		FORCE_INLINE void SetSupportsOpenGL(const bool value) NOEXCEPT
		{
			_SupportsOpenGL = value;
		}

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
		*	Prepares rendering.
		*/
		void PrepareRendering() NOEXCEPT;

		/*
		*	Finalizes rendering.
		*/
		void FinalizeRendering() NOEXCEPT;

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
		Platform::WindowHandle _Window{ Platform::INVALID_WINDOW_HANDLE };

		//The timer identifier.
		uint64 _TimerIdentifier{ UINT64_MAXIMUM };

		//Denotes whether or not this window supports OpenGL.
		bool _SupportsOpenGL{ false };

		//The keyboard state.
		KeyboardState _KeyboardState;

		//The mouse state.
		MouseState _MouseState;

		//The scroll wheel step.
		int8 _ScrollWheelStep{ 0 };

	};

}
#endif