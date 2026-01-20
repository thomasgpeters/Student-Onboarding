#include "InstitutionSettings.h"
#include <sstream>

namespace StudentIntake {
namespace Models {

std::string InstitutionSettings::getValue(const std::string& key, const std::string& defaultValue) const {
    auto it = settings_.find(key);
    if (it != settings_.end() && !it->second.empty()) {
        return it->second;
    }
    return defaultValue;
}

void InstitutionSettings::setValue(const std::string& key, const std::string& value) {
    settings_[key] = value;
}

std::string InstitutionSettings::getFormattedAddress() const {
    std::stringstream ss;

    std::string street1 = getAddressStreet1();
    std::string street2 = getAddressStreet2();
    std::string city = getAddressCity();
    std::string state = getAddressState();
    std::string postal = getAddressPostalCode();
    std::string country = getAddressCountry();

    if (!street1.empty()) {
        ss << street1;
        if (!street2.empty()) {
            ss << ", " << street2;
        }
    }

    if (!city.empty() || !state.empty() || !postal.empty()) {
        if (ss.tellp() > 0) ss << ", ";
        if (!city.empty()) {
            ss << city;
            if (!state.empty() || !postal.empty()) ss << ", ";
        }
        if (!state.empty()) {
            ss << state;
            if (!postal.empty()) ss << " ";
        }
        if (!postal.empty()) {
            ss << postal;
        }
    }

    if (!country.empty() && country != "United States") {
        if (ss.tellp() > 0) ss << ", ";
        ss << country;
    }

    return ss.str();
}

nlohmann::json InstitutionSettings::toJson() const {
    nlohmann::json j;
    for (const auto& [key, value] : settings_) {
        j[key] = value;
    }
    return j;
}

InstitutionSettings InstitutionSettings::fromJson(const nlohmann::json& json) {
    InstitutionSettings settings;
    if (json.is_object()) {
        for (auto& [key, value] : json.items()) {
            if (value.is_string()) {
                settings.setValue(key, value.get<std::string>());
            } else if (value.is_number()) {
                settings.setValue(key, std::to_string(value.get<int>()));
            } else if (value.is_boolean()) {
                settings.setValue(key, value.get<bool>() ? "true" : "false");
            }
        }
    }
    return settings;
}

InstitutionSettings InstitutionSettings::fromApiResponse(const nlohmann::json& response) {
    InstitutionSettings settings;

    // Handle JSON:API format with array of setting objects
    nlohmann::json items;

    if (response.is_array()) {
        items = response;
    } else if (response.contains("data")) {
        if (response["data"].is_array()) {
            items = response["data"];
        } else if (response["data"].is_object()) {
            items.push_back(response["data"]);
        }
    }

    for (const auto& item : items) {
        std::string key;
        std::string value;

        // Get the setting_key and setting_value from attributes
        if (item.contains("attributes")) {
            auto& attrs = item["attributes"];
            if (attrs.contains("setting_key") && attrs["setting_key"].is_string()) {
                key = attrs["setting_key"].get<std::string>();
            }
            if (attrs.contains("setting_value")) {
                if (attrs["setting_value"].is_string()) {
                    value = attrs["setting_value"].get<std::string>();
                } else if (attrs["setting_value"].is_null()) {
                    value = "";
                }
            }
        } else {
            // Direct format (non JSON:API)
            if (item.contains("setting_key") && item["setting_key"].is_string()) {
                key = item["setting_key"].get<std::string>();
            }
            if (item.contains("setting_value")) {
                if (item["setting_value"].is_string()) {
                    value = item["setting_value"].get<std::string>();
                } else if (item["setting_value"].is_null()) {
                    value = "";
                }
            }
        }

        if (!key.empty()) {
            settings.setValue(key, value);
        }
    }

    return settings;
}

} // namespace Models
} // namespace StudentIntake
