add_library(mem-list mem-list.c)

target_include_directories(mem-list INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_include_directories(mem-list PRIVATE
    ${PROJECT_LIB_FOLDER}
)


