#if defined(CATALYST_PLATFORM_LINUX)
//Header file.
#include <Platform/File/BinaryOutputFile.h>

//STL.
#include <fstream>

namespace Platform
{

	/*
	*	Binary output file implementation class definition.
	*/
	class BinaryOutputFileImplementation final
	{

	public:

		//The underlying file stream.
		std::ofstream _FileStream;

		//The file path.
		const char *RESTRICT _FilePath;

	};

	static_assert(sizeof(BinaryOutputFileImplementation) <= BinaryOutputFile::ANY_SIZE, "Increase any size!");

	/*
	*	Constructor taking the file path.
	*/
	BinaryOutputFile::BinaryOutputFile(const char *const RESTRICT file_path) NOEXCEPT
	{
		//Cache the implementation.
		BinaryOutputFileImplementation *const RESTRICT implementation{ _Implementation.Get<BinaryOutputFileImplementation>() };

		//Construct the implementation.
		new (implementation) BinaryOutputFileImplementation();

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
		const BinaryOutputFileImplementation *const RESTRICT implementation{ _Implementation.Get<BinaryOutputFileImplementation>() };

		//Return the file path.
		return implementation->_FilePath;
	}

	/*
	*	Writes to this file.
	*/
	void BinaryOutputFile::Write(const void *const RESTRICT input, const uint64 size) NOEXCEPT
	{
		//Cache the implementation.
		BinaryOutputFileImplementation *const RESTRICT implementation{ _Implementation.Get<BinaryOutputFileImplementation>() };

		//Write!
		implementation->_FileStream.write(static_cast<const char *const RESTRICT>(input), size);
	}

	/*
	*	Closes this file.
	*/
	void BinaryOutputFile::Close() NOEXCEPT
	{
		//Cache the implementation.
		BinaryOutputFileImplementation *const RESTRICT implementation{ _Implementation.Get<BinaryOutputFileImplementation>() };

		//Close the file.
		implementation->_FileStream.close();
	}
	
}
#endif