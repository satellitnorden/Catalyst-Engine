#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Enumeration covering all I/O modes.
enum class IOMode : uint8
{
	In,
	Out
};

template <IOMode IoMode>
class BinaryFile final
{
	
};

/*
*	Template specialization for the In I/O mode.
*/
template<>
class BinaryFile<IOMode::In> final
{

public:

	/*
	*	Default constructor - prohibited, must be constructed with the proper arguments.
	*/
	BinaryFile() NOEXCEPT = delete;

	/*
	*	Constructor taking the file path.
	*/
	BinaryFile(const char *RESTRICT filePath) NOEXCEPT
		:
		_FileStream(filePath, std::ios::in | std::ios::binary | std::ios::ate)
	{
		//Save the size.
		_Size = static_cast<uint64>(_FileStream.tellg());
		_FileStream.seekg(0);
	}

	/*
	*	Returns the size of the binary file, in bytes.
	*/
	uint64 Size() const
	{
		return _Size;
	}

	/*
	*	Reads from this file.
	*/
	void Read(void *RESTRICT output, const uint64 size) NOEXCEPT
	{
		_FileStream.read(static_cast<char *RESTRICT>(output), size);
	}

	/*
	*	Closes this file.
	*/
	void Close() NOEXCEPT
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
*	Template specialization for the Out I/O mode.
*/
template<>
class BinaryFile<IOMode::Out> final
{

public:

	/*
	*	Default constructor - prohibited, must be constructed with the proper arguments.
	*/
	BinaryFile() NOEXCEPT = delete;

	/*
	*	Constructor taking the file path.
	*/
	BinaryFile(const char *RESTRICT filePath) NOEXCEPT
		:
		_FileStream(filePath, std::ios::out | std::ios::binary)
	{

	}

	/*
	*	Writes to this file.
	*/
	void Write(const void *RESTRICT input, const uint64 size) NOEXCEPT
	{
		_FileStream.write(static_cast<const char *RESTRICT>(input), size);
	}

	/*
	*	Closes this file.
	*/
	void Close() NOEXCEPT
	{
		_FileStream.close();
	}

private:

	//The underlying file stream.
	std::ofstream _FileStream;

};