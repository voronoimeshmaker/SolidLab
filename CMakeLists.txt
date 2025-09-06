# ------------------------------------------------------------
# CMake minimum version + policies
# ------------------------------------------------------------
cmake_minimum_required(VERSION 3.16)
# Evita warning do FetchContent (CMP0135) ao extrair arquivos
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()



# ------------------------------------------------------------
# Project (ESSENCIAL: precisa existir no CMake raiz)
# ------------------------------------------------------------
project(SolidLab VERSION 1.0 LANGUAGES CXX)

# Debug: mostre claramente qual CMakeLists é o raiz
message(STATUS "[Root] Source dir = ${CMAKE_SOURCE_DIR}")
message(STATUS "[Root] Binary dir = ${CMAKE_BINARY_DIR}")

# ------------------------------------------------------------
# C++ standard
# ------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------
# Options
# ------------------------------------------------------------
option(BUILD_DOCUMENTATION "Build Doxygen and Sphinx documentation" OFF)
option(BUILD_EXAMPLES      "Build examples"                             OFF)
option(BUILD_TESTS         "Build tests"                                ON)

# ------------------------------------------------------------
# Default build type (Release if not specified)
# ------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# ------------------------------------------------------------
# Library layout (all headers/sources live under SolidLabLib)
# ------------------------------------------------------------
set(SOLIDLABLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SolidLabLib)

# ------------------------------------------------------------
# Output root (send binaries to ./bin)
# ------------------------------------------------------------
set(SOLIDLAB_OUTPUT_BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# ------------------------------------------------------------
# Include directories (project headers)
# ------------------------------------------------------------
include_directories(${SOLIDLABLIB_DIR}/include)

# ------------------------------------------------------------
# Sources: pick up every new .cpp recursively (no CMake edits required)
# ------------------------------------------------------------
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
     "${SOLIDLABLIB_DIR}/src/*.cpp")

# ------------------------------------------------------------
# Library (shared)
# ------------------------------------------------------------
add_library(SolidLab SHARED ${SOURCES})

# Link dependencies here if/when needed:
# target_link_libraries(SolidLab PUBLIC Some::Dependency)

# ------------------------------------------------------------
# Output directories (build artefacts to ./bin)
# ------------------------------------------------------------
set_target_properties(SolidLab PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${SOLIDLAB_OUTPUT_BIN_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SOLIDLAB_OUTPUT_BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SOLIDLAB_OUTPUT_BIN_DIR}
)

# ------------------------------------------------------------
# Compiler optimisations
# ------------------------------------------------------------
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(SolidLab PRIVATE
        -O3
        -march=native
        -flto
        -fopenmp
    )
elseif(MSVC)
    target_compile_options(SolidLab PRIVATE
        /O2
        /GL
    )
endif()

# ------------------------------------------------------------
# Install rules
# (keeps install lib inside ./bin and headers under ./include)
# ------------------------------------------------------------
install(TARGETS SolidLab
    EXPORT SolidLabTargets
    LIBRARY DESTINATION ${SOLIDLAB_OUTPUT_BIN_DIR}
    ARCHIVE DESTINATION ${SOLIDLAB_OUTPUT_BIN_DIR}
    RUNTIME DESTINATION ${SOLIDLAB_OUTPUT_BIN_DIR}
)

# Install public headers from SolidLabLib/include
install(DIRECTORY ${SOLIDLABLIB_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.tpp"
)

# If you keep template impls in src/, ship their .tpp as headers too
install(DIRECTORY ${SOLIDLABLIB_DIR}/src/
    DESTINATION include
    FILES_MATCHING PATTERN "*.tpp"
)

# ------------------------------------------------------------
# Meta target to run all examples (used by the macro below)
# ------------------------------------------------------------
if(NOT TARGET run_examples)
    add_custom_target(run_examples
        COMMAND ${CMAKE_COMMAND} -E echo "Running all examples..."
    )
endif()

# ------------------------------------------------------------
# Macro: build every *.cpp under a directory (recursively)
# - Creates one executable per .cpp
# - Links against SolidLab
# - Emits the executable next to its source file
# - Optionally runs it post-build
# ------------------------------------------------------------
macro(add_examples_in_directory dir)
    file(GLOB_RECURSE EXAMPLE_SOURCES CONFIGURE_DEPENDS
         RELATIVE "${dir}" "${dir}/*.cpp")
    if(EXAMPLE_SOURCES)
        foreach(example_source ${EXAMPLE_SOURCES})
            message(STATUS "Found example source: ${example_source}")
            set(example_full_path "${dir}/${example_source}")
            get_filename_component(example_name ${example_source} NAME_WE)

            add_executable(${example_name} "${example_full_path}")
            target_link_libraries(${example_name} PRIVATE SolidLab)
            target_include_directories(${example_name} PRIVATE
                ${SOLIDLABLIB_DIR}/include)

            if(CMAKE_COMPILER_IS_GNUCXX OR
               CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
                target_compile_options(${example_name} PRIVATE
                    -O3 -march=native -flto)
            elseif(MSVC)
                target_compile_options(${example_name} PRIVATE /O2 /GL)
            endif()

            get_filename_component(example_dir
                "${example_full_path}" DIRECTORY)
            set_target_properties(${example_name} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${example_dir}"
            )

            if(TARGET run_examples)
                add_dependencies(run_examples ${example_name})
                add_custom_command(
                    TARGET ${example_name}
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E echo
                            "Executable ${example_name} successfully built!"
                    COMMAND ${CMAKE_COMMAND} -E echo
                            "Running example ${example_name}..."
                    COMMAND "${example_dir}/${example_name}"
                    WORKING_DIRECTORY "${example_dir}"
                    COMMENT "Running example ${example_name}..."
                )
            endif()
        endforeach()
    else()
        message(STATUS "No examples found in ${dir}.")
    endif()
endmacro()

# ------------------------------------------------------------
# Mutual exclusion: examples vs tests
# ------------------------------------------------------------
if(BUILD_EXAMPLES AND BUILD_TESTS)
    message(FATAL_ERROR
        "You can only enable either BUILD_EXAMPLES or BUILD_TESTS, not both.")
endif()

# ------------------------------------------------------------
# Examples
# ------------------------------------------------------------
if(BUILD_EXAMPLES)
    message(STATUS "Building Examples...")
    add_subdirectory(examples)
endif()

# ------------------------------------------------------------
# Tests (GoogleTest via FetchContent)  <<<<<< CORREÇÃO AQUI
# ------------------------------------------------------------
if(BUILD_TESTS)
    message(STATUS "Building Tests...")
    include(CTest)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    # Para MSVC multi-config (Visual Studio)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # Agora os targets GTest::gtest e GTest::gtest_main existem
    add_subdirectory(tests)
endif()

# ------------------------------------------------------------
# distclean (purge build-tree artefacts)
# ------------------------------------------------------------
add_custom_target(distclean
    COMMAND ${CMAKE_COMMAND} -E rm -rf
    ${CMAKE_BINARY_DIR}/*
    ${CMAKE_BINARY_DIR}/CMakeFiles
    ${CMAKE_BINARY_DIR}/Makefile
    ${CMAKE_BINARY_DIR}/cmake_install.cmake
    COMMENT "Cleaning up all generated files"
)
