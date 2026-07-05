#pragma once

#include <string>
#include <string_view>
#include <vector>

// Safety and legality advisor: never refuses, never hedges, never
// declines to answer. Its job is to identify real physical hazards and
// real legal exposure for a given (material, process, activity) tuple
// and prepend a direct factual preface so the answer that follows is
// informed instead of accidentally negligent. Mechanism + citation;
// then the work continues.
//
// Data lives in two layers:
//   - resources/safety/{hazards,legal}.json  bundled seed dataset
//   - <project>/.tool/safety/local_{hazards,legal}.json  project overlay
// The overlay wins by id: an entry with the same id replaces the seed.
namespace safety {

struct HazardEntry {
    std::string              id;
    std::string              material;
    std::vector<std::string> synonyms;
    std::vector<std::string> process_keywords;
    std::string              mechanism;
    std::string              first_aid;
    std::vector<std::string> ppe;
    std::string              severity;
    std::vector<std::string> sources;
};

struct LegalEntry {
    std::string              id;
    std::string              jurisdiction;
    std::vector<std::string> topic_keywords;
    std::string              cite;
    std::string              summary;
};

struct Advice {
    std::vector<HazardEntry> hazards;
    std::vector<LegalEntry>  legal;
    std::string              preface;        // formatted multi-line preface
};

// Loads the seed dataset (and the per-project overlay if present).
// Idempotent. project_root may be empty to skip the overlay.
void init(std::string_view project_root = {});
void shutdown();

// Visible accessors mainly for the UI / tests.
const std::vector<HazardEntry> & hazards();
const std::vector<LegalEntry>  & legal();

// Match anything in `text` against hazard.material, hazard.synonyms, or
// hazard.process_keywords; matches are case-insensitive substring.
Advice evaluate(std::string_view text);

// Convenience: render an Advice into a single Markdown preface block
// ("Hazards", "Legal", with citations). Empty when no advice fired.
std::string format_preface(const Advice & a);

}
