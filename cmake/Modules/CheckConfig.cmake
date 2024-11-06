# FindCheck.cmake - Locate the Check library
find_library(CHECK_LIBRARY check)
find_path(CHECK_INCLUDE_DIR check.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Check DEFAULT_MSG CHECK_LIBRARY CHECK_INCLUDE_DIR)

if(CHECK_LIBRARY AND CHECK_INCLUDE_DIR)
    set(Check_FOUND TRUE)
    set(Check_LIBRARIES ${CHECK_LIBRARY})
    set(Check_INCLUDE_DIRS ${CHECK_INCLUDE_DIR})
endif()
