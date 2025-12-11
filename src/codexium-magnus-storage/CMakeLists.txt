cmake_minimum_required(VERSION 3.20)

project(codexium-magnus-storage VERSION 1.0.0 LANGUAGES CXX)

# Source files
set(STORAGE_SOURCES
    DbInitializer.cpp
)

set(STORAGE_HEADERS
    DbInitializer.h
)

# Create library
add_library(codexium-magnus-storage STATIC
    ${STORAGE_SOURCES}
    ${STORAGE_HEADERS}
)

target_link_libraries(codexium-magnus-storage
    PRIVATE
    Qt6::Core
    Qt6::Sql
)

target_include_directories(codexium-magnus-storage
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)