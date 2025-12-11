cmake_minimum_required(VERSION 3.20)
project(CodexiumMagnus VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dynamic linking configuration
# Note: Qt WebEngine cannot be statically linked due to Chromium dependencies.
# Therefore, we use dynamic linking for all Qt components for consistency.
# Qt libraries will be bundled with the application for deployment.
set(BUILD_SHARED_LIBS ON)

# Set Qt6_DIR to help CMake find Qt
# This points to the Qt installation at ~/Qt/6.9.3/macos
set(Qt6_DIR "$ENV{HOME}/Qt/6.9.3/macos/lib/cmake/Qt6" CACHE PATH "Qt6 CMake directory")

# Find Qt 6 components (all required)
# Note: WebEngineWidgets provides both WebEngine and WebEngineWidgets functionality
find_package(Qt6 REQUIRED COMPONENTS 
    Core
    Widgets
    WebEngineWidgets
    Sql
    Network
)

# Enable Qt MOC, UIC, RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Add subdirectories
add_subdirectory(src/codexium-magnus-core)
add_subdirectory(src/codexium-magnus-storage)
add_subdirectory(src/codexium-magnus)

# Tests (optional, can be enabled with -DBUILD_TESTS=ON)
option(BUILD_TESTS "Build test suite" OFF)
if(BUILD_TESTS)
    find_package(Qt6 REQUIRED COMPONENTS Test)
    enable_testing()
    add_subdirectory(tests/codexium-magnus-core-tests)
    add_subdirectory(tests/codexium-magnus-storage-tests)
endif()