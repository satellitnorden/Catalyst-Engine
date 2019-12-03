#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class Microbenchmarker final
{

public:

	static void StartBenchmark(const uint64 iterations, std::function<void()> function_1, std::function<void()> function_2) NOEXCEPT
	{
		std::thread thread_1{ &ExecuteFunction, "Function 1", iterations, function_1 };
		std::thread thread_2{ &ExecuteFunction, "Function 2", iterations, function_2 };

		thread_1.join();
		thread_2.join();
	}

private:

	static void ExecuteFunction(std::string function_name, const uint64 iterations, std::function<void()> function) NOEXCEPT
	{
		std::chrono::time_point<std::chrono::steady_clock> start_time{ std::chrono::high_resolution_clock::now() };

		uint64 total_duration{ 0 };

		uint32 current_percent{ 0 };
		uint64 percent_counter{ 0 };
		const uint64 iterations_per_percent{ iterations / 100 };

		for (uint64 i{ 0 }; i < iterations; ++i)
		{
			function();

			++percent_counter;

			if (percent_counter >= iterations_per_percent)
			{
				++current_percent;
				percent_counter = 0;

				PRINT_TO_OUTPUT(function_name << ": " << current_percent << "%");
			}

			total_duration += std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start_time).count();

			start_time = std::chrono::high_resolution_clock::now();
		}

		const uint64 average_duration{ total_duration / iterations };

		PRINT_TO_OUTPUT("Average duration for " << function_name << " was " << FormatNumber(average_duration) << " nanoseconds.");
	}

	static std::string FormatNumber(const uint64 number) NOEXCEPT
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