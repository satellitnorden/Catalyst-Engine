#include "Platform/Core/WindowManager.h"
#if defined(AUDIO_PLUGIN)
//Header file.
#include <Audio/Plugins/Editor/PluginWindow.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

//Audio plugin.
#include <AudioPluginGlue.Generated.h>

namespace Audio
{

	/*
	*	Initializes this plugin window.
	*/
	void PluginWindow::Initialize(const Resolution resolution, void *const RESTRICT parent_window) NOEXCEPT
	{
		//Set the resolution.
		_Resolution = resolution;

		//Create the plugin window.
		Platform::WindowManager::CreateParameters parameters;

		parameters._Backend = Platform::WindowManager::Backend::X11;
		parameters._Title = AudioPluginGlue::GetName();
		parameters._Resolution = resolution;
		parameters._Parent = parent_window;
		parameters._FullScreen = false;
		parameters._ResolutionIncludesPlatformElements = false;
		parameters._SupportOpenGL = true;

		_Window = Platform::WindowManager::Create(parameters);

		//Assume that we support OpenGL.
		SetSupportsOpenGL(Platform::WindowManager::SupportsOpenGL(_Window));

		//Register the callbacks.
		Platform::WindowManager::AddWindowClosedCallback
		(
			_Window,
			[](void *const RESTRICT arguments)
			{
				CatalystEngineSystem::Instance->SetShouldTerminate();
			},
			nullptr
		);
		Platform::WindowManager::AddKeyboardButtonDownCallback
		(
			_Window,
			[](const KeyboardButton keyboard_button, void *const RESTRICT user_data)
			{
				(*static_cast<KeyboardState *const RESTRICT>(user_data))[keyboard_button] = ButtonState::PRESSED;
			},
			&_KeyboardState
		);
		Platform::WindowManager::AddKeyboardButtonUpCallback
		(
			_Window,
			[](const KeyboardButton keyboard_button, void *const RESTRICT user_data)
			{
				(*static_cast<KeyboardState *const RESTRICT>(user_data))[keyboard_button] = ButtonState::RELEASED;
			},
			&_KeyboardState
		);
		Platform::WindowManager::AddMouseMoveCallback
		(
			_Window,
			[](const Vector2<float32> position, void *const RESTRICT user_data)
			{
				MouseState *const RESTRICT mouse_state{ static_cast<MouseState *const RESTRICT>(user_data) };

				mouse_state->_CurrentX = position._X;
				mouse_state->_CurrentY = position._Y;

				mouse_state->_DeltaX = mouse_state->_CurrentX - mouse_state->_PreviousX;
				mouse_state->_DeltaY = mouse_state->_CurrentY - mouse_state->_PreviousY;
			},
			&_MouseState
		);
		Platform::WindowManager::AddMouseButtonDownCallback
		(
			_Window,
			[](const MouseButton mouse_button, void *const RESTRICT user_data)
			{
				MouseState *const RESTRICT mouse_state{ static_cast<MouseState *const RESTRICT>(user_data) };

				switch (mouse_button)
				{
					case MouseButton::LEFT:
					{
						mouse_state->_Left = ButtonState::PRESSED;

						break;
					}

					case MouseButton::MIDDLE:
					{
						mouse_state->_ScrollWheel = ButtonState::PRESSED;

						break;
					}

					case MouseButton::RIGHT:
					{
						mouse_state->_Right = ButtonState::PRESSED;

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
				}
			},
			&_MouseState
		);
		Platform::WindowManager::AddMouseButtonUpCallback
		(
			_Window,
			[](const MouseButton mouse_button, void *const RESTRICT user_data)
			{
				MouseState *const RESTRICT mouse_state{ static_cast<MouseState *const RESTRICT>(user_data) };

				switch (mouse_button)
				{
					case MouseButton::LEFT:
					{
						mouse_state->_Left = ButtonState::RELEASED;

						break;
					}

					case MouseButton::MIDDLE:
					{
						mouse_state->_ScrollWheel = ButtonState::RELEASED;

						break;
					}

					case MouseButton::RIGHT:
					{
						mouse_state->_Right = ButtonState::RELEASED;

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
				}
			},
			&_MouseState
		);
		Platform::WindowManager::AddMouseScrollWheelCallback
		(
			_Window,
			[](const int8 step, void *const RESTRICT user_data)
			{
				static_cast<MouseState *const RESTRICT>(user_data)->_ScrollWheelStep += step;
			},
			&_MouseState
		);
	}

	/*
	*	Shows this plugin window.
	*/
	void PluginWindow::Show()
	{		
		//Show the window.
		Platform::WindowManager::Show(_Window);
	}

	/*
	*	Hides this plugin window.
	*/
	void PluginWindow::Hide()
	{		
		//Hide the window.
		Platform::WindowManager::Hide(_Window);
	}

	/*
	*	Destroys this plugin window.
	*/
	void PluginWindow::Destroy()
	{
		if (_Window)
		{
			Platform::WindowManager::Destroy(_Window);

			_Window = Platform::INVALID_WINDOW_HANDLE;
		}
	}

	/*
	*	Prepares rendering.
	*/
	void PluginWindow::PrepareRendering() NOEXCEPT
	{
		Platform::WindowManager::Update(_Window);

		if (!Platform::WindowManager::SupportsOpenGL(_Window))
		{
			return;
		}

		Platform::WindowManager::PrepareOpenGL(_Window);
	}

	/*
	*	Returns the mouse state.
	*/
	NO_DISCARD const MouseState *const RESTRICT PluginWindow::GetMouseState() NOEXCEPT
	{
		//Return the mouse state.
		return &_MouseState;
	}

	/*
	*	Finalizes rendering.
	*/
	void PluginWindow::FinalizeRendering() NOEXCEPT
	{
		//Update the keyboard/mouse states.
		{
			//Update the keyboard state.
			for (ButtonState &button_state : _KeyboardState._ButtonStates)
			{
				if (button_state == ButtonState::PRESSED)
				{
					button_state = ButtonState::PRESSED_HELD;
				}

				else if (button_state == ButtonState::RELEASED)
				{
					button_state = ButtonState::RELEASED_HELD;
				}
			}

			//Update the mouse state.
			_MouseState._PreviousX = _MouseState._CurrentX;
			_MouseState._PreviousY = _MouseState._CurrentY;
			_MouseState._DeltaX = 0.0f;
			_MouseState._DeltaY = 0.0f;

			if (_MouseState._Left == ButtonState::PRESSED)
			{
				_MouseState._Left = ButtonState::PRESSED_HELD;
			}

			else if (_MouseState._Left == ButtonState::RELEASED)
			{
				_MouseState._Left = ButtonState::RELEASED_HELD;
			}

			if (_MouseState._ScrollWheel == ButtonState::PRESSED)
			{
				_MouseState._ScrollWheel = ButtonState::PRESSED_HELD;
			}

			else if (_MouseState._ScrollWheel == ButtonState::RELEASED)
			{
				_MouseState._ScrollWheel = ButtonState::RELEASED_HELD;
			}

			if (_MouseState._Right == ButtonState::PRESSED)
			{
				_MouseState._Right = ButtonState::PRESSED_HELD;
			}

			else if (_MouseState._Right == ButtonState::RELEASED)
			{
				_MouseState._Right = ButtonState::RELEASED_HELD;
			}

			_MouseState._ScrollWheelStep = 0;
		}

		if (Platform::WindowManager::SupportsOpenGL(_Window))
		{
			Platform::WindowManager::FinalizeOpenGL(_Window);
		}
	}

	/*
	*	Renders to this window.
	*/
	void PluginWindow::RenderTo(const uint32 *const RESTRICT data) NOEXCEPT
	{
		Platform::WindowManager::Render(_Window, data);
	}

}
#endif