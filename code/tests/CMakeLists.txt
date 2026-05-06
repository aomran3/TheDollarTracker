# tests/CMakeLists.txt

include(FetchContent)

# Download GoogleTest + GoogleMock at configure time
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

target_sources(DollarTrackerDummyTests
  PRIVATE
    test_validation.h test_validation.cpp
    test_payroll.h test_payroll.cpp
    test_filegateway_mock.h test_filegateway_mock.cpp
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ---- Test executable ----
add_executable(DollarTrackerTests
    test_validation.cpp
    test_payroll.cpp
    test_filegateway_mock.cpp
)

target_link_libraries(DollarTrackerTests
    PRIVATE
        GTest::gtest_main
        GTest::gmock
        Qt6::Core               # because we use QString, QDate, etc.
)

include(GoogleTest)
gtest_discover_tests(DollarTrackerTests)
