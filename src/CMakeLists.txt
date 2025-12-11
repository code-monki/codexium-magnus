cmake_minimum_required(VERSION 3.20)

project(codexium-magnus-core VERSION 1.0.0 LANGUAGES CXX)

# Source files
set(CORE_SOURCES
    Models/TypographyConfig.cpp
    Models/BibliographyConfig.cpp
    Reporting/ReportEntry.cpp
    Reporting/ReportSeverity.cpp
    Reporting/ReportWriter.cpp
    Configuration/ConfigurationSource.cpp
    Configuration/CompositeConfigurationResolver.cpp
)

set(CORE_HEADERS
    Models/TypographyConfig.h
    Models/BibliographyConfig.h
    Reporting/ReportEntry.h
    Reporting/ReportSeverity.h
    Reporting/ReportWriter.h
    Configuration/ConfigurationSource.h
    Configuration/CompositeConfigurationResolver.h
)

# Create library
add_library(codexium-magnus-core STATIC
    ${CORE_SOURCES}
    ${CORE_HEADERS}
)

target_link_libraries(codexium-magnus-core
    PRIVATE
    Qt6::Core
)

target_include_directories(codexium-magnus-core
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)