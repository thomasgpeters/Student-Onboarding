#include "SchedulingWidget.h"
#include <Wt/WBreak.h>
#include <Wt/WMessageBox.h>
#include <Wt/WDate.h>
#include <Wt/WTime.h>

namespace StudentIntake {
namespace Instructor {

SchedulingWidget::SchedulingWidget()
    : instructorId_(0)
    , scheduleDialog_(nullptr)
{
    setStyleClass("scheduling-widget");
    setupUI();
}

void SchedulingWidget::setupUI() {
    // Header
    headerSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    headerSection_->setStyleClass("widget-header");

    backButton_ = headerSection_->addWidget(std::make_unique<Wt::WPushButton>("Back"));
    backButton_->setStyleClass("btn btn-secondary");
    backButton_->clicked().connect([this] { backClicked_.emit(); });

    titleText_ = headerSection_->addWidget(std::make_unique<Wt::WText>("Session Schedule"));
    titleText_->setStyleClass("widget-title");

    newSessionButton_ = headerSection_->addWidget(std::make_unique<Wt::WPushButton>("Schedule New Session"));
    newSessionButton_->setStyleClass("btn btn-primary");
    newSessionButton_->clicked().connect([this] { showScheduleDialog(); });

    // Filter section
    filterSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    filterSection_->setStyleClass("filter-section");

    auto viewLabel = filterSection_->addWidget(std::make_unique<Wt::WText>("View: "));
    viewLabel->setStyleClass("filter-label");

    viewFilter_ = filterSection_->addWidget(std::make_unique<Wt::WComboBox>());
    viewFilter_->addItem("Upcoming");
    viewFilter_->addItem("Today");
    viewFilter_->addItem("This Week");
    viewFilter_->addItem("All");
    viewFilter_->setStyleClass("filter-combo");
    viewFilter_->changed().connect([this] { updateSessionsTable(); });

    auto dateLabel = filterSection_->addWidget(std::make_unique<Wt::WText>("Date Range: "));
    dateLabel->setStyleClass("filter-label");

    startDateFilter_ = filterSection_->addWidget(std::make_unique<Wt::WDateEdit>());
    startDateFilter_->setStyleClass("filter-date");
    startDateFilter_->setDate(Wt::WDate::currentDate());

    filterSection_->addWidget(std::make_unique<Wt::WText>(" to "));

    endDateFilter_ = filterSection_->addWidget(std::make_unique<Wt::WDateEdit>());
    endDateFilter_->setStyleClass("filter-date");
    endDateFilter_->setDate(Wt::WDate::currentDate().addDays(30));

    // Sessions table
    createSessionsTable();

    noSessionsText_ = addWidget(std::make_unique<Wt::WText>("No sessions found"));
    noSessionsText_->setStyleClass("no-data-text");
    noSessionsText_->hide();
}

void SchedulingWidget::createSessionsTable() {
    sessionsTable_ = addWidget(std::make_unique<Wt::WTable>());
    sessionsTable_->setStyleClass("sessions-table");
    sessionsTable_->setHeaderCount(1);

    sessionsTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Date"));
    sessionsTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Time"));
    sessionsTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Type"));
    sessionsTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Student"));
    sessionsTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Location"));
    sessionsTable_->elementAt(0, 5)->addWidget(std::make_unique<Wt::WText>("Status"));
    sessionsTable_->elementAt(0, 6)->addWidget(std::make_unique<Wt::WText>("Actions"));
}

void SchedulingWidget::loadSessions() {
    updateSessionsTable();
}

void SchedulingWidget::updateSessionsTable() {
    // Clear existing rows
    while (sessionsTable_->rowCount() > 1) {
        sessionsTable_->removeRow(1);
    }

    if (!instructorService_) {
        noSessionsText_->show();
        sessionsTable_->hide();
        return;
    }

    std::vector<Models::ScheduledSession> sessions;

    int selectedView = viewFilter_->currentIndex();
    if (selectedView == 0) { // Upcoming
        sessions = instructorService_->getUpcomingSessions(instructorId_, 50);
    } else if (selectedView == 1) { // Today
        sessions = instructorService_->getTodaySessions(instructorId_);
    } else {
        // Date range
        std::string startDate = startDateFilter_->date().toString("yyyy-MM-dd").toUTF8();
        std::string endDate = endDateFilter_->date().toString("yyyy-MM-dd").toUTF8();
        sessions = instructorService_->getSessionsByDateRange(instructorId_, startDate, endDate);
    }

    if (sessions.empty()) {
        noSessionsText_->show();
        sessionsTable_->hide();
        return;
    }

    noSessionsText_->hide();
    sessionsTable_->show();

    for (const auto& session : sessions) {
        int row = sessionsTable_->rowCount();

        // Date
        sessionsTable_->elementAt(row, 0)->addWidget(std::make_unique<Wt::WText>(session.getScheduledDate()));

        // Time
        std::string timeStr = session.getStartTime() + " - " + session.getEndTime();
        sessionsTable_->elementAt(row, 1)->addWidget(std::make_unique<Wt::WText>(timeStr));

        // Type
        sessionsTable_->elementAt(row, 2)->addWidget(std::make_unique<Wt::WText>(session.getDisplayName()));

        // Student
        sessionsTable_->elementAt(row, 3)->addWidget(
            std::make_unique<Wt::WText>("Student #" + std::to_string(session.getStudentId())));

        // Location
        sessionsTable_->elementAt(row, 4)->addWidget(std::make_unique<Wt::WText>(session.getLocationName()));

        // Status
        auto statusText = sessionsTable_->elementAt(row, 5)->addWidget(
            std::make_unique<Wt::WText>(session.getStatusString()));
        statusText->setStyleClass("status-badge status-" + session.getStatusString());

        // Actions
        auto actionsCell = sessionsTable_->elementAt(row, 6);
        actionsCell->setStyleClass("actions-cell");

        std::string sessionId = session.getId();
        Models::SessionStatus status = session.getStatus();

        if (status == Models::SessionStatus::Scheduled || status == Models::SessionStatus::Confirmed) {
            auto startBtn = actionsCell->addWidget(std::make_unique<Wt::WPushButton>("Start"));
            startBtn->setStyleClass("btn btn-sm btn-success");
            startBtn->clicked().connect([this, sessionId] { startSession(sessionId); });

            auto cancelBtn = actionsCell->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
            cancelBtn->setStyleClass("btn btn-sm btn-danger");
            cancelBtn->clicked().connect([this, sessionId] { cancelSession(sessionId); });
        } else if (status == Models::SessionStatus::InProgress) {
            auto completeBtn = actionsCell->addWidget(std::make_unique<Wt::WPushButton>("Complete"));
            completeBtn->setStyleClass("btn btn-sm btn-primary");
            completeBtn->clicked().connect([this, sessionId] { completeSession(sessionId); });
        }

        auto editBtn = actionsCell->addWidget(std::make_unique<Wt::WPushButton>("Edit"));
        editBtn->setStyleClass("btn btn-sm btn-secondary");
        editBtn->clicked().connect([this, sessionId] { editSession(sessionId); });
    }
}

void SchedulingWidget::createScheduleDialog() {
    scheduleDialog_ = addChild(std::make_unique<Wt::WDialog>("Schedule Session"));
    scheduleDialog_->setStyleClass("schedule-dialog");
    scheduleDialog_->setModal(true);
    scheduleDialog_->setClosable(true);

    auto content = scheduleDialog_->contents();
    content->setStyleClass("dialog-content");

    // Student selection
    auto studentRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    studentRow->setStyleClass("form-row");
    studentRow->addWidget(std::make_unique<Wt::WText>("Student:"))->setStyleClass("form-label");
    studentCombo_ = studentRow->addWidget(std::make_unique<Wt::WComboBox>());
    studentCombo_->setStyleClass("form-input");

    // Session type
    auto typeRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    typeRow->setStyleClass("form-row");
    typeRow->addWidget(std::make_unique<Wt::WText>("Session Type:"))->setStyleClass("form-label");
    sessionTypeCombo_ = typeRow->addWidget(std::make_unique<Wt::WComboBox>());
    sessionTypeCombo_->setStyleClass("form-input");
    populateSessionTypes();

    // Title
    auto titleRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    titleRow->setStyleClass("form-row");
    titleRow->addWidget(std::make_unique<Wt::WText>("Title (optional):"))->setStyleClass("form-label");
    titleEdit_ = titleRow->addWidget(std::make_unique<Wt::WLineEdit>());
    titleEdit_->setStyleClass("form-input");

    // Date
    auto dateRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    dateRow->setStyleClass("form-row");
    dateRow->addWidget(std::make_unique<Wt::WText>("Date:"))->setStyleClass("form-label");
    dateEdit_ = dateRow->addWidget(std::make_unique<Wt::WDateEdit>());
    dateEdit_->setStyleClass("form-input");
    dateEdit_->setDate(Wt::WDate::currentDate());

    // Time
    auto timeRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    timeRow->setStyleClass("form-row");
    timeRow->addWidget(std::make_unique<Wt::WText>("Time:"))->setStyleClass("form-label");
    startTimeEdit_ = timeRow->addWidget(std::make_unique<Wt::WTimeEdit>());
    startTimeEdit_->setStyleClass("form-input time-input");
    startTimeEdit_->setTime(Wt::WTime(9, 0));
    timeRow->addWidget(std::make_unique<Wt::WText>(" to "));
    endTimeEdit_ = timeRow->addWidget(std::make_unique<Wt::WTimeEdit>());
    endTimeEdit_->setStyleClass("form-input time-input");
    endTimeEdit_->setTime(Wt::WTime(10, 0));

    // Location
    auto locationRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    locationRow->setStyleClass("form-row");
    locationRow->addWidget(std::make_unique<Wt::WText>("Location:"))->setStyleClass("form-label");
    locationEdit_ = locationRow->addWidget(std::make_unique<Wt::WLineEdit>());
    locationEdit_->setStyleClass("form-input");

    // Notes
    auto notesRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    notesRow->setStyleClass("form-row");
    notesRow->addWidget(std::make_unique<Wt::WText>("Notes:"))->setStyleClass("form-label");
    notesEdit_ = notesRow->addWidget(std::make_unique<Wt::WTextArea>());
    notesEdit_->setStyleClass("form-input");
    notesEdit_->setRows(3);

    // Dialog buttons
    auto footer = scheduleDialog_->footer();
    footer->setStyleClass("dialog-footer");

    dialogCancelButton_ = footer->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
    dialogCancelButton_->setStyleClass("btn btn-secondary");
    dialogCancelButton_->clicked().connect([this] { scheduleDialog_->reject(); });

    saveButton_ = footer->addWidget(std::make_unique<Wt::WPushButton>("Save"));
    saveButton_->setStyleClass("btn btn-primary");
    saveButton_->clicked().connect([this] { saveSession(); });
}

void SchedulingWidget::showScheduleDialog(int studentId) {
    editingSessionId_ = "";

    if (!scheduleDialog_) {
        createScheduleDialog();
    }

    populateStudentCombo();

    // Select student if provided
    if (studentId > 0) {
        for (int i = 0; i < studentCombo_->count(); ++i) {
            // Simple check - would need proper ID matching in production
            if (studentCombo_->itemText(i).toUTF8().find(std::to_string(studentId)) != std::string::npos) {
                studentCombo_->setCurrentIndex(i);
                break;
            }
        }
    }

    // Reset form
    titleEdit_->setText("");
    dateEdit_->setDate(Wt::WDate::currentDate());
    startTimeEdit_->setTime(Wt::WTime(9, 0));
    endTimeEdit_->setTime(Wt::WTime(10, 0));
    locationEdit_->setText("");
    notesEdit_->setText("");

    scheduleDialog_->setWindowTitle("Schedule New Session");
    scheduleDialog_->show();
}

void SchedulingWidget::editSession(const std::string& sessionId) {
    if (!instructorService_) return;

    auto session = instructorService_->getSession(sessionId);
    if (session.getId().empty()) return;

    editingSessionId_ = sessionId;

    if (!scheduleDialog_) {
        createScheduleDialog();
    }

    populateStudentCombo();

    // Set form values
    titleEdit_->setText(session.getTitle());

    // Parse and set date
    if (!session.getScheduledDate().empty()) {
        auto date = Wt::WDate::fromString(session.getScheduledDate(), "yyyy-MM-dd");
        dateEdit_->setDate(date);
    }

    // Set times
    if (!session.getStartTime().empty()) {
        auto time = Wt::WTime::fromString(session.getStartTime(), "HH:mm");
        startTimeEdit_->setTime(time);
    }
    if (!session.getEndTime().empty()) {
        auto time = Wt::WTime::fromString(session.getEndTime(), "HH:mm");
        endTimeEdit_->setTime(time);
    }

    locationEdit_->setText(session.getLocationName());
    notesEdit_->setText(session.getNotes());

    // Set session type
    std::string typeStr = session.getSessionTypeString();
    for (int i = 0; i < sessionTypeCombo_->count(); ++i) {
        if (sessionTypeCombo_->itemText(i).toUTF8() == session.getDisplayName()) {
            sessionTypeCombo_->setCurrentIndex(i);
            break;
        }
    }

    scheduleDialog_->setWindowTitle("Edit Session");
    scheduleDialog_->show();
}

void SchedulingWidget::saveSession() {
    if (!instructorService_) return;

    Models::ScheduledSession session;

    if (!editingSessionId_.empty()) {
        session = instructorService_->getSession(editingSessionId_);
    }

    session.setInstructorId(instructorId_);
    session.setTitle(titleEdit_->text().toUTF8());
    session.setScheduledDate(dateEdit_->date().toString("yyyy-MM-dd").toUTF8());
    session.setStartTime(startTimeEdit_->time().toString("HH:mm").toUTF8());
    session.setEndTime(endTimeEdit_->time().toString("HH:mm").toUTF8());
    session.setLocationName(locationEdit_->text().toUTF8());
    session.setNotes(notesEdit_->text().toUTF8());

    // Get selected session type
    int typeIndex = sessionTypeCombo_->currentIndex();
    Models::SessionType types[] = {
        Models::SessionType::Orientation,
        Models::SessionType::Classroom,
        Models::SessionType::PreTripInspection,
        Models::SessionType::BasicControl,
        Models::SessionType::RangePractice,
        Models::SessionType::RoadPractice,
        Models::SessionType::SkillsTest,
        Models::SessionType::RoadTest,
        Models::SessionType::FinalExam
    };
    if (typeIndex >= 0 && typeIndex < 9) {
        session.setSessionType(types[typeIndex]);
    }

    // Get selected student (parse from combo text)
    // In production, would store student IDs in combo data

    Api::InstructorResult result;
    if (editingSessionId_.empty()) {
        session.setStatus(Models::SessionStatus::Scheduled);
        result = instructorService_->scheduleSession(session);
    } else {
        result = instructorService_->updateSession(session);
    }

    if (result.success) {
        scheduleDialog_->accept();
        updateSessionsTable();
    } else {
        Wt::WMessageBox::show("Error", "Failed to save session: " + result.message,
                              Wt::StandardButton::Ok);
    }
}

void SchedulingWidget::cancelSession(const std::string& sessionId) {
    if (!instructorService_) return;

    auto result = Wt::WMessageBox::show("Cancel Session",
        "Are you sure you want to cancel this session?",
        Wt::StandardButton::Yes | Wt::StandardButton::No);

    if (result == Wt::StandardButton::Yes) {
        auto apiResult = instructorService_->cancelSession(sessionId, "Cancelled by instructor");
        if (apiResult.success) {
            updateSessionsTable();
        }
    }
}

void SchedulingWidget::startSession(const std::string& sessionId) {
    if (!instructorService_) return;

    auto result = instructorService_->startSession(sessionId);
    if (result.success) {
        updateSessionsTable();
    }
}

void SchedulingWidget::completeSession(const std::string& sessionId) {
    if (!instructorService_) return;

    auto result = instructorService_->completeSession(sessionId);
    if (result.success) {
        updateSessionsTable();
        sessionCompleted_.emit(sessionId);
    }
}

void SchedulingWidget::populateStudentCombo() {
    studentCombo_->clear();

    if (!instructorService_) return;

    auto students = instructorService_->getAssignedStudentProgress(instructorId_);
    for (const auto& student : students) {
        std::string label = student.studentName.empty() ?
            "Student #" + std::to_string(student.studentId) : student.studentName;
        studentCombo_->addItem(label);
    }
}

void SchedulingWidget::populateSessionTypes() {
    sessionTypeCombo_->addItem("Orientation");
    sessionTypeCombo_->addItem("Classroom Session");
    sessionTypeCombo_->addItem("Pre-Trip Inspection");
    sessionTypeCombo_->addItem("Basic Control Skills");
    sessionTypeCombo_->addItem("Range Practice");
    sessionTypeCombo_->addItem("Road Practice");
    sessionTypeCombo_->addItem("Skills Test");
    sessionTypeCombo_->addItem("Road Test");
    sessionTypeCombo_->addItem("Final Exam");
}

} // namespace Instructor
} // namespace StudentIntake
