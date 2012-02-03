# Build file for Isana

file(GLOB sources *.cpp)
file(GLOB headers ../include/reader/*.hpp)

add_library(reader STATIC ${sources} ${headers})
add_dependencies(reader ast)
target_link_libraries(reader ast)

add_subdirectory(ast)
