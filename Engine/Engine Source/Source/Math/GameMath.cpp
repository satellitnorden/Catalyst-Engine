//Header file.
#include <Math/GameMath.h>

//Static variable definitions.
std::random_device GameMath::randomDevice;
std::mt19937 GameMath::randomEngine{ randomDevice() };