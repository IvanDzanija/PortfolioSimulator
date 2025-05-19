cmake_minimum_required(VERSION 3.14)
project(PortfolioSimulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# === Find Qt ===
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)

# === Find OpenMP ===
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    message(STATUS "Found OpenMP")
else()
    message(WARNING "OpenMP not found. Compilation may be slower.")
endif()

# === Backend Sources ===
file(GLOB_RECURSE BACKEND_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

# === GUI ===
file(GLOB_RECURSE GUI_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gui/*.cpp)
add_executable(gui ${GUI_SOURCES} ${BACKEND_SOURCES})

target_include_directories(gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(gui PRIVATE
    Qt6::Widgets
    Qt6::Charts
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(gui PRIVATE OpenMP::OpenMP_CXX)
endif()

# === Tests ===
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)
add_executable(run_tests ${TEST_SOURCES} ${BACKEND_SOURCES})

target_include_directories(run_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(run_tests PRIVATE OpenMP::OpenMP_CXX)
endif()
