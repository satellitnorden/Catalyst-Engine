#if defined(CATALYST_PLATFORM_LINUX)
//Header file.
#include <Platform/Core/Main.h>

//TODO: Check if Linux has some more "special" main function, for now, just go with a standard C++ one.
int main()
{
	Main();

	return 0;
}
#endif