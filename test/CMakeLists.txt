include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


enable_testing()

add_executable(tests integration_tests.cc execute_tests.cc heap_tests.cc snapshot_test.cc)
target_link_libraries(tests tam GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)
