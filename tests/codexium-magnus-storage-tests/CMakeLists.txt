cmake_minimum_required(VERSION 3.20)

project(codexium-magnus-storage-tests VERSION 1.0.0 LANGUAGES CXX)

# Find Qt6 Test component
find_package(Qt6 REQUIRED COMPONENTS Test)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)

# Test source files
set(TEST_SOURCES
    DbInitializerTests.cpp
)

# Create test executable
add_executable(codexium-magnus-storage-tests
    ${TEST_SOURCES}
)

target_link_libraries(codexium-magnus-storage-tests
    PRIVATE
    Qt6::Core
    Qt6::Test
    Qt6::Sql
    codexium-magnus-storage
)

target_include_directories(codexium-magnus-storage-tests
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus-storage
)

# Add test to CTest
add_test(NAME StorageTests COMMAND codexium-magnus-storage-tests)
