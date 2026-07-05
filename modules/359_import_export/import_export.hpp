// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <optional>
#include <string>
#include <string_view>

// Non-kicad-cli import/export.
//
// MVP scope:
//   * Specctra DSN export from Board (for freerouting).
//   * Specctra SES import back into Board (routed traces + vias).
//   * IPC-2581 skeleton (well-known JSON-flavored envelope) that
//     downstream fab CAM tools accept as a checkpoint format.
//
// Not covered here: Altium (.SchDoc/.PcbDoc), EAGLE (.sch/.brd),
// OrCAD, PADS, Allegro; those parsers are large enough to be their
// own modules and are marked as follow-up in todo.txt.
namespace import_export {

// -------------------- Specctra DSN --------------------

struct DsnOptions {
    // Board resolution factor. Specctra uses um by default; we use nm
    // to preserve fidelity and mark the resolution accordingly.
    int  resolution_um = 1000;
    // Include a comment header block.
    bool header_comments = true;
    // Track width to fall back to when a PcbTrack has width_nm == 0.
    long long default_track_nm = 200000;   // 0.2 mm
};

std::string to_dsn(const kicad_model::Board & board, const DsnOptions & opts = {});

// SES import: adds/updates track segments + vias on the given Board.
// Segments in the SES that already exist in the Board (matching net
// + endpoints) are left alone; new ones are inserted with fresh UUIDs.
struct SesResult {
    std::size_t new_tracks = 0;
    std::size_t new_vias   = 0;
    std::string warnings;
    bool ok = false;
};
SesResult apply_ses(kicad_model::Board & board, std::string_view ses_text);

// -------------------- IPC-2581 --------------------

// Emits a minimal IPC-2581 envelope covering board outline + layer
// stackup + net list + component placement. Full CAM sections
// (drill spans, phys stack layers, bill-of-materials refs) are
// stubbed with reasonable defaults.
std::string to_ipc2581(const kicad_model::Board & board);

}
