#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

/*
*	Allocator intended for temporary allocations that is reset at a regular interval,
*	for example per frame.
*	Uses a set of pre-allocated pages of a predetermined size.
*	Will allocate more pages if a request cannot be satisfied.
*	This means that the allocator cannot satsify any requests that are larger than a page size.
*/
template <uint64 PAGE_SIZE>
class FrameAllocator final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE FrameAllocator() NOEXCEPT
	{
		//Allocate the root node.
		_Root = new (Memory::Allocate(sizeof(FrameAllocatorNode))) FrameAllocatorNode();
	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~FrameAllocator() NOEXCEPT
	{
		//Free
		FrameAllocatorNode *RESTRICT next{ _Root };

		do
		{
			FrameAllocatorNode *RESTRICT previous{ next };
			next = next->_Next;
			Memory::Free(previous);
		} while (next);
	}

	/*
	*	Allocates a chunk of memory with the given size.
	*/
	template <typename TYPE>
	FORCE_INLINE RESTRICTED NO_DISCARD TYPE *const RESTRICT Allocate(const uint64 size) NOEXCEPT
	{
		ASSERT(size <= PAGE_SIZE, "Cannot satisfy requests larger than the page size!");

		//Traverse all the nodes and try to find a spot.
		FrameAllocatorNode *RESTRICT current{ _Root };

		for (;;)
		{
			//Calculate the free space that this node has.
			const uint64 free_space{ PAGE_SIZE - current->_CurrentOffset };

			//If this node has enough free space, allocate from this node.
			if (size <= free_space)
			{
				const uint64 memory_offset{ current->_CurrentOffset };

				current->_CurrentOffset += size;

				return static_cast<TYPE *const RESTRICT>(static_cast<void *const RESTRICT>(current->_Data + memory_offset));
			}

			//If not, update the current node with it's next node.
			else
			{
				//Allocate a new next node if the current node doesn't have one.
				if (!current->_Next)
				{
					current->_Next = new (Memory::Allocate(sizeof(FrameAllocatorNode))) FrameAllocatorNode();
				}

				current = current->_Next;
			}
		}

		ASSERT(false, "What happened here?");

		//Should never get here.
		return nullptr;
	}

	/*
	*	Resets this frame allocator.
	*/
	FORCE_INLINE void Reset() NOEXCEPT
	{
		//Traverse all the nodes and reset their current offset.
		FrameAllocatorNode *RESTRICT current{ _Root };

		for (;;)
		{
			current->_CurrentOffset = 0;

			if (current->_Next)
			{
				current = current->_Next;
			}

			else
			{
				break;
			}
		}
	}

private:

	/*
	*	Frame allocator node class definition.
	*/
	class FrameAllocatorNode final
	{

	public:

		//The current offset.
		uint64 _CurrentOffset{ 0 };

		//The data.
		uint8 _Data[PAGE_SIZE];

		//The next node.
		FrameAllocatorNode *RESTRICT _Next{ nullptr };

	};

	//The root node.
	FrameAllocatorNode *RESTRICT _Root;

};