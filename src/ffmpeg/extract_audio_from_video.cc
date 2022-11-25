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
    const AVCodec* p_avcodec_input           = nullptr;
    AVCodecParameters* p_avcodec_param_input = nullptr;
    // 编解码器参数信息
    for (int i = 0; i < p_format_context->nb_streams; i++) {
        spdlog::info("AVStream duration {} ", p_format_context->streams[i]->duration);
        spdlog::info("AVStream frame rate {}/{}",
                     p_format_context->streams[i]->r_frame_rate.num,
                     p_format_context->streams[i]->r_frame_rate.den);

        AVCodecParameters* p_avcodec_param_decoder = NULL;
        p_avcodec_param_decoder                    = p_format_context->streams[i]->codecpar;
        // 找到注册的编解码器
        // 找到视频编码器信息
        const AVCodec* p_avcodec_decoder = avcodec_find_decoder(p_avcodec_param_decoder->codec_id);
        if (p_avcodec_decoder == nullptr) {
            spdlog::info("unsupported codec!");
            continue;
        }
        // 寻找视频流和语音流的index
        if (p_avcodec_param_decoder->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            // 获取视频流的信息

            spdlog::info("Video Codec: resolution {}x{}  ",
                         p_avcodec_param_decoder->width,
                         p_avcodec_param_decoder->height);
        } else if (p_avcodec_param_decoder->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
            spdlog::info("Audio Codec:{} channels, sample rate {}",
                         p_avcodec_param_decoder->channels,
                         p_avcodec_param_decoder->sample_rate);
        }
    }

    // 从流中获取视频的编解码器上下文
    // 利用编解码器获取编解码器的上下文，为其分配内存
    AVCodecContext* p_codec_context_input = avcodec_alloc_context3(p_avcodec_input);
    if (!p_codec_context_input) {
        spdlog::info("failed to allocated memory for AVCodecContext");
        return -1;
    }
    // 使用编解码器参数初始化上下文
    if (avcodec_parameters_to_context(p_codec_context_input, p_avcodec_param_input) < 0) {
        spdlog::info("failed to copy codec params to codec context");
        return -1;
    }

    // 打开编解码器，初始化编解码器的上下文
    if (avcodec_open2(p_codec_context_input, p_avcodec_input, NULL) < 0) {
        spdlog::info("failed initial codec context using avcode_open2");
    };
    // 输出封装格式上下文
    // char outputfile[]                      = "demo.wav";
    // AVFormatContext* output_format_context = nullptr;

    // if (avformat_alloc_output_context2(&output_format_context, NULL, NULL, outputfile) < 0) {
    //     spdlog::error("alloc output context error");
    //     return -1;
    // };
    // if (!output_format_context) {
    //     spdlog::error("alloc output format contex error");
    //     return -1;
    // }
    // AVStream* audio_stream = nullptr;
    // audio_stream           = avformat_new_stream(output_format_context, NULL);
    // if (!audio_stream) {
    //     spdlog::error("Failed allocating output stream");
    //     return -1;
    // }
    // // 获取编码器
    // AVCodec* output_encode               = avcodec_find_encoder(AV_CODEC_ID_AAC);
    // AVCodecContext* output_codec_context = nullptr;
    // // 构建输出的编码器的上下文
    // we can copy the streams, packet by packet, from our input to our output streams

    //读取视频文件
    AVFrame* p_frame = av_frame_alloc();
    if (p_frame == nullptr) {
        spdlog::info("alloc frame memory failed");
    }

    // 通过读取包来读取整个视频流，然后把它解码成帧，最后转换格式并且保存。
    AVPacket* p_packet = av_packet_alloc();
    if (p_packet == nullptr) {
        spdlog::info("alloc packet memory failed");
    }
    int response = 0;
    while (av_read_frame(p_format_context, p_packet) >= 0) {
        if (p_packet->stream_index == audio_stream_index) {

            // //  Supply raw packet data as input to a decoder向视频解码器发送packet
            // int response = avcodec_send_packet(p_codec_context_input, p_packet);
            // if (response < 0) {
            //     spdlog::info("avcodec send packet error");
            //     return response;
            // }

            // while (response >= 0) {
            //     response = avcodec_receive_frame(p_codec_context_input, p_frame);

            //     if (response < 0) {
            //         spdlog::info("avcodec receive frame error");
            //         return response;
            //     }
            // }
        }
    }




    return 0;
}
