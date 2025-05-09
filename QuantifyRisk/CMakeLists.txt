# CMakeLists.txt
cmake_minimum_required(VERSION 3.14)
project(PyQt6_main)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(pybind11 REQUIRED)
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(Boost 1.88.0 REQUIRED)

pybind11_add_module(main SHARED src/gui_handler.cpp)
target_link_libraries(main PRIVATE
    Boost::headers
    pybind11::pybind11
)

# === Tests ===
add_executable(test_parser tests/Parser_test.cpp)
add_executable(test_alignment tests/Alignment_test.cpp)

target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${pybind11_INCLUDE_DIRS}
    ${Python_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

target_include_directories(test_parser PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${pybind11_INCLUDE_DIRS}
    ${Python_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

target_include_directories(test_alignment PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${pybind11_INCLUDE_DIRS}
    ${Python_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)


set_target_properties(main PROPERTIES
    PREFIX ""
    SUFFIX ".so"
    # Direktno izgradimo modul u python direktorij kako bi Python kod mogao učitati isti
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/gui"
)
