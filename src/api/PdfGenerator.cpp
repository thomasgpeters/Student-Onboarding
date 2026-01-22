#include "PdfGenerator.h"
#include "utils/Logger.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <cstring>

#ifdef HAVE_LIBHARU
#include <hpdf.h>
#endif

namespace StudentIntake {
namespace Api {

#ifdef HAVE_LIBHARU
// Error handler for libharu
static void hpdf_error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void* user_data) {
    std::string* errorMsg = static_cast<std::string*>(user_data);
    std::ostringstream oss;
    oss << "HPDF Error: error_no=" << std::hex << error_no
        << ", detail_no=" << detail_no;
    *errorMsg = oss.str();
    LOG_ERROR("PdfGenerator", *errorMsg);
}
#endif

PdfGenerator::PdfGenerator() : lastError_("") {
}

PdfGenerator::~PdfGenerator() {
}

bool PdfGenerator::isAvailable() {
#ifdef HAVE_LIBHARU
    return true;
#else
    return false;
#endif
}

std::string PdfGenerator::generateUniqueFilename(const std::string& prefix) {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << prefix << "_"
        << std::put_time(std::localtime(&time_t_now), "%Y%m%d_%H%M%S")
        << "_" << ms.count() << ".pdf";
    return oss.str();
}

std::string PdfGenerator::formatPhoneNumber(const std::string& phone) {
    // Extract digits only
    std::string digits;
    for (char c : phone) {
        if (std::isdigit(c)) {
            digits += c;
        }
    }

    // Format as (XXX) XXX-XXXX if 10 digits
    if (digits.length() == 10) {
        return "(" + digits.substr(0, 3) + ") " + digits.substr(3, 3) + "-" + digits.substr(6, 4);
    }
    return phone;
}

std::string PdfGenerator::formatDate(const std::string& date) {
    if (date.empty()) return "";

    // Try to parse ISO format and convert to readable format
    if (date.length() >= 10 && date[4] == '-' && date[7] == '-') {
        std::string year = date.substr(0, 4);
        std::string month = date.substr(5, 2);
        std::string day = date.substr(8, 2);

        // Convert month to name
        const char* months[] = {"", "January", "February", "March", "April", "May", "June",
                                "July", "August", "September", "October", "November", "December"};
        int monthNum = std::stoi(month);
        if (monthNum >= 1 && monthNum <= 12) {
            return std::string(months[monthNum]) + " " + std::to_string(std::stoi(day)) + ", " + year;
        }
    }
    return date;
}

std::string PdfGenerator::generateFormPdf(const PdfFormData& formData, const std::string& outputDir) {
#ifdef HAVE_LIBHARU
    lastError_ = "";

    HPDF_Doc pdf = HPDF_New(hpdf_error_handler, &lastError_);
    if (!pdf) {
        lastError_ = "Failed to create PDF document";
        return "";
    }

    try {
        // Set compression
        HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);

        // Add a page (Letter size: 8.5 x 11 inches = 612 x 792 points)
        HPDF_Page page = HPDF_AddPage(pdf);
        HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_LETTER, HPDF_PAGE_PORTRAIT);

        float pageWidth = HPDF_Page_GetWidth(page);
        float pageHeight = HPDF_Page_GetHeight(page);
        float margin = 50;  // 0.7 inch margins
        float contentWidth = pageWidth - 2 * margin;

        // Load fonts
        HPDF_Font fontRegular = HPDF_GetFont(pdf, "Helvetica", nullptr);
        HPDF_Font fontBold = HPDF_GetFont(pdf, "Helvetica-Bold", nullptr);

        float yPos = pageHeight - margin;

        // Header - Institution Name
        HPDF_Page_SetFontAndSize(page, fontBold, 18);
        HPDF_Page_SetRGBFill(page, 0.12, 0.23, 0.37);  // Dark blue #1e3a5f

        std::string instName = formData.institutionName.empty() ?
            "Student Intake System" : formData.institutionName;
        float textWidth = HPDF_Page_TextWidth(page, instName.c_str());
        HPDF_Page_BeginText(page);
        HPDF_Page_TextOut(page, (pageWidth - textWidth) / 2, yPos, instName.c_str());
        HPDF_Page_EndText(page);

        yPos -= 18;

        // Tagline
        HPDF_Page_SetFontAndSize(page, fontRegular, 10);
        HPDF_Page_SetRGBFill(page, 0.4, 0.45, 0.53);  // Gray #64748b
        std::string tagline = formData.institutionTagline.empty() ?
            "Official Student Records" : formData.institutionTagline;
        textWidth = HPDF_Page_TextWidth(page, tagline.c_str());
        HPDF_Page_BeginText(page);
        HPDF_Page_TextOut(page, (pageWidth - textWidth) / 2, yPos, tagline.c_str());
        HPDF_Page_EndText(page);

