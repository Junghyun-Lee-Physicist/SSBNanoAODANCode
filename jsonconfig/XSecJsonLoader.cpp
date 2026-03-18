// XSecJsonLoader implementation
#include "XSecJsonLoader.hpp"
#include <fstream>
#include <iomanip>

const SampleXSecInfo XSecJsonLoader::EMPTY_SAMPLE = {"", "", 0, 0, 0, 0, 0.0, 1.0, 0.0, ""};

XSecJsonLoader::XSecJsonLoader() : loaded_(false) {}

XSecJsonLoader::~XSecJsonLoader() {}

bool XSecJsonLoader::LoadFile(const std::string& filepath) {
    filepath_ = filepath;
    std::ifstream ifs(filepath);
    if (!ifs.is_open()) {
        std::cerr << "[XSecJsonLoader] ERROR: Cannot open file: " << filepath << std::endl;
        return false;
    }

    nlohmann::json json_data;
    try {
        ifs >> json_data;
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "[XSecJsonLoader] JSON parse error in " << filepath << ": " << e.what() << std::endl;
        return false;
    }

    samples_.clear();
    for (auto it = json_data.begin(); it != json_data.end(); ++it) {
        const std::string& name = it.key();
        const auto& obj = it.value();

        SampleXSecInfo info;
        info.name = name;
        info.das_path = obj.value("das_path", "");
        info.total_events = obj.value("total_events", 0);
        info.positive_events = obj.value("positive_events", 0);
        info.negative_events = obj.value("negative_events", 0);
        info.effective_events = obj.value("effective_events", 0);
        info.cross_section_pb = obj.value("cross_section_pb", 0.0);
        info.branching_fraction = obj.value("branching_fraction", 1.0);
        info.frac_neg_weight = obj.value("frac_neg_weight", 0.0);
        info.comment = obj.value("comment", "");

        samples_[name] = info;
    }

    loaded_ = true;
    std::cout << "[XSecJsonLoader] Loaded: " << filepath << " (" << samples_.size() << " samples)" << std::endl;
    return true;
}

void XSecJsonLoader::PrintoutSamples() const {
    if (!loaded_) {
        std::cerr << "[XSecJsonLoader] No file loaded." << std::endl;
        return;
    }

    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << " XSecJsonLoader - Cross Section Table" << std::endl;
    std::cout << " File: " << filepath_ << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::left
              << std::setw(45) << "SampleName"
              << std::setw(14) << "TotalEvt"
              << std::setw(14) << "Positive"
              << std::setw(14) << "Negative"
              << std::setw(14) << "Pos+Neg"
              << std::setw(12) << "XSec(pb)"
              << std::setw(8)  << "BR"
              << std::endl;
    std::cout << std::string(120, '-') << std::endl;

    for (const auto& pair : samples_) {
        const auto& s = pair.second;
        std::cout << std::left
                  << std::setw(45) << s.name
                  << std::setw(14) << s.total_events
                  << std::setw(14) << s.positive_events
                  << std::setw(14) << s.negative_events
                  << std::setw(14) << s.effective_events
                  << std::setw(12) << std::setprecision(6) << s.cross_section_pb
                  << std::setw(8)  << std::setprecision(5) << s.branching_fraction
                  << std::endl;
    }
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
}

bool XSecJsonLoader::HasSample(const std::string& sampleName) const {
    return samples_.find(sampleName) != samples_.end();
}

const SampleXSecInfo& XSecJsonLoader::GetSample(const std::string& sampleName) const {
    auto it = samples_.find(sampleName);
    if (it != samples_.end()) return it->second;
    std::cerr << "[XSecJsonLoader] Sample not found: " << sampleName << std::endl;
    return EMPTY_SAMPLE;
}

double XSecJsonLoader::GetXSec(const std::string& sampleName) const {
    return GetSample(sampleName).cross_section_pb;
}

double XSecJsonLoader::GetBR(const std::string& sampleName) const {
    return GetSample(sampleName).branching_fraction;
}

int XSecJsonLoader::GetTotalEvents(const std::string& sampleName) const {
    return GetSample(sampleName).total_events;
}

int XSecJsonLoader::GetPositiveEvents(const std::string& sampleName) const {
    return GetSample(sampleName).positive_events;
}

int XSecJsonLoader::GetNegativeEvents(const std::string& sampleName) const {
    return GetSample(sampleName).negative_events;
}

int XSecJsonLoader::GetEffectiveEvents(const std::string& sampleName) const {
    return GetSample(sampleName).effective_events;
}

std::vector<std::string> XSecJsonLoader::GetSampleNames() const {
    std::vector<std::string> names;
    names.reserve(samples_.size());
    for (const auto& pair : samples_) {
        names.push_back(pair.first);
    }
    return names;
}
