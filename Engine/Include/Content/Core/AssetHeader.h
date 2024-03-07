#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/HashString.h>
#include <Core/General/StaticString.h>

class AssetHeader final
{

public:

	//The asset type identifier.
	HashString _AssetTypeIdentifier;

	//The version.
	uint64 _Version;

	//The asset identifier.
	HashString _AssetIdentifier;

	//The asset name.
	StaticString<64> _AssetName;

	/*
	*	Default constuctor.
	*/
	FORCE_INLINE AssetHeader() NOEXCEPT
		:
		_AssetTypeIdentifier(),
		_Version(0),
		_AssetIdentifier(),
		_AssetName()
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE AssetHeader(const HashString asset_type_identifier, const uint64 version, const HashString asset_identifier, const char *const RESTRICT asset_name) NOEXCEPT
		:
		_AssetTypeIdentifier(asset_type_identifier),
		_Version(version),
		_AssetIdentifier(asset_identifier),
		_AssetName(asset_name)
	{

	}

};