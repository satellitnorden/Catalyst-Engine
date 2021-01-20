//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/HashTable.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Microbenchmarker.
#include <Microbenchmarker.h>

#define TEST_HASH_TABLE (0)
#define TEST_PRE_VS_POST_INCREMENT (1)

#define CURRENT_TEST TEST_PRE_VS_POST_INCREMENT

#if CURRENT_TEST == TEST_HASH_TABLE
//STD.
#include <unordered_map>

#define ELEMENTS (64)

HashTable<uint64, uint64> first_hash_table;
std::unordered_map<uint64, uint64> second_hash_table;

void FillHashTables() NOEXCEPT
{
	for (uint64 i{ 0 }; i < ELEMENTS; ++i)
	{
		
	}

	for (uint64 i{ 0 }; i < ELEMENTS; ++i)
	{
		
	}
}

void Function1() NOEXCEPT
{
	first_hash_table.Add(CatalystRandomMath::RandomIntegerInRange<uint64>(0, UINT64_MAXIMUM), CatalystRandomMath::RandomIntegerInRange<uint64>(0, UINT64_MAXIMUM));
}

void Function2() NOEXCEPT
{
	second_hash_table.emplace(CatalystRandomMath::RandomIntegerInRange<uint64>(0, UINT64_MAXIMUM), CatalystRandomMath::RandomIntegerInRange<uint64>(0, UINT64_MAXIMUM));
}

int main() NOEXCEPT
{
	FillHashTables();

	Microbenchmarker::StartBenchmark(100'000'000, Function1, Function2);

	return EXIT_SUCCESS;
}
#elif CURRENT_TEST == TEST_PRE_VS_POST_INCREMENT

#define ELEMENTS (1'024 * 64)

DynamicArray<uint64> first_array;
DynamicArray<uint64> second_array;

void FillArrays() NOEXCEPT
{
	for (uint64 i{ 0 }; i < ELEMENTS + 1; ++i)
	{
		first_array.Emplace(CatalystRandomMath::RandomIntegerInRange<uint64>(0, UINT64_MAXIMUM));
	}

	for (uint64 i{ 0 }; i < ELEMENTS + 1; ++i)
	{
		second_array.Emplace(CatalystRandomMath::RandomIntegerInRange<uint64>(0, UINT64_MAXIMUM));
	}
}

void Function1() NOEXCEPT
{
	uint64 sum{ 0 };

	for (uint64 i{ 0 }; i < ELEMENTS;)
	{
		sum += first_array[++i];
	}

	((volatile uint64*)sum)[0] = sum;
}

void Function2() NOEXCEPT
{
	uint64 sum{ 0 };

	for (uint64 i{ 0 }; i < ELEMENTS;)
	{
		sum += second_array[i++];
	}

	((volatile uint64*)sum)[0] = sum;
}

int main() NOEXCEPT
{
	FillArrays();

	Microbenchmarker::StartBenchmark(ELEMENTS, Function1, Function2);

	return EXIT_SUCCESS;
}
#endif