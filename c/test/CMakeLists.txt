# Tests

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# Add the binary tree directory to the search path for linking and include files
link_directories (${PROJECT_BINARY_DIR}/src/binsearch)
include_directories (${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_BINARY_DIR}/src/binsearch)

#define macro to simplify adding tests
macro (do_test arg)
  add_test (${arg} ${TARGET_SYSTEM_EMULATOR} ${arg}${CMAKE_EXECUTABLE_SUFFIX})
endmacro ()

# create a smoke test
function(SMOKE_TEST test_name test_file dependencies)
  add_executable (${test_name} ${test_file})
  # Link the executable to the libraries
  target_link_libraries (${test_name} ${dependencies})
  # run test
  do_test (${test_name})
endfunction(SMOKE_TEST)

file(GLOB TEST_BIN_FILES "data/*.bin")
file (COPY ${TEST_BIN_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

SMOKE_TEST (test_binsearch test_binsearch.c binsearch)
SMOKE_TEST (test_binsearch_col test_binsearch_col.c binsearch)
SMOKE_TEST (test_binsearch_file test_binsearch_file.c binsearch)
SMOKE_TEST (test_binsearch_mmap test_binsearch_mmap.c binsearch)
SMOKE_TEST (test_binsearch_many test_binsearch_many.c binsearch)
SMOKE_TEST (test_binsearch_range test_binsearch_range.c binsearch)

# The find_many page requests need MADV_WILLNEED, which is POSIX and so is not
# declared in the strict ISO C build the tests above use: there the prefetch
# code is not generated at all. These two targets build the same tests with it
# declared, which is the configuration an ordinary caller compiles.
#
# A small BINSEARCH_PROBE_EVERY makes the tests cross a probe interval, and the
# two thresholds cover both answers the probe can give: BINSEARCH_SLOW_NS=1
# makes every batch count as reading from a device, the default makes none of
# them, since the test data is resident.
SMOKE_TEST (test_binsearch_prefetch test_binsearch_many.c binsearch)
target_compile_definitions (test_binsearch_prefetch PRIVATE _DEFAULT_SOURCE BINSEARCH_PROBE_EVERY=2 BINSEARCH_SLOW_NS=1)

SMOKE_TEST (test_binsearch_prefetch_auto test_binsearch_many.c binsearch)
target_compile_definitions (test_binsearch_prefetch_auto PRIVATE _DEFAULT_SOURCE BINSEARCH_PROBE_EVERY=2)
