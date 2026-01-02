cmake_minimum_required(VERSION 3.19)
project(tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/opt/homebrew")

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

# Find GTest
find_package(GTest REQUIRED)

qt_standard_project_setup()

add_executable(tests test.cpp
    ../models/User.cpp
    ../models/User.h
    ../models/Category.cpp
    ../models/Category.h
    ../models/Record.cpp
    ../models/Record.h
    ../models/Budget.cpp
    ../models/Budget.h
    ../services/ReminderService.cpp
    ../services/ReminderService.h
    ../services/ReportService.cpp
    ../services/ReportService.h
    ../services/DataStorageService.cpp
    ../services/DataStorageService.h
)

target_link_libraries(tests
    PRIVATE
        Qt::Core
        Qt::Widgets
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)