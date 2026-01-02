cmake_minimum_required(VERSION 3.19)
project(emsumble_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/opt/homebrew")

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

# Find GTest
find_package(GTest REQUIRED)

qt_standard_project_setup()

add_executable(emsumble_test test.cpp
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

target_link_libraries(emsumble_test
    PRIVATE
        Qt::Core
        Qt::Widgets
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)