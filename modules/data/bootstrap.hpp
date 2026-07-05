// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// data::bootstrap -- pull-and-chunk missing assets at startup so a
// fresh clone of ac9 comes online without any pre-populated data/.
//
// Reads data/sources.json (committed), consults data/manifest.json
// (updated in place). For every role in sources.json that is NOT
// already in manifest.json AND has at least one URL, downloads via
// libcurl, hashes + chunks via data::chunk_asset(), and merges the
// result into manifest.json.
//
// Idempotent. Failing a single role does not abort the run; the
// runtime layer decides per-role whether that role is required.

#include <filesystem>
#include <string>
#include <vector>

namespace data {

// Fetch every role listed in sources.json that isn't already in
// manifest.json. Prints progress to stderr. Returns the number of
// roles successfully fetched this call (0 if nothing to do).
int bootstrap(const std::filesystem::path & data_dir);

// Fetch one role by name. `role` must appear in sources.json. Returns
// true on success (including "already in manifest, nothing to do").
bool bootstrap_role(const std::filesystem::path & data_dir,
                    const std::string           & role,
                    bool                          force = false);

// CLI: `ac9 fetch [role]`. With no role, fetches all missing. With a
// role, fetches just that one (adds --force to re-fetch even if
// present in the manifest).
int cli_fetch(int argc, char ** argv);

}  // namespace data
