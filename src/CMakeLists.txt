

add_subdirectory(ffmpeg)

add_executable(main main.cc)
set_target_properties(main PROPERTIES CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)
target_include_directories(main PRIVATE ${OpenCV_INCLUDE_DIRS} ${FFMPEG_INCLUDE_DIRS})
target_link_libraries(main PRIVATE ${OpenCV_LIBS} ${FFMPEG_LIBRARIES} spdlog::spdlog nlohmann_json::nlohmann_json)