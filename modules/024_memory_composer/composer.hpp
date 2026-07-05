#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Memory entry composer: typed wrapper that captures a single
// observation / decision / preference / fact into project memory or
// personal memory, with structured fields (tags, source link,
// confidence, scope) the raw stores do not enforce. Designed for
// "quick-capture from selected text in any pane": the UI fills a
// minimal Composer, validates it, and submits.
//
// Confidence is a 0..1 float and is serialized into the source field
// alongside any caller-provided link so the underlying store does not
// need a schema migration.
namespace memory_composer {

enum class Scope     { Project = 0, Personal = 1 };
enum class EntryKind { Observation = 0, Decision = 1, Preference = 2, Fact = 3 };

const char * to_string(Scope s);
const char * to_string(EntryKind k);
EntryKind    kind_from_string(std::string_view);

struct Draft {
    Scope                    scope      = Scope::Project;
    EntryKind                kind       = EntryKind::Observation;
    std::string              content;
    std::vector<std::string> tags;
    std::string              source_link;     // optional URL / file / chat reference
    double                   confidence = 1.0;
};

struct Submission {
    int64_t id        = 0;     // backing store row id
    Scope   scope     = Scope::Project;
    std::string canonical_source;             // what the composer actually wrote
};

// Validate without writing. Empty `content` and out-of-range confidence
// produce diagnostics; an empty vector means the draft would be accepted.
std::vector<std::string> validate(const Draft & d);

// Compose-and-store. Throws if validate(d) is non-empty. Returns the
// row id of the new entry in whichever store the scope selected.
Submission submit(const Draft & d);

// Edit by id: replaces content / tags / source_link / confidence on
// an existing entry. The kind and scope are immutable here (use
// forget + submit to recategorize / move).
void edit(Scope scope, int64_t id,
          std::string_view content,
          const std::vector<std::string> & tags,
          std::string_view source_link,
          double confidence);

// Format the persisted source field. Exposed for the editor UI to
// show "what will be stored" without round-tripping through submit().
std::string format_source(std::string_view source_link, double confidence);

}
