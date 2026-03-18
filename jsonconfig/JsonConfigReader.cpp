// JsonConfigReader implementation
#include "JsonConfigReader.hpp"

const std::string JsonConfigReader::DUMMY_TEXT = "DUMMY";

JsonConfigReader::JsonConfigReader() : loaded_(false) {}

JsonConfigReader::~JsonConfigReader() {}

void JsonConfigReader::ReadFile(const std::string& filepath) {
    filepath_ = filepath;
    std::ifstream ifs(filepath);
    if (!ifs.is_open()) {
        std::cerr << "[JsonConfigReader] ERROR: Cannot open file: " << filepath << std::endl;
        return;
    }
    try {
        ifs >> json_data_;
        loaded_ = true;
        std::cout << "[JsonConfigReader] Loaded: " << filepath << " (" << json_data_.size() << " keys)" << std::endl;
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "[JsonConfigReader] JSON parse error in " << filepath << ": " << e.what() << std::endl;
    }
}

void JsonConfigReader::PrintoutVariables() const {
    if (!loaded_) {
        std::cerr << "[JsonConfigReader] No file loaded." << std::endl;
        return;
    }
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << " JsonConfigReader Variables" << std::endl;
    std::cout << " File: " << filepath_ << std::endl;
    std::cout << "========================================" << std::endl;

    for (auto it = json_data_.begin(); it != json_data_.end(); ++it) {
        const auto& key = it.key();
        const auto& val = it.value();

        if (val.is_array()) {
            std::cout << "  " << std::left << std::setw(30) << key << " : {";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) std::cout << ", ";
                if (val[i].is_string()) {
                    std::cout << "\"" << val[i].get<std::string>() << "\"";
                } else {
                    std::cout << val[i];
                }
            }
            std::cout << "}" << std::endl;
        } else if (val.is_string()) {
            std::cout << "  " << std::left << std::setw(30) << key << " : \"" << val.get<std::string>() << "\"" << std::endl;
        } else if (val.is_number_float()) {
            std::cout << "  " << std::left << std::setw(30) << key << " : " << std::setprecision(15) << val.get<double>() << std::endl;
        } else if (val.is_number_integer()) {
            std::cout << "  " << std::left << std::setw(30) << key << " : " << val.get<int64_t>() << std::endl;
        } else if (val.is_boolean()) {
            std::cout << "  " << std::left << std::setw(30) << key << " : " << (val.get<bool>() ? "true" : "false") << std::endl;
        } else if (val.is_null()) {
            std::cout << "  " << std::left << std::setw(30) << key << " : null" << std::endl;
        }
    }
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
}

// --- Scalar getters ---

double JsonConfigReader::GetNumber(const std::string& key, bool PrintError) const {
    if (!json_data_.contains(key)) {
        if (PrintError) std::cerr << "[JsonConfigReader] Key not found (number): " << key << std::endl;
        return 0.0;
    }
    const auto& val = json_data_[key];
    if (val.is_number()) return val.get<double>();
    if (val.is_string()) return std::atof(val.get<std::string>().c_str());
    if (PrintError) std::cerr << "[JsonConfigReader] Key is not a number: " << key << std::endl;
    return 0.0;
}

double JsonConfigReader::GetNumber(const std::string& key, int index, bool PrintError) const {
    if (!json_data_.contains(key)) {
        if (PrintError) std::cerr << "[JsonConfigReader] Key not found (array number): " << key << std::endl;
        return 0.0;
    }
    const auto& val = json_data_[key];
    if (!val.is_array()) {
        if (PrintError) std::cerr << "[JsonConfigReader] Key is not an array: " << key << std::endl;
        return 0.0;
    }
    int idx = index - 1; // TextReader uses 1-based indexing
    if (idx < 0 || idx >= (int)val.size()) {
        if (PrintError) std::cerr << "[JsonConfigReader] Index out of range for " << key << ": " << index << std::endl;
        return 0.0;
    }
    if (val[idx].is_number()) return val[idx].get<double>();
    if (val[idx].is_string()) return std::atof(val[idx].get<std::string>().c_str());
    return 0.0;
}

int JsonConfigReader::GetNumberInt(const std::string& key, bool PrintError) const {
    return static_cast<int>(GetNumber(key, PrintError));
}

int JsonConfigReader::GetNumberInt(const std::string& key, int index, bool PrintError) const {
    return static_cast<int>(GetNumber(key, index, PrintError));
}

unsigned int JsonConfigReader::GetNumberUint(const std::string& key, bool PrintError) const {
    return static_cast<unsigned int>(GetNumber(key, PrintError));
}

