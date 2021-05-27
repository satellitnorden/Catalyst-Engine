#if defined(CATALYST_PLATFORM_ANDROID)
//Header file.
#include <Platform/Android/CatalystPlatformAndroid.h>

//Core.
#include <Core/Essential/CatalystEssential.h>

//Input.
#include <Input/TouchState.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

//Android.
#include <android/input.h>
#include <android/log.h>

//Static variable definitions.
android_app *RESTRICT CatalystPlatform::_App = nullptr;
ANativeWindow *RESTRICT CatalystPlatform::_Window = nullptr;
AndroidTouchState CatalystPlatform::_CurrentAndroidTouchState;

//Catalyst platform Android data.
namespace CatalystPlatformAndroidData
{

	//The screen width.
	uint32 _ScreenWidth;

	//The screen height.
	uint32 _ScreenHeight;
}

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
			CatalystPlatformAndroidData::_ScreenWidth = ANativeWindow_getWidth(CatalystPlatform::_Window);
			CatalystPlatformAndroidData::_ScreenHeight = ANativeWindow_getHeight(CatalystPlatform::_Window);

			break;
		}

		case APP_CMD_TERM_WINDOW:
		{
			//CatalystEngineSystem::Instance->SetShouldTerminate();

			break;
		}
	}
}

/*
*   Handles android input.
*/
int32 HandleInput(android_app *RESTRICT app, AInputEvent *RESTRICT event) NOEXCEPT
{
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN)
		{
			CatalystPlatform::_CurrentAndroidTouchState._IsCurrentlyTouched = true;
		}

		else if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_UP)
		{
			CatalystPlatform::_CurrentAndroidTouchState._IsCurrentlyTouched = false;
		}

		CatalystPlatform::_CurrentAndroidTouchState._CurrentX = AMotionEvent_getX(event, 0) / CatalystPlatformAndroidData::_ScreenWidth;
		CatalystPlatform::_CurrentAndroidTouchState._CurrentY = 1.0f - (AMotionEvent_getY(event, 0) / CatalystPlatformAndroidData::_ScreenHeight);

		return 1;
	}

	else
	{
		return 0;
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
		if (source &&
			(source->id == LOOPER_ID_MAIN
			|| source->id == LOOPER_ID_INPUT))
		{
			source->process(CatalystPlatform::_App, source);
		}
	}
}

/*
*	Updates the Catalyst Platform Android during the POST update phase.
*/
void PostUpdate() NOEXCEPT
{
	//Poll events.
	PollEvents();

	//If the app has received a destroy request, oblige.
	if (CatalystPlatform::_App->destroyRequested != 0)
	{
		//CatalystEngineSystem::Instance->SetShouldTerminate();
	}
}

/*
*	Initializes the platform.
*/
void CatalystPlatform::Initialize() NOEXCEPT
{
	//Set callbacks.
    _App->onAppCmd = HandleCommand;
    _App->onInputEvent = HandleInput;

	//Need to wait for the window to be set before proceeding.
	while (CatalystPlatform::_Window == nullptr)
	{
		PollEvents();
	}

	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT)
	{
	    PostUpdate();
	},
	nullptr,
	UpdatePhase::POST,
	UpdatePhase::PRE,
	true,
	false);
}

/*
*	Returns whether or not the window is in focus.
*/
bool CatalystPlatform::IsWindowInFocus() NOEXCEPT
{
	//Uhm. The Window is always in focus?
	return true;
}

/*
*	Returns the default resolution.
*/
Resolution CatalystPlatform::GetDefaultResolution() NOEXCEPT
{
	//Need the callbacks to be set.
	_App->onAppCmd = HandleCommand;
	_App->onInputEvent = HandleInput;

	//Need to wait for the window to be set before proceeding.
	while (CatalystPlatform::_Window == nullptr)
	{
		PollEvents();
	}

	//Return the default resolution.
	return Resolution(CatalystPlatformAndroidData::_ScreenWidth, CatalystPlatformAndroidData::_ScreenHeight);
}

/*
*	Hides the cursor.
*/
void CatalystPlatform::HideCursor() NOEXCEPT
{
	//Do nothing.
}

/*
*	Shows the cursor.
*/
void CatalystPlatform::ShowCursor() NOEXCEPT
{
	//Do nothing.
}

/*
*	Returns whether or not the cursor is shown.
*/
bool CatalystPlatform::IsCursorShown() NOEXCEPT
{
	//Cursor is never shown on Android. (:
	return false;
}

/*
*	Returns the scroll wheel step and resets it.
*/
int8 CatalystPlatform::GetAndResetScrollWheelStep() NOEXCEPT
{
	return 0;
}

#if defined(CATALYST_CONFIGURATION_DEBUG)
/*
*	Prints a string to output.
*/
void CatalystPlatform::PrintToOutput(const char *const RESTRICT message) NOEXCEPT
{
	__android_log_print(ANDROID_LOG_DEBUG, "Catalyst Engine:", "%s", message);
}
#endif
#endif