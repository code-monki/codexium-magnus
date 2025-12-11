cmake_minimum_required(VERSION 3.20)

project(codexium-magnus VERSION 1.0.0 LANGUAGES CXX)

# Source files
set(APP_SOURCES
    main.cpp
    MainWindow.cpp
    Services/WebEngineBridge.cpp
    Services/CartridgeService.cpp
    Services/SearchService.cpp
    Services/LinkService.cpp
    Services/PrintService.cpp
    Services/SignatureService.cpp
    Theme/ThemeManager.cpp
    UI/NavigationPane.cpp
    UI/SearchPane.cpp
    UI/SettingsDialog.cpp
    UI/HelpDialog.cpp
    UI/TypographySettingsWidget.cpp
    UI/BibliographySettingsWidget.cpp
)

set(APP_HEADERS
    MainWindow.h
    Services/WebEngineBridge.h
    Services/ICartridgeService.h
    Services/CartridgeService.h
    Services/ISearchService.h
    Services/SearchService.h
    Services/ILinkService.h
    Services/LinkService.h
    Services/IPrintService.h
    Services/PrintService.h
    Services/ISignatureService.h
    Services/SignatureService.h
    Theme/ThemeManager.h
    UI/NavigationPane.h
    UI/SearchPane.h
    UI/SettingsDialog.h
    UI/HelpDialog.h
    UI/TypographySettingsWidget.h
    UI/BibliographySettingsWidget.h
)

# Create executable
add_executable(codexium-magnus
    ${APP_SOURCES}
    ${APP_HEADERS}
)

# On macOS, create .app bundle
if(APPLE)
    set_target_properties(codexium-magnus PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "Codexium Magnus"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.codexium.magnus"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
        MACOSX_BUNDLE_INFO_STRING "Codexium Magnus ${PROJECT_VERSION}"
        MACOSX_BUNDLE_COPYRIGHT "Copyright (C) 2025, Codexium Magnus Contributors"
    )
endif()

target_link_libraries(codexium-magnus
    PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Sql
    Qt6::Network
    codexium-magnus-core
    codexium-magnus-storage
)

# Link libsodium if available
if(LIBSODIUM_FOUND)
    target_link_libraries(codexium-magnus PRIVATE ${LIBSODIUM_LIBRARIES})
    target_include_directories(codexium-magnus PRIVATE ${LIBSODIUM_INCLUDE_DIRS})
    if(LIBSODIUM_LIBRARY_DIRS)
        target_link_directories(codexium-magnus PRIVATE ${LIBSODIUM_LIBRARY_DIRS})
    endif()
endif()