        yPos -= 25;

        // Form Title
        HPDF_Page_SetFontAndSize(page, fontBold, 14);
        HPDF_Page_SetRGBFill(page, 0.2, 0.25, 0.33);  // #334155
        textWidth = HPDF_Page_TextWidth(page, formData.formTitle.c_str());
        HPDF_Page_BeginText(page);
        HPDF_Page_TextOut(page, (pageWidth - textWidth) / 2, yPos, formData.formTitle.c_str());
        HPDF_Page_EndText(page);

        yPos -= 15;

        // Header line
        HPDF_Page_SetRGBStroke(page, 0.12, 0.23, 0.37);
        HPDF_Page_SetLineWidth(page, 2);
        HPDF_Page_MoveTo(page, margin, yPos);
        HPDF_Page_LineTo(page, pageWidth - margin, yPos);
        HPDF_Page_Stroke(page);

        yPos -= 25;

        // Submission date bar
        if (!formData.submissionDate.empty()) {
            HPDF_Page_SetRGBFill(page, 0.97, 0.98, 0.99);  // Light gray background
            HPDF_Page_Rectangle(page, margin, yPos - 20, contentWidth, 25);
            HPDF_Page_Fill(page);

            HPDF_Page_SetFontAndSize(page, fontBold, 10);
            HPDF_Page_SetRGBFill(page, 0.28, 0.33, 0.41);
            HPDF_Page_BeginText(page);
            HPDF_Page_TextOut(page, margin + 10, yPos - 12, "Submitted:");
            HPDF_Page_EndText(page);

            HPDF_Page_SetFontAndSize(page, fontRegular, 10);
            HPDF_Page_SetRGBFill(page, 0.12, 0.16, 0.23);
            HPDF_Page_BeginText(page);
            HPDF_Page_TextOut(page, margin + 75, yPos - 12, formatDate(formData.submissionDate).c_str());
            HPDF_Page_EndText(page);

            yPos -= 35;
        }

        // Table header
        float col1Width = contentWidth * 0.4;
        float col2Width = contentWidth * 0.6;

        HPDF_Page_SetRGBFill(page, 0.95, 0.96, 0.97);  // Header background
        HPDF_Page_Rectangle(page, margin, yPos - 20, contentWidth, 22);
        HPDF_Page_Fill(page);

        HPDF_Page_SetFontAndSize(page, fontBold, 10);
        HPDF_Page_SetRGBFill(page, 0.28, 0.33, 0.41);
        HPDF_Page_BeginText(page);
        HPDF_Page_TextOut(page, margin + 8, yPos - 13, "Field");
        HPDF_Page_TextOut(page, margin + col1Width + 8, yPos - 13, "Value");
        HPDF_Page_EndText(page);

        // Header bottom line
        HPDF_Page_SetRGBStroke(page, 0.8, 0.84, 0.88);
        HPDF_Page_SetLineWidth(page, 1.5);
        HPDF_Page_MoveTo(page, margin, yPos - 20);
        HPDF_Page_LineTo(page, pageWidth - margin, yPos - 20);
        HPDF_Page_Stroke(page);

        yPos -= 25;

        // Table rows
        HPDF_Page_SetLineWidth(page, 0.5);
        for (const auto& field : formData.fields) {
            // Skip empty fields
            if (field.label.empty() && field.value.empty()) continue;

            // Check if we need a new page
            if (yPos < margin + 80) {
                page = HPDF_AddPage(pdf);
                HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_LETTER, HPDF_PAGE_PORTRAIT);
                yPos = pageHeight - margin;
            }

            // Handle section headers
            if (field.type == "header") {
                yPos -= 10;  // Extra space before header
                HPDF_Page_SetRGBFill(page, 0.93, 0.96, 1.0);  // Light blue background
                HPDF_Page_Rectangle(page, margin, yPos - 18, contentWidth, 22);
                HPDF_Page_Fill(page);

                // Blue left border
                HPDF_Page_SetRGBFill(page, 0.23, 0.51, 0.97);  // #3b82f6
                HPDF_Page_Rectangle(page, margin, yPos - 18, 3, 22);
                HPDF_Page_Fill(page);

                HPDF_Page_SetFontAndSize(page, fontBold, 10);
                HPDF_Page_SetRGBFill(page, 0.12, 0.25, 0.5);
                HPDF_Page_BeginText(page);
                HPDF_Page_TextOut(page, margin + 10, yPos - 12, field.label.c_str());
                HPDF_Page_EndText(page);

                yPos -= 28;
                continue;
            }

            // Regular row
            float rowHeight = 20;

