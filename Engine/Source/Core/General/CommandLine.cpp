//Header file.
#include <Core/General/CommandLine.h>

//Singleton definition.
DEFINE_SINGLETON(CommandLine);

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

					_Keys.Emplace(current_argument);
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