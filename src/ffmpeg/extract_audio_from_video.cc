#ifdef __cplusplus
#    define __STDC_CONSTANT_MACROS
extern "C" {
#endif
#include "libavutil/avutil.h"
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#ifdef __cplusplus
}
#endif

#include "cxxopts.hpp"
#include "spdlog/spdlog.h"
#include <filesystem>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, const char* argv[]) {

    cxxopts::Options options("libvideo", "extract video audio");
    options.add_options()("f,file", "file name", cxxopts::value<std::string>());

    auto opt         = options.parse(argc, argv);
    const char* path = opt["file"].as<std::string>().c_str();


    spdlog::info("initializing all the containers, codecs and protocols.");
    //构建按容器格式上下文（封装格式上下文）

    AVFormatContext* p_format_context = avformat_alloc_context();
    if (!p_format_context) {
        spdlog::info("ERROR could not allocate memory for Format Context");
        return -1;
    }
    spdlog::info("opening the input file {} and loading format (container) header", path);

    // 打开视频输入文件，返回格式（MP4 FLV...）上下文。This function reads the file header and
    // stores information about the file format in the AVFormatContext structure we have given it
    if (avformat_open_input(&p_format_context, path, NULL, NULL) != 0) {
        spdlog::error("ERROR could not open the file");
        return -1;
    }
    if (avformat_find_stream_info(p_format_context, NULL) < 0) {
        spdlog::error("could not get the stream info");
        return -1;
    }

    int i;
    int video_stream_index, audio_stream_index;
    // AVCodecContext* p_avcodec_contex;
    // 编解码器上下文
    const AVCodec* p_avcodec_video           = nullptr;
    AVCodecParameters* p_avcodec_param_video = nullptr;
    // 编解码器参数信息
    for (int i = 0; i < p_format_context->nb_streams; i++) {
        spdlog::info("AVStream duration {} ", p_format_context->streams[i]->duration);
        spdlog::info("AVStream frame rate {}/{}",
                     p_format_context->streams[i]->r_frame_rate.num,
                     p_format_context->streams[i]->r_frame_rate.den);

        AVCodecParameters* p_local_avcodec_param = NULL;
        p_local_avcodec_param                    = p_format_context->streams[i]->codecpar;
        // 找到注册的编解码器
        // 找到视频编码器信息
        const AVCodec* p_local_avcodec = avcodec_find_decoder(p_local_avcodec_param->codec_id);
        if (p_local_avcodec == nullptr) {
            spdlog::info("unsupported codec!");
            continue;
        }
        // 寻找视频流和语音流的index
        if (p_local_avcodec_param->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index    = i;
            p_avcodec_video       = p_local_avcodec;
            p_avcodec_param_video = p_local_avcodec_param;
            // 获取视频流的信息

            spdlog::info("Video Codec: resolution {}x{}  ",
                         p_avcodec_param_video->width,
                         p_avcodec_param_video->height);
        } else if (p_local_avcodec_param->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
            spdlog::info("Audio Codec:{} channels, sample rate {}",
                         p_avcodec_param_video->channels,
                         p_avcodec_param_video->sample_rate);
        }
    }
    AVFormatContext* p_output_format_ctx = avformat_alloc_context();
    if (!p_output_format_ctx) {
        spdlog::info("ERROR could not allocate memory for output Format Context");
        return -1;
    }
    AVOutputFormat* p_output_format = av_guess_format("wav",NULL,NULL);
    p_output_format->audio_codec=AV_CODEC_ID_WAVPACK




    return 0;
}
