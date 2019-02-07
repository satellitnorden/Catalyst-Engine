#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class SortingAlgorithms
{

public:

	//Type aliases.
	template <typename Type>
	using ComparisonFunction = bool(*)(const void *const RESTRICT userData, const Type *const RESTRICT firstElement, const Type *const RESTRICT secondElement);

	/*
	*	Bubble sort.
	*
	*	Best case time complexity: O(n). Best case occurs when array is already sorted.
	*	Worst case time complexity: O(n^2). Worst case occurs when array is already reverse sorted.
	*	Sorting in place: Yes.
	*	Stable: Yes.
	*	Uses: Useful when an array is almost sorted as it almost only takes linear complexity.
	*/
	template <typename Type>
	static void BubbleSort(Type *const RESTRICT begin, Type *const RESTRICT end, const void *const RESTRICT userData, ComparisonFunction<Type> comparisonFunction = [](const void *const RESTRICT userData, const Type *const RESTRICT firstElement, const Type *const RESTRICT secondElement) { return *firstElement < *secondElement; }) NOEXCEPT
	{
		bool swapped;

		do
		{
			swapped = false;

			for (Type *RESTRICT iterator{ begin }; iterator != end - 1; ++iterator)
			{
				if (comparisonFunction(userData, iterator + 1, iterator))
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
	template <typename Type>
	static void InsertionSort(Type *const RESTRICT begin, Type *const RESTRICT end, const void *const RESTRICT userData, ComparisonFunction<Type> comparisonFunction = [](const void *const RESTRICT userData, const Type *const RESTRICT firstElement, const Type *const RESTRICT secondElement) { return *firstElement < *secondElement; }) NOEXCEPT
	{
		for (Type *RESTRICT iterator{ begin + 1 }; iterator != end; ++iterator)
		{
			Type *RESTRICT reverseIterator{ iterator };

			while (reverseIterator != begin && comparisonFunction(userData, reverseIterator, reverseIterator - 1))
			{
				Swap(reverseIterator, reverseIterator - 1);

				--reverseIterator;
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
	template <typename Type>
	static void SelectionSort(Type *const RESTRICT begin, Type *const RESTRICT end, const void *const RESTRICT userData, ComparisonFunction<Type> comparisonFunction = [](const void *const RESTRICT userData, const Type *const RESTRICT firstElement, const Type *const RESTRICT secondElement) { return *firstElement < *secondElement; }) NOEXCEPT
	{
		for (Type *RESTRICT iterator{ begin }; iterator != end; ++iterator)
		{
			Type *RESTRICT selected{ iterator };

			for (Type *RESTRICT secondIterator{ iterator + 1 }; secondIterator != end; ++secondIterator)
			{
				if (comparisonFunction(userData, secondIterator, selected))
				{
					selected = secondIterator;
				}
			}

			if (iterator != selected)
			{
				Swap(iterator, selected);
			}
		}
	}

private:

	/*
	*	Swaps two elements.
	*/
	template <typename Type>
	static void Swap(Type *const RESTRICT firstElement, Type *const RESTRICT secondElement) NOEXCEPT
	{
		Type temporary{ std::move(*firstElement) };
		*firstElement = std::move(*secondElement);
		*secondElement = std::move(temporary);
	}

};