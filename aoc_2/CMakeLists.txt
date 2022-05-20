# copy binary to the code folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(aoc_2)

target_sources(
        aoc_2

        PRIVATE

        aoc_2.c
)