//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Microbenchmarker.
#include <Microbenchmarker.h>

uint64 sum_1{ 0 };
uint64 sum_2{ 0 };

void Test_1_1()
{
	sum_1 += 52;
}

void Test_1_2()
{
	sum_1 += 64;
}

void Test_1_3()
{
	sum_1 += 34;
}

void Test_1_4()
{
	sum_1 += 88;
}

void Test_1_5()
{
	sum_1 += 75;
}

void Test_1_6()
{
	sum_1 += 37;
}

void Test_1_7()
{
	sum_1 += 94;
}

void Test_1_8()
{
	sum_1 += 25;
}

void Test_1_9()
{
	sum_1 += 34;
}

void Test_1_10()
{
	sum_1 += 76;
}

void Test_2_1()
{
	sum_2 += 52;
}

void Test_2_2()
{
	sum_2 += 64;
}

void Test_2_3()
{
	sum_2 += 34;
}

void Test_2_4()
{
	sum_2 += 88;
}

void Test_2_5()
{
	sum_2 += 75;
}

void Test_2_6()
{
	sum_2 += 37;
}

void Test_2_7()
{
	sum_2 += 94;
}

void Test_2_8()
{
	sum_2 += 25;
}

void Test_2_9()
{
	sum_2 += 34;
}

void Test_2_10()
{
	sum_2 += 76;
}

void Function1() NOEXCEPT
{
	switch (CatalystRandomMath::RandomIntegerInRange<uint8>(0, 9))
	{
		case 0:
		{
			Test_1_1();

			break;
		}

		case 1:
		{
			Test_1_2();

			break;
		}

		case 2:
		{
			Test_1_3();

			break;
		}

		case 3:
		{
			Test_1_4();

			break;
		}

		case 4:
		{
			Test_1_5();

			break;
		}

		case 5:
		{
			Test_1_6();

			break;
		}

		case 6:
		{
			Test_1_7();

			break;
		}

		case 7:
		{
			Test_1_8();

			break;
		}

		case 8:
		{
			Test_1_9();

			break;
		}

		case 9:
		{
			Test_1_10();

			break;
		}
	}
}

void Function2() NOEXCEPT
{
	constexpr StaticArray<void(*)(), 10> FUNCTIONS
	{
		Test_2_1,
		Test_2_2,
		Test_2_3,
		Test_2_4,
		Test_2_5,
		Test_2_6,
		Test_2_7,
		Test_2_8,
		Test_2_9,
		Test_2_10
	};

	FUNCTIONS[CatalystRandomMath::RandomIntegerInRange<uint8>(0, 9)]();
}

int main() NOEXCEPT
{
	Microbenchmarker::StartBenchmark(1'000'000'000, Function1, Function2);

	PRINT_TO_OUTPUT(sum_1);
	PRINT_TO_OUTPUT(sum_2);

	return EXIT_SUCCESS;
}