// XSecJsonLoader - Load cross section & event info from the-xs-barn CPV JSON files
// Replaces the fscanf-based text parser in Analysis::MCSF()

#ifndef __XSecJsonLoader_hpp_Included__
#define __XSecJsonLoader_hpp_Included__

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "json.hpp"

struct SampleXSecInfo {
    std::string name;
    std::string das_path;
    int total_events;
    int positive_events;
    int negative_events;
    int effective_events;  // positive - negative (= posi_nega in old code)
    double cross_section_pb;
    double branching_fraction;
    double frac_neg_weight;
    std::string comment;
};

class XSecJsonLoader {

public:
    XSecJsonLoader();
    ~XSecJsonLoader();

    // Load from JSON file (the-xs-barn cpv_samples format)
    bool LoadFile(const std::string& filepath);

    // Print all loaded samples (for comparison with old text-based output)
    void PrintoutSamples() const;

    // Access sample info
    bool HasSample(const std::string& sampleName) const;
    const SampleXSecInfo& GetSample(const std::string& sampleName) const;

    // Convenience getters (matching old MCSF() variable names)
    double GetXSec(const std::string& sampleName) const;
    double GetBR(const std::string& sampleName) const;
    int GetTotalEvents(const std::string& sampleName) const;
    int GetPositiveEvents(const std::string& sampleName) const;
    int GetNegativeEvents(const std::string& sampleName) const;
    int GetEffectiveEvents(const std::string& sampleName) const;

    // Get all sample names
    std::vector<std::string> GetSampleNames() const;

    // Get number of loaded samples
    int GetNumSamples() const { return static_cast<int>(samples_.size()); }

private:
    std::map<std::string, SampleXSecInfo> samples_;
    std::string filepath_;
    bool loaded_;

    static const SampleXSecInfo EMPTY_SAMPLE;
};

#endif // __XSecJsonLoader_hpp_Included__
