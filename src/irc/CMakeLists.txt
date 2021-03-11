add_library(irc irc.c)

target_include_directories(irc INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_include_directories(irc PRIVATE
    ${PROJECT_LIB_FOLDER}
)

