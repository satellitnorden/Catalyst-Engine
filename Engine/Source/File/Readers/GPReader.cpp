//Header file.
#include <File/Readers/GPReader.h>

//Third party.
#include <ThirdParty/Minizip/minizip/unzip.h>
#include <ThirdParty/pugixml/pugixml.hpp>

/*
*	Reads the tablature at the given file path. Returns if the read was succesful.
*/
NO_DISCARD bool GPReader::Read(const char *const RESTRICT file_path, Tablature *const RESTRICT tablature) NOEXCEPT
{
	PROFILING_SCOPE("GPReader_Read");

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
		constexpr const char* const RESTRICT FILE_POSTFIX{ "_debug.xml" };
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
			{
				const pugi::xml_node bars_node{ master_bar_node.child("Bars") };
				const pugi::char_t *RESTRICT bars_string{ bars_node.child_value() };

				temporary_data._TrackToBarMappings.Emplace();
				DynamicArray<int32> &track_to_bar_mapping{ temporary_data._TrackToBarMappings.Back() };

				track_to_bar_mapping.Emplace(std::stoi(bars_string));

				while (*bars_string)
				{
					if ((*bars_string) == ' ')
					{
						++bars_string;
						track_to_bar_mapping.Emplace(std::stoi(bars_string));
					}

					else
					{
						++bars_string;
					}
				}
			}

			{
				const pugi::xml_node time_node{ master_bar_node.child("Time") };

				tablature->_Bars.Emplace();
				Tablature::Bar &new_bar{ tablature->_Bars.Back() };

				new_bar._Tempo = FLOAT64_MAXIMUM; //Will be filled in when we read the master track data. (:
				const pugi::char_t *RESTRICT numerator_string{ time_node.child_value() };
				new_bar._TimeSignatureNumerator = static_cast<uint8>(std::stoul(numerator_string));
				const pugi::char_t *RESTRICT denominator_string{ numerator_string };
				while (*denominator_string) { if ((*denominator_string) == '/') { ++denominator_string; break; } else { ++denominator_string; } }
				new_bar._TimeSignatureDenominator = std::stoul(denominator_string);
			}

			//Add in repeat regions.
			if (const pugi::xml_node repeat_node{ master_bar_node.child("Repeat") })
			{
				const bool is_start{ repeat_node.attribute("start").as_bool() };
				const bool is_end{ repeat_node.attribute("end").as_bool() };
				const uint32 count{ repeat_node.attribute("count").as_uint() };

				if (is_start)
				{
					ASSERT(!is_end, "Repeat can't be both start and end!");

					temporary_data._RepeatRegions.Emplace();
					TemporaryData::RepeatRegion &new_repeat_region{ temporary_data._RepeatRegions.Back() };

					new_repeat_region._StartBarIndex = tablature->_Bars.LastIndex();
					new_repeat_region._EndBarIndex = UINT64_MAXIMUM;
					new_repeat_region._Count = count;
				}

				else if (is_end)
				{
					ASSERT(!is_start, "Repeat can't be both start and end!");

					TemporaryData::RepeatRegion &repeat_region{ temporary_data._RepeatRegions.Back() };

					ASSERT(repeat_region._EndBarIndex == UINT64_MAXIMUM, "Doubly-closed repeat region!");

					repeat_region._EndBarIndex = tablature->_Bars.LastIndex();
					if (repeat_region._Count == 0) repeat_region._Count = count;
					else ASSERT(repeat_region._Count == count, "Mismatching count!");
				}

				else
				{
					ASSERT(false, "Repeat is neither start nor end. /:");
				}
			}
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

			//Initialize the last played note for this track.
			temporary_data._LastPlayedNotes.Emplace();
			DynamicArray<TemporaryData::LastPlayedNote> &last_played_notes{ temporary_data._LastPlayedNotes.Back() };

			for (uint64 string_index{ 0 }; string_index < new_track._Tuning.Size(); ++string_index)
			{
				last_played_notes.Emplace();
				TemporaryData::LastPlayedNote &last_played_note{ last_played_notes.Back() };

				last_played_note._FretIndex = -1;
				last_played_note._WasTieOrigin = false;
				last_played_note._WasLegatoSlideOrigin = false;
				last_played_note._BarIndex = UINT64_MAXIMUM;
				last_played_note._EventIndex = UINT64_MAXIMUM;
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
			const pugi::char_t *RESTRICT beats_string{ voice_node.child("Beats").child_value() };

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

			//Read the text.
			{
				const pugi::xml_node free_text_node{ beat_node.child("FreeText") };

				if (free_text_node)
				{
					const pugi::char_t *RESTRICT free_text_string{ free_text_node.child_value() };

					new_beat._Text = free_text_string;
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
				if (StringUtilities::IsEqual(tie_node.attribute("origin").value(), "true"))
				{
					new_note._Flags = static_cast<TemporaryData::Note::Flags>(UNDERLYING(new_note._Flags) | UNDERLYING(TemporaryData::Note::Flags::TIE_ORIGIN));
				}

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

				else if (StringUtilities::IsEqual(property_node.attribute("name").value(), "String"))
				{
					//Set the string index.
					new_note._StringIndex = static_cast<uint8>(std::stoul(property_node.child("String").child_value()));
				}

				else if (StringUtilities::IsEqual(property_node.attribute("name").value(), "HarmonicType"))
				{
					const pugi::xml_node h_type_node{ property_node.child("HType") };

					if (StringUtilities::IsEqual(h_type_node.child_value(), "Natural"))
					{
						new_note._Flags = static_cast<TemporaryData::Note::Flags>(UNDERLYING(new_note._Flags) | UNDERLYING(TemporaryData::Note::Flags::NATURAL_HARMONIC));
					}

					//TODO: Handle pinch harmonics.
				}

				else if (StringUtilities::IsEqual(property_node.attribute("name").value(), "Muted"))
				{
					//Add the flag.
					new_note._Flags = static_cast<TemporaryData::Note::Flags>(UNDERLYING(new_note._Flags) | UNDERLYING(TemporaryData::Note::Flags::DEAD));
				}

				else if (StringUtilities::IsEqual(property_node.attribute("name").value(), "HopoDestination"))
				{
					//Add the flag.
					new_note._Flags = static_cast<TemporaryData::Note::Flags>(UNDERLYING(new_note._Flags) | UNDERLYING(TemporaryData::Note::Flags::HOPO_DESTINATION));
				}

				else if (StringUtilities::IsEqual(property_node.attribute("name").value(), "PalmMuted"))
				{
					//Add the flag.
					new_note._Flags = static_cast<TemporaryData::Note::Flags>(UNDERLYING(new_note._Flags) | UNDERLYING(TemporaryData::Note::Flags::PALM_MUTED));
				}

				else if (StringUtilities::IsEqual(property_node.attribute("name").value(), "Slide"))
				{
					constexpr uint32 LEGATO_SLIDE_BIT{ BIT(1) };
					constexpr uint32 SLIDE_OUT_BIT{ BIT(2) };

					const uint32 flags{ std::stoul(property_node.child("Flags").child_value()) };

					//Add the flag.
					if (TEST_BIT(flags, LEGATO_SLIDE_BIT)) //Legato slide.
					{
						new_note._Flags = static_cast<TemporaryData::Note::Flags>(UNDERLYING(new_note._Flags) | UNDERLYING(TemporaryData::Note::Flags::LEGATO_SLIDE));
					}

					if (TEST_BIT(flags, SLIDE_OUT_BIT)) //Slide out.
					{
						new_note._Flags = static_cast<TemporaryData::Note::Flags>(UNDERLYING(new_note._Flags) | UNDERLYING(TemporaryData::Note::Flags::SLIDE_OUT));
					}
				}

				else if (StringUtilities::IsEqual(property_node.attribute("name").value(), "BendDestinationOffset")
					|| StringUtilities::IsEqual(property_node.attribute("name").value(), "BendMiddleOffset1"))
				{
					const pugi::xml_node float_node{ property_node.child("Float") };
					const float32 value{ std::stof(float_node.child_value()) };

					new_note._BendOffsets.Emplace(value / 100.0f);
				}

				else if (StringUtilities::IsEqual(property_node.attribute("name").value(), "BendDestinationValue")
					|| StringUtilities::IsEqual(property_node.attribute("name").value(), "BendMiddleValue"))
				{
					const pugi::xml_node float_node{ property_node.child("Float") };
					const float32 value{ std::stof(float_node.child_value()) };

					new_note._BendValues.Emplace(value / 100.0f * 4.0f);
				}

				else if (StringUtilities::IsEqual(property_node.attribute("name").value(), "Tapped"))
				{
					//Add the flag.
					new_note._Flags = static_cast<TemporaryData::Note::Flags>(UNDERLYING(new_note._Flags) | UNDERLYING(TemporaryData::Note::Flags::TAPPED));
				}
			}

			ASSERT(new_note._BendOffsets.Size() == new_note._BendValues.Size(), "Bend offset/value mismatch!");

			//Sort the bend offsets/values.
			if (!new_note._BendOffsets.Empty())
			{
				for (uint64 iterator{ 1 }; iterator != new_note._BendOffsets.Size(); ++iterator)
				{
					uint64 reverse_iterator{ iterator };

					while (reverse_iterator != 0 && new_note._BendOffsets[reverse_iterator] < new_note._BendOffsets[reverse_iterator - 1])
					{
						Swap(&new_note._BendOffsets[reverse_iterator], &new_note._BendOffsets[reverse_iterator - 1]);
						Swap(&new_note._BendValues[reverse_iterator], &new_note._BendValues[reverse_iterator - 1]);

						--reverse_iterator;
					}
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

			//Retrieve the track to bar mapping.
			const int32 track_to_bar_mapping{ temporary_data._TrackToBarMappings[bar_index][track_index] };

			//Cache the bar to voices mapping.
			const DynamicArray<int32> &bar_to_voice_mappings{ temporary_data._BarToVoiceMappings[track_to_bar_mapping] };

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

						//Cache the last played note on this string.
						TemporaryData::LastPlayedNote &last_played_note{ temporary_data._LastPlayedNotes[track_index][note._StringIndex] };

						//If the last played note was a tie origin, simply extend it.
						if (last_played_note._WasTieOrigin && TEST_BIT(UNDERLYING(note._Flags), UNDERLYING(TemporaryData::Note::Flags::TIE_DESTINATION)))
						{
							//Cache the tie origin track bar/event.
							Tablature::Track::TrackBar &tie_origin_track_bar{ track._TrackBars[last_played_note._BarIndex] };
							Tablature::Track::TrackBar::Event &tie_origin_event{ tie_origin_track_bar._Events[last_played_note._EventIndex] };

							//If the tie origin event was from the same bar, the duration is simply the current offset minus the offset of the tie origin event.
							if (last_played_note._BarIndex == bar_index)
							{
								tie_origin_event._Duration = (current_offset + SoundUtilities::CalculateNoteDuration(rhythm._NoteDuration, rhythm._NoteType, 60.0)) - tie_origin_event._Offset;
							}

							//Otherwise, add up the duration!
							else
							{
								tie_origin_event._Duration = 0.0;

								//Add the duration from the start of the tie origin event to the end of it's bar.
								tie_origin_event._Duration += CalculateDurationOfBar(*tie_origin_track_bar._Bar) - tie_origin_event._Offset;

								//Add the duration of all in-between bars. This needs to be relative to the tie origin event's tempo, so include that.
								for (uint64 in_between_index{ last_played_note._BarIndex + 1 }; in_between_index < bar_index; ++in_between_index)
								{
									const Tablature::Bar &in_between_bar{ tablature->_Bars[in_between_index] };

									tie_origin_event._Duration += CalculateDurationOfBar(in_between_bar) * (in_between_bar._Tempo / tie_origin_track_bar._Bar->_Tempo);
								}

								//Finally, add the current offset, again taking into consideration tempo changes.
								tie_origin_event._Duration += (current_offset + SoundUtilities::CalculateNoteDuration(rhythm._NoteDuration, rhythm._NoteType, 60.0)) * (bar._Tempo / tie_origin_track_bar._Bar->_Tempo);
							}
						}

						//Otherwise, add a new event!
						else
						{
							//Add the new event.
							track_bar._Events.Emplace();
							Tablature::Track::TrackBar::Event &new_event{ track_bar._Events.Back() };

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

							if (TEST_BIT(UNDERLYING(note._Flags), UNDERLYING(TemporaryData::Note::Flags::NATURAL_HARMONIC)))
							{
								new_event._Articulation = Tablature::Track::TrackBar::Event::Articulation::NATURAL_HARMONIC;
							}

							else if (TEST_BIT(UNDERLYING(note._Flags), UNDERLYING(TemporaryData::Note::Flags::HOPO_DESTINATION)))
							{
								//Figure out if this note is a hammer on or a pull off.
								if (last_played_note._FretIndex < note._FretIndex)
								{
									new_event._Articulation = Tablature::Track::TrackBar::Event::Articulation::HAMMER_ON;
								}

								else
								{
									new_event._Articulation = Tablature::Track::TrackBar::Event::Articulation::PULL_OFF;
								}
							}

							else if (TEST_BIT(UNDERLYING(note._Flags), UNDERLYING(TemporaryData::Note::Flags::PALM_MUTED)))
							{
								new_event._Articulation = Tablature::Track::TrackBar::Event::Articulation::PALM_MUTED;
							}

							else if (TEST_BIT(UNDERLYING(note._Flags), UNDERLYING(TemporaryData::Note::Flags::TAPPED)))
							{
								new_event._Articulation = Tablature::Track::TrackBar::Event::Articulation::TAPPED;
							}

							else if (TEST_BIT(UNDERLYING(note._Flags), UNDERLYING(TemporaryData::Note::Flags::DEAD)))
							{
								new_event._Articulation = Tablature::Track::TrackBar::Event::Articulation::DEAD;
							}

							//Set the bend offsets/values.
							new_event._BendOffsets = note._BendOffsets;
							new_event._BendValues = note._BendValues;

							//Set whether or not this is a slide event.
							new_event._IsSlideEvent = last_played_note._WasLegatoSlideOrigin;

							//Set whether or not this event slides out.
							new_event._SlideOut = TEST_BIT(UNDERLYING(note._Flags), UNDERLYING(TemporaryData::Note::Flags::SLIDE_OUT));

							//Set the text.
							new_event._Text = beat._Text;

							//Update the last played note.
							last_played_note._FretIndex = note._FretIndex;
							last_played_note._WasTieOrigin = TEST_BIT(UNDERLYING(note._Flags), UNDERLYING(TemporaryData::Note::Flags::TIE_ORIGIN));
							last_played_note._WasLegatoSlideOrigin = TEST_BIT(UNDERLYING(note._Flags), UNDERLYING(TemporaryData::Note::Flags::LEGATO_SLIDE));
							last_played_note._BarIndex = bar_index;
							last_played_note._EventIndex = track_bar._Events.LastIndex();
						}
					}

					//Update the current offset.
					current_offset += SoundUtilities::CalculateNoteDuration(rhythm._NoteDuration, rhythm._NoteType, 60.0);
				}
			}
		}
	}

	//Apply the repeat regions.
	if (!temporary_data._RepeatRegions.Empty())
	{
		for (int64 repeat_region_index{ static_cast<int64>(temporary_data._RepeatRegions.LastIndex()) }; repeat_region_index >= 0; --repeat_region_index)
		{
			const TemporaryData::RepeatRegion &repeat_region{ temporary_data._RepeatRegions[repeat_region_index] };

			uint64 current_insert_index{ repeat_region._EndBarIndex + 1 };

			for (uint32 count_index{ 0 }; count_index < (repeat_region._Count - 1); ++count_index)
			{
				for (uint64 repeat_bar_index{ repeat_region._StartBarIndex }; repeat_bar_index < (repeat_region._EndBarIndex + 1); ++repeat_bar_index)
				{
					Tablature::Bar bar_to_insert{ tablature->_Bars[repeat_bar_index] };
					tablature->_Bars.Insert(bar_to_insert, current_insert_index);

					for (Tablature::Track &track : tablature->_Tracks)
					{
						Tablature::Track::TrackBar bar_to_insert{ track._TrackBars[repeat_bar_index] };
						track._TrackBars.Insert(bar_to_insert, current_insert_index);
					}

					++current_insert_index;
				}
			}
		}

		//Resolve all bars.
		for (Tablature::Track &track : tablature->_Tracks)
		{
			for (uint64 bar_index{ 0 }; bar_index < track._TrackBars.Size(); ++bar_index)
			{
				track._TrackBars[bar_index]._Bar = &tablature->_Bars[bar_index];
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