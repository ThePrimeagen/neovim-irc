add_library(message-queue message-queue.c)

target_include_directories(message-queue INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_include_directories(message-queue PRIVATE
    ${PROJECT_LIB_FOLDER}
)



