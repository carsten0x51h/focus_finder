
# 
# Target 'doc'
# 

if(NOT OPTION_BUILD_DOC)
    return()
endif()

add_custom_target(doc)


# 
# Documentation
# 

add_subdirectory(api-doc)
add_subdirectory(manual)
