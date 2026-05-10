cmake_minimum_required(VERSION 3.21)

project(TabletEbookReader VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Sql Test)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(TabletEbookReader
    src/main.cpp
    src/storage/AppDatabase.cpp
    src/storage/AppDatabase.h
    src/storage/AppPaths.cpp
    src/storage/AppPaths.h
    src/book/Book.h
    src/book/BookRepository.cpp
    src/book/BookRepository.h
)

qt_add_qml_module(TabletEbookReader
    URI EbookReader
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/pages/LibraryPage.qml
        qml/pages/ReaderPage.qml
        qml/pages/SettingsPage.qml
)

target_link_libraries(TabletEbookReader
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Quick
        Qt6::Sql
)

target_include_directories(TabletEbookReader
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

enable_testing()
add_subdirectory(tests)
