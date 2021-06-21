#if defined(CATALYST_PLATFORM_ANDROID)
#pragma once

//Android.
#include <android/asset_manager.h>

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
	{
		//Cache the asset manager.
		AAssetManager *const RESTRICT asset_manager{ CatalystPlatform::_App->activity->assetManager };

		//Open the asset.
		_Asset = AAssetManager_open(asset_manager, file_path, AASSET_MODE_STREAMING);

		//Remember the size.
		_Size = AAsset_getLength(_Asset);
	}

	/*
	*	Bool operator overload.
	*/
	FORCE_INLINE NO_DISCARD operator bool() NOEXCEPT
	{
		return _Asset != nullptr;
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
		return _Size - AAsset_getRemainingLength64(_Asset);
	}

	/*
	*	Sets the current position, in bytes.
	*/
	FORCE_INLINE void SetCurrentPosition(const uint64 position) NOEXCEPT
	{
		AAsset_seek64(_Asset, position, SEEK_SET);
	}

	/*
	*	Reads from this binary file.
	*/
	template <bool FLIP_ENDIAN = false>
	FORCE_INLINE void Read(void *const RESTRICT output, const uint64 size) NOEXCEPT
	{
		AAsset_read(_Asset, output, size);
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
		if (_Asset)
		{
			AAsset_close(_Asset);
		}
	}

private:

	//The underlying asset
	AAsset *RESTRICT _Asset;

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
#endif