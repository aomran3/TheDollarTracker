cmake_minimum_required(VERSION 3.16)
project(TheDollarTracker LANGUAGES CXX)

# Qt auto-gen
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# We need Qt Widgets for the app, and Qt Core for tests (QString, QDate, etc.)
find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

# ---- Find main.cpp (supports src/main.cpp or root/main.cpp) ----
set(MAIN_CANDIDATE "")
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    set(MAIN_CANDIDATE "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
elseif (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
    set(MAIN_CANDIDATE "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
endif()

if (MAIN_CANDIDATE STREQUAL "")
    message(FATAL_ERROR "Could not find main.cpp (expected in src/ or project root)")
endif()

# ---- Collect sources ----
file(GLOB SRCS_CORE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.h"
)

file(GLOB SRCS_UI
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/*.ui"
)

set(SRCS
    ${MAIN_CANDIDATE}
    ${SRCS_CORE}
    ${SRCS_UI}
)

# ---- Main app target ----
add_executable(TheDollarTracker MACOSX_BUNDLE
    ${SRCS}
)

target_link_libraries(TheDollarTracker PRIVATE Qt6::Widgets)

set_target_properties(TheDollarTracker PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER "uwo.cs3307.thedollartracker"
    MACOSX_BUNDLE_BUNDLE_NAME "The Dollar Tracker"
)

# ----------------------------
# GoogleTest + GoogleMock
# ----------------------------
include(CTest)
enable_testing()

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

# Recommended on some platforms, harmless on mac
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Collect all test sources
file(GLOB TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
)

# Test executable (reuses core code)
add_executable(DollarTrackerTests
    ${TEST_SOURCES}
    ${SRCS_CORE}
)

# Link tests against Qt Core + GTest/GMock
target_link_libraries(DollarTrackerTests
    PRIVATE
        Qt6::Core      # <== gives access to QString, QDate, etc.
        GTest::gtest
        GTest::gmock
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(DollarTrackerTests)
