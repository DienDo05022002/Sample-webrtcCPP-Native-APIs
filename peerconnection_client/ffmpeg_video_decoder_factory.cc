#include "ffmpeg_video_decoder_factory.h"
#include "absl/memory/memory.h"
#include "media/engine/internal_decoder_factory.h"
#include "rtc_base/logging.h"
#include "absl/strings/match.h"
#include "modules/video_coding/codecs/h264/include/h264.h"
#include "modules/video_coding/codecs/vp8/include/vp8.h"
#include "modules/video_coding/codecs/vp9/include/vp9.h"
#include "external_decoder.h"

namespace ffmpeg {

bool IsFormatSupported(
	const std::vector<webrtc::SdpVideoFormat>& supported_formats,
	const webrtc::SdpVideoFormat& format) {
	for (const webrtc::SdpVideoFormat& supported_format : supported_formats) {
		if (cricket::IsSameCodec(format.name, format.parameters,
			supported_format.name,
			supported_format.parameters)) {
			return true;
		}
	}
	return false;
}

class FFmpegInternalDecoderFactory : public webrtc::VideoDecoderFactory {
public:
	std::vector<webrtc::SdpVideoFormat> GetSupportedFormats() 
		const override {
		std::vector<webrtc::SdpVideoFormat> formats;
		for (const webrtc::SdpVideoFormat& h264_format : webrtc::SupportedH264Codecs())
			formats.push_back(h264_format);
		return formats;
	}

	std::unique_ptr<webrtc::VideoDecoder> CreateVideoDecoder(
		const webrtc::SdpVideoFormat& format) override {
		if (!IsFormatSupported(GetSupportedFormats(), format)) {
			RTC_LOG(LS_ERROR) << "Trying to create decoder for unsupported format";
			return nullptr;
		}

		//if (absl::EqualsIgnoreCase(format.name, cricket::kVp8CodecName))
		//	return webrtc::VP8Decoder::Create();
		//if (absl::EqualsIgnoreCase(format.name, cricket::kVp9CodecName))
		//	return webrtc::VP9Decoder::Create();
		if (absl::EqualsIgnoreCase(format.name, cricket::kH264CodecName))
			return webrtc::H264Decoder::Create();

		RTC_NOTREACHED();
		return nullptr;
	}
};

std::unique_ptr<webrtc::VideoDecoderFactory> CreateBuiltinVideoDecoderFactory() {
	return absl::make_unique<FFmpegInternalDecoderFactory>();
}

}
