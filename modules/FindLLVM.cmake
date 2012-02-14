# FindLLVM

find_program(
    LLVM_CONFIG_EXECUTABLE
    NAMES llvm-config
)

if (LLVM_CONFIG_EXECUTABLE)
    message(STATUS "llvm-config found at: ${LLVM_CONFIG_EXECUTABLE}")
else ()
    message(FATAL_ERROR "llvm-config not found")
endif (LLVM_CONFIG_EXECUTABLE)

execute_process(
    COMMAND ${LLVM_CONFIG_EXECUTABLE} --includedir
    OUTPUT_VARIABLE LLVM_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND ${LLVM_CONFIG_EXECUTABLE} --libdir
    OUTPUT_VARIABLE LLVM_LIB_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND ${LLVM_CONFIG_EXECUTABLE} --bindir
    OUTPUT_VARIABLE LLVM_BIN_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND ${LLVM_CONFIG_EXECUTABLE} --cppflags
    OUTPUT_VARIABLE LLVM_CPPFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND ${LLVM_CONFIG_EXECUTABLE} --ldflags
    OUTPUT_VARIABLE LLVM_LDFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    "llvm configurations"
    LLVM_INCLUDE_DIR
    LLVM_LIB_DIR
    LLVM_BIN_DIR
    LLVM_CPPFLAGS
    LLVM_LDFLAGS
)
