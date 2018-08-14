#if defined(CATALYST_ANDROID)
//Header file.
#include <Platform/Android/CatalystAndroid.h>

//Core.
#include <Core/EngineCore.h>

//Systems.
#include <Systems/EngineSystem.h>

//Static variable definitions.
android_app *RESTRICT CatalystPlatform::app = nullptr;
ANativeWindow *RESTRICT CatalystPlatform::window = nullptr;

/*
*	Handles android commands.
*/
void HandleCommand(android_app *RESTRICT app, int32 command) NOEXCEPT
{
	switch (command)
	{
		case APP_CMD_INIT_WINDOW:
		{
			CatalystPlatform::window = app->window;

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

	if (ALooper_pollAll(0, nullptr, &events, reinterpret_cast<void *RESTRICT *RESTRICT>(&source)) >= 0)
	{
		if (source)
		{
			source->process(CatalystPlatform::app, source);
		}
	}
}

/*
*	Initializes the platform.
*/
void CatalystPlatform::Initialize() NOEXCEPT
{
	//The the handle command callback.
	app->onAppCmd = HandleCommand;

	//Need to wait for the window to be set before proceeding.
	while (CatalystPlatform::window == nullptr)
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
	if (app->destroyRequested != 0)
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
#endif