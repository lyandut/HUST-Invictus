cmake_minimum_required(VERSION 3.12)
project(MyMaxSat)

set(CMAKE_CXX_STANDARD 14)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")

set(Data
    data/variable.hpp 
	data/clause.hpp 
	data/formula.hpp
	data/instanceReader.hpp
)

set(Algorithm
    algorithm/randomizedSolver.hpp
    algorithm/derandomizedSolver.hpp
    algorithm/LPSolver.hpp
	algorithm/LPDerandomizedSolver.hpp)

set(MpSolver
	MpSolver/Common.h
	MpSolver/Config.h
	MpSolver/LogSwitch.h
	MpSolver/MpSolver.h
	MpSolver/MpSolverBase.h
	MpSolver/MpSolverGurobi.h
	MpSolver/MpSolverGurobi.cpp
	MpSolver/Utility.h
)

add_executable(MyMaxSat 
	main.cpp config.h
	${Data} ${Algorithm} ${MpSolver}
)

# Find Gurobi
find_package(GUROBI REQUIRED)
if(GUROBI_FOUND)
    MESSAGE( STATUS "GUROBI_INCLUDE_DIRS = ${GUROBI_INCLUDE_DIRS}.")
    MESSAGE( STATUS "GUROBI_LIBRARIES = ${GUROBI_LIBRARIES}.")
    include_directories(SYSTEM ${GUROBI_INCLUDE_DIRS})
    target_link_libraries (MyMaxSat ${GUROBI_LIBRARIES})
endif()