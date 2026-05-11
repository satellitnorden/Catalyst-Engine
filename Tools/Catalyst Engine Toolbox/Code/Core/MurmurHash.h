#pragma once

//Core.
#include <Core/Core.h>

/*
*	Hashes with the murmur hash implementation.
*/
static constexpr uint64 MurmurHash64(const char *const input, const uint64 length)
{
	constexpr uint64 M{ 0xc6a4a7935bd1e995 };
	constexpr int32 R{ 47 };

	uint64 hash{ 0 ^ (length * M) };

	const char *data{ input };
	const char *end{ data + (length & ~0b111ull) };

	while (data != end)
	{
		uint64 k
		{
			0ull
			| static_cast<uint64>(data[0]) << 0
			| static_cast<uint64>(data[1]) << 8
			| static_cast<uint64>(data[2]) << 16
			| static_cast<uint64>(data[3]) << 24
			| static_cast<uint64>(data[4]) << 32
			| static_cast<uint64>(data[5]) << 40
			| static_cast<uint64>(data[6]) << 48
			| static_cast<uint64>(data[7]) << 56
		};

		k *= M;
		k ^= k >> R;
		k *= M;

		hash ^= k;
		hash *= M;

		data += 8;
	}

	switch (length & 0b111ull)
	{
	case 7: hash ^= uint64(data[6]) << 48; [[fallthrough]];
	case 6: hash ^= uint64(data[5]) << 40; [[fallthrough]];
	case 5: hash ^= uint64(data[4]) << 32; [[fallthrough]];
	case 4: hash ^= uint64(data[3]) << 24; [[fallthrough]];
	case 3: hash ^= uint64(data[2]) << 16; [[fallthrough]];
	case 2: hash ^= uint64(data[1]) << 8; [[fallthrough]];
	case 1: hash ^= uint64(data[0]);
		hash *= M;
	};

	hash ^= hash >> R;
	hash *= M;
	hash ^= hash >> R;

	return hash;
}