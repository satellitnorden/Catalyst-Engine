//Header file.
#include <Core/General/CommandLine.h>

#if defined(CATALYST_PLATFORM_WINDOWS)
//Platform.
#include <Platform/Windows/CatalystPlatformWindows.h>
#endif

/*
*	Initializes the command line.
*/
void CommandLine::Initialize() NOEXCEPT
{
	//Check if the command line has already been initialized.
	if (_Initialized)
	{
		return;
	}

	//Retrieve the command line.
#if defined(CATALYST_PLATFORM_WINDOWS)
	_CommandLine = GetCommandLine();
#endif

	//Fill up the keys and values.
	if (_CommandLine.Data())
	{
		const char *RESTRICT current_argument{ &_CommandLine[0] }; 

		for (uint64 i{ 0 }; i < _CommandLine.Length(); ++i)
		{
			if (_CommandLine[i] == ' '
				|| i == _CommandLine.Length() - 1)
			{
				//Ignore the executable string.
				if (current_argument[0] != '\"')
				{
					if (current_argument[0] == '-')
					{
						if (i != _CommandLine.Length() - 1)
						{
							_CommandLine[i] = '\0';
						}

						if (_Keys.Size() > _Values.Size())
						{
							_Values.Emplace(nullptr);
						}

						_Keys.Emplace(current_argument + 1);
					}

					else if (_Keys.Size() > _Values.Size())
					{
						if (i != _CommandLine.Length() - 1)
						{
							_CommandLine[i] = '\0';
						}

						_Values.Emplace(current_argument);
					}
				}

				current_argument = &_CommandLine[i + 1];
			}
		}
	}

	//If the last key didn't have a value, insert an empty value.
	if (_Keys.Size() > _Values.Size())
	{
		_Values.Emplace(nullptr);
	}

	//The command line is now initialized!
	_Initialized = true;
}

/*
*	Returns if the command line has the given key.
*/
NO_DISCARD bool CommandLine::HasKey(const char *const RESTRICT key) NOEXCEPT
{
	//Initialize, if needed.
	if (!_Initialized)
	{
		Initialize();
	}

	for (const char *const RESTRICT _key : _Keys)
	{
		if (StringUtilities::IsEqual(_key, key))
		{
			return true;
		}
	}

	return false;
}

/*
*	Returns the value for the given key.
*/
NO_DISCARD bool CommandLine::GetValue(const char *const RESTRICT key, const char *RESTRICT *const RESTRICT value) NOEXCEPT
{
	//Initialize, if needed.
	if (!_Initialized)
	{
		Initialize();
	}

	for (uint64 i{ 0 }; i < _Keys.Size(); ++i)
	{
		if (StringUtilities::IsEqual(_Keys[i], key))
		{
			*value = _Values[i];

			return true;
		}
	}

	return false;
}