cmake_minimum_required(VERSION 2.8.12.)
project(tophat)

set(umka_lib_path  ${CMAKE_CURRENT_SOURCE_DIR}/lib/umka/umka_windows_msvc/libumka_static.lib)

if(MSVC)
    # TODO(skejeton): Build umka here.....

    file( GLOB th_sources
        "src/*.c"
        "lib/glad.c"
    )

    if (NOT EXISTS ${umka_lib_path})
        message("Recompiling umka.")
        execute_process(WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/umka COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/lib/umka/build_windows_msvc.bat)
    endif()

    add_executable(tophat ${th_sources})
    
    # NOTE(skejeton): _USE_MATH_DEFINITIONS flag for M_PI and so on.
    # NOTE(skejeton): UMKA_STATIC flag for we are linking umka statically not as a DLL. 
    target_compile_definitions(tophat PUBLIC UMKA_STATIC _USE_MATH_DEFINES)

    target_include_directories(tophat PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/miniaudio
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/umprof
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/umka/src
        ${CMAKE_CURRENT_SOURCE_DIR}/lib
    )

    target_link_libraries(tophat gdi32 opengl32 user32 ${umka_lib_path})
else()
    message("TODO(skejeton): This script only works with MSVC. If you are on Linux or WSL/MSYS2 run `make` instead.")
endif()