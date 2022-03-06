# Target name
set(target starmath-test)
message(STATUS "Test ${target}")

set(sources
    starmath_test.C
    test1.C
)

# create an executable, which instantiates a runner from
# GoogleTest, Boost.Test, QtTest or whatever framework you use
# Build executable
add_executable(${target}
        ${sources}
        )

# depending on the framework, you need to link to it
target_link_libraries(${target}
        PRIVATE
        ${DEFAULT_LIBRARIES}
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        ${DEFAULT_LINKER_OPTIONS}
        )

# now register the executable with CTest
add_test(NAME ${target} COMMAND ${target})
