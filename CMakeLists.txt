
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(PyBind)

# Python 3.5
# ==========
find_path(PYTHON_3_DIR "python3.dll")

find_library(PYTHON_35_LIB "python35" PATHS "${PYTHON_3_DIR}/libs")
if(NOT PYTHON_35_LIB)
  message(FATAL_ERROR "Unable to find python 3.5. Make sure it's in your path.")
endif()

find_library(PYTHON_35_LIB_DEBUG "python35_d" "python35" PATHS "${PYTHON_3_DIR}/libs")

add_library(python35 INTERFACE)
target_link_libraries(python35 INTERFACE debug "${PYTHON_35_LIB_DEBUG}" optimized "${PYTHON_35_LIB}")
target_include_directories(python35 INTERFACE "${PYTHON_3_DIR}/include")

# PyBind
# ======
file(GLOB_RECURSE PyBind_SOURCES "${CMAKE_SOURCE_DIR}/PyBind/inc/*.hpp"
                                 "${CMAKE_SOURCE_DIR}/PyBind/inc/*.h"
                                 "${CMAKE_SOURCE_DIR}/PyBind/src/*.cpp"
                                 )

# This is a header-only library (for now).
add_library(PyBind_Headers INTERFACE)
target_link_libraries(PyBind_Headers INTERFACE python35)
target_include_directories(PyBind_Headers INTERFACE "${CMAKE_SOURCE_DIR}/PyBind/inc")

add_custom_target(PyBind SOURCES ${PyBind_SOURCES})
add_dependencies(PyBind PyBind_Headers)

# PyBindTest
# ==========
file(GLOB_RECURSE PyBindTests_SOURCES "${CMAKE_SOURCE_DIR}/PyBindTest/*.hpp"
                                      "${CMAKE_SOURCE_DIR}/PyBindTest/*.h"
                                      "${CMAKE_SOURCE_DIR}/PyBindTest/*.cpp"
                                      )

add_executable(PyBindTest ${PyBindTests_SOURCES})
target_link_libraries(PyBindTest PUBLIC PyBind_Headers)
