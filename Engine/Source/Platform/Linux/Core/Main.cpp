#if defined(CATALYST_PLATFORM_LINUX) && !defined(AUDIO_PLUGIN_CLAP) && !defined(AUDIO_PLUGIN_VST3)
//Header file.
#include <Platform/Core/Main.h>

//TODO: Check if Linux has some more "special" main function, for now, just go with a standard C++ one.
int main()
{
	Platform::Main();

	return 0;
}
#endif