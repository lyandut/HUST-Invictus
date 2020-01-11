# - Try to find GUROBI
# Once done this will define
#  GUROBI_FOUND - System has Gurobi
#  GUROBI_INCLUDE_DIRS - The Gurobi include directories
#  GUROBI_LIBRARIES - The libraries needed to use Gurobi

if (GUROBI_INCLUDE_DIR)
    # in cache already
    set(GUROBI_FOUND TRUE)
    set(GUROBI_INCLUDE_DIRS "${GUROBI_INCLUDE_DIR}")
    set(GUROBI_LIBRARIES "${GUROBI_LIBRARY};${GUROBI_CXX_LIBRARY}")
else (GUROBI_INCLUDE_DIR)

    find_path(GUROBI_INCLUDE_DIR
            NAMES
                gurobi_c++.h
            PATHS
                "$ENV{GUROBI_HOME}/include"
                # "/Library/gurobi811/mac64/include"
                # "E:\\Program Files\\gurobi801\\win64\\include"
            )

    find_library(GUROBI_LIBRARY
            NAMES
                gurobi gurobi80 gurobi81
            PATHS
                "$ENV{GUROBI_HOME}/lib"
                # "/Library/gurobi811/mac64/lib"
                # "E:\\Program Files\\gurobi801\\win64\\lib"
            )

	if (MSVC)
        set(GUROBI_LIB_NAME gurobi_c++md2017)
    else (MSVC)
        set(GUROBI_LIB_NAME gurobi_c++)
    endif (MSVC)

    find_library(GUROBI_CXX_LIBRARY
            NAMES
                ${GUROBI_LIB_NAME}
            PATHS
                "$ENV{GUROBI_HOME}/lib"
                # "/Library/gurobi811/mac64/lib"
                # "E:\\Program Files\\gurobi801\\win64\\lib"
            )

    set(GUROBI_INCLUDE_DIRS "${GUROBI_INCLUDE_DIR}")
    set(GUROBI_LIBRARIES "${GUROBI_LIBRARY};${GUROBI_CXX_LIBRARY}")

    # use c++ headers as default
    # set(GUROBI_COMPILER_FLAGS "-DIL_STD" CACHE STRING "Gurobi Compiler Flags")

    include(FindPackageHandleStandardArgs)
    # handle the QUIETLY and REQUIRED arguments and set LIBCPLEX_FOUND to TRUE
    # if all listed variables are TRUE
    find_package_handle_standard_args(GUROBI DEFAULT_MSG GUROBI_LIBRARY GUROBI_CXX_LIBRARY GUROBI_INCLUDE_DIR)

    mark_as_advanced(GUROBI_INCLUDE_DIR GUROBI_LIBRARY GUROBI_CXX_LIBRARY)

endif (GUROBI_INCLUDE_DIR)