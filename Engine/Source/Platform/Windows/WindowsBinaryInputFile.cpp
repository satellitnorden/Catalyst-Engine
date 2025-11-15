#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <File/Core/BinaryInputFile.h>

//Third party.
#include <ThirdParty/mio/mio.hpp>

/*
*	Windows binary input file implementation class definition.
*/
class WindowsBinaryInputFileImplementation final
{

public:

	//The map.
	mio::ummap_source _Map;

	//The file path.
	const char *RESTRICT _FilePath;

	//The size of the file stream.
	uint64 _Size;

	//The data.
	const byte* RESTRICT _Data;

	//The current position.
	uint64 _CurrentPosition;

};

static_assert(sizeof(WindowsBinaryInputFileImplementation) <= BinaryInputFile::ANY_SIZE, "Increase any size!");

/*
*	Constructor taking the file path.
*/
BinaryInputFile::BinaryInputFile(const char* const RESTRICT file_path) NOEXCEPT
{
	//Cache the implementation.
	WindowsBinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<WindowsBinaryInputFileImplementation>() };

	//Construct the implementation.
	new (implementation) WindowsBinaryInputFileImplementation();

	//Map the file.
	std::error_code error;
	implementation->_Map.map(std::string(file_path), 0, mio::map_entire_file, error);

	ASSERT(!error, error.message().c_str());

	//Set the file path.
	implementation->_FilePath = file_path;

	//Retrieve the size.
	implementation->_Size = static_cast<uint64>(implementation->_Map.mapped_length());

	//Retrieve the data.
	implementation->_Data = implementation->_Map.data();

	//Set the current position.
	implementation->_CurrentPosition = 0;
}

/*
*	Bool operator overload.
*/
NO_DISCARD BinaryInputFile::operator bool() NOEXCEPT
{
	//Cache the implementation.
	WindowsBinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<WindowsBinaryInputFileImplementation>() };

	return implementation->_Map.is_mapped();
}

/*
*	Returns the file path.
*/
RESTRICTED NO_DISCARD const char *const RESTRICT BinaryInputFile::GetFilePath() const NOEXCEPT
{
	//Cache the implementation.
	const WindowsBinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<WindowsBinaryInputFileImplementation>() };

	return implementation->_FilePath;
}

/*
*	Returns the size of this binary input file, in bytes.
*/
NO_DISCARD uint64 BinaryInputFile::Size() const NOEXCEPT
{
	//Cache the implementation.
	const WindowsBinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<WindowsBinaryInputFileImplementation>() };

	return implementation->_Size;
}

/*
*	Reads from this binary file.
*/
void BinaryInputFile::Read(void *const RESTRICT output, const uint64 size, const File::Endian endian) NOEXCEPT
{
	//Cache the implementation.
	WindowsBinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<WindowsBinaryInputFileImplementation>() };

	//Read.
	Memory::Copy(output, &implementation->_Data[implementation->_CurrentPosition], size);
	implementation->_CurrentPosition += size;

	if (endian == File::Endian::BIG)
	{
		FlipEndian(output, size);
	}
}

/*
*	Returns the current position, in bytes.
*/
NO_DISCARD uint64 BinaryInputFile::GetCurrentPosition() NOEXCEPT
{
	//Cache the implementation.
	const WindowsBinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<WindowsBinaryInputFileImplementation>() };

	return implementation->_CurrentPosition;
}

/*
*	Sets the current position, in bytes.
*/
void BinaryInputFile::SetCurrentPosition(const uint64 position) NOEXCEPT
{
	//Cache the implementation.
	WindowsBinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<WindowsBinaryInputFileImplementation>() };

	implementation->_CurrentPosition = position;
}

/*
*	Skips a number of bytes.
*/
void BinaryInputFile::Skip(const uint64 size) NOEXCEPT
{
	SetCurrentPosition(GetCurrentPosition() + size);
}

/*
*	Returns if this binary file has reached the end of file.
*/
NO_DISCARD bool BinaryInputFile::HasReachedEndOfFile() NOEXCEPT
{
	return GetCurrentPosition() >= Size();
}

/*
*	Closes this binary file.
*/
void BinaryInputFile::Close() NOEXCEPT
{
	//Cache the implementation.
	WindowsBinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<WindowsBinaryInputFileImplementation>() };

	//Unmap.
	implementation->_Map.unmap();
}
#endif