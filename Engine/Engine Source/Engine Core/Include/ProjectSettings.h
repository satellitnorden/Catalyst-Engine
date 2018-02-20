#pragma once

//Defines the current project.
#define CLAIRVOYANT

#if defined(CLAIRVOYANT)
	#include <Clairvoyant/ClairvoyantSettings.h>
#else
	#include <DefaultProjectSettings.h>
#endif