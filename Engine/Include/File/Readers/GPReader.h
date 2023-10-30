#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/DynamicString.h>

//Sound.
#include <Sound/SoundUtilities.h>

//Third party.
#include <ThirdParty/Minizip/minizip/unzip.h>
#include <ThirdParty/pugixml/pugixml.hpp>

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
						PULL_OFF
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
	FORCE_INLINE static NO_DISCARD bool Read(const char *const RESTRICT file_path, Tablature *const RESTRICT tablature) NOEXCEPT
	{
		//Set up the temporary data.
		TemporaryData temporary_data;

		//Unzip the file.
		const unzFile unzipped_file{ unzOpen(file_path) };

		if (!unzipped_file)
		{
			return false;
		}

		//Try to locate the file.
		int32 result{ unzLocateFile(unzipped_file, "Content/score.gpif", 1) };

		if (result != UNZ_OK)
		{
			return false;
		}

		//Retrieve information about the current file.
		unz_file_info file_info;
		char file_name[128];

		unzGetCurrentFileInfo
		(
			unzipped_file,
			&file_info,
			file_name,
			128,
			nullptr,
			0,
			nullptr,
			0
		);

		//Open the current file.
		result = unzOpenCurrentFile(unzipped_file);

		if (result != UNZ_OK)
		{
			return false;
		}

		//Allocate the necessary amount of data.
		byte *const RESTRICT data{ static_cast<byte *const RESTRICT>(Memory::Allocate(file_info.uncompressed_size)) };

		//Read the data.
		const int32 bytes_read{ unzReadCurrentFile(unzipped_file, data, file_info.uncompressed_size) };

		ASSERT(bytes_read == file_info.uncompressed_size, "Didn't read all of the data, something went wrong!");

		pugi::xml_document xml_document;

		const pugi::xml_parse_result xml_parse_result{ xml_document.load_buffer(data, file_info.uncompressed_size) };

		if (result)
		{
			return false;
		}

#if 1
		//Write out a "debug" version of the xml document.
		{
			constexpr const char *const RESTRICT FILE_POSTFIX{ "_debug.xml" };
			constexpr uint64 FILE_POSTFIX_LENGTH{ StringUtilities::StringLength(FILE_POSTFIX) };

			const uint64 file_path_length{ StringUtilities::StringLength(file_path) };

			char buffer[MAXIMUM_FILE_PATH_LENGTH];
			sprintf_s(buffer, file_path);

			uint64 last_dot_position{ 0 };

			for (int64 i{ static_cast<int64>(file_path_length) - 1 }; i >= 0; --i)
			{
				if (buffer[i] == '.')
				{
					last_dot_position = i;

					break;
				}
			}

			for (uint64 i{ 0 }; i < FILE_POSTFIX_LENGTH; ++i)
			{
				buffer[last_dot_position + i] = FILE_POSTFIX[i];
			}

			buffer[last_dot_position + FILE_POSTFIX_LENGTH] = '\0';

			xml_document.save_file(buffer);
		}
#endif

		//Cache the root GPIF node.
		const pugi::xml_node gpif_node{ xml_document.child("GPIF") };

		//Write in the version information.
		{
			const pugi::xml_node gpversion_node{ gpif_node.child("GPVersion") };

			const pugi::char_t *const RESTRICT child_value{ gpversion_node.child_value() };

			char child_value_buffer[32];
			sprintf_s(child_value_buffer, child_value);

			char version_buffer[32];
			uint32 current_position{ 0 };

			{
				for (uint32 iterator{ current_position }; iterator < ARRAY_LENGTH(child_value_buffer); ++iterator)
				{
					if (child_value_buffer[iterator] != '.')
					{
						version_buffer[iterator - current_position] = child_value_buffer[iterator];
					}

					else
					{
						version_buffer[iterator - current_position] = '\0';
						current_position = iterator + 1;

						break;
					}
				}

				tablature->_MajorVersion = std::stoul(version_buffer);
			}

			{
				for (uint32 iterator{ current_position }; iterator < ARRAY_LENGTH(child_value_buffer); ++iterator)
				{
					if (child_value_buffer[iterator] != '.')
					{
						version_buffer[iterator - current_position] = child_value_buffer[iterator];
					}

					else
					{
						version_buffer[iterator - current_position] = '\0';
						current_position = iterator + 1;

						break;
					}
				}

				tablature->_MinorVersion = std::stoul(version_buffer);
			}

			{
				for (uint32 iterator{ current_position }; iterator < ARRAY_LENGTH(child_value_buffer); ++iterator)
				{
					if (child_value_buffer[iterator] != '.' && child_value_buffer[iterator] != '\0')
					{
						version_buffer[iterator - current_position] = child_value_buffer[iterator];
					}

					else
					{
						version_buffer[iterator - current_position] = '\0';
						current_position = iterator + 1;

						break;
					}
				}

				tablature->_PatchVersion = std::stoul(version_buffer);
			}
		}

		//Fill in the master bars data.
		{
			const pugi::xml_node master_bars_node{ gpif_node.child("MasterBars") };

			for (const pugi::xml_node master_bar_node : master_bars_node)
			{
				const pugi::xml_node time_node{ master_bar_node.child("Time") };

				tablature->_Bars.Emplace();
				Tablature::Bar& new_bar{ tablature->_Bars.Back() };

				new_bar._Tempo = FLOAT64_MAXIMUM; //Will be filled in when we read the master track data. (:
				const pugi::char_t *RESTRICT numerator_string{ time_node.child_value() };
				new_bar._TimeSignatureNumerator = static_cast<uint8>(std::stoul(numerator_string));
				const pugi::char_t* RESTRICT denominator_string{ numerator_string };
				while (*denominator_string) { if ((*denominator_string) == '/') { ++denominator_string; break; } else { ++denominator_string; } }
				new_bar._TimeSignatureDenominator = std::stoul(denominator_string);
			}
		}

		//Fill in the master track data.
		{
			const pugi::xml_node master_track_node{ gpif_node.child("MasterTrack") };
			const pugi::xml_node automations_node{ master_track_node.child("Automations") };

			for (const pugi::xml_node automation_node : automations_node)
			{
				const pugi::xml_node type_node{ automation_node.child("Type") };

				if (StringUtilities::IsEqual(type_node.child_value(), "Tempo"))
				{
					const pugi::xml_node bar_node{ automation_node.child("Bar") };
					const uint32 bar_index{ std::stoul(bar_node.child_value()) };
					const pugi::xml_node value_node{ automation_node.child("Value") };
					const float64 tempo{ std::stod(value_node.child_value()) };

					tablature->_Bars[bar_index]._Tempo = tempo;
				}
			}
		}

		//Tempo seems to not be defined to all bars, so fill in the gaps.
		for (uint64 bar_index{ 1 }; bar_index < tablature->_Bars.Size(); ++bar_index)
		{
			if (tablature->_Bars[bar_index]._Tempo == FLOAT64_MAXIMUM)
			{
				tablature->_Bars[bar_index]._Tempo = tablature->_Bars[bar_index - 1]._Tempo;
			}
		}

		//Fill in the tracks data.
		{
			const pugi::xml_node tracks_node{ gpif_node.child("Tracks") };

			for (const pugi::xml_node track_node : tracks_node)
			{
				//Create the new track.
				tablature->_Tracks.Emplace();
				Tablature::Track &new_track{ tablature->_Tracks.Back() };

				//Retrieve the name.
				new_track._Name = track_node.child("Name").child_value();

				//Read the tuning.
				const pugi::xml_node properties_node{ track_node.child("Staves").child("Staff").child("Properties") };

				for (const pugi::xml_node property_node : properties_node)
				{
					if (StringUtilities::IsEqual(property_node.attribute("name").value(), "Tuning"))
					{
						const pugi::char_t *RESTRICT tuning_string{ property_node.child("Pitches").child_value() };

						new_track._Tuning.Emplace(static_cast<uint8>(std::stoul(tuning_string)));

						while (*tuning_string)
						{
							if ((*tuning_string) == ' ')
							{
								++tuning_string;
								new_track._Tuning.Emplace(static_cast<uint8>(std::stoul(tuning_string)));
							}

							else
							{
								++tuning_string;
							}
						}
					}
					
				}
			}
		}

		//Fill in the bars data.
		{
			const pugi::xml_node bars_node{ gpif_node.child("Bars") };

			for (const pugi::xml_node bar_node : bars_node)
			{
				const pugi::char_t *RESTRICT voices_string{ bar_node.child("Voices").child_value() };

				temporary_data._BarToVoiceMappings.Emplace();
				temporary_data._BarToVoiceMappings.Back().Emplace(std::stoi(voices_string));

				while (*voices_string)
				{
					if ((*voices_string) == ' ')
					{
						++voices_string;
						temporary_data._BarToVoiceMappings.Back().Emplace(std::stoi(voices_string));
					}

					else
					{
						++voices_string;
					}
				}
			}
		}

		//Fill in the voices data.
		{
			const pugi::xml_node voices_node{ gpif_node.child("Voices") };

			for (const pugi::xml_node voice_node : voices_node)
			{
				const pugi::char_t* RESTRICT beats_string{ voice_node.child("Beats").child_value() };

				temporary_data._VoiceToBeatsMappings.Emplace();
				temporary_data._VoiceToBeatsMappings.Back().Emplace(std::stoul(beats_string));

				while (*beats_string)
				{
					if ((*beats_string) == ' ')
					{
						++beats_string;
						temporary_data._VoiceToBeatsMappings.Back().Emplace(std::stoul(beats_string));
					}

					else
					{
						++beats_string;
					}
				}
			}
		}

		//Fill in the beats data.
		{
			const pugi::xml_node beats_node{ gpif_node.child("Beats") };

			for (const pugi::xml_node beat_node : beats_node)
			{
				//Add the new beat.
				temporary_data._Beats.Emplace();
				TemporaryData::Beat &new_beat{ temporary_data._Beats.Back() };

				//Set the rhythm index.
				new_beat._RhythmIndex = std::stoul(beat_node.child("Rhythm").attribute("ref").value());

				//Set the note indices.
				{
					const pugi::xml_node notes_node{ beat_node.child("Notes") };

					if (notes_node)
					{
						const pugi::char_t *RESTRICT notes_string{ notes_node.child_value() };

						new_beat._NoteIndices.Emplace(std::stoul(notes_string));

						while (*notes_string)
						{
							if ((*notes_string) == ' ')
							{
								++notes_string;
								new_beat._NoteIndices.Emplace(std::stoul(notes_string));
							}

							else
							{
								++notes_string;
							}
						}
					}
				}
			}
		}

		//Fill in the notes data.
		{
			const pugi::xml_node notes_node{ gpif_node.child("Notes") };

			for (const pugi::xml_node note_node : notes_node)
			{
				//Add the new note.
				temporary_data._Notes.Emplace();
				TemporaryData::Note &new_note{ temporary_data._Notes.Back() };

				new_note._Flags = TemporaryData::Note::Flags::NONE;

				if (const pugi::xml_node tie_node{ note_node.child("Tie") })
				{
					if (StringUtilities::IsEqual(tie_node.attribute("destination").value(), "true"))
					{
						new_note._Flags = static_cast<TemporaryData::Note::Flags>(UNDERLYING(new_note._Flags) | UNDERLYING(TemporaryData::Note::Flags::TIE_DESTINATION));
					}
				}

				//Fill in the properties.
				const pugi::xml_node properties_node{ note_node.child("Properties") };

				for (const pugi::xml_node property_node : properties_node)
				{
					if (StringUtilities::IsEqual(property_node.attribute("name").value(), "Fret"))
					{
						//Set the fret index.
						new_note._FretIndex = static_cast<uint8>(std::stoul(property_node.child("Fret").child_value()));
					}

					else if(StringUtilities::IsEqual(property_node.attribute("name").value(), "String"))
					{
						//Set the string index.
						new_note._StringIndex = static_cast<uint8>(std::stoul(property_node.child("String").child_value()));
					}

					else if (StringUtilities::IsEqual(property_node.attribute("name").value(), "HopoDestination"))
					{
						//Add the flag,
						new_note._Flags = static_cast<TemporaryData::Note::Flags>(UNDERLYING(new_note._Flags) | UNDERLYING(TemporaryData::Note::Flags::HOPO_DESTINATION));
					}

					else if (StringUtilities::IsEqual(property_node.attribute("name").value(), "PalmMuted"))
					{
						//Add the flag,
						new_note._Flags = static_cast<TemporaryData::Note::Flags>(UNDERLYING(new_note._Flags) | UNDERLYING(TemporaryData::Note::Flags::PALM_MUTED));
					}
				}
			}
		}

		//Fill in the rhythms data.
		{
			const pugi::xml_node rhythms_node{ gpif_node.child("Rhythms") };

			for (const pugi::xml_node rhythm_node : rhythms_node)
			{
				//Add the new rhythm.
				temporary_data._Rhythms.Emplace();
				TemporaryData::Rhythm &new_rhythm{ temporary_data._Rhythms.Back() };

				//Set the note duration.
				const pugi::char_t *const RESTRICT note_value_string{ rhythm_node.child("NoteValue").child_value() };

				if (StringUtilities::IsEqual(note_value_string, "Whole"))
				{
					new_rhythm._NoteDuration = NoteDuration::WHOLE;
				}

				else if (StringUtilities::IsEqual(note_value_string, "Half"))
				{
					new_rhythm._NoteDuration = NoteDuration::HALF;
				}

				else if (StringUtilities::IsEqual(note_value_string, "Quarter"))
				{
					new_rhythm._NoteDuration = NoteDuration::QUARTER;
				}

				else if (StringUtilities::IsEqual(note_value_string, "Eighth"))
				{
					new_rhythm._NoteDuration = NoteDuration::EIGHTH;
				}

				else if (StringUtilities::IsEqual(note_value_string, "16th"))
				{
					new_rhythm._NoteDuration = NoteDuration::SIXTEENTH;
				}

				else if (StringUtilities::IsEqual(note_value_string, "32nd"))
				{
					new_rhythm._NoteDuration = NoteDuration::THIRTYSECOND;
				}

				else if (StringUtilities::IsEqual(note_value_string, "64th"))
				{
					new_rhythm._NoteDuration = NoteDuration::SIXTEENTH;
				}

				else if (StringUtilities::IsEqual(note_value_string, "128th"))
				{
					new_rhythm._NoteDuration = NoteDuration::HUNDREDTWENTYEIGHTH;
				}

				//Set the note type.
				if (rhythm_node.child("AugmentationDot"))
				{
					new_rhythm._NoteType = NoteType::DOTTED;
				}

				else if (rhythm_node.child("PrimaryTuplet"))
				{
					new_rhythm._NoteType = NoteType::TRIPLET;
				}

				else
				{
					new_rhythm._NoteType = NoteType::REGULAR;
				}
			}
		}

		//Alright, time to piece together all the data.
		for (uint64 bar_index{ 0 }; bar_index < tablature->_Bars.Size(); ++bar_index)
		{
			const Tablature::Bar &bar{ tablature->_Bars[bar_index] };

			for (uint64 track_index{ 0 }; track_index < tablature->_Tracks.Size(); ++track_index)
			{
				Tablature::Track &track{ tablature->_Tracks[track_index] };

				track._TrackBars.Emplace();
				Tablature::Track::TrackBar &track_bar{ track._TrackBars.Back() };

				//Set the bar.
				track_bar._Bar = &bar;

				//Remember the current offset.
				float64 current_offset{ 0.0 };

				//Cache the bar to voices mapping.
				const DynamicArray<int32> &bar_to_voice_mappings{ temporary_data._BarToVoiceMappings[track_index + (bar_index * tablature->_Tracks.Size())] };

				for (const int32 bar_to_voice_mapping : bar_to_voice_mappings)
				{
					if (bar_to_voice_mapping < 0)
					{
						continue;
					}

					const DynamicArray<uint32> &voice_to_beat_mappings{ temporary_data._VoiceToBeatsMappings[bar_to_voice_mapping] };

					for (const uint32 voice_to_beat_mapping : voice_to_beat_mappings)
					{
						const TemporaryData::Beat &beat{ temporary_data._Beats[voice_to_beat_mapping] };
						const TemporaryData::Rhythm &rhythm{ temporary_data._Rhythms[beat._RhythmIndex] };

						for (const uint32 note_index : beat._NoteIndices)
						{
							const TemporaryData::Note &note{ temporary_data._Notes[note_index] };

							//Disregard illegal notes. (:
							if (note._FretIndex > 24)
							{
								continue;
							}

							//If this is a tie destination, iterate backwards and try to extend a previous event.
							if (TEST_BIT(UNDERLYING(note._Flags), UNDERLYING(TemporaryData::Note::Flags::TIE_DESTINATION)))
							{
								int64 current_event_index{ static_cast<int64>(track_bar._Events.Size()) - 1 };
								int64 current_bar_index{ static_cast<int64>(bar_index) };
								bool found{ false };

								while (current_bar_index > 0 && !found)
								{
									if (current_event_index < 0)
									{
										--current_bar_index;

										if (current_bar_index < 0)
										{
											break;
										}

										current_event_index = static_cast<int64>(track._TrackBars[current_bar_index]._Events.Size()) - 1;
									}

									Tablature::Track::TrackBar &current_track_bar{ track._TrackBars[current_bar_index] };
									Tablature::Track::TrackBar::Event &current_event{ current_track_bar._Events[current_event_index] };

									if (current_event._StringIndex == note._StringIndex && current_event._FretIndex == note._FretIndex)
									{
										//TODO: Do proper calculation here.
										current_event._Duration += SoundUtilities::CalculateNoteDuration(rhythm._NoteDuration, rhythm._NoteType, 60.0);

										found = true;
									}

									else
									{
										--current_event_index;
									}
								}
							}

							else
							{
								//Add the new event.
								track_bar._Events.Emplace();
								Tablature::Track::TrackBar::Event& new_event{ track_bar._Events.Back() };

								//Set the offset.
								new_event._Offset = current_offset;

								//Calculate the duration.
								new_event._Duration = SoundUtilities::CalculateNoteDuration(rhythm._NoteDuration, rhythm._NoteType, 60.0);

								//Set the string index.
								new_event._StringIndex = note._StringIndex;

								//Set the fret index.
								new_event._FretIndex = note._FretIndex;

								//Set the articulation.
								new_event._Articulation = Tablature::Track::TrackBar::Event::Articulation::SUSTAIN;

								if (TEST_BIT(UNDERLYING(note._Flags), UNDERLYING(TemporaryData::Note::Flags::HOPO_DESTINATION)))
								{
									//TODO: Figure out if this is a hammer-on or a pull-off. (:
									new_event._Articulation = Tablature::Track::TrackBar::Event::Articulation::HAMMER_ON;
								}

								else if (TEST_BIT(UNDERLYING(note._Flags), UNDERLYING(TemporaryData::Note::Flags::PALM_MUTED)))
								{
									new_event._Articulation = Tablature::Track::TrackBar::Event::Articulation::PALM_MUTED;
								}
							}
						}

						//Update the current offset.
						current_offset += SoundUtilities::CalculateNoteDuration(rhythm._NoteDuration, rhythm._NoteType, 60.0);
					}
				}
			}
		}

		//Free the data.
		Memory::Free(data);

		//Close the current file.
		result = unzCloseCurrentFile(unzipped_file);

		if (result != UNZ_OK)
		{
			return false;
		}

		//Close the file.
		unzClose(unzipped_file);

		return true;
	}

private:

	/*
	*	Temporary data class definition.
	*/
	class TemporaryData final
	{

	public:

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

		};

		/*
		*	Note class definition.
		*/
		class Note final
		{

		public:

			//Enumeration covering all flags.
			enum class Flags : uint8
			{
				NONE = 0,
				HOPO_DESTINATION = BIT(0),
				PALM_MUTED = BIT(1),
				TIE_DESTINATION = BIT(2)
			};

			//The string index.
			uint8 _StringIndex;

			//The fret index.
			uint8 _FretIndex;

			//The flags.
			Flags _Flags;

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

	};

};