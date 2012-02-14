# Build file for Isana

file(GLOB sources *.cpp)
file(GLOB headers ../include/runtime/*.hpp)

add_library(runtime STATIC ${sources} ${headers})
