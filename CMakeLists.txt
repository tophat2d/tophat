cmake_minimum_required(VERSION 3.18)
project(tophat)

set(UMKA_SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/lib/umka/src)

add_library(umka
	${UMKA_SRC_ROOT}/umka_api.c
	${UMKA_SRC_ROOT}/umka_common.c
	${UMKA_SRC_ROOT}/umka_compiler.c
	${UMKA_SRC_ROOT}/umka_const.c
	${UMKA_SRC_ROOT}/umka_decl.c
	${UMKA_SRC_ROOT}/umka_expr.c
	${UMKA_SRC_ROOT}/umka_gen.c
	${UMKA_SRC_ROOT}/umka_ident.c
	${UMKA_SRC_ROOT}/umka_lexer.c
	${UMKA_SRC_ROOT}/umka_runtime.c
	${UMKA_SRC_ROOT}/umka_stmt.c
	${UMKA_SRC_ROOT}/umka_types.c
	${UMKA_SRC_ROOT}/umka_vm.c
)

target_compile_definitions(umka PRIVATE -DUMKA_BUILD -DUMKA_EXT_LIBS)

if (MSVC)
	execute_process(
		COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/cmd/win-buildmodules.bat
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		RESULT_VARIABLE RET
	)

	if (NOT RET EQUAL "0")
	    message(FATAL_ERROR "Failed to generate staembed.c")
	endif()
else()
	message("staembed.c might be outdated, rebuild using makefile")
endif()


file(GLOB TH_SOURCE_FILES
	src/*.c
)


if (CMAKE_SYSTEM_NAME STREQUAL "iOS")
	file(GLOB TH_SOURCE_FILES
		src/atlas.c		src/font.c		src/particles.c		src/tophat.c
		src/audio.c		src/image.c		src/quad.c		src/transform.c
		src/bindings.c		src/input.c		src/shader.c		src/utf8.c
		src/canvas.c		src/main.c		src/sokol.m		src/window.c
		src/collisions.c	src/miniaudio.m		src/staembed.c
		src/color.c		src/misc.c		src/stbi.c
		src/entity.c		src/nav.c		src/tilemap.c src/ios.m
	)

	add_executable(tophat MACOSX_BUNDLE ${TH_SOURCE_FILES})
	
	target_compile_definitions(tophat PRIVATE UMKA_STATIC)
	
	target_include_directories(tophat PUBLIC
	  ${CMAKE_CURRENT_SOURCE_DIR}/src
	  ${CMAKE_CURRENT_SOURCE_DIR}/lib/miniaudio
	  ${CMAKE_CURRENT_SOURCE_DIR}/lib/stb
	  ${CMAKE_CURRENT_SOURCE_DIR}/lib/umprof
	  ${CMAKE_CURRENT_SOURCE_DIR}/lib/umka/src
	  ${CMAKE_CURRENT_SOURCE_DIR}/lib/sokol
	)
	
	target_link_libraries(tophat PRIVATE
    umka
    "-framework OpenGLES"
    "-framework GLKit"
    "-framework AVFoundation"
    "-framework AudioToolbox"
    "-framework CoreAudio"
    "-framework Foundation"
    "-framework UIKit"
    "-framework QuartzCore"
    "-framework Metal"
    "-framework CoreGraphics"
)

	target_sources(tophat PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/package
	)

	set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/package
    PROPERTIES MACOSX_PACKAGE_LOCATION Resources
	)
	
	set_target_properties(tophat PROPERTIES
    MACOSX_BUNDLE TRUE

    MACOSX_BUNDLE_GUI_IDENTIFIER "dev.tophat2d.tophat"
    MACOSX_BUNDLE_BUNDLE_NAME "tophat"
    MACOSX_BUNDLE_BUNDLE_VERSION "1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"

    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER
        "dev.tophat2d.tophat"

    XCODE_ATTRIBUTE_CODE_SIGN_STYLE
        "Automatic"

    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY
        "1,2"

    XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "Tophat2D"
	)
else()
	add_executable(tophat ${TH_SOURCE_FILES})
	
	# NOTE(ske): _USE_MATH_DEFINES flag for M_PI and so on.
	# NOTE(ske): UMKA_STATIC flag for we are linking umka statically not as a DLL.
	target_compile_definitions(tophat PRIVATE UMKA_STATIC _USE_MATH_DEFINES _CONSOLE)
	
	target_include_directories(tophat PUBLIC
	  ${CMAKE_CURRENT_SOURCE_DIR}/src
	  ${CMAKE_CURRENT_SOURCE_DIR}/lib/miniaudio
	  ${CMAKE_CURRENT_SOURCE_DIR}/lib/stb
	  ${CMAKE_CURRENT_SOURCE_DIR}/lib/umprof
	  ${CMAKE_CURRENT_SOURCE_DIR}/lib/umka/src
	  ${CMAKE_CURRENT_SOURCE_DIR}/lib/sokol
	)
	
	file(READ ${CMAKE_CURRENT_SOURCE_DIR}/version VERSION)
	execute_process(COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE GIT_VERSION)
	string(STRIP "${ver}" ver)
	string(STRIP "${gitver}" gitver)
	target_compile_definitions(tophat PUBLIC -DTH_VERSION=\"${ver}\" -DTH_GITVER=\"${gitver}\")
	
	target_link_libraries(tophat xinput gdi32 opengl32 user32 umka)
endif()
