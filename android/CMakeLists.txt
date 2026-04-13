cmake_minimum_required(VERSION 3.22.1)
project(tophat)

set(UMKA_SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../lib/umka/src)

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
  "../src/*.c"
)

add_library(tophat SHARED ${TH_SOURCE_FILES})

target_compile_definitions(tophat PRIVATE UMKA_STATIC _USE_MATH_DEFINES _CONSOLE)

target_include_directories(tophat PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
  ${CMAKE_CURRENT_SOURCE_DIR}/../lib/miniaudio
  ${CMAKE_CURRENT_SOURCE_DIR}/../lib/stb
  ${CMAKE_CURRENT_SOURCE_DIR}/../lib/umprof
  ${CMAKE_CURRENT_SOURCE_DIR}/../lib/umka/src
  ${CMAKE_CURRENT_SOURCE_DIR}/../lib/sokol
)

file(READ ${CMAKE_CURRENT_SOURCE_DIR}/../version VERSION)
execute_process(COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE GIT_VERSION)
string(STRIP "${ver}" ver)
string(STRIP "${gitver}" gitver)
target_compile_definitions(tophat PUBLIC -DTH_VERSION=\"${ver}\" -DTH_GITVER=\"${gitver}\")

find_library(log-lib log)
find_library(android-lib android)
find_library(gles3-lib GLESv3)
find_library(egl-lib EGL)

target_link_libraries(tophat ${log-lib} ${android-lib} ${gles3-lib} ${egl-lib} umka)
