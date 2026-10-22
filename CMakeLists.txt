cmake_minimum_required(VERSION 3.25)
project(Database VERSION 0.1.0 LANGUAGES C CXX)

# Подавляем предупреждения о политиках CMake для старого FindBoost
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 OLD) # Разрешаем встроенный FindBoost
endif()
if(POLICY CMP0144)
    cmake_policy(SET CMP0144 NEW)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Явно задаем пути к вашей папке Boost
set(BOOST_ROOT "C:/Boost/boost_1_88_0/boost_1_88_0")
set(Boost_INCLUDE_DIR "C:/Boost/boost_1_88_0/boost_1_88_0")

# Ищем Boost (только заголовочные файлы, без компонентов)
find_package(Boost REQUIRED)

# Создаем статическую библиотеку из файла sqlite3.c
add_library(sqlite3_lib STATIC 
    "C:/sqlite-amalgamation-3530000/sqlite-amalgamation-3530000/sqlite3.c"
)

# Указываем, где лежат заголовки sqlite3.h для самой библиотеки
target_include_directories(sqlite3_lib PUBLIC 
    "C:/sqlite-amalgamation-3530000/sqlite-amalgamation-3530000"
)
# Создаем исполняемый файл
add_executable(Database
    main.cpp
    src/app/requester_database.cpp
    src/database/database.cpp
)

target_include_directories(Database PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Передаем компилятору путь, где искать заголовки Boost (исправлено имя цели на Database)
target_include_directories(Database PRIVATE ${Boost_INCLUDE_DIRS})

# Связываем проект с библиотекой SQLite3 (автоматически добавит пути к заголовкам и .lib/.a файлам)
target_link_libraries(Database PRIVATE sqlite3_lib)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/example.db"
    "${CMAKE_CURRENT_BINARY_DIR}/example.db"
    COPYONLY
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND WIN32)
    target_link_libraries(Database PRIVATE stdc++exp)
endif()