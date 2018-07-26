#if defined(CATALYST_ANDROID)

#pragma once

//JNI.
#include <jni.h>

/*
*	Main function signature.
*/
#define MAIN JNICALL Java_com_chrysalisinteractive_catalystenginetemplate_Main_main(JNIEnv *environment, jobject)

#endif