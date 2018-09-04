#if defined(CATALYST_ANDROID)
//Header file.
#include <Platform/Android/CatalystAndroid.h>

//Core.
#include <Core/Core/CatalystCore.h>

//Input.
#include <Input/TouchState.h>

//Systems.
#include <Systems/EngineSystem.h>

//Android.
#include <android/input.h>
#include <android/log.h>

//Static variable definitions.
android_app *RESTRICT CatalystPlatform::_App = nullptr;
ANativeWindow *RESTRICT CatalystPlatform::_Window = nullptr;

//Globals.
TouchState touchState;

/*
*	Handles android commands.
*/
void HandleCommand(android_app *RESTRICT app, int32 command) NOEXCEPT
{
	switch (command)
	{
		case APP_CMD_INIT_WINDOW:
		{
			CatalystPlatform::_Window = app->window;

			break;
		}

		case APP_CMD_TERM_WINDOW:
		{
			EngineSystem::Instance->Terminate();

			break;
		}
	}
}

/*
*	Polls events.
*/
void PollEvents() NOEXCEPT
{
	int32 events;
	android_poll_source *RESTRICT source;

	if (ALooper_pollAll(0, nullptr, &events, (void**) &source) >= 0)
	{
		if (source && source->id == LOOPER_ID_MAIN)
		{
			source->process(CatalystPlatform::_App, source);
		}
	}
}

/*
*	Initializes the platform.
*/
void CatalystPlatform::Initialize() NOEXCEPT
{
	//Set callbacks.
	_App->onAppCmd = HandleCommand;

	//Need to wait for the window to be set before proceeding.
	while (CatalystPlatform::_Window == nullptr)
	{
		PollEvents();
	}
}

/*
*	Post-nitializes the platform.
*/
void CatalystPlatform::PostInitialize() NOEXCEPT
{

}

/*
*	Pre-updates the platform.
*/
void CatalystPlatform::PreUpdate() NOEXCEPT
{

}

/*
*	Post-updates the platform.
*/
void CatalystPlatform::PostUpdate() NOEXCEPT
{
	//Poll events.
	PollEvents();

	//If the app has received a destroy request, oblige.
	if (_App->destroyRequested != 0)
	{
		EngineSystem::Instance->Terminate();
	}
}

/*
*	Releases the platform.
*/
void CatalystPlatform::Release() NOEXCEPT
{

}

/*
*	Retrieves the current gamepad state.
*/
void CatalystPlatform::GetCurrentGamepadState(const uint8 index, GamepadState *const RESTRICT state) NOEXCEPT
{
	//Not implemented on Android.
}

/*
*	Retrieves the current keyboard state.
*/
void CatalystPlatform::GetCurrentKeyboardState(KeyboardState *const RESTRICT state) NOEXCEPT
{
	//Not implemented on Android.
}

/*
*	Retrieves the current mouse state.
*/
void CatalystPlatform::GetCurrentMouseState(MouseState *const RESTRICT state) NOEXCEPT
{
	//Not implemented on Android.
}

/*
*	Retrieves the current touch state.
*/
void CatalystPlatform::GetCurrentTouchState(TouchState *const RESTRICT state) NOEXCEPT
{
	//Copy the previous X and Y positions.
	state->_PreviousX = state->_CurrentX;
	state->_PreviousY = state->_CurrentY;

    //Keep track if the screen was touched or released this update.
    bool touched{ false };
    bool released{ false };

	AInputEvent *event;

	while (AInputQueue_getEvent(_App->inputQueue, &event) >= 0)
	{
		if (AInputQueue_preDispatchEvent(_App->inputQueue, event))
		{
			continue;
		}

		if (AInputEvent_getSource(event) == AINPUT_SOURCE_TOUCHSCREEN
            && AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
		{
            if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN)
            {
                state->_ButtonState = ButtonState::Pressed;
                state->_CurrentX = AMotionEvent_getRawX(event, 0) / static_cast<float>(ANativeWindow_getWidth(_Window));
                state->_CurrentY = 1.0f - AMotionEvent_getRawY(event, 0) / static_cast<float>(ANativeWindow_getHeight(_Window));

                touched = true;
            }

            else if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_MOVE)
            {
                state->_CurrentX = AMotionEvent_getRawX(event, 0) / static_cast<float>(ANativeWindow_getWidth(_Window));
                state->_CurrentY = 1.0f - AMotionEvent_getRawY(event, 0) / static_cast<float>(ANativeWindow_getHeight(_Window));
            }

            else if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_UP)
            {
                state->_ButtonState = ButtonState::Released;

                released = true;
            }
		}

		AInputQueue_finishEvent(_App->inputQueue, event, 1);
	}

    //Update the button state.
    if (state->_ButtonState == ButtonState::Pressed && !touched)
    {
        state->_ButtonState = ButtonState::PressedHold;
    }

    if (state->_ButtonState == ButtonState::Released && !released)
    {
        state->_ButtonState = ButtonState::ReleasedHold;
    }

	//Calculate the delta positions.
	state->_DeltaX = state->_CurrentX - state->_PreviousX;
	state->_DeltaY = state->_CurrentY - state->_PreviousY;
}

#if !defined(CATALYST_FINAL)
/*
*	Prints a string to output.
*/
void CatalystPlatform::PrintToOutput(const char *const RESTRICT message) NOEXCEPT
{
	__android_log_print(ANDROID_LOG_DEBUG, "Catalyst Engine:", "%s", message);
}
#endif
#endif