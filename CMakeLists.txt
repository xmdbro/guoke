cmake_minimum_required(VERSION 3.20)

project(guoke VERSION 0.1.0 LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "guoke supports Windows only.")
endif()

enable_language(RC)

option(GUOKE_BUILD_EXAMPLES "Build the demo application and DLL" ON)

function(guoke_configure_target target)
    target_compile_features(${target} PRIVATE cxx_std_20)
    target_compile_definitions(${target} PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX UNICODE _UNICODE)

    set_target_properties(${target} PROPERTIES
        CXX_EXTENSIONS OFF
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    if(MSVC)
        target_compile_options(${target} PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:/W4;/permissive->")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra;-Wpedantic>")
    endif()
endfunction()

add_executable(guoke WIN32
    src/main.cpp
    src/ui/main_window.cpp
    src/resources/guoke.rc
)
guoke_configure_target(guoke)
target_compile_definitions(guoke PRIVATE _WIN32_WINNT=0x0A00)
target_include_directories(guoke PRIVATE src/resources)
target_link_libraries(guoke PRIVATE user32 gdi32 comdlg32)

if(MSVC)
    # The resource file embeds the complete manifest for both toolchains.
    target_link_options(guoke PRIVATE /MANIFEST:NO)
endif()

if(GUOKE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
