# Build file for Isana

file(GLOB sources *.cpp)
file(GLOB headers ../include/env/*.hpp)

add_library(env STATIC ${sources} ${headers})
