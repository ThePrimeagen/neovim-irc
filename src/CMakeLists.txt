# add_subdirectory(system-commands)
# add_subdirectory(hashmap)
# add_subdirectory(twitch)
add_subdirectory(irc)
add_subdirectory(mem-list)
# #add_subdirectory(comms)
#
# list(APPEND EXTRA_LIBS system-commands)
# list(APPEND EXTRA_LIBS json-c)
# list(APPEND EXTRA_LIBS twitch)
# list(APPEND EXTRA_LIBS hashmap)
# list(APPEND EXTRA_LIBS websockets)
list(APPEND EXTRA_LIBS irc)
list(APPEND EXTRA_LIBS mem-list)
#
# add the executable
add_executable(neovim-irc neovim-irc.c)

find_package(Threads REQUIRED)

target_link_libraries(neovim-irc PUBLIC
    ${EXTRA_LIBS}
    Threads::Threads
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

target_include_directories(neovim-irc PUBLIC
    "${PROJECT_BINARY_DIR}"
)

target_include_directories(neovim-irc PRIVATE
    "${PROJECT_LIB_FOLDER}"
)


