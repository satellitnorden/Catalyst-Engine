#pragma once

//Engine core.
#include <EngineCore.h>

//STL.
#include <fstream>

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
		fileStream(filePath, std::ios::in | std::ios::binary)
	{

	}

	/*
	*	Reads from this file.
	*/
	void Read(void *RESTRICT output, const uint64 size) NOEXCEPT
	{
		fileStream.read(static_cast<char *RESTRICT>(output), size);
	}

	/*
	*	Closes this file.
	*/
	void Close() NOEXCEPT
	{
		fileStream.close();
	}

private:

	//The underlying file stream.
	std::ifstream fileStream;

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
		fileStream(filePath, std::ios::out | std::ios::binary)
	{

	}

	/*
	*	Writes to this file.
	*/
	void Write(const void *RESTRICT input, const uint64 size) NOEXCEPT
	{
		fileStream.write(static_cast<const char *RESTRICT>(input), size);
	}

	/*
	*	Closes this file.
	*/
	void Close() NOEXCEPT
	{
		fileStream.close();
	}

private:

	//The underlying file stream.
	std::ofstream fileStream;

};