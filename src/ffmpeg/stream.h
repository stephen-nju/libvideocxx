#ifndef SRC_FFMPEG_STREAM_H_
#define SRC_FFMPEG_STREAM_H_

#ifdef __cplusplus
#    define __STDC_CONSTANT_MACROS
extern "C" {
#endif
#include <libavformat/avformat.h>
#include <libavutil/error.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#ifdef __cplusplus
}
#endif
#include <memory>
#include <thread>

namespace libvideo {

struct StreamReader {
    virtual ~StreamReader(){};
    virtual bool readFrame(AVPacket& packet, Frame& frame) { return false; };
    virtual bool readTo(AVPacket& packet, Frame& frame) { return false; };
    virtual void flushBuffer(){};
    virtual void getProperties(InputStreamProperties& prop) { prop = InputStreamProperties(); };
    virtual void close(){};
};

struct AudioStreamReader : public StreamReader {
    AudioStreamReader();
    ~AudioStreamReader();
    void init();
    bool open(AVFormatContext* fmtCtx, int index, int sampleType);
    bool readFrame(AVPacket& packet, Frame& frame);
    bool readTo(AVPacket& packet, Frame& frame);
    void flushBuffer();
    void getProperties(InputStreamProperties& prop);
    void close();

    AVFormatContext* fmtCtx;
    AVStream* stream;
    int streamIndex;
    AVCodecContext* decCtx;
    AVFrame* frame;
    int numFrames;
    int numSamples;
    AVSampleFormat origSampleFormat;
    int sampleType;
    int numChannels;
    int channelLayout;
    int sampleRate;
    unsigned char* sampleData[8];
    int sampleLineSize;
    SwrContext* swrCtx;
};

struct VideoStreamReader : public StreamReader {
    virtual ~VideoStreamReader(){};
    virtual bool open(AVFormatContext* fmtCtx, int index, int pixelType) { return false; };
    virtual bool readFrame(AVPacket& packet, Frame& frame) { return false; };
    virtual bool readTo(AVPacket& packet, Frame& frame) { return false; };
    virtual void flushBuffer(){};
    virtual void getProperties(InputStreamProperties& prop){};
    virtual void close(){};

    AVFormatContext* fmtCtx;
    AVStream* stream;
    int streamIndex;
    AVCodecContext* decCtx;
    int width, height;
    AVPixelFormat origPixelFormat;
    int pixelType;
    double frameRate;
    int numFrames;
    unsigned char* pixelData[4];
    int pixelLinesize[4];
    SwsContext* swsCtx;
};

struct BuiltinCodecVideoStreamReader : public VideoStreamReader {
    BuiltinCodecVideoStreamReader();
    ~BuiltinCodecVideoStreamReader();
    void init();
    bool open(AVFormatContext* fmtCtx, int index, int pixelType);
    bool readFrame(AVPacket& packet, Frame& frame);
    bool readTo(AVPacket& packet, Frame& frame);
    void flushBuffer();
    void getProperties(InputStreamProperties& prop);
    void close();

    AVFrame* frame;
};

struct StreamWriter {
    virtual ~StreamWriter(){};
    virtual bool writeFrame(const Frame& frame) { return false; };
    virtual void close(){};
};

struct AudioStreamWriter : public StreamWriter {
    AudioStreamWriter();
    ~AudioStreamWriter();
    void init();
    bool open(AVFormatContext* fmtCtx, const std::string& format, int useExternTS,
              long long int* ptrFirstTS, int sampleType, int channelLayout, int sampleRate,
              int audioBPS, const std::vector<Option>& options);
    bool writeFrame(const Frame& frame);
    void close();

    AVFormatContext* fmtCtx;
    AVStream* stream;
    AVFrame* audioFrameSrc;
    AVFrame* audioFrameDst;
    int numSamplesSrc;
    int numSamplesDst;
    SwrContext* swrCtx;
    int sampleTypeRequested;
    int sampleRateRequested;
    int numChannelsRequested;
    int channelLayoutRequested;
    int sampleTypeAcquired;
    int sampleRateAcquired;
    int numChannelsAcquired;
    int channelLayoutAcquired;
    long long int inputSampleCount;
    long long int swrSampleCount;
    long long int sampleCount;
    AudioFrameAdaptor adaptor;

    int useExternTimeStamp;
    long long int* firstTimeStamp;
    double audioIncrementUnit;
};

struct VideoStreamWriter : public StreamWriter {
    virtual ~VideoStreamWriter() {}
    virtual bool open(AVFormatContext* fmtCtx, const std::string& format, int useExternTS,
                      long long int* ptrFirstTS, int pixelType, int width, int height, double fps,
                      int bps, const std::vector<Option>& options) {
        return false;
    }
    virtual bool writeFrame(const Frame& frame) { return false; }
    virtual void close(){};
};

struct BuiltinCodecVideoStreamWriter : public VideoStreamWriter {
    ~BuiltinCodecVideoStreamWriter();
    bool open(AVFormatContext* fmtCtx, const std::string& format, int externTimeStamp,
              long long int* firstTimeStamp, int pixelType, int width, int height, double fps,
              int bps, const std::vector<Option>& options);
    bool writeFrame(const Frame& frame);
    void close();

    BuiltinCodecVideoStreamWriter();
    void init();

    AVFormatContext* fmtCtx;
    AVStream* stream;
    AVFrame* yuvFrame;
    SwsContext* swsCtx;
    int framePixelTypeRequested;
    AVPixelFormat framePixelFormatAcquired;
    int frameWidth, frameHeight;
    int frameCount;
    double frameRate;

    int useExternTimeStamp;
    long long int* firstTimeStamp;
    double videoIncrementUnit;
};

}   // namespace libvideo


#endif