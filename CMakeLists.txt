cmake_minimum_required(VERSION 3.10)
project(VCFLite VERSION 0.1 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)

# add_subdirectory(hkl)

find_library(sqlite3 NAMES libsqlite3)
FIND_PATH(SQLITE3_INCLUDE_DIR NAMES sqlite3.h)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "..." FORCE)
endif()

if (NOT HEADER_INSTALL_PREFIX)
    set(HEADER_INSTALL_PREFIX "/usr/local" CACHE PATH "..." FORCE)
endif()

add_executable(VCFLite
  ${CMAKE_CURRENT_SOURCE_DIR}/src/vcflite.cpp
)

target_link_libraries(VCFLite sqlite3)

target_include_directories(VCFLite
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/hkl/include
        ${CMAKE_CURRENT_SOURCE_DIR}/hkl/agizmo/include
        ${SQLITE3_INCLUDE_DIR}
)

target_compile_options(VCFLite PRIVATE
    -Werror -Wall
    -pthread -O3
    -mtune=native -march=x86-64
    -pipe -fPIE -fPIC)

install(TARGETS VCFLite  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