            // Field label
            HPDF_Page_SetFontAndSize(page, fontRegular, 10);
            HPDF_Page_SetRGBFill(page, 0.28, 0.33, 0.41);
            HPDF_Page_BeginText(page);
            HPDF_Page_TextOut(page, margin + 8, yPos - 13, field.label.c_str());
            HPDF_Page_EndText(page);

            // Field value - format based on type
            std::string displayValue = field.value;
            if (field.type == "phone") {
                displayValue = formatPhoneNumber(field.value);
            } else if (field.type == "date") {
                displayValue = formatDate(field.value);
            }

            HPDF_Page_SetRGBFill(page, 0.12, 0.16, 0.23);
            HPDF_Page_BeginText(page);
            HPDF_Page_TextOut(page, margin + col1Width + 8, yPos - 13, displayValue.c_str());
            HPDF_Page_EndText(page);

            // Row bottom line
            HPDF_Page_SetRGBStroke(page, 0.89, 0.91, 0.94);
            HPDF_Page_MoveTo(page, margin, yPos - rowHeight);
            HPDF_Page_LineTo(page, pageWidth - margin, yPos - rowHeight);
            HPDF_Page_Stroke(page);

            yPos -= rowHeight + 2;
        }

        // Footer
        yPos = margin + 40;

        // Footer line
        HPDF_Page_SetRGBStroke(page, 0.89, 0.91, 0.94);
        HPDF_Page_SetLineWidth(page, 0.5);
        HPDF_Page_MoveTo(page, margin, yPos);
        HPDF_Page_LineTo(page, pageWidth - margin, yPos);
        HPDF_Page_Stroke(page);

        yPos -= 15;

        // Generated timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::ostringstream timeStr;
        timeStr << "Generated: " << std::put_time(std::localtime(&time_t_now), "%B %d, %Y at %I:%M %p");

        HPDF_Page_SetFontAndSize(page, fontRegular, 8);
        HPDF_Page_SetRGBFill(page, 0.4, 0.45, 0.53);
        textWidth = HPDF_Page_TextWidth(page, timeStr.str().c_str());
        HPDF_Page_BeginText(page);
        HPDF_Page_TextOut(page, (pageWidth - textWidth) / 2, yPos, timeStr.str().c_str());
        HPDF_Page_EndText(page);

        yPos -= 12;

        // Confidential notice
        HPDF_Page_SetFontAndSize(page, fontBold, 7);
        HPDF_Page_SetRGBFill(page, 0.86, 0.15, 0.15);  // Red
        const char* confText = "CONFIDENTIAL - OFFICIAL STUDENT RECORD";
        textWidth = HPDF_Page_TextWidth(page, confText);
        HPDF_Page_BeginText(page);
        HPDF_Page_TextOut(page, (pageWidth - textWidth) / 2, yPos, confText);
        HPDF_Page_EndText(page);

        // Save to file
        std::string filename = generateUniqueFilename("form");
        std::string fullPath = outputDir + "/" + filename;

        HPDF_SaveToFile(pdf, fullPath.c_str());
        HPDF_Free(pdf);

        LOG_INFO("PdfGenerator", "Generated PDF: " << fullPath);
        return fullPath;

    } catch (const std::exception& e) {
        lastError_ = std::string("Exception: ") + e.what();
        HPDF_Free(pdf);
        return "";
    }

#else
    lastError_ = "PDF generation not available - libharu not installed";
    return "";
#endif
}

