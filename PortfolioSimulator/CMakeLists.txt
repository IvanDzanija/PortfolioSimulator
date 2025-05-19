cmake_minimum_required(VERSION 3.14)
project(PortfolioSimulator)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Enforce Clang compiler
if(NOT (CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    message(FATAL_ERROR "This project requires Clang compiler. Please set CMAKE_CXX_COMPILER to clang++.")
endif()

# === Find Qt ===
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)

# === OpenMP Configuration for Clang ===
# On macOS, use Homebrew-installed LLVM with OpenMP
if(APPLE)
    # Try to locate Homebrew LLVM installation
    execute_process(
        COMMAND brew --prefix llvm
        RESULT_VARIABLE BREW_LLVM_RESULT
        OUTPUT_VARIABLE BREW_LLVM_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(BREW_LLVM_RESULT EQUAL 0)
        # Found Homebrew LLVM
        set(OPENMP_FLAGS "-fopenmp")
        set(OPENMP_INCLUDES "${BREW_LLVM_PREFIX}/include")
        set(OPENMP_LIBRARIES "${BREW_LLVM_PREFIX}/lib/libomp.dylib")
        set(OPENMP_FOUND TRUE)

        include_directories(${OPENMP_INCLUDES})
        link_directories(${BREW_LLVM_PREFIX}/lib)

        message(STATUS "Using Homebrew LLVM OpenMP at: ${BREW_LLVM_PREFIX}")
    else()
        # Try to find standalone libomp installation
        execute_process(
            COMMAND brew --prefix libomp
            RESULT_VARIABLE BREW_LIBOMP_RESULT
            OUTPUT_VARIABLE BREW_LIBOMP_PREFIX
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        if(BREW_LIBOMP_RESULT EQUAL 0)
            set(OPENMP_FLAGS "-fopenmp")
            set(OPENMP_INCLUDES "${BREW_LIBOMP_PREFIX}/include")
            set(OPENMP_LIBRARIES "${BREW_LIBOMP_PREFIX}/lib/libomp.dylib")
            set(OPENMP_FOUND TRUE)

            include_directories(${OPENMP_INCLUDES})
            link_directories(${BREW_LIBOMP_PREFIX}/lib)

            message(STATUS "Using Homebrew OpenMP at: ${BREW_LIBOMP_PREFIX}")
        else()
            message(WARNING "OpenMP not found. Please install with: brew install libomp")
            message(STATUS "Continuing without OpenMP support")
            set(OPENMP_FLAGS "")
            set(OPENMP_LIBRARIES "")
            set(OPENMP_FOUND FALSE)
        endif()
    endif()
else()
    # For non-macOS systems using Clang, try standard OpenMP find
    find_package(OpenMP)
    if(OpenMP_FOUND)
        set(OPENMP_FLAGS ${OpenMP_CXX_FLAGS})
        set(OPENMP_LIBRARIES OpenMP::OpenMP_CXX)
    else()
        message(WARNING "OpenMP not found for your system. Building without OpenMP support.")
        set(OPENMP_FLAGS "")
        set(OPENMP_LIBRARIES "")
    endif()
endif()

# Define preprocessor macro if OpenMP is available
if(OPENMP_FOUND)
    add_compile_definitions(HAVE_OPENMP)
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

# Apply OpenMP flags if available
if(OPENMP_FOUND)
    target_compile_options(gui PRIVATE ${OPENMP_FLAGS})
    target_link_libraries(gui PRIVATE ${OPENMP_LIBRARIES})
endif()

# === Tests ===
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)
add_executable(run_tests ${TEST_SOURCES} ${BACKEND_SOURCES})
target_include_directories(run_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Apply OpenMP flags to tests target as well
if(OPENMP_FOUND)
    target_compile_options(run_tests PRIVATE ${OPENMP_FLAGS})
    target_link_libraries(run_tests PRIVATE ${OPENMP_LIBRARIES})
endif()
