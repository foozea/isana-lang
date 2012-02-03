# Build file for Isana

file(GLOB sources *.cpp */*.cpp)
file(GLOB headers ../include/codegen/builtin.hpp)

add_library(builtin STATIC ${sources} ${headers})
