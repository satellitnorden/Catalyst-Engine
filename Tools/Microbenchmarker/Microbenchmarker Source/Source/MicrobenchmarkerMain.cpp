//Engine core.
#include <Engine Core/EngineCore.h>

//Math.
#include <Math/CatalystMath.h>

//Microbenchmarker.
#include <Microbenchmarker.h>

void Function1() NOEXCEPT
{
	int32 x = 0;

	x += CatalystMath::RandomIntegerInRange<int32>(0, 1'000);

	(void)x;
}

void Function2() NOEXCEPT
{
	int64 x = 0;

	x += CatalystMath::RandomIntegerInRange<int64>(0, 1'000);

	(void)x;
}

int main() NOEXCEPT
{
	Microbenchmarker::StartBenchmark(1'000'000, Function1, Function2);

	return EXIT_SUCCESS;
}