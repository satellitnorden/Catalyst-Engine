#pragma once

//Constants.
#define INCLUDE_CODE (0)

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//File.
#include <File/Core/BinaryFile.h>

//Resources.
#include <Resources/Core/VideoResource.h>

#if INCLUDE_CODE
//Third party.
extern "C"
{
#include <ThirdParty/libavcodec/avcodec.h>
#include <ThirdParty/libavformat/avformat.h>
#include <ThirdParty/libswscale/swscale.h>
}
#endif

namespace MP4Reader
{

	/*
	*	Reads the video at the given file path. Returns if the read was succesful.
	*/
	FORCE_INLINE static NO_DISCARD bool Read(const char *const RESTRICT file_path, VideoResource *const RESTRICT video_resource) NOEXCEPT
	{
#if INCLUDE_CODE
		//Allocate the format context.
		AVFormatContext *RESTRICT format_context{ avformat_alloc_context() };

		if (!format_context)
		{
			ASSERT(false, "avformat_alloc_context() failed.");

			return false;
		}

		//Open the file.
		if (avformat_open_input(&format_context, file_path, nullptr, nullptr) != 0)
		{
			ASSERT(false, "avformat_open_input() failed.");

			return false;
		}

		//Iterate over all the streams to finds the audio/video stream index.
		int32 audio_stream_index{ -1 };
		AVStream *RESTRICT audio_stream{ nullptr };
		AVCodec *RESTRICT audio_decoder{ nullptr };
		int32 video_stream_index{ -1 };
		AVStream *RESTRICT video_stream{ nullptr };
		AVCodec *RESTRICT video_decoder{ nullptr };

		for (uint32 stream_index{ 0 }; stream_index < format_context->nb_streams; ++stream_index)
		{
			//Cache the stream.
			AVStream* const RESTRICT stream{ format_context->streams[stream_index] };

			//Find the decoder.
			AVCodec *const RESTRICT decoder{ avcodec_find_decoder(stream->codecpar->codec_id) };

			//Is this the audio stream?
			if (audio_stream_index == -1
				&& decoder->type == AVMEDIA_TYPE_AUDIO)
			{
				audio_stream_index = static_cast<int32>(stream_index);
				audio_stream = stream;
				audio_decoder = decoder;
			}

			//Is this the video stream?
			if (video_stream_index == -1
				&& decoder->type == AVMEDIA_TYPE_VIDEO)
			{
				video_stream_index = static_cast<int32>(stream_index);
				video_stream = stream;
				video_decoder = decoder;
			}
		}

		if (audio_stream_index == -1
			|| video_stream_index == -1)
		{
			ASSERT(false, "Couldn't find video stream index.");

			return false;
		}

		//Process the video.
		{
			AVCodecContext *RESTRICT video_codec_context{ avcodec_alloc_context3(video_decoder) };

			if (!video_codec_context)
			{
				ASSERT(false, "avcodec_alloc_context3() failed.");

				return false;
			}

			if (avcodec_parameters_to_context(video_codec_context, video_stream->codecpar) < 0)
			{
				ASSERT(false, "avcodec_parameters_to_context() failed.");

				return false;
			}

			if (avcodec_open2(video_codec_context, video_decoder, nullptr) < 0)
			{
				ASSERT(false, "avcodec_open2() failed.");

				return false;
			}

			AVFrame *RESTRICT current_frame{ av_frame_alloc() };

			if (!current_frame)
			{
				ASSERT(false, "av_frame_alloc() failed.");

				return false;
			}

			AVPacket* const RESTRICT current_packet{ av_packet_alloc() };

			if (!current_packet)
			{
				ASSERT(false, "av_packet_alloc() failed.");

				return false;
			}

			while (av_read_frame(format_context, current_packet) >= 0)
			{
				if (current_packet->stream_index != video_stream_index)
				{
					continue;
				}

				if (avcodec_send_packet(video_codec_context, current_packet) < 0)
				{
					ASSERT(false, "avcodec_send_packet() failed.");

					return false;
				}

				const int32 response{ avcodec_receive_frame(video_codec_context, current_frame) };

				if (response == AVERROR(EAGAIN) || response == AVERROR_EOF)
				{
					continue;
				}

				else if (response < 0)
				{
					ASSERT(false, "avcodec_receive_frame() failed.");

					return false;
				}

				video_resource->_Width = current_frame->width;
				video_resource->_Height = current_frame->height;
				video_resource->_FramesPerSecond = static_cast<float32>(video_stream->avg_frame_rate.num / 1'000);
				video_resource->_FramesPerSecondReciprocal = 1.0f / video_resource->_FramesPerSecond;

				video_resource->_Frames.Emplace();
				VideoResource::Frame& new_frame{ video_resource->_Frames.Back() };

				new_frame._DataX.Initialize(current_frame->width, current_frame->height);
				new_frame._DataY.Initialize(current_frame->width, current_frame->height);
				new_frame._DataZ.Initialize(current_frame->width, current_frame->height);

				SwsContext* const RESTRICT sws_context{	sws_getContext(	current_frame->width,
																		current_frame->height,
																		video_codec_context->pix_fmt,
																		current_frame->width,
																		current_frame->height,
																		AV_PIX_FMT_RGB0,
																		SWS_BILINEAR,
																		nullptr,
																		nullptr,
																		nullptr) };

				if (!sws_context)
				{
					ASSERT(false, "sws_getContext() failed.");

					return false;
				}

				uint8 *const RESTRICT destination_data{ new uint8[current_frame->width * current_frame->height * 4] };
				uint8 *const RESTRICT destionation_proxy[4]{ destination_data, nullptr, nullptr, nullptr };
				int32 destination_linesizes[4]{ current_frame->width * 4, 0, 0, 0 };

				sws_scale(	sws_context,
							current_frame->data,
							current_frame->linesize,
							0,
							current_frame->height,
							destionation_proxy,
							destination_linesizes);

				for (int32 Y{ 0 }; Y < current_frame->height; ++Y)
				{
					for (int32 X{ 0 }; X < current_frame->width; ++X)
					{
						new_frame._DataX.At(X, Y) = destination_data[(Y * current_frame->width + X) * 4 + 0];
						new_frame._DataY.At(X, Y) = destination_data[(Y * current_frame->width + X) * 4 + 1];
						new_frame._DataZ.At(X, Y) = destination_data[(Y * current_frame->width + X) * 4 + 2];
					}
				}

				delete destination_data;
				sws_freeContext(sws_context);
				av_packet_unref(current_packet);
			}

			//Deallocate temporary data.
			av_frame_free(&current_frame);
			avcodec_free_context(&video_codec_context);
		}
		
		//Deallocate temporary data.
		avformat_close_input(&format_context);
		avformat_free_context(format_context);

		return true;
#else
		return false;
#endif
	}

}