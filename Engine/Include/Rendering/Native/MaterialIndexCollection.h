#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

/*
*	Class for storing a (packed) collection of material indices.
*	Can store up to 4 material indices.
*	This needs to be constant size, so depending on the number of indices it currently stores,
*	the maximum indices varies, meaning;
*
*	For 1 index, it can store up to UINT32_MAXIMUM.
*	For 2 indices, it can store up to UINT16_MAXIMUM.
*	For 3-4 indices, it can store up to UINT8_MAXIMUM.
*
*	The collection doesn't explicitly store the number of indices,
*	which is why the number of indices must be passed whenever getting/setting indices.
*/
class MaterialIndexCollection final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE MaterialIndexCollection() NOEXCEPT
	{

	}

	/*
	*	Copy operator overload.
	*/
	FORCE_INLINE void operator=(const MaterialIndexCollection &other) NOEXCEPT
	{
		//Just copy the underlying data.
		Memory::Copy(this, &other, sizeof(MaterialIndexCollection));
	}

	/*
	*	Returns the material index at the given index.
	*/
	FORCE_INLINE NO_DISCARD uint32 GetMaterialIndexAt(const uint8 index, const uint8 number_of_indices) const NOEXCEPT
	{
		switch (number_of_indices)
		{
			case 1:
			{
				return _32BitIndex;
			}

			case 2:
			{
				ASSERT(index < 2, "Index must be under two!");

				return static_cast<uint32>(_16BitIndices[index]);
			}

			case 3:
			case 4:
			{
				ASSERT(index < 4, "Index must be under four!");

				return static_cast<uint32>(_8BitIndices[index]);
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return 0;
			}
		}
	}

	/*
	*	Sets the material index at the given index.
	*/
	FORCE_INLINE void SetMaterialIndexAt(const uint32 material_index, const uint8 index, const uint8 number_of_indices) NOEXCEPT
	{
		switch (number_of_indices)
		{
			case 1:
			{
				ASSERT(index == 0, "Index must be equal to zero!");
				ASSERT(material_index < UINT32_MAXIMUM, "Material index must be under UINT32_MAXIMUM");

				_32BitIndex = material_index;

				break;
			}

			case 2:
			{
				ASSERT(index < 2, "Index must be under two!");
				ASSERT(material_index < UINT16_MAXIMUM, "Material index must be under UINT16_MAXIMUM");

				_16BitIndices[index] = static_cast<uint16>(material_index);

				break;
			}

			case 3:
			case 4:
			{
				ASSERT(index < 4, "Index must be under four!");
				ASSERT(material_index < UINT8_MAXIMUM, "Material index must be under UINT8_MAXIMUM");

				_8BitIndices[index] = static_cast<uint8>(material_index);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");
			}
		}
	}

private:

	//The indices.
	union
	{
		//The 8 bit indices.
		StaticArray<uint8, 4> _8BitIndices;

		//The 16 bit indices.
		StaticArray<uint16, 2> _16BitIndices;

		//The 32 bit index.
		uint32 _32BitIndex;
	};

};

static_assert(sizeof(MaterialIndexCollection) == sizeof(uint32), "Size of MaterialIndexCollection is assumed to be the size of a uint32!");