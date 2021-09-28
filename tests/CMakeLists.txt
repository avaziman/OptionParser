enable_testing()

set(GTEST_COLOR 1)

add_executable(${PROJECT_NAME}_test test.cpp)

target_link_libraries(
    ${PROJECT_NAME}_test
    gtest_main
)

include(GoogleTest)

gtest_discover_tests(${PROJECT_NAME}_test)