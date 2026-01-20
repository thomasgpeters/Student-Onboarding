#ifndef INSTITUTION_SETTINGS_H
#define INSTITUTION_SETTINGS_H

#include <string>
#include <map>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Models {

/**
 * @brief Represents institution/school settings for personalization
 */
class InstitutionSettings {
public:
    InstitutionSettings() = default;

    // Branding getters/setters
    std::string getInstitutionName() const { return getValue("institution_name", "Student Intake System"); }
    void setInstitutionName(const std::string& name) { setValue("institution_name", name); }

    std::string getTagline() const { return getValue("institution_tagline", "Official Student Records"); }
    void setTagline(const std::string& tagline) { setValue("institution_tagline", tagline); }

    std::string getLogoUrl() const { return getValue("institution_logo_url", ""); }
    void setLogoUrl(const std::string& url) { setValue("institution_logo_url", url); }

    // Contact getters/setters
    std::string getContactEmail() const { return getValue("contact_email", ""); }
    void setContactEmail(const std::string& email) { setValue("contact_email", email); }

    std::string getContactPhone() const { return getValue("contact_phone", ""); }
    void setContactPhone(const std::string& phone) { setValue("contact_phone", phone); }

    std::string getContactFax() const { return getValue("contact_fax", ""); }
    void setContactFax(const std::string& fax) { setValue("contact_fax", fax); }

    std::string getWebsiteUrl() const { return getValue("website_url", ""); }
    void setWebsiteUrl(const std::string& url) { setValue("website_url", url); }

    // Location getters/setters
    std::string getAddressStreet1() const { return getValue("address_street1", ""); }
    void setAddressStreet1(const std::string& street) { setValue("address_street1", street); }

    std::string getAddressStreet2() const { return getValue("address_street2", ""); }
    void setAddressStreet2(const std::string& street) { setValue("address_street2", street); }

    std::string getAddressCity() const { return getValue("address_city", ""); }
    void setAddressCity(const std::string& city) { setValue("address_city", city); }

    std::string getAddressState() const { return getValue("address_state", ""); }
    void setAddressState(const std::string& state) { setValue("address_state", state); }

    std::string getAddressPostalCode() const { return getValue("address_postal_code", ""); }
    void setAddressPostalCode(const std::string& postalCode) { setValue("address_postal_code", postalCode); }

    std::string getAddressCountry() const { return getValue("address_country", "United States"); }
    void setAddressCountry(const std::string& country) { setValue("address_country", country); }

    // General settings
    std::string getAcademicYear() const { return getValue("academic_year", ""); }
    void setAcademicYear(const std::string& year) { setValue("academic_year", year); }

    std::string getAccreditationInfo() const { return getValue("accreditation_info", ""); }
    void setAccreditationInfo(const std::string& info) { setValue("accreditation_info", info); }

    std::string getTimezone() const { return getValue("timezone", "America/New_York"); }
    void setTimezone(const std::string& tz) { setValue("timezone", tz); }

    // Formatted address helper
    std::string getFormattedAddress() const;

    // Generic value access
    std::string getValue(const std::string& key, const std::string& defaultValue = "") const;
    void setValue(const std::string& key, const std::string& value);

    // Get all settings as a map
    const std::map<std::string, std::string>& getAllSettings() const { return settings_; }

    // JSON serialization
    nlohmann::json toJson() const;
    static InstitutionSettings fromJson(const nlohmann::json& json);

    // Load from API response (array of setting objects)
    static InstitutionSettings fromApiResponse(const nlohmann::json& response);

private:
    std::map<std::string, std::string> settings_;
};

} // namespace Models
} // namespace StudentIntake

#endif // INSTITUTION_SETTINGS_H