unsigned int JsonConfigReader::GetNumberUint(const std::string& key, int index, bool PrintError) const {
    return static_cast<unsigned int>(GetNumber(key, index, PrintError));
}

float JsonConfigReader::GetNumberFloat(const std::string& key, bool PrintError) const {
    return static_cast<float>(GetNumber(key, PrintError));
}

float JsonConfigReader::GetNumberFloat(const std::string& key, int index, bool PrintError) const {
    return static_cast<float>(GetNumber(key, index, PrintError));
}

double JsonConfigReader::GetNumberDouble(const std::string& key, bool PrintError) const {
    return GetNumber(key, PrintError);
}

double JsonConfigReader::GetNumberDouble(const std::string& key, int index, bool PrintError) const {
    return GetNumber(key, index, PrintError);
}

std::string JsonConfigReader::GetText(const std::string& key, bool PrintError) const {
    if (!json_data_.contains(key)) {
        if (PrintError) std::cerr << "[JsonConfigReader] Key not found (text): " << key << std::endl;
        return DUMMY_TEXT;
    }
    const auto& val = json_data_[key];
    if (val.is_string()) return val.get<std::string>();
    if (val.is_number()) return std::to_string(val.get<double>());
    if (val.is_boolean()) return val.get<bool>() ? "true" : "false";
    if (PrintError) std::cerr << "[JsonConfigReader] Key is not text: " << key << std::endl;
    return DUMMY_TEXT;
}

std::string JsonConfigReader::GetText(const std::string& key, int index, bool PrintError) const {
    if (!json_data_.contains(key)) {
        if (PrintError) std::cerr << "[JsonConfigReader] Key not found (array text): " << key << std::endl;
        return DUMMY_TEXT;
    }
    const auto& val = json_data_[key];
    if (!val.is_array()) {
        if (PrintError) std::cerr << "[JsonConfigReader] Key is not an array: " << key << std::endl;
        return DUMMY_TEXT;
    }
    int idx = index - 1; // TextReader uses 1-based indexing
    if (idx < 0 || idx >= (int)val.size()) {
        if (PrintError) std::cerr << "[JsonConfigReader] Index out of range for " << key << ": " << index << std::endl;
        return DUMMY_TEXT;
    }
    if (val[idx].is_string()) return val[idx].get<std::string>();
    if (val[idx].is_number()) return std::to_string(val[idx].get<double>());
    return DUMMY_TEXT;
}

bool JsonConfigReader::GetBool(const std::string& key, bool PrintError) const {
    if (!json_data_.contains(key)) {
        if (PrintError) std::cerr << "[JsonConfigReader] Key not found (bool): " << key << std::endl;
        return false;
    }
    const auto& val = json_data_[key];
    if (val.is_boolean()) return val.get<bool>();
    if (val.is_string()) {
        std::string s = val.get<std::string>();
        return (s == "true" || s == "True" || s == "1" || s == "TRUE");
    }
    if (val.is_number()) return val.get<int>() != 0;
    return false;
}

bool JsonConfigReader::GetBool(const std::string& key, int index, bool PrintError) const {
    if (!json_data_.contains(key)) {
        if (PrintError) std::cerr << "[JsonConfigReader] Key not found (array bool): " << key << std::endl;
        return false;
    }
    const auto& val = json_data_[key];
    if (!val.is_array()) return GetBool(key, PrintError);
    int idx = index - 1;
    if (idx < 0 || idx >= (int)val.size()) return false;
    if (val[idx].is_boolean()) return val[idx].get<bool>();
    if (val[idx].is_string()) {
        std::string s = val[idx].get<std::string>();
        return (s == "true" || s == "True" || s == "1" || s == "TRUE");
    }
    return false;
}

// --- Check helpers ---

bool JsonConfigReader::Check(const std::string& key) const {
    return json_data_.contains(key);
}

bool JsonConfigReader::CheckNumber(const std::string& key) const {
    return json_data_.contains(key) && json_data_[key].is_number();
}

bool JsonConfigReader::CheckText(const std::string& key) const {
    return json_data_.contains(key) && json_data_[key].is_string();
}

bool JsonConfigReader::CheckArray(const std::string& key) const {
    return json_data_.contains(key) && json_data_[key].is_array();
}

int JsonConfigReader::Size(const std::string& key) const {
    if (!json_data_.contains(key)) return 0;
    const auto& val = json_data_[key];
    if (val.is_array()) return static_cast<int>(val.size());
    return 1; // scalar has size 1
}

// --- Direct JSON access ---

const nlohmann::json& JsonConfigReader::Get(const std::string& key) const {
    if (!json_data_.contains(key)) {
        throw std::runtime_error("[JsonConfigReader] Key not found: " + key);
    }
    return json_data_[key];
}
