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
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// decode packets into frames
static int decode_packet(AVPacket* pPacket, AVCodecContext* pCodecContext, AVFrame* pFrame);
// save a frame into a .pgm file
static void save_gray_frame(unsigned char* buf, int wrap, int xsize, int ysize, char* filename);

int main(int argc, const char* argv[]) {

    cxxopts::Options options("libvideo", "video proecessing arguement");
    options.add_options()("f,file", "File name", cxxopts::value<std::string>());

    auto opt         = options.parse(argc, argv);
    const char* path = opt["file"].as<std::string>().c_str();
    spdlog::info("initializing all the containers, codecs and protocols.");

    // // AVFormatContext holds the header information from the format (Container)
    // // Allocating memory for this component
    // // http://ffmpeg.org/doxygen/trunk/structAVFormatContext.html
    AVFormatContext* pFormatContext = avformat_alloc_context();
    if (!pFormatContext) {
        spdlog::info("ERROR could not allocate memory for Format Context");
        return -1;
    }
    spdlog::info("opening the input file {} and loading format (container) header", path);


    // // Open the file and read its header. The codecs are not opened.
    // // The function arguments are:
    // // AVFormatContext (the component we allocated memory for),
    // url (filename),
    // AVInputFormat (if you pass NULL it'll do the auto detect)
    // and AVDictionary (which are options to the demuxer)
    //
    // http: //
    // ffmpeg.org/doxygen/trunk/group__lavf__decoding.html#ga31d601155e9035d5b0e7efedc894ee49
    //     if (avformat_open_input(&pFormatContext, path, NULL, NULL) != 0) {
    //         spdlog::error("ERROR could not open the file");
    //         return -1;
    //     }

    //     // now we have access to some information about our file
    //     // since we read its header we can say what format (container) it's
    //     // and some other information related to the format itself.
    //     spdlog::info("format {}, duration {} us, bit_rate {}",
    //                  pFormatContext->iformat->name,
    //                  pFormatContext->duration,
    //                  pFormatContext->bit_rate);

    //     spdlog::info("finding stream info from format");
    // // read Packets from the Format to get stream information
    // // this function populates pFormatContext->streams
    // // (of size equals to pFormatContext->nb_streams)
    // // the arguments are:
    // // the AVFormatContext
    // // and options contains options for codec corresponding to i-th stream.
    // // On return each dictionary will be filled with options that were not found.
    // //
    // https:   //
    // ffmpeg.org/doxygen/trunk/group__lavf__decoding.html#gad42172e27cddafb81096939783b157bb
    //     if (avformat_find_stream_info(pFormatContext, NULL) < 0) {
    //         spdlog::info("ERROR could not get the stream info");
    //         return -1;
    //     }

    //     // the component that knows how to enCOde and DECode the stream
    //     // it's the codec (audio or video)
    //     // http://ffmpeg.org/doxygen/trunk/structAVCodec.html
    //     const AVCodec* pCodec = NULL;
    //     // this component describes the properties of a codec used by the stream i
    //     // https://ffmpeg.org/doxygen/trunk/structAVCodecParameters.html
    //     AVCodecParameters* pCodecParameters = NULL;
    //     int video_stream_index              = -1;

    //     // loop though all the streams and print its main information
    //     for (int i = 0; i < pFormatContext->nb_streams; i++) {
    //         AVCodecParameters* pLocalCodecParameters = NULL;
    //         pLocalCodecParameters                    = pFormatContext->streams[i]->codecpar;
    //         spdlog::info("AVStream->time_base before open coded %d/%d",
    //                      pFormatContext->streams[i]->time_base.num,
    //                      pFormatContext->streams[i]->time_base.den);
    //         spdlog::info("AVStream->r_frame_rate before open coded %d/%d",
    //                      pFormatContext->streams[i]->r_frame_rate.num,
    //                      pFormatContext->streams[i]->r_frame_rate.den);
    //         spdlog::info("AVStream->start_time %" PRId64,
    //         pFormatContext->streams[i]->start_time); spdlog::info("AVStream->duration %" PRId64,
    //         pFormatContext->streams[i]->duration);

    //         spdlog::info("finding the proper decoder (CODEC)");

    //         const AVCodec* pLocalCodec = NULL;

    //     // finds the registered decoder for a codec ID
    //     //
    //     https
    //         :   //
    //         ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga19a0ca553277f019dd5b0fec6e1f9dca
    //         pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

    //         if (pLocalCodec == NULL) {
    //             spdlog::info("ERROR unsupported codec!");
    //             // In this example if the codec is not found we just skip it
    //             continue;
    //         }

    //         // when the stream is a video we store its index, codec parameters and codec
    //         if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
    //             if (video_stream_index == -1) {
    //                 video_stream_index = i;
    //                 pCodec             = pLocalCodec;
    //                 pCodecParameters   = pLocalCodecParameters;
    //             }

    //             spdlog::info("Video Codec: resolution %d x %d",
    //                          pLocalCodecParameters->width,
    //                          pLocalCodecParameters->height);
    //         } else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
    //             spdlog::info("Audio Codec: %d channels, sample rate %d",
    //                          pLocalCodecParameters->channels,
    //                          pLocalCodecParameters->sample_rate);
    //         }

    //         // print its name, id and bitrate
    //         spdlog::info("\tCodec %s ID %d bit_rate %lld",
    //                      pLocalCodec->name,
    //                      pLocalCodec->id,
    //                      pLocalCodecParameters->bit_rate);
    //     }

    //     if (video_stream_index == -1) {
    //         spdlog::info("File %s does not contain a video stream!", argv[1]);
    //         return -1;
    //     }

    //     // https://ffmpeg.org/doxygen/trunk/structAVCodecContext.html
    //     AVCodecContext* pCodecContext = avcodec_alloc_context3(pCodec);
    //     if (!pCodecContext) {
    //         spdlog::info("failed to allocated memory for AVCodecContext");
    //         return -1;
    //     }

    // // Fill the codec context based on the values from the supplied codec parameters
    // //
    // https:   //
    // ffmpeg.org/doxygen/trunk/group__lavc__core.html#gac7b282f51540ca7a99416a3ba6ee0d16
    //     if (avcodec_parameters_to_context(pCodecContext, pCodecParameters) < 0) {
    //         spdlog::info("failed to copy codec params to codec context");
    //         return -1;
    //     }

    // // Initialize the AVCodecContext to use the given AVCodec.
    // //
    // https:   //
    // ffmpeg.org/doxygen/trunk/group__lavc__core.html#ga11f785a188d7d9df71621001465b0f1d
    //     if (avcodec_open2(pCodecContext, pCodec, NULL) < 0) {
    //         spdlog::info("failed to open codec through avcodec_open2");
    //         return -1;
    //     }

    //     // https://ffmpeg.org/doxygen/trunk/structAVFrame.html
    //     AVFrame* pFrame = av_frame_alloc();
    //     if (!pFrame) {
    //         spdlog::info("failed to allocate memory for AVFrame");
    //         return -1;
    //     }
    //     // https://ffmpeg.org/doxygen/trunk/structAVPacket.html
    //     AVPacket* pPacket = av_packet_alloc();
    //     if (!pPacket) {
    //         spdlog::info("failed to allocate memory for AVPacket");
    //         return -1;
    //     }

    //     int response                    = 0;
    //     int how_many_packets_to_process = 8;

    // // fill the Packet with data from the Stream
    // //
    // https:   //
    // ffmpeg.org/doxygen/trunk/group__lavf__decoding.html#ga4fdb3084415a82e3810de6ee60e46a61
    //     while (av_read_frame(pFormatContext, pPacket) >= 0) {
    //         // if it's the video stream
    //         if (pPacket->stream_index == video_stream_index) {
    //             spdlog::info("AVPacket->pts %" PRId64, pPacket->pts);
    //             response = decode_packet(pPacket, pCodecContext, pFrame);
    //             if (response < 0) break;
    //             // stop it, otherwise we'll be saving hundreds of frames
    //             if (--how_many_packets_to_process <= 0) break;
    //         }
    //     //
    //     https:   //
    //     ffmpeg.org/doxygen/trunk/group__lavc__packet.html#ga63d5a489b419bd5d45cfd09091cbcbc2
    //         av_packet_unref(pPacket);
    //     }

    //     spdlog::info("releasing all the resources");

    //     avformat_close_input(&pFormatContext);
    //     av_packet_free(&pPacket);
    //     av_frame_free(&pFrame);
    //     avcodec_free_context(&pCodecContext);
    //     return 0;
    // }

    // static int decode_packet(AVPacket* pPacket, AVCodecContext* pCodecContext, AVFrame* pFrame) {
    // // Supply raw packet data as input to a decoder
    // //
    // https:   //
    // ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga58bc4bf1e0ac59e27362597e467efff3
    //     int response = avcodec_send_packet(pCodecContext, pPacket);

    //     if (response < 0) {
    //         spdlog::info("Error while sending a packet to the decoder");
    //         return response;
    //     }

    //     while (response >= 0) {
    //     // Return decoded output data (into a frame) from a decoder
    //     //
    //     https
    //         :   //
    //         ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga11e6542c4e66d3028668788a1a74217c
    //         response = avcodec_receive_frame(pCodecContext, pFrame);
    //         if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
    //             break;
    //         } else if (response < 0) {
    //             spdlog::info("Error while receiving a frame from the decoder");
    //             return response;
    //         }

    //         if (response >= 0) {
    //                 spdlog::info(
    //                     "Frame %d (type=%c, size=%d bytes, format=%d) pts %d key_frame %d [DTS
    //                     %d]", pCodecContext->frame_number,
    //                     av_get_picture_type_char(pFrame->pict_type),
    //                     pFrame->pkt_size,
    //                     pFrame->format,
    //                     pFrame->pts,
    //                     pFrame->key_frame,
    //                     pFrame->coded_picture_number);

    //                 char frame_filename[1024];
    //                 snprintf(frame_filename,
    //                          sizeof(frame_filename),
    //                          "%s-%d.pgm",
    //                          "frame",
    //                          pCodecContext->frame_number);
    //                 // Check if the frame is a planar YUV 4:2:0, 12bpp
    //                 // That is the format of the provided .mp4 file
    //                 // RGB formats will definitely not give a gray image
    //                 // Other YUV image may do so, but untested, so give a warning
    //                 if (pFrame->format != AV_PIX_FMT_YUV420P) {
    //                     spdlog::info(
    //                         "Warning: the generated file may not be a grayscale image, but could
    //                         e.g. " "be just the R component if the video format is RGB");
    //                 }
    //                 // save a grayscale frame into a .pgm file
    //                 save_gray_frame(pFrame->data[0],
    //                                 pFrame->linesize[0],
    //                                 pFrame->width,
    //                                 pFrame->height,
    //                                 frame_filename);
    //         }
    //     }
    return 0;
}

// static void save_gray_frame(unsigned char* buf, int wrap, int xsize, int ysize, char* filename) {
//     FILE* f;
//     int i;
//     f = fopen(filename, "w");
//     // writing the minimal required header for a pgm file format
//     // portable graymap format -> https://en.wikipedia.org/wiki/Netpbm_format#PGM_example
//     fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);

//     // writing line by line
//     for (i = 0; i < ysize; i++) fwrite(buf + i * wrap, 1, xsize, f);
//     fclose(f);
// }