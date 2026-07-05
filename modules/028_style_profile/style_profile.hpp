#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Personal Style Profile: the user-facing one-pager that describes how
// the user wants the assistant to write. Stored as plain markdown at
// $TOOL_PERSONAL_DIR or $HOME/.tool/personal/style.md so the user can
// open it in any editor. The module knows how to load, save, and
// distill the profile into a system-prompt fragment the answering
// models can be prefixed with.
namespace style_profile {

struct Profile {
    std::string              tone;              // "terse", "warm", "academic", ...
    std::string              pronouns;          // free-form
    std::vector<std::string> units_preferred;   // "metric", "imperial", "SI", ...
    std::vector<std::string> avoid;             // "em dashes", "emoji", "exclamation"
    std::vector<std::string> use;               // "code fences", "inline citations"
    std::string              notes;             // catch-all free-form notes
};

void init();
void shutdown();

// Where is the profile stored? Pass override_root non-empty to force a
// directory (useful in tests). Without it, $TOOL_PERSONAL_DIR /
// $HOME/.tool/personal/ in that order.
std::string profile_path(std::string_view override_root = {});

// Returns the on-disk profile, or a default-constructed Profile if no
// file exists yet.
Profile load(std::string_view override_root = {});

// Write the profile to disk in a sectioned markdown format the user
// can hand-edit safely.
void save(const Profile & p, std::string_view override_root = {});

// Distill the profile into a system-prompt fragment. Empty when the
// profile carries no actionable signal.
std::string format_system_prompt(const Profile & p);

}
