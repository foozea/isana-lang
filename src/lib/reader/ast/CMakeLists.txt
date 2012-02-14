# Build file for Isana

file(GLOB sources *.cpp)
file(GLOB headers ../include/reader/ast/*.hpp)

add_library(ast STATIC ${sources} ${headers})
