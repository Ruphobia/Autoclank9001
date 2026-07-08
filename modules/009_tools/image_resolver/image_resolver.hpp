// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

// Resolve "which image does the user want to edit" from a natural-language
// prompt. The image editor (advanced_raster_image_editor_photoshop_gimp_class)
// used to fail with "no input image to edit" whenever the session had no
// prior gen/edit turn. That put ac9 in the position of giving up on a request
// the user was perfectly capable of specifying - by filename ("edit foo.png")
// or by description ("edit the kitty picture"). This module never gives up
// while there is anything in the project tree to look at.
//
// Cascade, first hit wins:
//   1. Explicit filename token in the prompt (e.g. "foo.png", "black-kitty").
//      Search .ac9_images/ first, then the ENTIRE project tree, exhaustively.
//      No depth cap, no file cap, no skip-list of "vendored" dirs - the point
//      of ac9 is not to give up.
//   2. Session state - the existing behavior. Newest gen_path/edit_path record
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
// chat pipeline - if there's no session image but the project has images, an
// edit-shaped prompt with a filename/descriptor hint should still route to
// the editor rather than fall through to the coder.
bool project_has_any_image(std::string_view cwd);

// -------------------------------------------------------------------------
// Canonical character storage.
//
// Per-project layout under <cwd>/.ac9_images/canonical/<char>/:
//   <char>.png              - approved canonical sprite (img2img reference)
//   <char>.txt              - tag-line prompt suffix appended to every draw
//   <char>.seed             - decimal seed reused for every draw
//   <char>.lora.safetensors - Week 2 LoRA output; picked up when present
//   sheet/                  - front/side/back/pose grid if operator built one
//
// See scratchpad/subject_consistency_research.md §6 (Level 1 + Level 3
// recipes) for the design rationale. Every helper is best-effort: an
// empty return means "not present" and the caller should fall back to
// the legacy text-only path - none of them throw.
// -------------------------------------------------------------------------

// Absolute path to the canonical directory for `char_name` under `cwd`.
// Never creates the directory (callers that need it use ensure_dir()).
std::string canonical_dir(std::string_view cwd, std::string_view char_name);

// True when the canonical PNG exists AND is >= the minimum-image-size
// filter (100 KB - the same threshold that gates blank sd-cli failures
// out of the resolver cascade).
bool canonical_exists(std::string_view cwd, std::string_view char_name);

// Absolute path to <cwd>/.ac9_images/canonical/<char>/<char>.png when
// canonical_exists() is true; empty string otherwise.
std::string canonical_ref(std::string_view cwd, std::string_view char_name);

// Read the persisted seed from <char>.seed. Returns 0 when the file is
// missing, empty, or unparseable - the caller then treats "no seed" as
// "let sd-cli pick" and the Result's rolled seed will be captured on
// first promotion.
std::uint64_t canonical_seed(std::string_view cwd, std::string_view char_name);

// Read the tag-line prompt suffix from <char>.txt (a single line of
// comma-separated tags describing the character - the "poor man's
// Textual Inversion" from §6 L1.3). Empty when the file is missing.
// Whitespace-only files are treated as empty. Newlines are collapsed
// so the returned string is safe to append verbatim to a subject.
std::string canonical_prompt_suffix(std::string_view cwd,
                                    std::string_view char_name);

// Absolute path to <char>.lora.safetensors when it exists on disk;
// empty string otherwise. Populated by the lora_trainer module in
// Week 2; consumed by the image_gen dispatch as a `<lora:...>` token.
std::string canonical_lora(std::string_view cwd, std::string_view char_name);

// Write the seed file. Overwrites any previous seed. Returns true on
// success. Callers use this from the promote-to-canonical tool.
bool canonical_write_seed(std::string_view cwd,
                          std::string_view char_name,
                          std::uint64_t    seed);

// Write the tag-line prompt suffix. Overwrites any previous version.
// Callers use this from the promote-to-canonical tool, seeded by the
// coder LLM's tag-form description of the promoted image.
bool canonical_write_prompt_suffix(std::string_view cwd,
                                   std::string_view char_name,
                                   std::string_view suffix);

// Enumerate every approved PNG under <cwd>/.ac9_images/canonical/<char>/,
// excluding the canonical PNG itself. Used by the LoRA trainer to build
// its training set. Empty on missing directory.
std::vector<std::string> canonical_training_images(
    std::string_view cwd, std::string_view char_name);

// Copy `source_image_path` into <cwd>/.ac9_images/canonical/<char>/<char>.png,
// creating parent directories as needed. Returns the destination path on
// success, empty string on failure.
std::string canonical_promote(std::string_view cwd,
                              std::string_view char_name,
                              std::string_view source_image_path);

}  // namespace image_resolver
