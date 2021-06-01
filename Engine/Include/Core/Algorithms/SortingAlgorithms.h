#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class SortingAlgorithms
{

public:

	//Type aliases.
	template <typename TYPE>
	using ComparisonFunction = bool(*)(const void *const RESTRICT user_data, const TYPE *const RESTRICT first, const TYPE *const RESTRICT second);

	/*
	*	Bubble sort.
	*
	*	Best case time complexity: O(n). Best case occurs when array is already sorted.
	*	Worst case time complexity: O(n^2). Worst case occurs when array is already reverse sorted.
	*	Sorting in place: Yes.
	*	Stable: Yes.
	*	Uses: Useful when an array is almost sorted as it almost only takes linear complexity.
	*/
	template <typename TYPE>
	static void BubbleSort(TYPE *const RESTRICT begin, TYPE *const RESTRICT end, const void *const RESTRICT user_data, ComparisonFunction<TYPE> comparison_function = [](const void *const RESTRICT user_data, const TYPE *const RESTRICT first, const TYPE *const RESTRICT second) { return *first < *second; }) NOEXCEPT
	{
		bool swapped;

		do
		{
			swapped = false;

			for (TYPE *RESTRICT iterator{ begin }; iterator != end - 1; ++iterator)
			{
				if (comparison_function(user_data, iterator + 1, iterator))
				{
					Swap(iterator, iterator + 1);

					swapped = true;
				}
			}
		} while (swapped);
	}

	/*
	*	Insertion sort.
	*
	*	Best case time complexity: O(n). Best case occurs when array is already sorted.
	*	Worst case time complexity: O(n^2). Worst case occurs when array is already reverse sorted.
	*	Sorting in place: Yes.
	*	Stable: Yes.
	*	Uses: Useful when number of elements is small or when the array is almost sorted.
	*/
	template <typename TYPE>
	static void InsertionSort(TYPE *const RESTRICT begin, TYPE *const RESTRICT end, const void *const RESTRICT user_data, ComparisonFunction<TYPE> comparison_function = [](const void *const RESTRICT user_data, const TYPE *const RESTRICT first, const TYPE *const RESTRICT second) { return *first < *second; }) NOEXCEPT
	{
		for (TYPE *RESTRICT iterator{ begin + 1 }; iterator != end; ++iterator)
		{
			TYPE *RESTRICT reverse_iterator{ iterator };

			while (reverse_iterator != begin && comparison_function(user_data, reverse_iterator, reverse_iterator - 1))
			{
				Swap(reverse_iterator, reverse_iterator - 1);

				--reverse_iterator;
			}
		}
	}

	/*
	*	Selection sort.
	*
	*	Time complexity: O(n^2). Default case always occurs as there are two nested loops.
	*	Sorting in place: Yes.
	*	Stable: No.
	*	Uses: Useful when memory writes are a costly process as selection sort never makes more than N swaps.
	*/
	template <typename TYPE>
	static void SelectionSort(TYPE *const RESTRICT begin, TYPE *const RESTRICT end, const void *const RESTRICT user_data, ComparisonFunction<TYPE> comparison_function = [](const void *const RESTRICT user_data, const TYPE *const RESTRICT first, const TYPE *const RESTRICT second) { return *first < *second; }) NOEXCEPT
	{
		for (TYPE *RESTRICT iterator{ begin }; iterator != end; ++iterator)
		{
			TYPE *RESTRICT selected{ iterator };

			for (TYPE *RESTRICT second_iterator{ iterator + 1 }; second_iterator != end; ++second_iterator)
			{
				if (comparison_function(user_data, second_iterator, selected))
				{
					selected = second_iterator;
				}
			}

			if (iterator != selected)
			{
				Swap(iterator, selected);
			}
		}
	}

	/*
	*	Standard sort.
	*
	*	Time complexity: At most O(n log n).
	*	Sorting in place: Yes.
	*	Stable: No.
	*	Uses: General sorting algorithm suited for most cases.
	*/
	template <typename TYPE>
	static void StandardSort(TYPE *const RESTRICT begin, TYPE *const RESTRICT end, const void *const RESTRICT user_data, ComparisonFunction<TYPE> comparison_function = [](const void *const RESTRICT user_data, const TYPE *const RESTRICT first, const TYPE *const RESTRICT second) { return *first < *second; }) NOEXCEPT
	{
		class Comparator final
		{

		public:

			/*
			*	Constructor taking all values as arguments.
			*/
			FORCE_INLINE Comparator(const void *const RESTRICT initial_user_data, const ComparisonFunction<TYPE> initial_comparison_function) NOEXCEPT
				:
				_UserData(initial_user_data),
				_ComparisonFunction(initial_comparison_function)
			{

			}

			/*
			*	Bool operator overload.
			*/
			FORCE_INLINE bool operator()(TYPE *const RESTRICT first, TYPE *const RESTRICT second) NOEXCEPT
			{
				return _ComparisonFunction(_UserData, first, second);
			}

		private:

			//The user data.
			const void *RESTRICT _UserData;

			//The comparison function.
			ComparisonFunction<TYPE> _ComparisonFunction;

		};

		std::sort(begin, end, Comparator(user_data, comparison_function));
	}

};