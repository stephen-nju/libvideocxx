

add_executable(extract_audio extract_audio_from_video.cc)
set_target_properties(extract_audio PROPERTIES CXX_STANDARD 17)
target_include_directories(extract_audio PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_libraries(extract_audio PRIVATE ${FFMPEG_LIBRARIES} spdlog::spdlog nlohmann_json::nlohmann_json cxxopts)