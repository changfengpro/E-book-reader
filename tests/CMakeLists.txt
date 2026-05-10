find_package(Qt6 REQUIRED COMPONENTS Core Sql Test)

add_executable(tst_bookrepository
    tst_bookrepository.cpp
    ../src/storage/AppDatabase.cpp
    ../src/storage/AppDatabase.h
    ../src/storage/AppPaths.cpp
    ../src/storage/AppPaths.h
    ../src/book/Book.h
    ../src/book/BookRepository.cpp
    ../src/book/BookRepository.h
)

target_include_directories(tst_bookrepository
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(tst_bookrepository
    PRIVATE
        Qt6::Core
        Qt6::Sql
        Qt6::Test
)

add_test(NAME tst_bookrepository COMMAND $<TARGET_FILE:tst_bookrepository>)

add_executable(tst_bookimporter
    tst_bookimporter.cpp
    ../src/book/BookImporter.cpp
    ../src/book/BookImporter.h
    ../src/storage/AppPaths.cpp
    ../src/storage/AppPaths.h
    ../src/book/Book.h
)

target_include_directories(tst_bookimporter
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(tst_bookimporter
    PRIVATE
        Qt6::Core
        Qt6::Test
)

add_test(NAME tst_bookimporter COMMAND $<TARGET_FILE:tst_bookimporter>)

add_executable(tst_txtdocument
    tst_txtdocument.cpp
    ../src/reader/TxtDocument.cpp
    ../src/reader/TxtDocument.h
)

target_include_directories(tst_txtdocument
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(tst_txtdocument
    PRIVATE
        Qt6::Core
        Qt6::Test
)

add_test(NAME tst_txtdocument COMMAND $<TARGET_FILE:tst_txtdocument>)
