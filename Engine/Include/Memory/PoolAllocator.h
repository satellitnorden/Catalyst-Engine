#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

template <uint64 SIZE>
class PoolAllocator final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE PoolAllocator() NOEXCEPT
		:
		_Root(nullptr)
	{
		
	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~PoolAllocator() NOEXCEPT
	{
		Reset();
	}

	/*
	*	Allocates memory.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD void *const RESTRICT Allocate() NOEXCEPT
	{
		if (!_Root)
		{
			InitializeRoot();
		}

		PoolAllocatorNode *RESTRICT current{ _Root };

		for (;;)
		{
			for (uint8 i = 0; i < 64; ++i)
			{
				if (!TEST_BIT(current->_Active, BIT(i)))
				{
					SET_BIT(current->_Active, BIT(i));

					return static_cast<void *const RESTRICT>(current->_Data + (i * SIZE));
				}
			}

			if (current->_Next)
			{
				current = current->_Next;
			}

			else
			{
				current->_Next = static_cast<PoolAllocatorNode *const RESTRICT>(Memory::Allocate(sizeof(PoolAllocatorNode)));
				current->_Next->_Active = 0;
				current->_Next->_Next = nullptr;

				current = current->_Next;
			}
		}
	}

	/*
	*	Frees memory.
	*/
	FORCE_INLINE void Free(void *const RESTRICT memory) NOEXCEPT
	{
		PoolAllocatorNode *previous{ _Root };
		PoolAllocatorNode *current{ _Root };

		for (;;)
		{
			byte *RESTRICT currentData{ current->_Data };

			for (uint8 i = 0; i < 64; ++i)
			{
				if (currentData == memory)
				{
					CLEAR_BIT(current->_Active, BIT(i));

					return;
				}

				else
				{
					currentData += SIZE;
				}
			}

			previous = current;
			current = current->_Next;
		}
	}

	/*
	*	Resets this pool allocator.
	*/
	FORCE_INLINE void Reset() NOEXCEPT
	{
		if (_Root)
		{
			FreeNode(_Root);
		}
	}

private:

	/*
	*	Pool allocator node definition.
	*/
	class PoolAllocatorNode final
	{

	public:

		//The active bitfield.
		uint64 _Active;

		//The data.
		uint8 _Data[SIZE * 64];

		//Pointer to the next node.
		PoolAllocatorNode *RESTRICT _Next;

	};

	//The root node.
	PoolAllocatorNode *RESTRICT _Root{ nullptr };

	/*
	*	Initializes the root.
	*/
	FORCE_INLINE void InitializeRoot() NOEXCEPT
	{
		//Allocate a new node that will be the root node.
		_Root = static_cast<PoolAllocatorNode *const RESTRICT>(Memory::Allocate(sizeof(PoolAllocatorNode)));

		//Reset the new node.
		_Root->_Active = static_cast<uint64>(0);
		_Root->_Next = nullptr;
	}

	/*
	*	Frees the given node.
	*/
	FORCE_INLINE void FreeNode(PoolAllocatorNode *const RESTRICT node) NOEXCEPT
	{
		if (node->_Next)
		{
			FreeNode(node->_Next);
		}

		Memory::Free(node);
	}

};