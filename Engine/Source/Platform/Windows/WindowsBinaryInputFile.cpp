#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <File/Core/BinaryInputFile.h>

//Platform.
#if defined(CATALYST_HAS_EMBEDDED_RESOURCES)
	#include <Platform/Windows/CatalystPlatformWindows.h>
#endif

//Third party.
#include <ThirdParty/mio/mio.hpp>

/*
*	Windows binary input file implementation class definition.
*/
class WindowsBinaryInputFileImplementation final
{

public:

	//Denotes whether the file could be opened.
	bool _CouldBeOpened{ false };

	//The file path.
	const char *RESTRICT _FilePath;

	//The size of the file stream.
	uint64 _Size;

	//The data.
	const byte *RESTRICT _Data;

	//The current position.
	uint64 _CurrentPosition;

	//The map.
	mio::ummap_source _Map;

	//The resource.
	HRSRC _Resource;

};

static_assert(sizeof(WindowsBinaryInputFileImplementation) <= BinaryInputFile::ANY_SIZE, "Increase any size!");

/*
*	Tries to open the file from disk.
*/
FORCE_INLINE NO_DISCARD bool TryOpenFromDisk(const char *const RESTRICT file_path, WindowsBinaryInputFileImplementation *const RESTRICT implementation) NOEXCEPT
{
	//Map the file.
	std::error_code error;
	implementation->_Map.map(std::string(file_path), 0, mio::map_entire_file, error);

	//If there was an error, the file could not be opened.
	if (error)
	{
		return false;
	}

	//Retrieve the size.
	implementation->_Size = static_cast<uint64>(implementation->_Map.mapped_length());

	//Retrieve the data.
	implementation->_Data = implementation->_Map.data();

	//The open succeeeded if the size is non-zero and the data is a valid pointer.
	return implementation->_Size > 0 && implementation->_Data != nullptr;
}

#if defined(CATALYST_HAS_EMBEDDED_RESOURCES)
/*
*	Tries to open the file from an embedded resource.
*/
FORCE_INLINE NO_DISCARD bool TryOpenFromEmbeddedResource(const char *const RESTRICT file_path, WindowsBinaryInputFileImplementation *const RESTRICT implementation) NOEXCEPT
{
	//Figure out the file name.
	const char *RESTRICT file_name{ nullptr };

	{
		//Find the last slash position.
		uint32 last_slash_position{ UINT32_MAXIMUM };

		{
			uint32 i{ 0 };

			while (file_path[i] != '\0')
			{
				if (file_path[i] == '/' || file_path[i] == '\\')
				{
					last_slash_position = i;
				}

				++i;
			}
		}

		//The file name starts at one past the last slash position - Or at the start if none was found.
		file_name = last_slash_position == UINT32_MAXIMUM ? file_path : &file_path[last_slash_position + 1];
	}

	//Find the resource.
	const HRSRC resource{ FindResource(CatalystPlatformWindows::_Instance, file_name, RT_RCDATA) };

	//If we couldn't find the resource, we can't proceed.
	if (!resource)
	{
		return false;
	}

	//Load the resource.
	const HGLOBAL loaded_resource{ LoadResource(CatalystPlatformWindows::_Instance, resource) };

	//If we couldn't load the resource, we can't proceed.
	if (!loaded_resource)
	{
		return false;
	}

	//Retrieve the size.
	implementation->_Size = SizeofResource(CatalystPlatformWindows::_Instance, resource);

	//Retrieve the data.
	implementation->_Data = static_cast<const byte *const RESTRICT>(LockResource(loaded_resource));

	//The open succeeeded if the size is non-zero and the data is a valid pointer.
	return implementation->_Size > 0 && implementation->_Data != nullptr;
}
#endif

/*
*	Constructor taking the file path.
*/
BinaryInputFile::BinaryInputFile(const char *const RESTRICT file_path) NOEXCEPT
{
	//Cache the implementation.
	WindowsBinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<WindowsBinaryInputFileImplementation>() };

	//Construct the implementation.
	new (implementation) WindowsBinaryInputFileImplementation();

	//Set the file path.
	implementation->_FilePath = file_path;

	//Try to open the file from different locations.
	if (!implementation->_CouldBeOpened)
	{
		implementation->_CouldBeOpened = TryOpenFromDisk(file_path, implementation);
	}

#if defined(CATALYST_HAS_EMBEDDED_RESOURCES)
	if (!implementation->_CouldBeOpened)
	{
		implementation->_CouldBeOpened = TryOpenFromEmbeddedResource(file_path, implementation);
	}
#endif

	//If we could open the file, set up it's initial state.
	if (implementation->_CouldBeOpened)
	{
		implementation->_CurrentPosition = 0;
	}

	ASSERT(implementation->_CouldBeOpened, "Could not open file!");
}

/*
*	Default destructor.
*/
BinaryInputFile::~BinaryInputFile() NOEXCEPT
{
	//Cache the implementation.
	WindowsBinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<WindowsBinaryInputFileImplementation>() };

	//Call the destructor.
	implementation->~WindowsBinaryInputFileImplementation();
}

/*
*	Bool operator overload.
*/
NO_DISCARD BinaryInputFile::operator bool() NOEXCEPT
{
	//Cache the implementation.
	WindowsBinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<WindowsBinaryInputFileImplementation>() };

	return implementation->_CouldBeOpened;
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
*	Returns if this binary input file is mapped.
*/
NO_DISCARD bool BinaryInputFile::IsMapped() const NOEXCEPT
{
	return true;
}

/*
*	Returns the mapped data (assuming this binary input file is mapped).
*/
NO_DISCARD const void *const RESTRICT BinaryInputFile::GetMappedData() const NOEXCEPT
{
	//Cache the implementation.
	const WindowsBinaryInputFileImplementation *const RESTRICT implementation{ _Implementation.Get<WindowsBinaryInputFileImplementation>() };

	//Return the data.
	return implementation->_Data;
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