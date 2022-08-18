#include <cstdio>

#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "spdlog/spdlog.h"


int main(int argc, char** argv) {
    cv::VideoCapture video_capture(
        "E:\\FDownload\\part\\10001.哔哩哔哩-【李佳琦】20200603带货直播回放.mp4");
    if (!video_capture.isOpened()) {
        spdlog::error("open file error");
        return -1;
    }
    int fps = video_capture.get(cv::CAP_PROP_FPS);
    printf("video fps =%d", fps);
}