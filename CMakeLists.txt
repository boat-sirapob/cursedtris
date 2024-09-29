cmake_minimum_required(VERSION 3.10)
project(cursedtris)

find_package(Curses REQUIRED)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories(include)
file(GLOB SRC src/*.cpp)

add_executable(cursedtris ${SRC})

target_link_libraries(cursedtris ncurses)
