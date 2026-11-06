cmake_minimum_required(VERSION 3.20)
project(expense_tracker CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)

add_library(sqlite3 third_party/sqlite/sqlite3.c)
target_include_directories(sqlite3 PUBLIC third_party/sqlite)
target_link_libraries(sqlite3 PUBLIC Threads::Threads ${CMAKE_DL_LIBS})

add_executable(expense_tracker
    src/main.cpp
    src/db/Database.cpp
    src/db/Statement.cpp
)

target_include_directories(expense_tracker PRIVATE src)
target_link_libraries(expense_tracker PRIVATE sqlite3)