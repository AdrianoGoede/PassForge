cmake_minimum_required(VERSION 3.16)

project(PassForge VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BOTAN_INCLUDE_DIR "/usr/include/botan-2")
set(BOTAN_LIBRARY "/usr/lib/x86_64-linux-gnu/libbotan-2.so")

include_directories(${BOTAN_INCLUDE_DIR})
link_directories(${BOTAN_LIBRARY})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Botan2 SQLite3)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES PassForge_en_US.ts)

set(PROJECT_SOURCES
    Main.cpp
    Forms/MainWindow.cpp
    Forms/MainWindow.h
    Forms/MainWindow.ui
    ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(PassForge
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    Forms/PasswordGenerator.h
    Forms/PasswordGenerator.cpp
    Forms/PasswordGenerator.ui
    Crypto/Crypto.h Crypto/Crypto.cpp
    Configs/Configs.h
    Configs/Constants.h
    Resources/Wordlists.qrc
    Forms/DatabaseCreator.h
    Forms/DatabaseCreator.cpp
    Forms/DatabaseCreator.ui
    Database/DatabaseHandler.h
    Database/DatabaseHandler.cpp
    Database/CredentialEntry.h
    Database/CredentialEntry.cpp
    Database/CryptocurrencyEntry.h
    Database/CryptocurrencyEntry.cpp
    Database/DatabaseEntry.h
    Database/DatabaseEntry.cpp
    Forms/DatabaseSettings.h
    Forms/DatabaseSettings.cpp
    Forms/DatabaseSettings.ui
)
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET PassForge APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(PassForge SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(PassForge
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(PassForge PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${BOTAN_LIBRARY} sqlite3)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.PassForge)
endif()
set_target_properties(PassForge PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS PassForge
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(PassForge)
endif()
