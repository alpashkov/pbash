cmake_minimum_required(VERSION 3.12)
project(pbash VERSION 0.1.0 LANGUAGES C)

include(GNUInstallDirs)

find_library(PTHREAD_LIBRARY pthread)

file(GLOB CMD_FILES CONFIGURE_DEPENDS src/commands/*.h)
list(LENGTH CMD_FILES CMD_COUNT)

### If no command header files are found, nothing to generate
if(CMD_COUNT EQUAL 0)
    message(WARNING "No commands found in commands folder. App will be prompt-only!")
else()
    message(STATUS "Found ${CMD_COUNT} commands (*.h), proceeding to codegen.")
    include(cmake/cmd_glue_gen.cmake)
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER "gcc")
if(CMAKE_BUILD_TYPE STREQUAL "debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wall -Wextra -Wpedantic")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall")
endif()

file(GLOB_RECURSE COMMANDS_H CONFIGURE_DEPENDS 
"${CMAKE_SOURCE_DIR}/src/commands/*.h")
file(GLOB_RECURSE COMMANDS_C CONFIGURE_DEPENDS 
"${CMAKE_SOURCE_DIR}/src/commands/*.c")

set(SOURCES
    src/main.c
    src/cli.c
    src/cmd_common.c
    src/cmd_glue_gen.c
    ${COMMANDS_C}
)

set(HEADERS
    src/cli.h
    src/cmd_common.h
    src/cmd_glue_gen.h
    ${COMMANDS_H}
)

set(TARGET_LIBS
    ${PTHREAD_LIBRARY}
)

add_executable(pbash ${SOURCES} ${HEADERS})

target_include_directories(pbash PRIVATE 
${CMAKE_CURRENT_SOURCE_DIR}/src
${CMAKE_CURRENT_SOURCE_DIR}/src/commands
)
target_link_libraries(pbash PRIVATE ${TARGET_LIBS})
target_compile_definitions(pbash PRIVATE CMD_COUNT=${CMD_COUNT})

install(TARGETS pbash
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
