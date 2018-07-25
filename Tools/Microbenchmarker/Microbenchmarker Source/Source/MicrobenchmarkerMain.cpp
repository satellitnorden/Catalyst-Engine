//Engine core.
#include <Engine Core/EngineCore.h>
#include <Engine Core/Algorithms/SortingAlgorithms.h>

//Math.
#include <Math/CatalystMath.h>

//Microbenchmarker.
#include <Microbenchmarker.h>

DynamicArray<uint64> first;
DynamicArray<uint64> second;

void Function1() NOEXCEPT
{
	SortingAlgorithms::StandardSort(first.begin(), first.end());
}

void Function2() NOEXCEPT
{
	SortingAlgorithms::InsertionSort(second.begin(), second.end());
}

int main() NOEXCEPT
{
	//Fill the arrays.
	constexpr uint64 arraySize{ 100'000 };

	first.Reserve(arraySize);
	
	for (uint64 i = 0; i < arraySize; ++i)
	{
		first.EmplaceFast(CatalystMath::RandomIntegerInRange<uint64>(UINT64_MINIMUM, UINT64_MAXIMUM));
	}

	second = first;

	Microbenchmarker::StartBenchmark(1, Function1, Function2);

	for (uint64 i = 0; i < arraySize; ++i)
	{
		if (first[i] != second[i])
		{
			PRINT_TO_CONSOLE("Arrays are not the same. ): " << first[i] << " " << second[i]);
		}
	}

	return EXIT_SUCCESS;
}