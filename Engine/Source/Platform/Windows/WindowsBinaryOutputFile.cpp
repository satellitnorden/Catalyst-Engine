#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <File/Core/BinaryOutputFile.h>

//STL.
#include <fstream>

/*
*	Windows binary output file implementation class definition.
*/
class WindowsBinaryOutputFileImplementation final
{

public:

	//The underlying file stream.
	std::ofstream _FileStream;

	//The file path.
	const char *RESTRICT _FilePath;

};

static_assert(sizeof(WindowsBinaryOutputFileImplementation) <= BinaryOutputFile::ANY_SIZE, "Increase any size!");

/*
*	Constructor taking the file path.
*/
BinaryOutputFile::BinaryOutputFile(const char *const RESTRICT file_path) NOEXCEPT
{
	//Cache the implementation.
	WindowsBinaryOutputFileImplementation *const RESTRICT implementation{ _Implementation.Get<WindowsBinaryOutputFileImplementation>() };

	//Construct the implementation.
	new (implementation) WindowsBinaryOutputFileImplementation();

	//Open the file stream.
	implementation->_FileStream.open(file_path, std::ios::out | std::ios::binary);

	//Set the file path.
	implementation->_FilePath = file_path;
}

/*
*	Returns the file path.
*/
RESTRICTED NO_DISCARD const char *const RESTRICT BinaryOutputFile::GetFilePath() const NOEXCEPT
{
	//Cache the implementation.
	const WindowsBinaryOutputFileImplementation *const RESTRICT implementation{ _Implementation.Get<WindowsBinaryOutputFileImplementation>() };

	//Return the file path.
	return implementation->_FilePath;
}

/*
*	Writes to this file.
*/
void BinaryOutputFile::Write(const void *const RESTRICT input, const uint64 size) NOEXCEPT
{
	//Cache the implementation.
	WindowsBinaryOutputFileImplementation *const RESTRICT implementation{ _Implementation.Get<WindowsBinaryOutputFileImplementation>() };

	//Write!
	implementation->_FileStream.write(static_cast<const char *const RESTRICT>(input), size);
}

/*
*	Closes this file.
*/
void BinaryOutputFile::Close() NOEXCEPT
{
	//Cache the implementation.
	WindowsBinaryOutputFileImplementation *const RESTRICT implementation{ _Implementation.Get<WindowsBinaryOutputFileImplementation>() };

	//Close the file.
	implementation->_FileStream.close();
}
#endif