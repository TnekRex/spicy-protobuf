# Generate C++ sources from spicy
set(SPICYC_GENERATED_SRC ${CMAKE_CURRENT_BINARY_DIR}/spicy_protobuf.cc)
set(SPICYC_SRC_CMD
    ${SPICYC} -c -g ${PROJECT_SOURCE_DIR}/analyzer/protobuf.spicy -o
    ${SPICYC_GENERATED_SRC})
add_custom_command(
  OUTPUT ${SPICYC_GENERATED_SRC}
  COMMAND ${SPICYC_SRC_CMD}
  COMMENT "Generating ${SPICYC_GENERATED_SRC}"
  MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/analyzer/protobuf.spicy
  VERBATIM COMMAND_EXPAND_LISTS)

# Generate C++ linker glue from spicy
set(SPICYC_GENERATED_LNK ${CMAKE_CURRENT_BINARY_DIR}/spicy_linker.cc)
set(SPICYC_LNK_CMD
    ${SPICYC} -l -g ${PROJECT_SOURCE_DIR}/analyzer/protobuf.spicy -o
    ${SPICYC_GENERATED_LNK})
add_custom_command(
  OUTPUT ${SPICYC_GENERATED_LNK}
  COMMAND ${SPICYC_LNK_CMD}
  COMMENT "Generating ${SPICYC_GENERATED_LNK}"
  MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/analyzer/protobuf.spicy
  VERBATIM COMMAND_EXPAND_LISTS)

# Generate C++ headers from spicy
set(SPICYC_GENERATED_HDR ${CMAKE_CURRENT_BINARY_DIR}/spicy_protobuf.h)
set(SPICYC_HDR_CMD
    ${SPICYC} -P -g ${PROJECT_SOURCE_DIR}/analyzer/protobuf.spicy -o
    ${SPICYC_GENERATED_HDR})
add_custom_command(
  OUTPUT ${SPICYC_GENERATED_HDR}
  COMMAND ${SPICYC_HDR_CMD}
  COMMENT "Generating ${SPICYC_GENERATED_HDR}"
  MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/analyzer/protobuf.spicy
  VERBATIM COMMAND_EXPAND_LISTS)

# Generated embedded test-data C++ headers
find_program(XXD xxd REQUIRED)
find_program(PRINTF printf REQUIRED)
file(GLOB_RECURSE BINPB CONFIGURE_DEPENDS *.binpb)
foreach(INPUT_BINPB ${BINPB})
  string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
                 XXD_OUTPUT_HDR ${INPUT_BINPB})
  string(REPLACE ".binpb" ".h" XXD_OUTPUT_HDR ${XXD_OUTPUT_HDR})
  string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/test-data/" "" XXD_INPUT_BINPB
                 ${INPUT_BINPB})
  set(XXD_HDR_CMD ${XXD} -i ${XXD_INPUT_BINPB} >> ${XXD_OUTPUT_HDR})
  get_filename_component(BINPB_DIR ${XXD_OUTPUT_HDR} DIRECTORY)
  file(MAKE_DIRECTORY ${BINPB_DIR})
  add_custom_command(
    OUTPUT ${XXD_OUTPUT_HDR}
    COMMAND ${PRINTF} "#pragma once\\n\\n" > ${XXD_OUTPUT_HDR}
    COMMAND ${XXD_HDR_CMD}
    COMMENT "Generating ${XXD_OUTPUT_HDR}"
    MAIN_DEPENDENCY ${INPUT_BINPB}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test-data/"
    VERBATIM COMMAND_EXPAND_LISTS)
  list(APPEND XXD_GENERATED_HDR ${XXD_OUTPUT_HDR})
endforeach()

# Pull down gtest library
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG b514bdc898e2951020cbdca1304b75f5950d1f59 # v1.15.2
)
FetchContent_MakeAvailable(googletest)
enable_testing()
include(GoogleTest)

# Create gtest executable
add_executable(
  ${CMAKE_PROJECT_NAME}_test
  SpicyProtobufTest.cpp ${SPICYC_GENERATED_SRC} ${SPICYC_GENERATED_LNK}
  ${SPICYC_GENERATED_HDR} ${XXD_GENERATED_HDR})
target_link_libraries(${CMAKE_PROJECT_NAME}_test PRIVATE gtest_main spicy hilti)
target_include_directories(${CMAKE_PROJECT_NAME}_test
                           PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Register with ctest
gtest_discover_tests(${CMAKE_PROJECT_NAME}_test)
