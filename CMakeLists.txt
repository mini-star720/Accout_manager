cmake_minimum_required(VERSION 3.19)
project(untitled LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools Charts)

# Find GTest
find_package(GTest REQUIRED)

qt_standard_project_setup()

qt_add_executable(untitled
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    # Models
    models/User.cpp
    models/User.h
    models/Category.cpp
    models/Category.h
    models/Record.cpp
    models/Record.h
    models/Budget.cpp
    models/Budget.h
    # Services
    services/ReminderService.cpp
    services/ReminderService.h
    services/ReportService.cpp
    services/ReportService.h
    services/DataStorageService.cpp
    services/DataStorageService.h
    # UI Widgets
    ui/TransactionWidget.cpp
    ui/TransactionWidget.h
    ui/StatisticsWidget.cpp
    ui/StatisticsWidget.h
    ui/BudgetWidget.cpp
    ui/BudgetWidget.h
    ui/SettingsWidget.cpp
    ui/SettingsWidget.h
    resources.qrc
)

qt_add_translations(
    TARGETS untitled
    TS_FILES untitled_zh_CN.ts
)

target_link_libraries(untitled
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Charts
)

include(GNUInstallDirs)

install(TARGETS untitled
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET untitled
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
