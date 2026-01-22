cmake_minimum_required(VERSION 3.16)

# Suppress deprecation warnings from system/third-party headers
add_compile_options(-Wno-deprecated-declarations)

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

# Find libharu for PDF generation
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(HPDF QUIET libhpdf)
endif()

if(NOT HPDF_FOUND)
    # Try to find libharu directly
    find_library(HPDF_LIBRARY NAMES hpdf libhpdf)
    find_path(HPDF_INCLUDE_DIR NAMES hpdf.h)
    if(HPDF_LIBRARY AND HPDF_INCLUDE_DIR)
        set(HPDF_FOUND TRUE)
        set(HPDF_LIBRARIES ${HPDF_LIBRARY})
        set(HPDF_INCLUDE_DIRS ${HPDF_INCLUDE_DIR})
    endif()
endif()

if(HPDF_FOUND)
    message(STATUS "Found libharu: ${HPDF_LIBRARIES}")
    add_compile_definitions(HAVE_LIBHARU)
else()
    message(WARNING "libharu not found. PDF generation will be disabled. Install with: apt-get install libhpdf-dev")
endif()

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
    src/models/StudentAddress.cpp
    src/models/EmergencyContact.cpp
    src/models/AcademicHistory.cpp
    src/models/FormData.cpp
    src/models/Curriculum.cpp
    src/models/InstitutionSettings.cpp
)

set(SESSION_SOURCES
    src/session/SessionManager.cpp
    src/session/StudentSession.cpp
)

set(API_SOURCES
    src/api/ApiClient.cpp
    src/api/FormSubmissionService.cpp
    src/api/PdfGenerator.cpp
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

set(ADMIN_SOURCES
    src/admin/models/AdminUser.cpp
    src/admin/models/AdminSession.cpp
    src/admin/AdminAuthManager.cpp
    src/admin/AdminLoginWidget.cpp
    src/admin/AdminNavigation.cpp
    src/admin/AdminSidebar.cpp
    src/admin/AdminDashboard.cpp
    src/admin/AdminApp.cpp
    src/admin/students/StudentListWidget.cpp
    src/admin/students/StudentDetailWidget.cpp
    src/admin/students/StudentFormViewer.cpp
    src/admin/curriculum/CurriculumListWidget.cpp
    src/admin/curriculum/CurriculumEditorWidget.cpp
    src/admin/forms/FormSubmissionsWidget.cpp
    src/admin/forms/FormDetailViewer.cpp
    src/admin/forms/FormPdfPreviewWidget.cpp
    src/admin/forms/FormTypesListWidget.cpp
    src/admin/forms/FormTypeDetailWidget.cpp
    src/admin/settings/InstitutionSettingsWidget.cpp
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
    ${ADMIN_SOURCES}
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
    ${CMAKE_SOURCE_DIR}/src/utils
    ${CMAKE_SOURCE_DIR}/src/admin
    ${CMAKE_SOURCE_DIR}/src/admin/models
    ${CMAKE_SOURCE_DIR}/src/admin/students
    ${CMAKE_SOURCE_DIR}/src/admin/curriculum
    ${CMAKE_SOURCE_DIR}/src/admin/forms
    ${CMAKE_SOURCE_DIR}/src/admin/settings
)

# Link libraries
target_link_libraries(student_intake PRIVATE
    Wt::Wt
    Wt::HTTP
    CURL::libcurl
    nlohmann_json::nlohmann_json
)

# Link libharu if found
if(HPDF_FOUND)
    target_include_directories(student_intake PRIVATE ${HPDF_INCLUDE_DIRS})
    target_link_libraries(student_intake PRIVATE ${HPDF_LIBRARIES})
endif()

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
