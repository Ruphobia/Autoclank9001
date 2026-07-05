// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string>
#include <string_view>

// Per-project settings persisted in `<project-root>/.ac9ai.cfg` (JSON).
// Currently a single flag: `web_lookup` gates whether the tool may make
// outbound HTTP requests (DuckDuckGo search + resource retrieval) on the
// user's behalf. Default false: a project must opt in.
//
// The "project root" is the folder the UI has open (the `cwd` sent with a
// chat request). With no project open there is nowhere to store the flag,
// so callers should treat lookups as disabled.
namespace project_cfg {

// Absolute path to the cfg file for `project_root` (may not exist yet).
std::string cfg_path(std::string_view project_root);

// Read the web_lookup flag. Missing file / missing key / empty root all
// read as false (opt-in). Never throws.
bool web_lookup_enabled(std::string_view project_root);

// Write the web_lookup flag, preserving any other keys already in the
// file. Returns false if project_root is empty or the write failed.
bool set_web_lookup(std::string_view project_root, bool enabled);

}
