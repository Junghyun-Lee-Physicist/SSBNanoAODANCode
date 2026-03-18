// JsonConfigReader - JSON-based configuration reader for SSB Analysis
// Provides the same interface as TextReader for seamless integration
// Uses nlohmann/json single-header library

#ifndef __JsonConfigReader_hpp_Included__
#define __JsonConfigReader_hpp_Included__

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include "json.hpp"

class JsonConfigReader {

public:
    JsonConfigReader();
    ~JsonConfigReader();

    // Load JSON file
    void ReadFile(const std::string& filepath);

    // Print all variables (matches TextReader::PrintoutVariables format)
    void PrintoutVariables() const;

    // --- Scalar getters (compatible with TextReader interface) ---
    double GetNumber(const std::string& key, bool PrintError = true) const;
    double GetNumber(const std::string& key, int index, bool PrintError = true) const;
    int GetNumberInt(const std::string& key, bool PrintError = true) const;
    int GetNumberInt(const std::string& key, int index, bool PrintError = true) const;
    unsigned int GetNumberUint(const std::string& key, bool PrintError = true) const;
    unsigned int GetNumberUint(const std::string& key, int index, bool PrintError = true) const;
    float GetNumberFloat(const std::string& key, bool PrintError = true) const;
    float GetNumberFloat(const std::string& key, int index, bool PrintError = true) const;
    double GetNumberDouble(const std::string& key, bool PrintError = true) const;
    double GetNumberDouble(const std::string& key, int index, bool PrintError = true) const;
    std::string GetText(const std::string& key, bool PrintError = true) const;
    std::string GetText(const std::string& key, int index, bool PrintError = true) const;
    bool GetBool(const std::string& key, bool PrintError = true) const;
    bool GetBool(const std::string& key, int index, bool PrintError = true) const;

    // --- Check helpers ---
    bool Check(const std::string& key) const;
    bool CheckNumber(const std::string& key) const;
    bool CheckText(const std::string& key) const;
    bool CheckArray(const std::string& key) const;
    int Size(const std::string& key) const;

    // --- Direct JSON access (new, intuitive API) ---
    // Use these for direct, type-safe access without TextReader compatibility layer
    const nlohmann::json& Get(const std::string& key) const;
    const nlohmann::json& GetJson() const { return json_data_; }
    bool Has(const std::string& key) const { return json_data_.contains(key); }

    template<typename T>
    T GetAs(const std::string& key) const {
        if (!json_data_.contains(key)) {
            throw std::runtime_error("[JsonConfigReader] Key not found: " + key);
        }
        return json_data_[key].get<T>();
    }

    template<typename T>
    T GetAs(const std::string& key, const T& default_value) const {
        if (!json_data_.contains(key)) {
            return default_value;
        }
        return json_data_[key].get<T>();
    }

private:
    nlohmann::json json_data_;
    std::string filepath_;
    bool loaded_;

    // Helper: return DUMMY string when key not found (matches TextReader behavior)
    static const std::string DUMMY_TEXT;
};

#endif // __JsonConfigReader_hpp_Included__
