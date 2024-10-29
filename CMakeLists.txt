cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
IF( NOT CMAKE_BUILD_TYPE )
   SET( CMAKE_BUILD_TYPE Release ... FORCE )
ENDIF()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(MY_PROJECT_NAME resupply)
project(${MY_PROJECT_NAME})

######## Cplex Dependency
find_package(CPLEX REQUIRED)
if(CPLEX_FOUND)
    include_directories(${CPLEX_INCLUDE_DIR})
endif()

if(CPLEX_CONCERT_FOUND)
    include_directories(${CPLEX_CONCERT_INCLUDE_DIR})
endif()

if(CPLEX_ILOCPLEX_FOUND)
    include_directories(${CPLEX_ILOCPLEX_INCLUDE_DIR})
endif()

if(CPLEX_CP_FOUND)
    include_directories(${CPLEX_CP_INCLUDE_DIR})
endif()

######## Compiler message
message(STATUS "System: ${CMAKE_SYSTEM}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")

######## Set output directories.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)

######## Set C++ 17 standard
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++17" COMPILER_SUPPORTS_CXX17)
if (COMPILER_SUPPORTS_CXX17)
    if (CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17")
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    endif ()
else ()
    message(FATAL_ERROR "Your compiler doesn't support C++17, please install it!")
endif ()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra -Werror -march=native -flto=auto -ffat-lto-objects -funroll-loops -ffast-math -funsafe-math-optimizations -mtune=native -mavx -mavx2 -mfma")

set(CommonLibs
        m
        pthread
        dl
)

######## Find all the source files.
file(GLOB HEADER_FILES "source/*.h" "cp/*.h" "milp/*.h" "heuristic/*.h")
file(GLOB SOURCES_FILES "source/*.cpp" "cp/*.cpp" "milp/*.cpp" "heuristic/*.cpp")

######## Set executable file name, and add the source files for it.
add_executable(${MY_PROJECT_NAME} ${HEADER_FILES} ${SOURCES_FILES})

######## Add Dependency Library
target_link_libraries(${MY_PROJECT_NAME} 
    ${CPLEX_LIBRARIES} 
    ${CPLEX_CONCERT_LIBRARIES} 
    ${CPLEX_ILOCPLEX_LIBRARIES} 
    ${CPLEX_CP_LIBRARIES} 
    ${CommonLibs}
)

######## Add Optimization Flags
target_compile_options(${MY_PROJECT_NAME} PRIVATE -O3)
