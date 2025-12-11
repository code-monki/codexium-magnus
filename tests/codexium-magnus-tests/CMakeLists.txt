cmake_minimum_required(VERSION 3.20)

project(codexium-magnus-tests VERSION 1.0.0 LANGUAGES CXX)

# Find Qt6 Test component
find_package(Qt6 REQUIRED COMPONENTS Test)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)

# Test source files
# Note: Test files should NOT have QTEST_MAIN - main.cpp runs all tests
set(TEST_SOURCES
    main.cpp
    Services/SignatureServiceTests.cpp
    Services/LinkServiceTests.cpp
    Services/CartridgeServiceTests.cpp
    Services/SearchServiceTests.cpp
    UI/TypographySettingsWidgetTests.cpp
    UI/BibliographySettingsWidgetTests.cpp
    UI/SettingsDialogTests.cpp
    Theme/ThemeManagerTests.cpp
)

# Include service implementations for testing
set(SERVICE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus/Services/SignatureService.cpp
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus/Services/LinkService.cpp
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus/Services/CartridgeService.cpp
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus/Services/SearchService.cpp
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus/UI/TypographySettingsWidget.cpp
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus/UI/BibliographySettingsWidget.cpp
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus/UI/SettingsDialog.cpp
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus/Theme/ThemeManager.cpp
)

# Include interface headers for MOC processing
set(SERVICE_HEADERS
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus/Services/ISignatureService.h
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus/Services/SignatureService.h
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus/Services/ILinkService.h
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus/Services/LinkService.h
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus/Services/ICartridgeService.h
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus/Services/CartridgeService.h
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus/Services/ISearchService.h
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus/Services/SearchService.h
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus/UI/TypographySettingsWidget.h
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus/UI/BibliographySettingsWidget.h
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus/UI/SettingsDialog.h
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus/Theme/ThemeManager.h
    # Test headers
    Services/SignatureServiceTests.h
    Services/LinkServiceTests.h
    Services/CartridgeServiceTests.h
    Services/SearchServiceTests.h
    UI/TypographySettingsWidgetTests.h
    UI/BibliographySettingsWidgetTests.h
    UI/SettingsDialogTests.h
    Theme/ThemeManagerTests.h
)

# Create test executable
add_executable(codexium-magnus-tests
    ${TEST_SOURCES}
    ${SERVICE_SOURCES}
    ${SERVICE_HEADERS}
)

target_link_libraries(codexium-magnus-tests
    PRIVATE
    Qt6::Core
    Qt6::Test
    Qt6::Sql
    Qt6::Widgets
    codexium-magnus-core
)

# Link libsodium if available (same as main app)
if(LIBSODIUM_FOUND)
    target_link_libraries(codexium-magnus-tests PRIVATE ${LIBSODIUM_LIBRARIES})
    target_include_directories(codexium-magnus-tests PRIVATE ${LIBSODIUM_INCLUDE_DIRS})
    if(LIBSODIUM_LIBRARY_DIRS)
        target_link_directories(codexium-magnus-tests PRIVATE ${LIBSODIUM_LIBRARY_DIRS})
    endif()
    target_compile_definitions(codexium-magnus-tests PRIVATE HAVE_LIBSODIUM)
endif()

target_include_directories(codexium-magnus-tests
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus
    ${CMAKE_SOURCE_DIR}/src/codexium-magnus-core
)

# Add test to CTest
add_test(NAME AppTests COMMAND codexium-magnus-tests)

