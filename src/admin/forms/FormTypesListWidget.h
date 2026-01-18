#ifndef FORM_TYPES_LIST_WIDGET_H
#define FORM_TYPES_LIST_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WTable.h>
#include <Wt/WSignal.h>
#include <memory>
#include <vector>
#include "../../api/FormSubmissionService.h"

namespace StudentIntake {
namespace Admin {

/**
 * @brief Represents a form type definition for display
 */
struct FormTypeRecord {
    int id;
    std::string code;
    std::string name;
    std::string description;
    std::string category;
    int displayOrder;
    bool isRequired;
    bool isActive;
    std::string createdAt;
    std::string updatedAt;
};

/**
 * @brief Widget for displaying list of form type definitions
 *
 * This widget shows all form types defined in the system,
 * allowing administrators to view and manage form definitions.
 */
class FormTypesListWidget : public Wt::WContainerWidget {
public:
    FormTypesListWidget();
    ~FormTypesListWidget() override;

    void setApiService(std::shared_ptr<Api::FormSubmissionService> apiService);
    void loadData();
    void clearData();

    // Signals
    Wt::Signal<int>& formTypeSelected() { return formTypeSelected_; }

private:
    void setupUI();
    void loadFormTypes();
    void updateTable();
    std::string formatDate(const std::string& dateStr);
    std::string getCategoryDisplayName(const std::string& category);

    std::shared_ptr<Api::FormSubmissionService> apiService_;
    std::vector<FormTypeRecord> formTypes_;

    // UI Elements
    Wt::WText* headerTitle_;
    Wt::WText* headerSubtitle_;
    Wt::WContainerWidget* statsContainer_;
    Wt::WText* totalFormsText_;
    Wt::WText* requiredFormsText_;
    Wt::WText* optionalFormsText_;
    Wt::WText* activeFormsText_;
    Wt::WContainerWidget* tableContainer_;
    Wt::WTable* formTypesTable_;
    Wt::WText* noDataMessage_;

    // Signals
    Wt::Signal<int> formTypeSelected_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // FORM_TYPES_LIST_WIDGET_H
