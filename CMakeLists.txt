cmake_minimum_required(VERSION 3.10)

project(guess_word_game LANGUAGES CXX)

# 按“接近 C89 的保守风格”约束，使用 C++98，避免新标准特性。
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

add_executable(guess_word
    src/main.cpp
    src/ConsoleUtil.cpp
    src/Draw.cpp
    src/GuessGame.cpp
)

# 把词库文件复制到可执行程序目录，避免在 build 目录运行时找不到 words.txt。
add_custom_command(TARGET guess_word POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/words.txt
            $<TARGET_FILE_DIR:guess_word>/words.txt
)

# Windows 下启用 /utf-8，避免中文注释与中文输出乱码。
if(MSVC)
    target_compile_options(guess_word PRIVATE /utf-8 /W4)
else()
    target_compile_options(guess_word PRIVATE -Wall -Wextra -pedantic)
endif()
