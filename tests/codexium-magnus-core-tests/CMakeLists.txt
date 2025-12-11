cmake_minimum_required(VERSION 3.20)

project(codexium-magnus-core-tests VERSION 1.0.0 LANGUAGES CXX)

# Find Qt6 Test component
find_package(Qt6 REQUIRED COMPONENTS Test)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)

# Test source files
set(TEST_SOURCES
    Configuration/CompositeConfigurationResolverTests.cpp
)

# Create test executable
add_executable(codexium-magnus-core-tests
    ${TEST_SOURCES}
)

target_link_libraries(codexium-magnus-core-tests
    PRIVATE
    Qt6::Core
    Qt6::Test
    codexium-magnus-core
)

target_include_directories(codexium-magnus-core-tests
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus-core
)

# Add test to CTest
add_test(NAME CoreTests COMMAND codexium-magnus-core-tests)
