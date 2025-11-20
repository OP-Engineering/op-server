project(OPServer)
cmake_minimum_required(VERSION 3.9.0)

set (PACKAGE_NAME "op-server")

file(GLOB_RECURSE shared_files RELATIVE ${CMAKE_SOURCE_DIR}
     "../cpp/**.cpp"
)
file(GLOB_RECURSE android_files RELATIVE ${CMAKE_SOURCE_DIR}
     "src/main/cpp/**.cpp"
)

add_library(
  ${PACKAGE_NAME}
  SHARED
  ${shared_files}
  ${android_files}
)

include_directories(
  ../cpp
  # Android-specific C++ includes
  src/main/cpp/core
  src/main/cpp/registry
  src/main/cpp/turbomodule
  src/main/cpp/platform
  src/main/cpp/utils
  src/main/cpp/views
)

find_package(ReactAndroid REQUIRED CONFIG)
find_package(fbjni REQUIRED CONFIG)
find_library(LOG_LIB log)

target_link_libraries(
  ${PACKAGE_NAME}
  ${LOG_LIB}
  ReactAndroid::reactnative
  ReactAndroid::jsi
  fbjni::fbjni
)