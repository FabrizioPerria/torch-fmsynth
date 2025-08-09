cmake_minimum_required(VERSION 3.22)

# ##############################################################################
# FUNCTIONS
# ##############################################################################
function(set_libtorch_url TORCH_VERSION)
  if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(LIBTORCH_URL
        "https://download.pytorch.org/libtorch/cpu/libtorch-win-shared-with-deps-${TORCH_VERSION}%2Bcpu.zip"
        PARENT_SCOPE)
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(LIBTORCH_URL
        "https://download.pytorch.org/libtorch/cpu/libtorch-macos-arm64-${TORCH_VERSION}.zip"
        PARENT_SCOPE)
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(LIBTORCH_URL
        "https://download.pytorch.org/libtorch/cpu/libtorch-shared-with-deps-${TORCH_VERSION}%2Bcpu.zip"
        PARENT_SCOPE)
  else()
    message(FATAL_ERROR "Unsupported platform for LibTorch")
  endif()
endfunction()
# ##############################################################################
# END FUNCTIONS
# ##############################################################################

project(TestProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(CPM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set_property(GLOBAL PROPERTY USE_FOLDERS YES)

# Download CPM.cmake
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.0/CPM.cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake
  EXPECTED_HASH
    SHA256=2020b4fc42dba44817983e06342e682ecfc3d2f484a581f11cc5731fbe4dce8a)

include(${CPM_DIR}/CPM.cmake)

cpmaddpackage(
  NAME
  JUCE
  GITHUB_REPOSITORY
  juce-framework/JUCE
  GIT_TAG
  develop
  SOURCE_DIR
  ${LIB_DIR}/juce)

cpmaddpackage(
  NAME
  TRACER
  GITHUB_REPOSITORY
  fabrizioperria/tracer
  GIT_TAG
  main
  SOURCE_DIR
  ${LIB_DIR}/tracer)

cpmaddpackage(
  NAME
  GOOGLETEST
  GITHUB_REPOSITORY
  google/googletest
  VERSION
  1.13.0
  SOURCE_DIR
  ${LIB_DIR}/googletest
  OPTIONS
  "INSTALL_GTEST OFF"
  "gtest_force_shared_crt ON")

set(TORCH_VERSION "2.8.0")
set_libtorch_url(${TORCH_VERSION})
cpmaddpackage(NAME LibTorch URL ${LIBTORCH_URL} DOWNLOAD_ONLY YES)

enable_testing()

# Compiler warnings
if(MSVC)
  add_compile_options(/Wall /WX)
  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(
    TARGET test
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TORCH_DLLS}
            $<TARGET_FILE_DIR:test>)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(src)
add_subdirectory(plugin)
