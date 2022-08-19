#include "nlohmann/json.hpp"
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "spdlog/spdlog.h"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
using json   = nlohmann::json;
namespace fs = std::filesystem;


int main(int argc, char** argv) {

    std::ifstream file(u8"E:\\FDownload\\part1_split.txt");
    json video_info = json::parse(file);
    fs::path write_root(u8"E:\\FDownload");

    for (auto& elem : video_info.items()) {
        std::string video_name       = elem.key();
        std::vector<int> split_index = elem.value();

        char raw_video_path[512];
        sprintf(raw_video_path, u8"E:\\FDownload\\part1\\%s", video_name.c_str());

        cv::VideoCapture video_capture(raw_video_path);
        if (!video_capture.isOpened()) {
            spdlog::error("open file error");
            return -1;
        }

        int fps    = video_capture.get(cv::CAP_PROP_FPS);
        int fourcc = video_capture.get(cv::CAP_PROP_FOURCC);

        int frame_width  = static_cast<int>(video_capture.get(cv::CAP_PROP_FRAME_WIDTH));
        int frame_height = static_cast<int>(video_capture.get(cv::CAP_PROP_FRAME_HEIGHT));
        cv::Size frame_size(frame_width, frame_height);

        cv::Mat frame;
        int count_frame = 0;

        for (int index = 0; index < split_index.size(); index++) {
            int stop_frame = (fps * split_index[index]) / 1000;
            char video_chunk_name[512];
            sprintf(video_chunk_name,
                    "%s\\%s_chunk_%d.mp4",
                    write_root.u8string().c_str(),
                    video_name.c_str(),
                    index);
            spdlog::info("reading video {}", video_name);
            cv::VideoWriter output_video(video_chunk_name, fourcc, fps, frame_size);
            while (true) {
                bool success = video_capture.read(frame);
                if (!success) {
                    spdlog::error("read vide error");
                    break;
                }
                output_video.write(frame);
                count_frame++;
                if (count_frame > stop_frame) {
                    break;
                }
            }
        }
    }
}