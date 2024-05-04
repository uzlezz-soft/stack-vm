# CMakeList.txt : CMake project for runtime, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (runtime "runtime.cpp" "runtime.h" "opcode.h" "value.h" "code.h" "machine.h" "machine.cpp" "code_builder.h" "test_code_builder.h" "test_code_builder.cpp" "hash.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET runtime PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
