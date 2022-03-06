
# 
# Setup test environment
# 

find_package(Boost COMPONENTS unit_test_framework program_options REQUIRED)

add_subdirectory(starmath-tests)
#add_dependencies(test indi_test_device)

