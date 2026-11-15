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
		:
		_FilePath(file_path)
	{
		//Define constants.
		constexpr uint64 CHUNK_SIZE{ 1'000'000 };

		//Cache the asset manager.
		AAssetManager *const RESTRICT asset_manager{ CatalystPlatform::_App->activity->assetManager };

		//Open the asset.
		AAsset *RESTRICT asset{ AAssetManager_open(asset_manager, file_path, AASSET_MODE_BUFFER) };

		if (asset)
		{
			//Remember the size.
			_Size = AAsset_getLength(asset);

			//Allocate the buffer.
			_Buffer = Memory::Allocate(_Size);

			//Read into the buffer.
			{
				uint64 bytes_left_to_read{ _Size };
				uint64 bytes_read{ 0 };

				while (bytes_left_to_read > 0)
				{
                    uint64 bytes_to_read{ BaseMath::Minimum<uint64>(CHUNK_SIZE, bytes_left_to_read) };

					AAsset_read(asset, static_cast<void *const RESTRICT>(static_cast<byte *const RESTRICT>(_Buffer) + bytes_read), bytes_to_read);

					bytes_left_to_read -= bytes_to_read;
					bytes_read += bytes_to_read;
				}
			}

			//Close the asset.
			AAsset_close(asset);
		}
	}

	/*
	*	Bool operator overload.
	*/
	FORCE_INLINE NO_DISCARD operator bool() NOEXCEPT
	{
		return _Buffer != nullptr;
	}

	/*
	*	Returns the file path.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const char *const RESTRICT GetFilePath() const NOEXCEPT
	{
		return _FilePath;
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
		return _CurrentPosition;
	}

	/*
	*	Sets the current position, in bytes.
	*/
	FORCE_INLINE void SetCurrentPosition(const uint64 position) NOEXCEPT
	{
		_CurrentPosition = position;
	}

	/*
	*	Reads from this binary file.
	*/
	template <bool FLIP_ENDIAN = false>
	FORCE_INLINE void Read(void *const RESTRICT output, const uint64 size) NOEXCEPT
	{
		Memory::Copy(output, static_cast<const void *const RESTRICT>(static_cast<const byte *const RESTRICT>(_Buffer) + _CurrentPosition), size);
		_CurrentPosition += size;
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
		if (_Buffer)
		{
			Memory::Free(_Buffer);
			_Buffer = nullptr;
		}
	}

private:

	//The file path.
	const char *RESTRICT _FilePath;

	//The buffer.
	void *RESTRICT _Buffer{ nullptr };

	//The current position.
	uint64 _CurrentPosition{ 0 };

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
		_FilePath(file_path),
		_FileStream(file_path, std::ios::out | std::ios::binary)
	{

	}

	/*
	*	Returns the file path.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const char *const RESTRICT GetFilePath() const NOEXCEPT
	{
		return _FilePath;
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

	//The file path.
	const char *RESTRICT _FilePath;

	//The underlying file stream.
	std::ofstream _FileStream;

};
#endif