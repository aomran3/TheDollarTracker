cmake_minimum_required(VERSION 3.16)
project(TheDollarTracker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

file(GLOB_RECURSE SOURCES
    code/src/*.cpp
)

file(GLOB_RECURSE HEADERS
    code/src/*.h
)

add_executable(the-dollar-tracker
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(the-dollar-tracker
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
