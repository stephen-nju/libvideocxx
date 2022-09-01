set(
    FFMPEG_SRC
    frame.h
    frame.cc
    stream.h
    stream_reader.h
    stream_reader.cc
    stream_writer.h
    stream_writer.cc
)

add_library(stream_io ${FFMPEG_SRC})
target_link_libraries(stream_io PRIVATE ${FFMPEG_LIBRARIES} spdlog::spdlog nlohmann_json::nlohmann_json)