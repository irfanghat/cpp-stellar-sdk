find_package(GTest REQUIRED)

file(GLOB_RECURSE TEST_SOURCES "*.cpp")

add_executable(sdk-tests ${TEST_SOURCES})

target_link_libraries(sdk-tests
    PRIVATE
        stellar::sdk
        GTest::gtest
        GTest::gmock
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(sdk-tests)

if(MSVC)
    target_compile_options(sdk-tests PRIVATE /W4 /permissive-)
else()
    target_compile_options(sdk-tests PRIVATE -Wall -Wextra -Wpedantic)
endif()