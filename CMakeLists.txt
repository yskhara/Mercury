cmake_minimum_required(VERSION 3.28)
project(mercury VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

find_package(PkgConfig)
pkg_check_modules(GTK REQUIRED gtk4)
pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)

include_directories("src/")
include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})
#message(\${GTKMM_INCLUDE_DIRS}="${GTKMM_INCLUDE_DIRS}")

set(CMAKE_CXX_STANDARD 20)

function(configure_module module_name)
  add_library(${module_name})
  file(GLOB SRC src/${module_name}/*.cpp)
  file(GLOB MODULE_SRC src/${module_name}/*.cppm)
  target_sources(${module_name}
    PRIVATE ${SRC}
    PUBLIC
      FILE_SET CXX_MODULES FILES
        ${MODULE_SRC})
endfunction()

file(GLOB SRC src/*.cpp src/*/*.cpp)
add_executable(mercury ${SRC})

#configure_module("ui")
#configure_module("chart")
#add_library(modules)
#file(GLOB SRC src/*/*.cpp)
#file(GLOB MODULE_SRC src/*/*.cppm)
#target_sources(mercury
#  PRIVATE ${SRC}
#  PUBLIC
#    FILE_SET CXX_MODULES FILES
#      ${MODULE_SRC})

target_link_libraries(mercury ${GTKMM_LIBRARIES})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

message("${BUILDSYSTEM_TARGETS}")
