cmake_minimum_required(VERSION 3.20)

project(guoke VERSION 0.1.0 LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "guoke supports Windows only.")
endif()

add_executable(guoke src/main.cpp)

target_compile_features(guoke PRIVATE cxx_std_20)
target_compile_definitions(guoke PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX UNICODE _UNICODE)

set_target_properties(guoke PROPERTIES
    CXX_EXTENSIONS OFF
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(MSVC)
    target_compile_options(guoke PRIVATE /W4 /permissive-)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(guoke PRIVATE -Wall -Wextra -Wpedantic)
endif()
