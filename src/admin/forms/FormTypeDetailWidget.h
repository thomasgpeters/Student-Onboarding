#ifndef FORM_TYPE_DETAIL_WIDGET_H
#define FORM_TYPE_DETAIL_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WTable.h>
#include <Wt/WPushButton.h>
#include <Wt/WCheckBox.h>
#include <Wt/WSignal.h>
#include <memory>
#include <vector>
#include "../../api/FormSubmissionService.h"

namespace StudentIntake {
namespace Admin {

/**
 * @brief Represents a form field definition for display
 */
struct FormFieldDefinition {
    std::string fieldName;      // Internal column/field name
    std::string label;          // Display label
    std::string dataType;       // string, int, bool, date, etc.
    bool isPrintable;           // Whether field should be printed on PDF
    bool isRequired;            // Whether field is required
};

/**
 * @brief Widget for displaying form type details and field definitions
 *
 * Shows form metadata (name, description, category, created date)
 * and a list of all fields defined for that form type.
 */
class FormTypeDetailWidget : public Wt::WContainerWidget {
public:
    FormTypeDetailWidget();
    ~FormTypeDetailWidget() override;

    void setApiService(std::shared_ptr<Api::FormSubmissionService> apiService);
    void loadFormType(int formTypeId);
    void clear() override;

    // Signals
    Wt::Signal<>& backClicked() { return backClicked_; }

private:
    void setupUI();
    void loadFormTypeData();
    void loadFieldDefinitions();
    void updateDisplay();
    void updateFieldsTable();
    std::string formatDate(const std::string& dateStr);
    std::string getDataTypeDisplayName(const std::string& dataType);
    std::vector<FormFieldDefinition> getFieldsForFormType(const std::string& formCode);

    std::shared_ptr<Api::FormSubmissionService> apiService_;
    int currentFormTypeId_;

    // Form type data
    std::string formCode_;
    std::string formName_;
    std::string formDescription_;
    std::string formCategory_;
    int displayOrder_;
    bool isRequired_;
    bool isActive_;
    std::string createdAt_;
    std::string updatedAt_;

    // Field definitions
    std::vector<FormFieldDefinition> fieldDefinitions_;

    // UI Elements - Header
    Wt::WContainerWidget* topBar_;
    Wt::WPushButton* backBtn_;

    // Metadata section
    Wt::WContainerWidget* metadataContainer_;
    Wt::WText* formNameText_;
    Wt::WText* formCodeText_;
    Wt::WText* formDescriptionText_;

    // Info cards
    Wt::WContainerWidget* infoGrid_;
    Wt::WText* categoryText_;
    Wt::WText* orderText_;
    Wt::WText* requiredText_;
    Wt::WText* statusText_;
    Wt::WText* createdText_;
    Wt::WText* updatedText_;

    // Fields section
    Wt::WContainerWidget* fieldsContainer_;
    Wt::WText* fieldsTitle_;
    Wt::WText* fieldsCount_;
    Wt::WTable* fieldsTable_;
    Wt::WText* noFieldsText_;

    // Signals
    Wt::Signal<> backClicked_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // FORM_TYPE_DETAIL_WIDGET_H
