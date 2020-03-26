#ifndef EXTERNAL_VIDEO_DECODER_H_
#define EXTERNAL_VIDEO_DECODER_H_

#include "api/video_codecs/video_decoder.h"

namespace ffmpeg {

class ExternalDecoder : public webrtc::VideoDecoder {
public:
	ExternalDecoder();
	virtual ~ExternalDecoder() {}

	int32_t InitDecode(const webrtc::VideoCodec* config,
		int32_t number_of_cores) override;;

	int32_t Decode(const webrtc::EncodedImage& input,
		bool missing_frames,
		const webrtc::CodecSpecificInfo* codec_specific_info,
		int64_t render_time_ms) override;

	int32_t RegisterDecodeCompleteCallback(
		webrtc::DecodedImageCallback* callback) override;

	int32_t Release() override;

private:
	webrtc::DecodedImageCallback* callback_;
};

} // namespace ffmpeg

#endif