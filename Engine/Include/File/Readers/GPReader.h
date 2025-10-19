#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/DynamicString.h>

//Profiling.
#include <Profiling/Profiling.h>

//Sound.
#include <Sound/SoundUtilities.h>

/*
*	TODO:
*	Handles linear tempo events.
*	Correctly identify tapped notes.
*	Correctly identify pinched harmonic notes.
*/
class GPReader
{

public:

	/*
	*	Tablature class definition.
	*/
	class Tablature final
	{

	public:

		/*
		*	Bar class definition.
		*/
		class Bar final
		{

		public:

			//The tempo.
			float64 _Tempo;

			//The time signature numerator.
			uint8 _TimeSignatureNumerator;

			//The time signature denominator.
			uint32 _TimeSignatureDenominator;
		};

		/*
		*	Text event class definition.
		*/
		class TextEvent final
		{

		public:

			//The time.
			float64 _Time;

			//The text.
			DynamicString _Text;

		};

		/*
		*	Track class definition.
		*/
		class Track final
		{

		public:

			/*
			*	Track bar class definition.
			*/
			class TrackBar final
			{

			public:

				/*
				*	Event class definition.
				*/
				class Event final
				{

				public:

					//Enumeration covering all articulations.
					enum class Articulation : uint8
					{
						SUSTAIN,
						PALM_MUTED,
						HAMMER_ON,
						PULL_OFF,
						TAPPED,
						DEAD,
						NATURAL_HARMONIC
					};

					//The offset (from the start of the bar, in quarter notes).
					float64 _Offset;

					//The duration (in quarter notes).
					float64 _Duration;

					//The string index.
					uint8 _StringIndex;

					//The fret index.
					uint8 _FretIndex;

					//The articulation.
					Articulation _Articulation;

					//The bend offsets.
					DynamicArray<float32> _BendOffsets;

					//The bend values.
					DynamicArray<float32> _BendValues;

					//The slide offsets.
					DynamicArray<float32> _SlideOffsets;

					//The slide values.
					DynamicArray<float32> _SlideValues;

					//Denotes if this is a slide event.
					bool _IsSlideEvent;

				};

				//Pointer to the bar.
				const Bar *RESTRICT _Bar;

				//The events.
				DynamicArray<Event> _Events;

			};

			//The name.
			DynamicString _Name;

			//The tuning.
			DynamicArray<uint8> _Tuning;

			//The track bars.
			DynamicArray<TrackBar> _TrackBars;

			//The text events.
			DynamicArray<TextEvent> _TextEvents;

		};

		//The major version.
		uint32 _MajorVersion;

		//The minor version.
		uint32 _MinorVersion;

		//The patch version.
		uint32 _PatchVersion;

		//The bars.
		DynamicArray<Bar> _Bars;

		//The tracks.
		DynamicArray<Track> _Tracks;

	};

	/*
	*	Reads the tablature at the given file path. Returns if the read was succesful.
	*/
	static NO_DISCARD bool Read(const char *const RESTRICT file_path, Tablature *const RESTRICT tablature) NOEXCEPT;

private:

	/*
	*	Temporary data class definition.
	*/
	class TemporaryData final
	{

	public:

		/*
		*	Repeat region class definition.
		*/
		class RepeatRegion final
		{

		public:

			//The start bar index.
			uint64 _StartBarIndex;

			//The end bar index.
			uint64 _EndBarIndex;

			//The count.
			uint32 _Count;

		};

		/*
		*	Beat class definition.
		*/
		class Beat final
		{

		public:

			//The rhythm index.
			uint32 _RhythmIndex;

			//The note indices.
			DynamicArray<uint32> _NoteIndices;

			//The text.
			DynamicString _Text;

		};

		/*
		*	Note class definition.
		*/
		class Note final
		{

		public:

			//Enumeration covering all flags.
			enum class Flags : uint64
			{
				NONE = 0,
				DEAD = BIT(0),
				HOPO_DESTINATION = BIT(1),
				NATURAL_HARMONIC = BIT(2),
				PALM_MUTED = BIT(3),
				LEGATO_SLIDE = BIT(4),
				TAPPED = BIT(5),
				TIE_ORIGIN = BIT(6),
				TIE_DESTINATION = BIT(7)
			};

			//The string index.
			uint8 _StringIndex;

			//The fret index.
			uint8 _FretIndex;

			//The flags.
			Flags _Flags;

			//The bend offsets.
			DynamicArray<float32> _BendOffsets;

			//The bend values.
			DynamicArray<float32> _BendValues;

			//The slide offsets.
			DynamicArray<float32> _SlideOffsets;

			//The slide values.
			DynamicArray<float32> _SlideValues;

		};

		/*
		*	Rhythm class definition.
		*/
		class Rhythm final
		{

		public:

			//The note duration.
			NoteDuration _NoteDuration;

			//The note type.
			NoteType _NoteType;

		};

		/*
		*	Last played note class definition.
		*/
		class LastPlayedNote final
		{

		public:

			//The fret index.
			int8 _FretIndex;

			//Whether or not this was a tie origin.
			bool _WasTieOrigin;

			//Whether or not this was a legato slide origin.
			bool _WasLegatoSlideOrigin;

			//The bar index.
			uint64 _BarIndex;

			//The event index.
			uint64 _EventIndex;

		};

		//The repeat regions.
		DynamicArray<RepeatRegion> _RepeatRegions;

		//The track to bar mappings.
		DynamicArray<DynamicArray<int32>> _TrackToBarMappings;

		//The bar to voice mappings.
		DynamicArray<DynamicArray<int32>> _BarToVoiceMappings;

		//The voice to beats mappings.
		DynamicArray<DynamicArray<uint32>> _VoiceToBeatsMappings;

		//The beats.
		DynamicArray<Beat> _Beats;

		//The notes.
		DynamicArray<Note> _Notes;

		//The rhythms.
		DynamicArray<Rhythm> _Rhythms;
		
		//Container for the last played note, per track, per string.
		DynamicArray<DynamicArray<LastPlayedNote>> _LastPlayedNotes;

	};

	/*
	*	Calculates the duration (in quarter notes) of a bar.
	*/
	FORCE_INLINE static NO_DISCARD float64 CalculateDurationOfBar(const Tablature::Bar &bar) NOEXCEPT
	{
		switch (bar._TimeSignatureDenominator)
		{
			case 4:
			{
				return static_cast<float64>(bar._TimeSignatureNumerator);
			}

			case 8:
			{
				return static_cast<float64>(bar._TimeSignatureNumerator) * 0.5;
			}

			case 16:
			{
				return static_cast<float64>(bar._TimeSignatureNumerator) * 0.25;
			}

			default:
			{
				ASSERT(false, "Handle this!");

				return 0.0;
			}
		}
	}

};