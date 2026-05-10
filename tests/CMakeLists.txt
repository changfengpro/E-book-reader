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

add_test(NAME tst_bookrepository COMMAND tst_bookrepository)
