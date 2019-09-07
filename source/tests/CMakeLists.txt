
# 
# Setup test environment
# 

# Check if tests are enabled
if(NOT OPTION_BUILD_TESTS)
    return()
endif()

# DISABLE CMP0037: Target names should not be reserved and should match a validity pattern.
set_policy(CMP0037 OLD)

# Function: Build test and add command to execute it via target 'test'
function(add_test_without_ctest target)
    add_subdirectory(${target})
    
    if(NOT TARGET ${target})
        return()
    endif()
    
    add_dependencies(test ${target})
    add_custom_command(TARGET test POST_BUILD 
      COMMAND $<TARGET_FILE:${target}>
      )
endfunction()


# 
# Target 'test'
# 

add_custom_target(test)
set_target_properties(test PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 1)


#
# TODO...
#
#add_subdirectory(indi_test_device)
#add_dependencies(test indi_test_device)


# 
# Tests
# 

add_test_without_ctest(focus_finder-test)

