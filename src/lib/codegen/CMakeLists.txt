# Build file for Isana

file(GLOB sources *.cpp)
file(GLOB headers ../include/codegen/*.hpp)

add_library(codegen STATIC ${sources} ${headers})
add_dependencies(codegen builtin)
target_link_libraries(codegen builtin)

add_subdirectory(builtin)
