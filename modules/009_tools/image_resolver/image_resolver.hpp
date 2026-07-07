// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string>
#include <string_view>
#include <vector>

// Resolve "which image does the user want to edit" from a natural-language
// prompt. The image editor (advanced_raster_image_editor_photoshop_gimp_class)
// used to fail with "no input image to edit" whenever the session had no
// prior gen/edit turn. That put ac9 in the position of giving up on a request
// the user was perfectly capable of specifying — by filename ("edit foo.png")
// or by description ("edit the kitty picture"). This module never gives up
// while there is anything in the project tree to look at.
//
// Cascade, first hit wins:
//   1. Explicit filename token in the prompt (e.g. "foo.png", "black-kitty").
//      Search .ac9_images/ first, then the ENTIRE project tree, exhaustively.
//      No depth cap, no file cap, no skip-list of "vendored" dirs — the point
//      of ac9 is not to give up.
//   2. Session state — the existing behavior. Newest gen_path/edit_path record
//      with a real file >=100 KB (skips blank sd-cli failure PNGs).
//   3. Description match. Enumerate every image in the project, describe each
//      via vision::describe() (Qwen3-VL-8B) with a persistent disk cache, then
//      let the coder LLM pick the best match against the user's prompt.
//      Cache lives at <cwd>/.ac9_images/.descriptions.json keyed by absolute
//      path -> {mtime, size, description}.
//
// When step 3 turns up two or more plausible matches with similar scores, the
// resolver returns Kind::Ambiguous with the top candidates so the caller can
// ask the user which one they meant instead of guessing wrong.
namespace image_resolver {

struct Candidate {
    std::string path;
    std::string basename;
    std::string description;   // vision description (empty for filename hits)
    double      score = 0.0;   // 0.0..1.0 confidence; higher is better
    std::string why;           // e.g. "filename exact", "vision top-1"
};

struct Match {
    enum class Kind {
        NotFound,   // nothing matched; caller should surface reason + steps
        Found,      // single resolved image at `path`
        Ambiguous,  // multiple plausible matches; caller should ask user
    };

    Kind        kind = Kind::NotFound;
    std::string path;                   // populated when kind == Found
    std::string reason;                 // one-line human-readable summary
    std::vector<std::string> steps;     // breadcrumbs of every cascade step tried
    std::vector<Candidate>   candidates; // populated when kind == Ambiguous
};

// Resolve which image to edit. `cwd` is the project root (may be empty; then
// only session state and the resolver's HOME .ac9_images/ are searchable).
// `user_prompt` is the full raw edit request (typically ImageIntent::edit_op).
Match resolve(std::string_view cwd, std::string_view user_prompt);

// Cheap boolean: does the project have at least one image file the resolver
// could plausibly match against? Used to loosen the edit-intent gate in the
// chat pipeline — if there's no session image but the project has images, an
// edit-shaped prompt with a filename/descriptor hint should still route to
// the editor rather than fall through to the coder.
bool project_has_any_image(std::string_view cwd);

}  // namespace image_resolver
