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

    cxxopts::Options options("libvideo", "video proecessing arguement");
    options.add_options()("d,root_dir", "video root dir", cxxopts::value<std::string>());

    auto opt         = options.parse(argc, argv);
    const char* path = opt["file"].as<std::string>().c_str();


    spdlog::info("initializing all the containers, codecs and protocols.");
    AVFormatContext* pFormatContext = avformat_alloc_context();
    if (!pFormatContext) {
        spdlog::info("ERROR could not allocate memory for Format Context");
        return -1;
    }
    spdlog::info("opening the input file {} and loading format (container) header", path);

    if (avformat_open_input(&pFormatContext, path, NULL, NULL) != 0) {
        spdlog::error("ERROR could not open the file");
        return -1;
    }
    if (avformat_find_stream_info(pFormatContext, NULL) < 0) {
        spdlog::error("could not get the stream info");
        return -1;
    }

    int i;
    int video_stream_index, audio_stream_index;
    AVCodecContext* p_avcodec_contex;
    AVCodecParameters* p_avcodec_param = nullptr;
    for (int i = 0; i < pFormatContext->nb_streams; i++) {
        spdlog::info("AVStream duration {} ", pFormatContext->streams[i]->duration);
        spdlog::info("AVStream frame rate {}/{}",
                     pFormatContext->streams[i]->r_frame_rate.num,
                     pFormatContext->streams[i]->r_frame_rate.den);
        p_avcodec_param = pFormatContext->streams[i]->codecpar;
        // 找到注册的编解码器
        const AVCodec* p_local_avcodec = nullptr;

        // 找到视频编码器信息
        p_local_avcodec = avcodec_find_decoder(p_avcodec_param->codec_id);
        if (p_local_avcodec == nullptr) {
            spdlog::info("unsupported codec!");
            continue;
        }
        // 寻找视频流和语音流的index
        if (p_avcodec_param->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            spdlog::info(
                "Video Codec: resolution {}x{}  ", p_avcodec_param->width, p_avcodec_param->height);
        } else if (p_avcodec_param->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
            spdlog::info("Audio Codec:{} channels, sample rate {}",
                         p_avcodec_param->channels,
                         p_avcodec_param->sample_rate);
        }
    }

    return 0;
}
