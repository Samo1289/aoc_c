# copy binary to the code folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(aoc_1)

target_sources(
        aoc_1

        PRIVATE

        aoc_1.c
)
