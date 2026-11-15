#if defined(CATALYST_PLATFORM_LINUX)
//Header file.
#include <Platform/File/BinaryInputFile.h>

//STD.
#include <fstream>

namespace Platform
{

	/*
	*	Binary input file implementation class definition.
	*/
	class BinaryInputFileImplementation final
	{

	public:

		//The file path.
		const char *RESTRICT _FilePath;

		//The underlying file stream.
		std::ifstream _FileStream;

		//The size.
		uint64 _Size;

	};

	static_assert(sizeof(BinaryInputFileImplementation) <= BinaryInputFile::ANY_SIZE, "Increase any size!");

	/*
	*	Constructor taking the file path.
	*/
	BinaryInputFile::BinaryInputFile(const char *const RESTRICT file_path) NOEXCEPT
	{
		//Cache the implementation.
		BinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<BinaryInputFileImplementation>() };

		//Construct the implementation.
		new (implementation) BinaryInputFileImplementation();

		//Open the file stream.
		implementation->_FileStream.open(file_path, std::ios::in | std::ios::binary);

		//Determine the size.
		if (implementation->_FileStream.is_open())
		{
			implementation->_FileStream.seekg(0, std::ios::end);
			implementation->_Size = implementation->_FileStream.tellg();
			implementation->_FileStream.clear();
			implementation->_FileStream.seekg(0, std::ios::beg);
		}

		//Set the file path.
		implementation->_FilePath = file_path;
	}

	/*
	*	Default destructor.
	*/
	BinaryInputFile::~BinaryInputFile() NOEXCEPT
	{
		//Cache the implementation.
		BinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<BinaryInputFileImplementation>() };

		//Close the file.
		implementation->_FileStream.close();

		//Call the destructor.
		implementation->~BinaryInputFileImplementation();
	}

	/*
	*	Bool operator overload.
	*/
	NO_DISCARD BinaryInputFile::operator bool() NOEXCEPT
	{
		//Cache the implementation.
		BinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<BinaryInputFileImplementation>() };

		return implementation->_FileStream.is_open();
	}

	/*
	*	Returns the file path.
	*/
	RESTRICTED NO_DISCARD const char *const RESTRICT BinaryInputFile::GetFilePath() const NOEXCEPT
	{
		//Cache the implementation.
		const BinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<BinaryInputFileImplementation>() };

		return implementation->_FilePath;
	}

	/*
	*	Returns the size of this binary input file, in bytes.
	*/
	NO_DISCARD uint64 BinaryInputFile::Size() const NOEXCEPT
	{
		//Cache the implementation.
		const BinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<BinaryInputFileImplementation>() };

		return implementation->_Size;
	}

	/*
	*	Reads from this binary file.
	*/
	void BinaryInputFile::Read(void *const RESTRICT output, const uint64 size, const File::Endian endian) NOEXCEPT
	{
		//Cache the implementation.
		BinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<BinaryInputFileImplementation>() };

		//Read.
		implementation->_FileStream.read(static_cast<char *const RESTRICT>(output), size);

		//Flip endian, if needed.
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
		BinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<BinaryInputFileImplementation>() };

		return static_cast<uint64>(implementation->_FileStream.tellg());
	}

	/*
	*	Sets the current position, in bytes.
	*/
	void BinaryInputFile::SetCurrentPosition(const uint64 position) NOEXCEPT
	{
		//Cache the implementation.
		BinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<BinaryInputFileImplementation>() };

		implementation->_FileStream.seekg(position);
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
	*	Returns if this binary input file is mapped.
	*/
	NO_DISCARD bool BinaryInputFile::IsMapped() const NOEXCEPT
	{
		return false;
	}

	/*
	*	Returns the mapped data (assuming this binary input file is mapped).
	*/
	NO_DISCARD const void *const RESTRICT BinaryInputFile::GetMappedData() const NOEXCEPT
	{
		return nullptr;
	}

	/*
	*	Closes this binary file.
	*/
	void BinaryInputFile::Close() NOEXCEPT
	{
		//Cache the implementation.
		BinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<BinaryInputFileImplementation>() };

		//Unmap.
		implementation->_FileStream.close();
	}
	
}
#endif