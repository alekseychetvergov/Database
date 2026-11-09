cmake_minimum_required(VERSION 3.25)
project(Database VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(unofficial-sqlite3 CONFIG REQUIRED)

add_library(DatabaseCore
    src/app/console_database.cpp
    src/database/database.cpp
)

target_include_directories(DatabaseCore PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/Include"
)

target_link_libraries(DatabaseCore PUBLIC unofficial::sqlite3::sqlite3)

add_executable(Database main.cpp)
target_link_libraries(Database PRIVATE DatabaseCore)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/example.db"
    "${CMAKE_CURRENT_BINARY_DIR}/example.db"
    COPYONLY
)

include(CTest)
if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(DatabaseTests tests/database_tests.cpp)
    target_link_libraries(DatabaseTests PRIVATE DatabaseCore GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(DatabaseTests)
endif()
