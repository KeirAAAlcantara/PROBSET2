cmake_minimum_required(VERSION 3.16)

project(probSet2 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt_standard_project_setup()

qt_add_executable(probSet2
    main.cpp
    Canvas.h Canvas.cpp
    reader.h reader.cpp
    mainWindow.h mainWindow.cpp
)

target_link_libraries(probSet2 PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)