std::string PdfGenerator::generateMultiFormPdf(const std::vector<PdfFormData>& forms,
                                                const std::string& studentName,
                                                const std::string& outputDir) {
#ifdef HAVE_LIBHARU
    lastError_ = "";

    if (forms.empty()) {
        lastError_ = "No forms provided";
        return "";
    }

    HPDF_Doc pdf = HPDF_New(hpdf_error_handler, &lastError_);
    if (!pdf) {
        lastError_ = "Failed to create PDF document";
        return "";
    }

    try {
        HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);

        HPDF_Font fontRegular = HPDF_GetFont(pdf, "Helvetica", nullptr);
        HPDF_Font fontBold = HPDF_GetFont(pdf, "Helvetica-Bold", nullptr);

        for (size_t formIndex = 0; formIndex < forms.size(); ++formIndex) {
            const auto& formData = forms[formIndex];

            HPDF_Page page = HPDF_AddPage(pdf);
            HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_LETTER, HPDF_PAGE_PORTRAIT);

            float pageWidth = HPDF_Page_GetWidth(page);
            float pageHeight = HPDF_Page_GetHeight(page);
            float margin = 50;
            float contentWidth = pageWidth - 2 * margin;
            float yPos = pageHeight - margin;

            // Header
            HPDF_Page_SetFontAndSize(page, fontBold, 16);
            HPDF_Page_SetRGBFill(page, 0.12, 0.23, 0.37);
            std::string instName = formData.institutionName.empty() ?
                "Student Intake System" : formData.institutionName;
            float textWidth = HPDF_Page_TextWidth(page, instName.c_str());
            HPDF_Page_BeginText(page);
            HPDF_Page_TextOut(page, (pageWidth - textWidth) / 2, yPos, instName.c_str());
            HPDF_Page_EndText(page);

            yPos -= 20;

            // Form title
            HPDF_Page_SetFontAndSize(page, fontBold, 12);
            HPDF_Page_SetRGBFill(page, 0.2, 0.25, 0.33);
            textWidth = HPDF_Page_TextWidth(page, formData.formTitle.c_str());
            HPDF_Page_BeginText(page);
            HPDF_Page_TextOut(page, (pageWidth - textWidth) / 2, yPos, formData.formTitle.c_str());
            HPDF_Page_EndText(page);

            yPos -= 15;

            // Line
            HPDF_Page_SetRGBStroke(page, 0.12, 0.23, 0.37);
            HPDF_Page_SetLineWidth(page, 1.5);
            HPDF_Page_MoveTo(page, margin, yPos);
            HPDF_Page_LineTo(page, pageWidth - margin, yPos);
            HPDF_Page_Stroke(page);

            yPos -= 20;

            // Submission date
            if (!formData.submissionDate.empty()) {
                HPDF_Page_SetFontAndSize(page, fontRegular, 9);
                HPDF_Page_SetRGBFill(page, 0.4, 0.45, 0.53);
                std::string dateStr = "Submitted: " + formatDate(formData.submissionDate);
                HPDF_Page_BeginText(page);
                HPDF_Page_TextOut(page, margin, yPos, dateStr.c_str());
                HPDF_Page_EndText(page);
                yPos -= 20;
            }

            // Fields
            float col1Width = contentWidth * 0.4;
            for (const auto& field : formData.fields) {
                if (field.label.empty() && field.value.empty()) continue;

                if (yPos < margin + 60) {
                    page = HPDF_AddPage(pdf);
                    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_LETTER, HPDF_PAGE_PORTRAIT);
                    yPos = pageHeight - margin;
                }

                if (field.type == "header") {
                    yPos -= 8;
                    HPDF_Page_SetFontAndSize(page, fontBold, 10);
                    HPDF_Page_SetRGBFill(page, 0.12, 0.25, 0.5);
                    HPDF_Page_BeginText(page);
                    HPDF_Page_TextOut(page, margin, yPos, field.label.c_str());
                    HPDF_Page_EndText(page);
                    yPos -= 15;
                    continue;
                }

                HPDF_Page_SetFontAndSize(page, fontRegular, 9);
                HPDF_Page_SetRGBFill(page, 0.28, 0.33, 0.41);
                HPDF_Page_BeginText(page);
                HPDF_Page_TextOut(page, margin, yPos, field.label.c_str());
                HPDF_Page_EndText(page);

                std::string displayValue = field.value;
                if (field.type == "phone") displayValue = formatPhoneNumber(field.value);
                else if (field.type == "date") displayValue = formatDate(field.value);

                HPDF_Page_SetRGBFill(page, 0.12, 0.16, 0.23);
                HPDF_Page_BeginText(page);
                HPDF_Page_TextOut(page, margin + col1Width, yPos, displayValue.c_str());
                HPDF_Page_EndText(page);

                yPos -= 16;
            }

            // Page number footer
            HPDF_Page_SetFontAndSize(page, fontRegular, 8);
            HPDF_Page_SetRGBFill(page, 0.4, 0.45, 0.53);
            std::ostringstream pageNum;
            pageNum << "Page " << (formIndex + 1) << " of " << forms.size();
            textWidth = HPDF_Page_TextWidth(page, pageNum.str().c_str());
            HPDF_Page_BeginText(page);
            HPDF_Page_TextOut(page, (pageWidth - textWidth) / 2, margin, pageNum.str().c_str());
            HPDF_Page_EndText(page);
        }

        std::string filename = generateUniqueFilename("student_forms");
        std::string fullPath = outputDir + "/" + filename;

        HPDF_SaveToFile(pdf, fullPath.c_str());
        HPDF_Free(pdf);

        LOG_INFO("PdfGenerator", "Generated multi-form PDF: " << fullPath);
        return fullPath;

    } catch (const std::exception& e) {
        lastError_ = std::string("Exception: ") + e.what();
        HPDF_Free(pdf);
        return "";
    }

#else
    lastError_ = "PDF generation not available - libharu not installed";
    return "";
#endif
}

} // namespace Api
} // namespace StudentIntake
