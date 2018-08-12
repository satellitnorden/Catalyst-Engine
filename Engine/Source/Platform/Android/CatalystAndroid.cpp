#if defined(CATALYST_ANDROID)
//Header file.
#include <Platform/Android/CatalystAndroid.h>

//Core.
#include <Core/EngineCore.h>

//Static variable definitions.
ANativeWindow *RESTRICT CatalystAndroid::window = nullptr;
#endif