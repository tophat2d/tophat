cmake_minimum_required(VERSION 3.18)
project(tophat)


if(MSVC)
    set(umka_dir  ${CMAKE_CURRENT_SOURCE_DIR}/lib/umka/umka_windows_msvc)
    set(umka_lib_path  ${umka_dir}/libumka_static.lib)
    
    execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/cmd/win-buildmodules.bat WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} RESULT_VARIABLE ret)
    if (NOT ret EQUAL "0")
        message(FATAL_ERROR "Failed to generate staembed.c")
    endif()

    file( GLOB th_sources
        "src/*.c"
        "lib/glad.c"
    )

    add_executable(tophat ${th_sources})
    
    # NOTE(skejeton): _USE_MATH_DEFINES flag for M_PI and so on.
    # NOTE(skejeton): UMKA_STATIC flag for we are linking umka statically not as a DLL. 
    target_compile_definitions(tophat PUBLIC UMKA_STATIC _USE_MATH_DEFINES)

    target_include_directories(tophat PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/miniaudio
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/umprof
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/umka/src
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/glad
    )

    target_link_libraries(tophat winmm gdi32 opengl32 user32 ${umka_lib_path})
else()
    message("TODO(skejeton): This script only works with MSVC. If you are on Linux or WSL/MSYS2 run `make` instead.")
endif()
