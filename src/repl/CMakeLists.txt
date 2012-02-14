# Build file for Isana

execute_process(
    COMMAND ${LLVM_CONFIG_EXECUTABLE} --libs all
    OUTPUT_VARIABLE LLVM_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_executable(isana repl.cpp)
add_dependencies(
    isana codegen runtime env reader
)
target_link_libraries(
    isana codegen runtime env reader ${LLVM_LIBS}
)
set_target_properties(
    isana PROPERTIES VERSION ${version}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LINK_FLAGS "${LLVM_LDFLAGS}"
)

install(TARGETS isana RUNTIME DESTINATION bin)
