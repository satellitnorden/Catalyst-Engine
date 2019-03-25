#pragma once

/*
*	Swaps two elements.
*/
template <typename TYPE>
static void Swap(TYPE *const RESTRICT first, TYPE *const RESTRICT second) NOEXCEPT
{
	TYPE temporary{ std::move(*first) };
	*first = std::move(*second);
	*second = std::move(temporary);
}