

add_executable(demo demo.cc)
set_target_properties(demo PROPERTIES CXX_STANDARD 17)
target_include_directories(demo PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_libraries(demo PRIVATE ${FFMPEG_LIBRARIES} spdlog::spdlog nlohmann_json::nlohmann_json cxxopts)