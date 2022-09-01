

add_executable(demo demo.cc)
target_include_directories(demo PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_libraries(demo PRIVATE ${FFMPEG_LIBRARIES} spdlog::spdlog nlohmann_json::nlohmann_json)