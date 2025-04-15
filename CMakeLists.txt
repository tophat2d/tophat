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

file(GLOB TH_SOURCE_FILES
  "src/*.c"
)

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
