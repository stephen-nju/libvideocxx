/**
 * @file frame.h
 * @author zhubin
 * @brief
 * @version 0.1
 * @date 2022-08-31
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef SRC_FMMPEG_FRAME_H_
#define SRC_FMMPEG_FRAME_H_

#include <memory>
#include <string>
#include <vector>

namespace libvideo {
enum MediaType { UNKNOWN = -1, AUDIO, VIDEO };

enum PixelType {
    PixelTypeUnknown = -1,
    PixelTypeBGR24   = 3,
    PixelTypeBGR32   = 298,
    PixelTypeYUV420P = 0,
    PixelTypeNV12    = 25
};

typedef std::pair<std::string, std::string> Option;

enum SampleType {
    SampleTypeUnknown = -1,
    SampleType8U,
    SampleType16S,
    SampleType32S,
    SampleType32F,
    SampleType64F,
    SampleType8UP,
    SampleType16SP,
    SampleType32SP,
    SampleType32FP,
    SampleType64FP
};

enum EncoderType {
    EncoderTypeMP3,
    EncoderTypeAAC,
    EncoderTypeLibX264,
    EncoderTypeNvEncH264,
    EncoderTypeIntelQsvH264,
    EncoderTypeCount
};

bool supportsEncoder(int type);

struct AudioVideoFrame {
    AudioVideoFrame(unsigned char* data_ = 0, int step_ = 0, int mediaType_ = UNKNOWN,
                    int pixelType_ = PixelTypeUnknown, int width_ = 0, int height_ = 0,
                    int sampleType_ = SampleTypeUnknown, int numChannels_ = 0,
                    int channelLayout_ = 0, int numSamples_ = 0, long long int timeStamp_ = -1LL,
                    int frameIndex_ = -1)
        : data(data_)
        , step(step_)
        , mediaType(mediaType_)
        , pixelType(pixelType_)
        , width(width_)
        , height(height_)
        , sampleType(sampleType_)
        , numChannels(numChannels_)
        , channelLayout(channelLayout_)
        , numSamples(numSamples_)
        , timeStamp(timeStamp_)
        , frameIndex(frameIndex_) {}
    unsigned char* data;
    int step;
    int mediaType;
    int pixelType;
    int width, height;
    int sampleType;
    int numChannels;
    int channelLayout;
    int numSamples;
    long long int timeStamp;
    int frameIndex;
};

inline AudioVideoFrame audioFrame(unsigned char* data, int step, int sampleType, int numChannels,
                                  int channelLayout, int numSamples, long long int timeStamp = -1LL,
                                  int frameIndex = -1) {
    AudioVideoFrame frame;
    frame.data          = data;
    frame.step          = step;
    frame.mediaType     = AUDIO;
    frame.sampleType    = sampleType;
    frame.numChannels   = numChannels;
    frame.channelLayout = channelLayout;
    frame.numSamples    = numSamples;
    frame.timeStamp     = timeStamp;
    frame.frameIndex    = frameIndex;
    return frame;
}

inline AudioVideoFrame videoFrame(unsigned char* data, int step, int pixelType, int width,
                                  int height, long long int timeStamp = -1LL, int frameIndex = -1) {
    AudioVideoFrame frame;
    frame.data       = data;
    frame.step       = step;
    frame.mediaType  = VIDEO;
    frame.pixelType  = pixelType;
    frame.width      = width;
    frame.height     = height;
    frame.timeStamp  = timeStamp;
    frame.frameIndex = frameIndex;
    return frame;
}


}   // namespace libvideo


#endif