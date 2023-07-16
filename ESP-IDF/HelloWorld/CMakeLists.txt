# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)
# I added the esp-idf-lib component as a submodule and is inside the ESP-IDF folder
# the intention is to make this common for all folders.
# set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../esp-idf-lib/components")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(blink)
