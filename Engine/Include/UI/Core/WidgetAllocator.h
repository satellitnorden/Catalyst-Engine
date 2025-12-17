#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/HashTable.h>

//UI.
#include <UI/Core/Container.h>
#include <UI/Core/Identifier.h>

namespace UI
{

	/*
	*	Simple allocator for widget memory.
	*	Each widget is tied to a unique identifier, and kept around in case any widget keeps state.
	*	Keeps track of alive widgets this frame, so dead widgets can be cleaned up at the end of the frame.
	*/
	class WidgetAllocator final
	{

	public:

		/*
		*	Allocates a widget (or returns an active one if it exists.
		*/
		template <typename TYPE>
		FORCE_INLINE TYPE *const Allocate(const UI::Identifier identifier, UI::Container *const RESTRICT parent) NOEXCEPT
		{
			WidgetAllocation *const RESTRICT allocation{ _Allocations.Find(identifier) };

			if (allocation)
			{
				allocation->_Alive = true;

				return static_cast<TYPE *const RESTRICT>(allocation->_Data);
			}

			else
			{
				WidgetAllocation new_allocation;

				new_allocation._Alive = true;
				new_allocation._Data = new TYPE(parent);

				_Allocations.Add(identifier, new_allocation);

				return static_cast<TYPE *const RESTRICT>(new_allocation._Data);
			}
		}

		/*
		*	Prepares this widget allocator for the coming frame.
		*/
		FORCE_INLINE void Prepare() NOEXCEPT
		{
			for (WidgetAllocation &allocation : _Allocations.ValueIterator())
			{
				allocation._Alive = false;
			}
		}

		/*
		*	Cleans this widget allocator after the frame.
		*/
		FORCE_INLINE void Clean() NOEXCEPT
		{
			for (uint64 i{ 0 }; i < _Allocations.Size();)
			{
				if (!_Allocations.ValueAt(i)._Alive)
				{
					_Allocations.Remove(_Allocations.KeyAt(i));
				}

				else
				{
					++i;
				}
			}
		}

	private:

		/*
		*	Widget allocation class definition.
		*/
		class WidgetAllocation final
		{

		public:

			//Denotes whether or not this widget is alive.
			bool _Alive;

			//The data.
			void *RESTRICT _Data;

		};

		//The allocations.
		HashTable<UI::Identifier, WidgetAllocation> _Allocations;

	};

}