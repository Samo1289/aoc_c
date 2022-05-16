add_executable(aoc_3)

target_sources(
        aoc_3

        PRIVATE

        aoc_3.c
)

# copy binary to the code folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})