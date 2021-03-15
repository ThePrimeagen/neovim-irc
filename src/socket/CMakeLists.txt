add_library(socket socket.c)

target_include_directories(socket INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_include_directories(socket PRIVATE
    ${PROJECT_LIB_FOLDER}
)


