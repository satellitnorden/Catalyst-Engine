#if defined(CATALYST_ANDROID)

#pragma once

//Android specific third party includes.
#include <ThirdParty/android_native_app_glue.h>

//Android specific includes.
#include <Platform/Android/AndroidPlatformVulkan.h>

/*
*	Main function
*/
#define MAIN_FUNCTION(implementation)																				\
void HandleCommand(android_app *app, int32 command)																	\
{																													\
	if (command == APP_CMD_INIT_WINDOW)																				\
	{																												\
		PlatformVulkan::surface = app->window;																		\
	}																												\
}																													\
																													\
void android_main(struct android_app *app)																			\
{																													\
	app->onAppCmd = HandleCommand;																					\
																													\
	while (PlatformVulkan::surface == nullptr)																		\
	{																												\
		int32 events;																								\
		android_poll_source *RESTRICT source;																		\
																													\
		if (ALooper_pollAll(0, nullptr, &events, reinterpret_cast<void *RESTRICT *RESTRICT>(&source)) >= 0)			\
		{																											\
			if (source)																								\
			{																										\
				source->process(app, source);																		\
			}																										\
		}																											\
	}																												\
																													\
	implementation;																									\
}																													\

#endif