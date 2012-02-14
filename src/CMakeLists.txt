# Build file for Isana

include_directories(include)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

add_definitions(
    -Wall
    -D__STDC_LIMIT_MACROS
    -D__STDC_CONSTANT_MACROS
    ${LLVM_CPPFLAGS}
)

add_subdirectory(repl)
add_subdirectory(lib/codegen)
add_subdirectory(lib/runtime)
add_subdirectory(lib/env)
add_subdirectory(lib/reader)
