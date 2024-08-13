#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Any.h>

class JSON final
{

public:

	//Constants.
	constexpr static uint64 ANY_SIZE{ 32 };

	/*
	*	Iterator class definition.
	*/
	class Iterator final
	{

	public:

		//Constants.
		constexpr static uint64 ITERATOR_ANY_SIZE{ 64 };

		//Friend declaration.
		friend class JSON;

		/*
		*	Dereference operator overload.
		*/
		JSON operator*() NOEXCEPT;

		/*
		*	Increment operator overload.
		*/
		void operator++() NOEXCEPT;

		/*
		*	Equality operator overload.
		*/
		NO_DISCARD bool operator==(const Iterator &other) NOEXCEPT;

		/*
		*	Inequality operator overload.
		*/
		NO_DISCARD bool operator!=(const Iterator &other) NOEXCEPT;

	private:

		//The implementation.
		Any<ITERATOR_ANY_SIZE> _Implementation;

	};

	/*
	*	Default constructor.
	*/
	JSON() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~JSON() NOEXCEPT;

	/*
	*	Assignment operator taking a C string.
	*/
	void operator=(const char *const RESTRICT value) NOEXCEPT;

	/*
	*	Assignment operator taking a bool.
	*/
	void operator=(const bool value) NOEXCEPT;

	/*
	*	Assignment operator taking a float64.
	*/
	void operator=(const float64 value) NOEXCEPT;

	/*
	*	Assignment operator taking an int32.
	*/
	void operator=(const int32 value) NOEXCEPT;

	/*
	*	Assignment operator taking a uint64.
	*/
	void operator=(const uint64 value) NOEXCEPT;

	/*
	*	Subscript operator overload.
	*/
	JSON operator[](const char *const RESTRICT key) NOEXCEPT;

	/*
	*	Begin operator overload.
	*/
	Iterator begin() NOEXCEPT;

	/*
	*	End operator overload.
	*/
	Iterator end() NOEXCEPT;

	/*
	*	Reads from the given file path.
	*/
	void Read(const char *const RESTRICT file_path) NOEXCEPT;

	/*
	*	Writes to the given file path.
	*/
	void Write(const char *const RESTRICT file_path) NOEXCEPT;

	/*
	*	Returns if this JSON object contains the given key.
	*/
	NO_DISCARD bool Contains(const char *const RESTRICT key) NOEXCEPT;

	/*
	*	Emplaces a new object under this JSON object.
	*/
	JSON Emplace() NOEXCEPT;

	/*
	*	Returns the size of this JSON object.
	*/
	NO_DISCARD uint64 Size() const NOEXCEPT;

	/*
	*	Converts this JSON object to a bool.
	*/
	NO_DISCARD bool ToBool() const NOEXCEPT;

	/*
	*	Converts this JSON object to a float32.
	*/
	NO_DISCARD float32 ToFloat32() const NOEXCEPT;

	/*
	*	Converts this JSON object to a float64.
	*/
	NO_DISCARD float64 ToFloat64() const NOEXCEPT;

	/*
	*	Converts this JSON object to an int8.
	*/
	NO_DISCARD int8 ToInt8() const NOEXCEPT;

	/*
	*	Converts this JSON object to an int32.
	*/
	NO_DISCARD int32 ToInt32() const NOEXCEPT;

	/*
	*	Converts this JSON object to a string.
	*/
	NO_DISCARD DynamicString ToString() const NOEXCEPT;

	/*
	*	Converts this JSON object to a uint8.
	*/
	NO_DISCARD uint8 ToUint8() const NOEXCEPT;

	/*
	*	Converts this JSON object to a uint32.
	*/
	NO_DISCARD uint32 ToUint32() const NOEXCEPT;

	/*
	*	Converts this JSON object to a uint64.
	*/
	NO_DISCARD uint64 ToUint64() const NOEXCEPT;

private:

	//The implementation.
	Any<ANY_SIZE> _Implementation;

};