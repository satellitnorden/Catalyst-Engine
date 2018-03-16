//Header file.
#include <Math/CatalystMath.h>

//Static variable definitions.
std::random_device CatalystMath::randomDevice;
std::mt19937 CatalystMath::randomEngine{ randomDevice() };