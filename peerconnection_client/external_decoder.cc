#include "external_decoder.h"
#include "modules/video_coding/include/video_error_codes.h"
#include "api/video/i420_buffer.h"

namespace ffmpeg {

ExternalDecoder::ExternalDecoder()
	: callback_(nullptr)
{

}

int32_t ExternalDecoder::InitDecode(const webrtc::VideoCodec* config,
	int32_t number_of_cores) 
{
	// init decoder
	return WEBRTC_VIDEO_CODEC_OK;
}

int32_t ExternalDecoder::Decode(const webrtc::EncodedImage& input,
	bool missing_frames,
	const webrtc::CodecSpecificInfo* codec_specific_info,
	int64_t render_time_ms) 
{
	// Decoding with external decoder
	// frame info: input.data(), input.size()

	webrtc::VideoFrame frame =
		webrtc::VideoFrame::Builder()
		.set_video_frame_buffer(webrtc::I420Buffer::Create(input._encodedWidth, input._encodedHeight))
		.set_rotation(webrtc::kVideoRotation_0)
		.set_timestamp_ms(render_time_ms)
		.build();

	frame.set_timestamp(input.Timestamp());
	frame.set_ntp_time_ms(input.ntp_time_ms_);

	callback_->Decoded(frame);

	// request idr
	//return WEBRTC_VIDEO_CODEC_OK_REQUEST_KEYFRAME;
	return WEBRTC_VIDEO_CODEC_OK;
}

int32_t ExternalDecoder::RegisterDecodeCompleteCallback(
	webrtc::DecodedImageCallback* callback) 
{
	callback_ = callback;
	return WEBRTC_VIDEO_CODEC_OK;
}

int32_t ExternalDecoder::Release() 
{
	// destroy decoder
	return WEBRTC_VIDEO_CODEC_OK;
}

} // namespace ffmpeg