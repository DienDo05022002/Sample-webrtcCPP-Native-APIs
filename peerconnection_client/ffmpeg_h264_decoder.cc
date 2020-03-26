#include "ffmpeg_h264_decoder.h"

namespace ffmpeg {

FFmpegH264Decoder::FFmpegH264Decoder()
	: pool_(true)
	, decoded_image_callback_(nullptr)
	, has_reported_init_(false)
	, has_reported_error_(false)
{

}

FFmpegH264Decoder::~FFmpegH264Decoder()
{
	
}

}