#include "FormContainer.h"

namespace StudentIntake {
namespace Widgets {

FormContainer::FormContainer()
    : WContainerWidget()
    , formStack_(nullptr)
    , currentIndex_(0) {
    setupUI();
}

FormContainer::~FormContainer() {
}

void FormContainer::setupUI() {
    addStyleClass("form-container");
    formStack_ = addWidget(std::make_unique<Wt::WStackedWidget>());
    formStack_->addStyleClass("form-stack");
}

void FormContainer::loadForms(const std::vector<std::string>& formIds) {
    formStack_->clear();
    forms_.clear();
    formIds_ = formIds;
    currentIndex_ = 0;

    if (!formFactory_) return;

    for (size_t i = 0; i < formIds.size(); ++i) {
        const auto& formId = formIds[i];
        auto form = formFactory_->createForm(formId);

        if (form) {
            Forms::BaseForm* formPtr = form.get();
            forms_[formId] = formPtr;

            // Configure form navigation
            formPtr->setShowPreviousButton(i > 0);
            formPtr->setShowNextButton(true);

            if (i == formIds.size() - 1) {
                formPtr->setNextButtonText("Complete Application");
            }

            // Connect signals
            formPtr->formSubmitted().connect([this, formId]() {
                handleFormSubmitted(formId);
            });

            formPtr->previousRequested().connect([this, formId]() {
                handleFormPrevious(formId);
            });

            // Load any existing data
            formPtr->loadData();

            formStack_->addWidget(std::move(form));
        }
    }

    updateFormNavigation();
}

void FormContainer::showForm(const std::string& formId) {
    for (size_t i = 0; i < formIds_.size(); ++i) {
        if (formIds_[i] == formId) {
            currentIndex_ = static_cast<int>(i);
            formStack_->setCurrentIndex(currentIndex_);
            updateFormNavigation();
            formChanged_.emit(formId);
            return;
        }
    }
}

void FormContainer::showNextForm() {
    if (currentIndex_ < static_cast<int>(formIds_.size()) - 1) {
        currentIndex_++;
        formStack_->setCurrentIndex(currentIndex_);
        updateFormNavigation();
        formChanged_.emit(formIds_[currentIndex_]);
    }
}

void FormContainer::showPreviousForm() {
    if (currentIndex_ > 0) {
        currentIndex_--;
        formStack_->setCurrentIndex(currentIndex_);
        updateFormNavigation();
        formChanged_.emit(formIds_[currentIndex_]);
    }
}

void FormContainer::showFirstIncompleteForm() {
    if (!session_) {
        currentIndex_ = 0;
        formStack_->setCurrentIndex(0);
        if (!formIds_.empty()) {
            formChanged_.emit(formIds_[0]);
        }
        return;
    }

    for (size_t i = 0; i < formIds_.size(); ++i) {
        if (!session_->getStudent().hasCompletedForm(formIds_[i])) {
            currentIndex_ = static_cast<int>(i);
            formStack_->setCurrentIndex(currentIndex_);
            updateFormNavigation();
            formChanged_.emit(formIds_[i]);
            return;
        }
    }

    // All forms complete
    allFormsCompleted_.emit();
}

std::string FormContainer::getCurrentFormId() const {
    if (currentIndex_ >= 0 && currentIndex_ < static_cast<int>(formIds_.size())) {
        return formIds_[currentIndex_];
    }
    return "";
}

int FormContainer::getCurrentFormIndex() const {
    return currentIndex_;
}

int FormContainer::getTotalFormsCount() const {
    return static_cast<int>(formIds_.size());
}

void FormContainer::handleFormSubmitted(const std::string& formId) {
    formSubmitted_.emit(formId);

    // Move to next form or complete
    if (currentIndex_ < static_cast<int>(formIds_.size()) - 1) {
        showNextForm();
    } else {
        allFormsCompleted_.emit();
    }
}

void FormContainer::handleFormPrevious(const std::string& formId) {
    showPreviousForm();
}

void FormContainer::updateFormNavigation() {
    for (size_t i = 0; i < formIds_.size(); ++i) {
        auto it = forms_.find(formIds_[i]);
        if (it != forms_.end()) {
            Forms::BaseForm* form = it->second;
            form->setShowPreviousButton(i > 0);

            if (i == formIds_.size() - 1) {
                form->setNextButtonText("Complete Application");
            } else {
                form->setNextButtonText("Next");
            }
        }
    }
}

} // namespace Widgets
} // namespace StudentIntake
