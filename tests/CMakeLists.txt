# Tests CMakeLists.txt
# Google Test configuration for Student Intake Forms

include(FetchContent)

# Fetch Google Test
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
    FIND_PACKAGE_ARGS NAMES GTest
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Include GoogleTest CMake module
include(GoogleTest)

# Define test source files
set(TEST_SOURCES
    # Model tests
    models/StudentTest.cpp
    models/EmergencyContactTest.cpp
    models/UserTest.cpp
    models/AcademicHistoryTest.cpp
    models/CurriculumTest.cpp
    models/ActivityLogTest.cpp

    # Service tests
    services/FormSubmissionServiceTest.cpp

    # Session tests
    session/StudentSessionTest.cpp

    # Utility tests
    utils/TestUtils.cpp
)

# Create test executable
add_executable(student_intake_tests ${TEST_SOURCES})

# Include directories for tests
target_include_directories(student_intake_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/app
    ${CMAKE_SOURCE_DIR}/src/auth
    ${CMAKE_SOURCE_DIR}/src/forms
    ${CMAKE_SOURCE_DIR}/src/curriculum
    ${CMAKE_SOURCE_DIR}/src/api
    ${CMAKE_SOURCE_DIR}/src/models
    ${CMAKE_SOURCE_DIR}/src/session
    ${CMAKE_SOURCE_DIR}/src/widgets
    ${CMAKE_SOURCE_DIR}/src/utils
    ${CMAKE_SOURCE_DIR}/src/admin
    ${CMAKE_SOURCE_DIR}/src/admin/models
    ${CMAKE_SOURCE_DIR}/tests
)

# Link test executable with Google Test and project libraries
target_link_libraries(student_intake_tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    student_intake_lib
    nlohmann_json::nlohmann_json
)

# Discover tests for CTest
gtest_discover_tests(student_intake_tests)

# Add custom target to run tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS student_intake_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running Student Intake Tests"
)
