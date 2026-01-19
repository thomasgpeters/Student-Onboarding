#ifndef PDF_GENERATOR_H
#define PDF_GENERATOR_H

#include <string>
#include <vector>
#include <memory>

namespace StudentIntake {
namespace Api {

// Structure to hold a form field for PDF generation
struct PdfFormField {
    std::string label;
    std::string value;
    std::string type;  // "text", "header", "email", "phone", "date", etc.

    PdfFormField(const std::string& l, const std::string& v, const std::string& t = "text")
        : label(l), value(v), type(t) {}
};

// Structure to hold form data for PDF generation
struct PdfFormData {
    std::string formTitle;
    std::string studentName;
    std::string studentEmail;
    std::string submissionDate;
    std::string institutionName;
    std::string institutionTagline;
    std::vector<PdfFormField> fields;
};

class PdfGenerator {
public:
    PdfGenerator();
    ~PdfGenerator();

    // Generate a PDF from form data and return the file path
    std::string generateFormPdf(const PdfFormData& formData, const std::string& outputDir = "/tmp");

    // Generate a PDF with multiple forms (for "Print All Forms")
    std::string generateMultiFormPdf(const std::vector<PdfFormData>& forms,
                                      const std::string& studentName,
                                      const std::string& outputDir = "/tmp");

    // Check if PDF generation is available (libharu found)
    static bool isAvailable();

    // Get the last error message
    std::string getLastError() const { return lastError_; }

private:
    std::string lastError_;

    // Helper methods
    std::string formatPhoneNumber(const std::string& phone);
    std::string formatDate(const std::string& date);
    std::string generateUniqueFilename(const std::string& prefix);
};

} // namespace Api
} // namespace StudentIntake

#endif // PDF_GENERATOR_H
