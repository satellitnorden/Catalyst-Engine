/*
Copyright (c) 2015, Warren Galyen All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef FLAC_DECODER_H
#define FLAC_DECODER_H

// http://lists.xiph.org/pipermail/flac-dev/2012-March/003276.html
#define FLAC__NO_DLL

#include "AudioDecoder.h"
#include <map>

namespace soundwave
{
	
	//@todo expose this in API
	inline std::map<int, std::string> GetQualityTable()
	{
		return {
			{ 0, "0 (Fastest)" },
			{ 1, "1" },
			{ 2, "2" },
			{ 3, "3" },
			{ 4, "4" },
			{ 5, "5 (Default)" },
			{ 6, "6" },
			{ 7, "7" },
			{ 8, "8 (Highest)" },
			};
	}

	struct FlacDecoder final : public soundwave::BaseDecoder
	{
		FlacDecoder() = default;
		virtual ~FlacDecoder() override {};
		virtual void LoadFromPath(soundwave::AudioData *data, const std::string &path) override final;
		virtual void LoadFromBuffer(soundwave::AudioData *data, const std::vector<uint8_t> &memory) override final;
		virtual std::vector<std::string> GetSupportedFileExtensions() override final;
	};

} // end namespace soundwave

#endif
