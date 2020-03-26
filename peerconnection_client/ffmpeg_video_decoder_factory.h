#ifndef FFMPEG_BUILTIN_VIDEO_DECODER_FACTORY_H_
#define FFMPEG_BUILTIN_VIDEO_DECODER_FACTORY_H_

#include <memory>
#include "rtc_base/system/rtc_export.h"
#include "api/video_codecs/video_decoder_factory.h"

namespace ffmpeg {

	// Creates a new factory that can create the built-in types of video decoders.
	RTC_EXPORT std::unique_ptr<webrtc::VideoDecoderFactory>
		CreateBuiltinVideoDecoderFactory();

}  // namespace webrtc

#endif  // FFMPEG_BUILTIN_VIDEO_DECODER_FACTORY_H_