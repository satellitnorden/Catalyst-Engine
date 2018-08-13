//Core.
#include <Core/EngineCore.h>

//Math.
#include <Math/CatalystMath.h>

//Microbenchmarker.
#include <Microbenchmarker.h>

DynamicArray<uint64> first;
DynamicArray<uint64> second;

int sum1 = 0;
int sum2 = 0;

#define ARRAY_SIZE 100

void Function1() NOEXCEPT
{
	int myInts[ARRAY_SIZE];

	for (int i = 0; i < ARRAY_SIZE; ++i)
	{
		myInts[i] = CatalystMath::RandomIntegerInRange<int>(0, 100);
	}

	for (int i = 0; i < ARRAY_SIZE; ++i)
	{
		sum1 += myInts[i];
	}
}

void Function2() NOEXCEPT
{
	int *myInts = new int[ARRAY_SIZE];

	for (int i = 0; i < ARRAY_SIZE; ++i)
	{
		myInts[i] = CatalystMath::RandomIntegerInRange<int>(0, 100);
	}

	for (int i = 0; i < ARRAY_SIZE; ++i)
	{
		sum1 += myInts[i];
	}

	delete[] myInts;
}

int main() NOEXCEPT
{
	Microbenchmarker::StartBenchmark(1'000'000, Function1, Function2);

	PRINT_TO_CONSOLE(sum1);
	PRINT_TO_CONSOLE(sum2);

	return EXIT_SUCCESS;
}