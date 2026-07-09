// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// models_settings::
// Canonical registry of "task flows" (planner, coder, cleanup, ...) and
// the environment variables that steer which model each flow loads. One
// source of truth so the Models settings UI, the /api/models/config
// endpoint, and the startup setenv() pass all agree on what's swappable.
//
// The persisted store is `settings/models.json`:
//   { "flows": { "<flow_key>": "<role_name>", ... } }
// Every value is a role name that data::role_available() must accept.
// main.cpp calls apply_from_disk_via_setenv() BEFORE the pipeline loader
// spawns so each subsystem's first-init env read sees the operator's
// choice.

#include <string>
#include <string_view>
#include <vector>

namespace models_settings {

struct FlowDef {
    // Short key used in the settings JSON and the /api/models/config wire
    // format ("planner", "coder", ...).
    std::string key;
    // Human label for the UI ("Ticket planner", "Coder", ...).
    std::string label;
    // One-line description for the UI subtitle.
    std::string description;
    // Env var the underlying subsystem reads at first init to pick its
    // role. Populating this via setenv() at startup is how the operator's
    // choice actually takes effect.
    std::string env_var;
    // Default role name if the operator has not overridden it. Matches
    // the historical hardcoded default of each subsystem.
    std::string default_role;
    // True iff the flow requires a role whose dir also has an mmproj
    // partner (vision). The UI can gray out roles missing mmproj for
    // these flows.
    bool        requires_mmproj = false;
};

// Canonical list. Order controls the UI row order.
const std::vector<FlowDef> & flow_defs();

// Return the operator's chosen role for `flow_key`, or the flow's
// default if unset / malformed. Reads settings/models.json each call
// (cheap; JSON is tiny). Empty string if `flow_key` is unknown.
std::string effective_role_for(std::string_view flow_key);

// setenv() every flow's env_var from settings/models.json. Called from
// main.cpp at startup BEFORE any subsystem's first init(). Idempotent.
void apply_from_disk_via_setenv();

}  // namespace models_settings
