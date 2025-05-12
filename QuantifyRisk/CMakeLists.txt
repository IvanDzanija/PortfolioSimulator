# CMakeLists.txt
cmake_minimum_required(VERSION 3.14)
project(PyQt6_main)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(pybind11 REQUIRED)
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(Boost 1.88.0 REQUIRED)

# === PyQt6 ===
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
pybind11_add_module(PortfolioSimulator SHARED ${SRC_FILES})
target_include_directories(PortfolioSimulator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${pybind11_INCLUDE_DIRS}
    ${Python_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(PortfolioSimulator PRIVATE
    Boost::headers
    pybind11::pybind11
)

# === Tests ===
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/gui_handler.cpp")
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)
add_executable(run_tests ${TEST_SOURCES} ${SRC_FILES})
target_include_directories(run_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${pybind11_INCLUDE_DIRS}
    ${Python_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(run_tests PRIVATE
    pybind11::pybind11
    Boost::headers
)

set_target_properties(PortfolioSimulator PROPERTIES
    PREFIX ""
    SUFFIX ".so"
    # Direktno izgradimo modul u python direktorij kako bi Python kod mogao učitati isti
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/gui"
)
