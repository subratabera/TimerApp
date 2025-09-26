cmake_minimum_required(VERSION 3.19)
project(TimerApp LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(TimerApp
    WIN32 MACOSX_BUNDLE
    main.cpp
    timerapp.cpp
    timerapp.h
    reminderdialog.h
    reminderdialog.cpp
    timesetdialog.h timesetdialog.cpp

)

target_link_libraries(TimerApp
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS TimerApp
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET TimerApp
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
