// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// office_docs:: -- read / write / edit primitives for Office document
// types (ODF: .odt / .ods / .odp / .odg; OOXML: .docx / .xlsx / .pptx).
//
// The little LLMs cannot open a compressed archive of XML with a table
// of contents and image blobs and reason about it. This module shells
// out to a headless LibreOffice (Collabora's `soffice` binary is
// preferred; a system soffice works fine as a fallback) to convert to
// plain text / CSV / HTML, caches the result by (path, mtime), and
// hands the resulting text to callers.
//
// All the public functions in this header are RESILIENT: on any
// failure the read paths return an empty string, the write paths
// return false, and the reason is logged to stderr with a loud
// `!!!! OFFICE DOCS !!!!` prefix so a silent break shows up in the
// operator's terminal instead of vanishing.
//
// Locate the soffice binary via the AC9_SOFFICE env var; default is
// /home/jwoods/work/collabora-prefix/opt/collaboraoffice/program/soffice
// (see modules/1720_office_suite/ for the vendorized build).
//
// Cache lives under /tmp/ac9-office-cache/ by default; override with
// AC9_OFFICE_CACHE.
//
// Operator policy: no em dashes anywhere in the text these functions
// produce. LLM-facing prompt templates are in tool_router.cpp; this
// module only handles the file plumbing.

#include <nlohmann/json.hpp>

#include <string>
#include <string_view>
#include <vector>

namespace office_docs {

// One find/replace edit for edit(). `whole_word` matches [A-Za-z0-9_]
// boundaries either side of the match. Empty `find` is a no-op (a patch
// with empty find is skipped, not treated as "replace everything").
struct Patch {
    std::string find;
    std::string replace;
    bool        whole_word = false;
};

// Extract plain text from any supported Office document. Returns an
// empty string on failure (missing file, soffice not found, conversion
// crash). Caches the extracted text by (absolute path, mtime) so
// repeated reads within one process are O(1).
std::string read(const std::string & path);

// Create a new Office document from a plain-text body. `format` is
// inferred from the extension of `path` (odt / ods / odp / docx / xlsx
// / pptx / txt). Returns true on success, false on failure. Overwrites
// any existing file at `path`.
bool write(const std::string & path, const std::string & plain_text);

// Apply a list of textual patches to an existing Office document.
// Reads via read(), applies patches in memory, writes back via write().
// Fires a `!!!! OFFICE DOC EDIT !!!!` marker to stderr with the doc
// path + patch count so an audit trail exists.
bool edit(const std::string & path, const std::vector<Patch> & patches);

// Ask the coder model for a 3-sentence summary of the document's
// contents. Returns an empty string if read() fails or if the coder
// LLM raises. Never throws.
std::string summarize(const std::string & path);

// Return a JSON object describing the doc's structure:
//   * .ods / .xlsx : {"kind":"spreadsheet", "sheets":[{"name":"...", "rows":N, "cols":M}, ...]}
//   * .odp / .pptx : {"kind":"slides",       "slides":[{"index":N, "title":"..."}, ...]}
//   * .odt / .docx : {"kind":"document",     "headings":[{"level":N, "text":"..."}, ...]}
//   * .odg         : {"kind":"drawing",      "pages":N}
// Returns {"kind":"unknown", "error":"..."} on failure (never throws).
nlohmann::json structure(const std::string & path);

// Extract a specific sheet as CSV. Empty `sheet_name` means the first
// sheet. Returns empty string on failure.
std::string sheet_read(const std::string & path,
                       const std::string & sheet_name = "");

// Overwrite a sheet with the given CSV body. If `sheet_name` is empty
// or the sheet does not exist, creates a fresh single-sheet spreadsheet
// at `path` from the CSV.
bool sheet_write(const std::string & path,
                 const std::string & csv,
                 const std::string & sheet_name = "");

// Extract text from a single slide (1-indexed). slide_num = 0 returns
// the concatenated text of every slide.
std::string slide_read(const std::string & path, int slide_num = 0);

// Build a presentation from a list of {title, body} slides. Overwrites
// `path`. body is broken into bullet points on newline.
struct SlideDraft {
    std::string title;
    std::string body;
};
bool slide_write(const std::string & path,
                 const std::vector<SlideDraft> & slides);

// True when the given extension names an Office document that this
// module knows how to open. Handy for the tool_router dispatch cases
// and the file-tree preview endpoint.
bool is_office_ext(std::string_view ext);

// The soffice binary path, as resolved via AC9_SOFFICE / built-in
// default. Non-empty even when the binary is missing (callers check
// separately). Public so operator-facing status pages can display it.
std::string soffice_path();

// Cache directory for converted plain-text extracts. Public so an
// operator can clear it manually.
std::string cache_dir();

}  // namespace office_docs
