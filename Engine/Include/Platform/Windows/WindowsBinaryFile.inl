#pragma once

/*
*	Template specialization for the IN binary file mode.
*/
template<>
class BinaryFile<BinaryFileMode::IN> final
{

public:

	/*
	*	Default constructor - prohibited, must be constructed with the proper arguments.
	*/
	BinaryFile() NOEXCEPT = delete;

	/*
	*	Constructor taking the file path.
	*/
	BinaryFile(const char *const RESTRICT file_path) NOEXCEPT
		:
		_FileStream(file_path, std::ios::in | std::ios::binary | std::ios::ate)
	{
		//Save the size.
		_Size = static_cast<uint64>(_FileStream.tellg());
		_FileStream.seekg(0);
	}

	/*
	*	Bool operator overload.
	*/
	FORCE_INLINE NO_DISCARD operator bool() NOEXCEPT
	{
		return static_cast<bool>(_FileStream);
	}

	/*
	*	Returns the size of this binary file, in bytes.
	*/
	FORCE_INLINE NO_DISCARD uint64 Size() const
	{
		return _Size;
	}

	/*
	*	Returns the current position, in bytes.
	*/
	FORCE_INLINE NO_DISCARD uint64 GetCurrentPosition() NOEXCEPT
	{
		return static_cast<uint64>(_FileStream.tellg());
	}

	/*
	*	Sets the current position, in bytes.
	*/
	FORCE_INLINE void SetCurrentPosition(const uint64 position) NOEXCEPT
	{
		_FileStream.seekg(static_cast<std::streampos>(position));
	}

	/*
	*	Reads from this binary file.
	*/
	template <bool FLIP_ENDIAN = false>
	FORCE_INLINE void Read(void *const RESTRICT output, const uint64 size) NOEXCEPT
	{
		_FileStream.read(static_cast<char *const RESTRICT>(output), size);
	}

	/*
	*	Template specialization for when the Read function should flip the endian.
	*/
	template <>
	FORCE_INLINE void Read<true>(void *const RESTRICT output, const uint64 size) NOEXCEPT
	{
		Read(output, size);
		FlipEndian(output, size);
	}

	/*
	*	Skips a number of bytes.
	*/
	FORCE_INLINE void Skip(const uint64 size) NOEXCEPT
	{
		SetCurrentPosition(GetCurrentPosition() + size);
	}

	/*
	*	Returns if this binary file has reached the end of file.
	*/
	FORCE_INLINE NO_DISCARD bool HasReachedEndOfFile() NOEXCEPT
	{
		return GetCurrentPosition() >= _Size;
	}

	/*
	*	Closes this binary file.
	*/
	FORCE_INLINE void Close() NOEXCEPT
	{
		_FileStream.close();
	}

private:

	//The underlying file stream.
	std::ifstream _FileStream;

	//The size of the file stream.
	uint64 _Size;

};

/*
*	Template specialization for the OUT binary file mode.
*/
template<>
class BinaryFile<BinaryFileMode::OUT> final
{

public:

	/*
	*	Default constructor - prohibited, must be constructed with the proper arguments.
	*/
	BinaryFile() NOEXCEPT = delete;

	/*
	*	Constructor taking the file path.
	*/
	BinaryFile(const char *const RESTRICT file_path) NOEXCEPT
		:
		_FileStream(file_path, std::ios::out | std::ios::binary)
	{

	}

	/*
	*	Writes to this file.
	*/
	FORCE_INLINE void Write(const void *const RESTRICT input, const uint64 size) NOEXCEPT
	{
		_FileStream.write(static_cast<const char *const RESTRICT>(input), size);
	}

	/*
	*	Closes this file.
	*/
	FORCE_INLINE void Close() NOEXCEPT
	{
		_FileStream.close();
	}

private:

	//The underlying file stream.
	std::ofstream _FileStream;

};