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
ANativeWindow *RESTRICT CatalystPlatform::_Window = nullptr;
ovrJava CatalystPlatform::_ovrJava;

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
		case APP_CMD_INIT_WINDOW:
		{
			CatalystPlatform::_Window = app->window;

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
	return 0;
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