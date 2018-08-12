#if defined(CATALYST_ANDROID)

#pragma once

//Android specific third party includes.
#include <ThirdParty/android_native_app_glue.h>

//Android specific includes.
#include <Platform/Android/AndroidPlatformVulkan.h>

/*
*	Main function.
*/
#define MAIN_FUNCTION(implementation)																		\
void HandleCommand(android_app *app, int32 command)															\
{																											\
	switch (command)																						\
	{																										\
		case APP_CMD_INIT_WINDOW:																			\
		{																									\
			CatalystAndroid::window = app->window;															\
																											\
			break;																							\
		}																									\
																											\
		case APP_CMD_TERM_WINDOW:																			\
		{																									\
			EngineSystem::Instance->Terminate();															\
																											\
			break;																							\
		}																									\
	}																										\
}																											\
																											\
void android_main(struct android_app *app)																	\
{																											\
	app->onAppCmd = HandleCommand;																			\
																											\
	while (CatalystAndroid::window == nullptr)																\
	{																										\
		int32 events;																						\
		android_poll_source *RESTRICT source;																\
																											\
		if (ALooper_pollAll(0, nullptr, &events, reinterpret_cast<void *RESTRICT *RESTRICT>(&source)) >= 0)	\
		{																									\
			if (source)																						\
			{																								\
				source->process(app, source);																\
			}																								\
		}																									\
	}																										\
																											\
	implementation;																							\
}																											\

/*
*	Main loop.
*/
#define MAIN_LOOP(implementation)																		\
while (!EngineSystem::Instance->ShouldTerminate())														\
{																										\
	int32 events;																						\
	android_poll_source *RESTRICT source;																\
																										\
	if (ALooper_pollAll(0, nullptr, &events, reinterpret_cast<void *RESTRICT *RESTRICT>(&source)) >= 0)	\
	{																									\
		if (source)																						\
		{																								\
			source->process(app, source);																\
		}																								\
	}																									\
																										\
	if (app->destroyRequested != 0)																		\
	{																									\
		EngineSystem::Instance->Terminate();															\
	}																									\
																										\
	else																								\
	{																									\
		implementation;																					\
	}																									\
}																										\

/*
*	Catalyst Android.
*/
class CatalystAndroid final
{

public:

	//The native window handle.
	static ANativeWindow *RESTRICT window;

};

#endif