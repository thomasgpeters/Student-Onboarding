cmake_minimum_required(VERSION 3.16)

# Set policy version minimum for fetched dependencies with older CMake requirements
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()

project(StudentIntakeForms VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find required packages
find_package(Wt REQUIRED COMPONENTS Wt HTTP)
find_package(CURL REQUIRED)
find_package(nlohmann_json 3.9 QUIET)

# If nlohmann_json not found via find_package, try pkg-config or fetch
if(NOT nlohmann_json_FOUND)
    include(FetchContent)
    # Use v3.11.3 which has updated CMake compatibility
    FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
        SYSTEM
    )
    # Disable tests for nlohmann_json to speed up build
    set(JSON_BuildTests OFF CACHE INTERNAL "")
    set(JSON_Install OFF CACHE INTERNAL "")
    FetchContent_MakeAvailable(json)
endif()

# Source files organized by module
set(MODEL_SOURCES
    src/models/Student.cpp
    src/models/FormData.cpp
    src/models/Curriculum.cpp
)

set(SESSION_SOURCES
    src/session/SessionManager.cpp
    src/session/StudentSession.cpp
)

set(API_SOURCES
    src/api/ApiClient.cpp
    src/api/FormSubmissionService.cpp
)

set(AUTH_SOURCES
    src/auth/AuthManager.cpp
    src/auth/LoginWidget.cpp
    src/auth/RegisterWidget.cpp
)

set(CURRICULUM_SOURCES
    src/curriculum/CurriculumManager.cpp
    src/curriculum/CurriculumSelector.cpp
)

set(FORMS_SOURCES
    src/forms/BaseForm.cpp
    src/forms/FormFactory.cpp
    src/forms/PersonalInfoForm.cpp
    src/forms/EmergencyContactForm.cpp
    src/forms/MedicalInfoForm.cpp
    src/forms/AcademicHistoryForm.cpp
    src/forms/FinancialAidForm.cpp
    src/forms/DocumentUploadForm.cpp
    src/forms/ConsentForm.cpp
)

set(WIDGETS_SOURCES
    src/widgets/NavigationWidget.cpp
    src/widgets/ProgressWidget.cpp
    src/widgets/FormContainer.cpp
    src/widgets/DashboardWidget.cpp
)

set(APP_SOURCES
    src/app/StudentIntakeApp.cpp
    src/main.cpp
)

# Combine all sources
set(ALL_SOURCES
    ${MODEL_SOURCES}
    ${SESSION_SOURCES}
    ${API_SOURCES}
    ${AUTH_SOURCES}
    ${CURRICULUM_SOURCES}
    ${FORMS_SOURCES}
    ${WIDGETS_SOURCES}
    ${APP_SOURCES}
)

# Create executable
add_executable(student_intake ${ALL_SOURCES})

# Include directories
target_include_directories(student_intake PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/app
    ${CMAKE_SOURCE_DIR}/src/auth
    ${CMAKE_SOURCE_DIR}/src/forms
    ${CMAKE_SOURCE_DIR}/src/curriculum
    ${CMAKE_SOURCE_DIR}/src/api
    ${CMAKE_SOURCE_DIR}/src/models
    ${CMAKE_SOURCE_DIR}/src/session
    ${CMAKE_SOURCE_DIR}/src/widgets
)

# Link libraries
target_link_libraries(student_intake PRIVATE
    Wt::Wt
    Wt::HTTP
    CURL::libcurl
    nlohmann_json::nlohmann_json
)

# Copy resources to build directory
file(COPY ${CMAKE_SOURCE_DIR}/config DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})

# Install targets
install(TARGETS student_intake RUNTIME DESTINATION bin)
install(DIRECTORY config/ DESTINATION share/student_intake/config)
install(DIRECTORY resources/ DESTINATION share/student_intake/resources)

# Custom target for running the application
add_custom_target(run
    COMMAND student_intake --docroot resources --http-address 0.0.0.0 --http-port 8080
    DEPENDS student_intake
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running Student Intake Forms Application"
)
