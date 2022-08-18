#if defined(CATALYST_PLATFORM_OCULUS_QUEST)
//Header file.
#include <Platform/OculusQuest/CatalystPlatformOculusQuest.h>

//Core.
#include <Core/Essential/CatalystEssential.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

//Third party.
#include <ThirdParty/VrApi/VrApi_Helpers.h>

//Android.
#include <android/input.h>
#include <android/log.h>

//Static variable definitions.
android_app *RESTRICT CatalystPlatform::_App = nullptr;
ovrJava CatalystPlatform::_ovrJava;
ovrMobile *RESTRICT CatalystPlatform::_ovrMobile = nullptr;
bool CatalystPlatform::_IsResumed = false;
ANativeWindow *RESTRICT CatalystPlatform::_NativeWindow = nullptr;

//Catalyst platform Oculus Quest data.
namespace CatalystPlatformOculusQuestData
{

	//Denotes if the VrApi is initialized.
	bool _VrApiInitialized{ false };

}

//Catalyst platform Oculus Quest logic.
namespace CatalystPlatformOculusQuestLogic
{

	/*
	*	Initializes the VrApi.
	*/
	void InitializeVrApi() NOEXCEPT
	{
		if (CatalystPlatformOculusQuestData::_VrApiInitialized)
		{
			return;
		}

		//Set up the ovrJava object.
		CatalystPlatform::_ovrJava.Vm = CatalystPlatform::_App->activity->vm;
		CatalystPlatform::_ovrJava.Vm->AttachCurrentThread(&CatalystPlatform::_ovrJava.Env, nullptr);
		CatalystPlatform::_ovrJava.ActivityObject = CatalystPlatform::_App->activity->clazz;

		//Initialize the VrApi.
		ovrInitParms initialization_parameters{ vrapi_DefaultInitParms(&CatalystPlatform::_ovrJava) };

		initialization_parameters.GraphicsAPI = VRAPI_GRAPHICS_API_VULKAN_1;
		const int32 result{ vrapi_Initialize(&initialization_parameters) };

		if (result != VRAPI_INITIALIZE_SUCCESS)
		{
			ASSERT(false, "Oh no!");

			return;
		}

		CatalystPlatformOculusQuestData::_VrApiInitialized = true;
	}

}

/*
*	Handles android commands.
*/
void HandleCommand(android_app *RESTRICT app, int32 command) NOEXCEPT
{
	switch (command)
	{
		case APP_CMD_RESUME:
		{
			CatalystPlatform::_IsResumed = true;

			break;
		}

		case APP_CMD_PAUSE:
		{
			CatalystPlatform::_IsResumed = false;

			break;
		}

		case APP_CMD_DESTROY:
		{
			CatalystPlatform::_NativeWindow = nullptr;

			break;
		}

		case APP_CMD_INIT_WINDOW:
		{
			CatalystPlatform::_NativeWindow = app->window;

			break;
		}

		case APP_CMD_TERM_WINDOW:
		{
			CatalystPlatform::_NativeWindow = nullptr;

			break;
		}
	}
}

/*
*   Handles android input.
*/
int32 HandleInput(android_app *RESTRICT app, AInputEvent *RESTRICT event) NOEXCEPT
{
	return 0;
}

/*
*	Handls VrApi evenets.
*/
void HandleVrApiEvents() NOEXCEPT
{
	ovrEventDataBuffer eventDataBuffer{ };

	for (;;)
	{
		ovrEventHeader *const RESTRICT event_header{ (ovrEventHeader*)(&eventDataBuffer) };
		ovrResult result{ vrapi_PollEvent(event_header) };

		if (result != ovrSuccess)
		{
			break;
		}
	}
}

/*
*	Polls events.
*/
void PollEvents() NOEXCEPT
{
	for (;;)
	{
		int32 events;
		android_poll_source *RESTRICT source;
		const int32 timeout_milliseconds{ CatalystPlatform::_ovrMobile == nullptr && CatalystPlatform::_App->destroyRequested == 0 ? -1 : 0 };

		if (ALooper_pollAll(timeout_milliseconds, nullptr, &events, (void**) &source) >= 0)
		{
			if (source)
			{
				source->process(CatalystPlatform::_App, source);
			}

			HandleVrApiEvents();
		}

		else
		{
			break;
		}
	}
}

/*
*	Updates the Catalyst platform during the PRE update phase.
*/
void PreUpdate() NOEXCEPT
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
*	Initializes the platform.
*/
void CatalystPlatform::Initialize() NOEXCEPT
{
	//Initialize the VrApi.
	CatalystPlatformOculusQuestLogic::InitializeVrApi();

	//Set callbacks.
    _App->onAppCmd = HandleCommand;
    _App->onInputEvent = HandleInput;

	//Need to wait for the window to be set before proceeding.
	while (CatalystPlatform::_NativeWindow == nullptr)
	{
		PollEvents();
	}

	//Register the updates.
	CatalystEngineSystem::Instance->RegisterUpdate
	(
		[](void* const RESTRICT)
		{
		   PreUpdate();
		},
		nullptr,
		UpdatePhase::PRE,
		UpdatePhase::ENTITY,
		true,
		false
	);
}

/*
*	Updates the platform before everything else.
*/
void CatalystPlatform::PlatformPreUpdate() NOEXCEPT
{

}

/*
*	Terminates the platform.
*/
void CatalystPlatform::Terminate() NOEXCEPT
{
	//Detach the current thread.
	CatalystPlatform::_ovrJava.Vm->DetachCurrentThread();
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
	//Initialize the VrApi.
	CatalystPlatformOculusQuestLogic::InitializeVrApi();

	//Ask the VR api. (:
	return Resolution(static_cast<uint32>(vrapi_GetSystemPropertyInt(&_ovrJava, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_WIDTH)),
					  static_cast<uint32>(vrapi_GetSystemPropertyInt(&_ovrJava, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_HEIGHT)));
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