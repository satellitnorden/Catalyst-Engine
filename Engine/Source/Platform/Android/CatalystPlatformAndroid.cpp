#if defined(CATALYST_PLATFORM_ANDROID)
//Header file.
#include <Platform/Android/CatalystPlatformAndroid.h>

//Core.
#include <Core/Essential/CatalystEssential.h>

//Input.
#include <Input/TouchState.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/RenderingSystem.h>

//Android.
#include <android/input.h>
#include <android/log.h>

//Static variable definitions.
android_app *RESTRICT CatalystPlatform::_App = nullptr;
ANativeWindow *RESTRICT CatalystPlatform::_Window = nullptr;
StaticArray<AndroidTouchState, InputConstants::MAXIMUM_NUMBER_OF_TOUCH_FINGERS> CatalystPlatform::_CurrentAndroidTouchStates;

//Catalyst platform Android data.
namespace CatalystPlatformAndroidData
{

	//The screen width.
	uint32 _ScreenWidth;

	//The screen height.
	uint32 _ScreenHeight;
}

//Catalyst platform Android logic.
namespace CatalystPlatformAndroidLogic
{

    /*
    *   Calls a void JNI method.
    */
    FORCE_INLINE void CallVoidJNIMethod(const char *const RESTRICT method_name) NOEXCEPT
    {
        //Retrieve the native activity.
        ANativeActivity *native_activity{ CatalystPlatform::_App->activity };

        //Retrieve the Java VM.
        JavaVM *JVM{ native_activity->vm };

        //Retrieve the environment.
        JNIEnv *environment{ nullptr };

        JVM->GetEnv((void**)&environment, JNI_VERSION_1_6);

        //Attach the current thread.
        const jint result{ JVM->AttachCurrentThread(&environment, nullptr) };

        if (result == JNI_ERR)
        {
            PRINT_TO_OUTPUT("Failed to do JNI stuff!");

            return;
        }

        //Retrieve the Java class.
        jclass java_class{ environment->GetObjectClass(native_activity->clazz) };

        //Retrieve the Java method ID.
        jmethodID java_method_id{ environment->GetMethodID(java_class, method_name, "()V") };

        //Call the method!
        environment->CallVoidMethod(native_activity->clazz, java_method_id);

        //Detach the current thread.
        JVM->DetachCurrentThread();
    }

}

/*
*	Handles android commands.
*/
void HandleCommand(android_app *RESTRICT app, int32 command) NOEXCEPT
{
	switch (command)
	{
		case APP_CMD_INPUT_CHANGED:
		{
			//Do nothing. Right now. (:

			break;
		}

		case APP_CMD_INIT_WINDOW:
		{
			CatalystPlatform::_Window = app->window;
			CatalystPlatformAndroidData::_ScreenWidth = ANativeWindow_getWidth(CatalystPlatform::_Window);
			CatalystPlatformAndroidData::_ScreenHeight = ANativeWindow_getHeight(CatalystPlatform::_Window);

			RenderingSystem::Instance->OnRenderingPlatformEvent(RenderingPlatformEvent::SURFACE_GAINED);

			break;
		}

		case APP_CMD_TERM_WINDOW:
		{
			RenderingSystem::Instance->OnRenderingPlatformEvent(RenderingPlatformEvent::SURFACE_LOST);

			break;
		}

		case APP_CMD_WINDOW_RESIZED:
		{
			//Do nothing. Right now. (:

			break;
		}

		case APP_CMD_WINDOW_REDRAW_NEEDED:
		{
			//Do nothing. Right now. (:

			break;
		}

		case APP_CMD_CONTENT_RECT_CHANGED:
		{
			//Do nothing. Right now. (:

			break;
		}

		case APP_CMD_GAINED_FOCUS:
		{
			//Unpause the engine.
			CatalystEngineSystem::Instance->UnpauseEngine();

			break;
		}

		case APP_CMD_LOST_FOCUS:
		{
			//Pause the engine.
			CatalystEngineSystem::Instance->PauseEngine();

			break;
		}

		case APP_CMD_CONFIG_CHANGED:
		{
			//Do nothing. Right now. (:

			break;
		}

		case APP_CMD_LOW_MEMORY:
		{
			PRINT_TO_OUTPUT("Warning! Low memory!");

			break;
		}

		case APP_CMD_START:
		{
			//Unpause the engine.
			CatalystEngineSystem::Instance->UnpauseEngine();

			break;
		}

		case APP_CMD_RESUME:
		{
			//Unpause the engine.
			CatalystEngineSystem::Instance->UnpauseEngine();

			break;
		}

		case APP_CMD_SAVE_STATE:
		{
			//Do nothing. Right now. (:

			break;
		}

		case APP_CMD_PAUSE:
		{
			//Pause the engine.
			CatalystEngineSystem::Instance->PauseEngine();

			break;
		}

		case APP_CMD_STOP:
		{
			//Pause the engine.
			CatalystEngineSystem::Instance->PauseEngine();

			break;
		}

		case APP_CMD_DESTROY:
		{
			CatalystEngineSystem::Instance->SetShouldTerminate();

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
*   Handles android input.
*/
int32 HandleInput(android_app *RESTRICT app, AInputEvent *RESTRICT event) NOEXCEPT
{
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
        //Retrieve the pointer index.
        const int32 pointer_index{ (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT };

        if (pointer_index >= InputConstants::MAXIMUM_NUMBER_OF_TOUCH_FINGERS)
        {
            ASSERT(false, "This was unexpected!");

            return 0;
        }

		if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN)
		{
			CatalystPlatform::_CurrentAndroidTouchStates[pointer_index]._IsCurrentlyTouched = true;
		}

		else if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_UP)
		{
			CatalystPlatform::_CurrentAndroidTouchStates[pointer_index]._IsCurrentlyTouched = false;
		}

		CatalystPlatform::_CurrentAndroidTouchStates[pointer_index]._CurrentX = AMotionEvent_getX(event, pointer_index) / CatalystPlatformAndroidData::_ScreenWidth;
		CatalystPlatform::_CurrentAndroidTouchStates[pointer_index]._CurrentY = 1.0f - (AMotionEvent_getY(event, pointer_index) / CatalystPlatformAndroidData::_ScreenHeight);

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

	while (ALooper_pollAll(CatalystEngineSystem::Instance->IsEnginePaused() ? -1 : 0, nullptr, &events, (void**) &source) >= 0)
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
}

/*
*	Updates the platform before everything else.
*/
void CatalystPlatform::PlatformPreUpdate() NOEXCEPT
{
    //Poll events.
    PollEvents();

    //If the app has received a destroy request, oblige.
    if (CatalystPlatform::_App->destroyRequested != 0)
    {
        CatalystEngineSystem::Instance->SetShouldTerminate();
    }
}

/*
*	Terminates the platform.
*/
void CatalystPlatform::Terminate() NOEXCEPT
{

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

/*
*	Retrieves the user name.
*/
bool CatalystPlatform::RetrieveUserName(DynamicString *const RESTRICT output) NOEXCEPT
{
	return false;
}

void CatalystPlatform::ShowBannerAd() NOEXCEPT
{
	//Call the method.
	CatalystPlatformAndroidLogic::CallVoidJNIMethod("ShowBannerAd");
}

/*
*	Shows an interstitial ad.
*/
void CatalystPlatform::ShowInterstitialAd() NOEXCEPT
{
	//Call the method.
	CatalystPlatformAndroidLogic::CallVoidJNIMethod("ShowInterstitialAd");
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