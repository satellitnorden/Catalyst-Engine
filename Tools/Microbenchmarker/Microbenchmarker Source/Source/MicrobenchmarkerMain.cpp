//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/HashTable.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Microbenchmarker.
#include <Microbenchmarker.h>

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