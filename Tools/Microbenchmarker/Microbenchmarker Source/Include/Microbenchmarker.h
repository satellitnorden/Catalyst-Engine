#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

class Microbenchmarker final
{

public:

	static void StartBenchmark(long long iterations, std::function<void()> function1, std::function<void()> function2) NOEXCEPT
	{
		std::thread function1Thread{ &ExecuteFunction, "Function 1", iterations, function1 };
		std::thread function2Thread{ &ExecuteFunction, "Function 2", iterations, function2 };

		function1Thread.join();
		function2Thread.join();
	}

private:

	static void ExecuteFunction(std::string functionName, long long iterations, std::function<void()> function) NOEXCEPT
	{
		std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::high_resolution_clock::now();
		std::vector<long long> durations;
		durations.reserve(static_cast<size_t>(iterations));
		unsigned short int currentPercent = 0;
		long long percentCounter = 0;
		const long long iterationsPerPercent = iterations / 100;

		for (long long i = 0; i < iterations; ++i)
		{
			function();

			++percentCounter;

			if (percentCounter >= iterationsPerPercent)
			{
				++currentPercent;
				percentCounter = 0;

				std::cout << functionName << ": " << currentPercent << "%" << std::endl;
			}

			durations.emplace_back(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - startTime).count());
			startTime = std::chrono::high_resolution_clock::now();
		}

		long long totalTime = 0;

		for (auto& duration : durations)
		{
			totalTime += duration;
		}

		long long averageTime = totalTime / iterations;

		std::cout << "Average duration for " << functionName << " was " << FormatNumber(averageTime) << " nanoseconds." << std::endl;
	}

	static std::string FormatNumber(const long long &number) NOEXCEPT
	{
		std::string formattedString = std::to_string(number);

		uint8 countDown{ 3 };

		for (size_t i = formattedString.size(); i > 0; --i)
		{
			--countDown;

			if (countDown == 0 && i > 1)
			{
				formattedString.insert(formattedString.begin() + i - 1, ',');
				countDown = 3;
			}
		}

		return formattedString;
	}
};