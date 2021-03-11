add_library(user user.c)

target_include_directories(user INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_include_directories(user PRIVATE
    ${PROJECT_LIB_FOLDER}
)

