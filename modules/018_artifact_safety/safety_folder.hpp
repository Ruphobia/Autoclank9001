#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Safety folder maintainer: writes a per-hazard markdown file under
// <project>/.tool/safety/incidents/ that accumulates over the life of
// the project. The 016_safety advisor uses this maintainer in two
// ways: it asks `has_been_explained(id)` before re-emitting the long
// preface for a hazard the project has already acknowledged, and it
// calls `record_hazard(id, ...)` the first time a hazard fires so the
// file exists for next time. Users can hand-edit the files in any
// markdown editor; the maintainer treats them as plain text on every
// subsequent read.
namespace safety_folder {

struct Entry {
    std::string id;                 // hazard id (matches safety::HazardEntry::id)
    std::string title;              // material name as recorded
    std::string body;               // raw markdown
    int64_t     first_seen_unix = 0;
    int64_t     last_touched_unix = 0;
    size_t      mitigation_count = 0;
    size_t      incident_count   = 0;
};

void init();
void shutdown();

// True if a file already exists for `hazard_id` under the project.
bool has_been_explained(std::string_view hazard_id,
                        std::string_view project_root);

// Create or update the per-hazard file. Idempotent on the header /
// "Explained at" section; subsequent calls update the timestamp but
// do not wipe the user's mitigations or incidents.
void record_hazard(std::string_view hazard_id,
                   std::string_view title,
                   std::string_view mechanism,
                   std::string_view project_root);

// Append a Mitigation or Incident bullet, dated. Creates the file
// (with a stub header) if it doesn't yet exist.
void log_mitigation(std::string_view hazard_id,
                    std::string_view note,
                    std::string_view project_root);
void log_incident  (std::string_view hazard_id,
                    std::string_view note,
                    std::string_view project_root);

// Listings / reads.
std::vector<std::string>      list_known(std::string_view project_root);
std::optional<Entry>          read(std::string_view hazard_id,
                                   std::string_view project_root);

}
