#ifndef	FFMPEG_MODULES_VIDEO_CODING_CODECS_H264_H264_DECODER_H_
#define FFMPEG_MODULES_VIDEO_CODING_CODECS_H264_H264_DECODER_H_

#include "modules/video_coding/codecs/h264/include/h264.h"
#include "common_video/h264/h264_bitstream_parser.h"
#include "common_video/include/i420_buffer_pool.h"

// FFmpeg version: 4.2.2
extern "C" {
#include "libavformat/avformat.h"
} // extenrn "C"

namespace ffmpeg {

//struct AVCodecContextDeleter {
//	void operator()(AVCodecContext* ptr) const { avcodec_free_context(&ptr); }
//};
//struct AVFrameDeleter {
//	void operator()(AVFrame* ptr) const { av_frame_free(&ptr); }
//};

class FFmpegH264Decoder : public webrtc::H264Decoder {
public:
	FFmpegH264Decoder();
	~FFmpegH264Decoder() override;

private:

	webrtc::I420BufferPool pool_;
	//std::unique_ptr<AVCodecContext, AVCodecContextDeleter> av_context_;
	//std::unique_ptr<AVFrame, AVFrameDeleter> av_frame_;

	webrtc::DecodedImageCallback* decoded_image_callback_;

	bool has_reported_init_;
	bool has_reported_error_;
};

} // ffmpeg

#endif  // MODULES_VIDEO_CODING_CODECS_H264_H264_DECODER_IMPL_H_
