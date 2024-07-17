# This file is part of LCSF C Stack.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>
#
# Author: Jean-Roland Gosse

# CMake version
cmake_minimum_required(VERSION 3.14)

# Project
project(zdc_poc VERSION 0.1 LANGUAGES C)

# Includes
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Build system debug option (optional)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Global output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Compilation flags
set(CMAKE_C_FLAGS "-Wpedantic -Werror -Wall -Wextra -Wshadow -Wundef")

# Debug symbol if building as debug (-DCMAKE_BUILD_TYPE="Debug")
add_compile_options("$<$<CONFIG:DEBUG>:-D_APP_DEBUG>")

# Target names
set(APP_TX_BIN_NAME "zdc_poc_tx")
set(APP_RX_BIN_NAME "zdc_poc_rx")

# Project subdirs
add_subdirectory(lib)
add_subdirectory(src)
