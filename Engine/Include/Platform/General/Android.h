#if defined(CATALYST_ANDROID)

#pragma once

/*
*	Android specific includes.
*/
#include <ThirdParty/android_native_app_glue.h>

/*
*	Main function signature.
*/
#define MAIN void android_main(struct android_app *state)

/*
*	Main return success.
*/
#define MAIN_RETURN_SUCCESS

#endif