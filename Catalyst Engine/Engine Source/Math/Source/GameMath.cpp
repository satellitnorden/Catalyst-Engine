//Header file.
#include <GameMath.h>

//Static variable definitions.
std::random_device GameMath::randomDevice;
std::mt19937 GameMath::randomEngine{ randomDevice() };