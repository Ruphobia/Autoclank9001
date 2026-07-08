// SPDX-License-Identifier: GPL-3.0-or-later
#include "image_resolver.hpp"

#include "../../005_context/context.hpp"
#include "../shell/coder.hpp"
#include "../vision/vision.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <chrono>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <system_error>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace image_resolver {

namespace {

namespace fs = std::filesystem;
using json   = nlohmann::json;

// --- Constants ---------------------------------------------------------------

// The blank-white failure PNG that sd-cli emits on failure is under 100 KB.
// Filter it out the same way the previous inline picker did.
constexpr std::uintmax_t kMinImageBytes = 100 * 1024;

// Extension whitelist. jxl / avif deliberately omitted: the Chroma pipeline
// only knows PNG/JPEG.
const std::set<std::string> & image_exts() {
    static const std::set<std::string> s = {
        ".png", ".jpg", ".jpeg", ".webp", ".gif", ".bmp",
    };
    return s;
}

// Ambiguity band. When the top vision-match score is below this, or when
// the gap to the runner-up is too small, we hand candidates back to the
// caller instead of guessing.
constexpr double kMinTopScore   = 0.60;
constexpr double kMinScoreDelta = 0.15;

// --- String helpers ----------------------------------------------------------

std::string to_lower(std::string_view s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) out.push_back(static_cast<char>(
        std::tolower(static_cast<unsigned char>(c))));
    return out;
}

// Normalize a basename for fuzzy substring matching: lowercase, collapse
// separators (`-_ .`) into a single space so "black-kitty" matches "black kitty".
std::string normalize_for_match(std::string_view s) {
    std::string out;
    out.reserve(s.size());
    bool prev_sep = false;
    for (char c : s) {
        const unsigned char uc = static_cast<unsigned char>(c);
        if (uc == '-' || uc == '_' || uc == ' ' || uc == '.' || uc == '/') {
            if (!prev_sep && !out.empty()) out.push_back(' ');
            prev_sep = true;
        } else {
            out.push_back(static_cast<char>(std::tolower(uc)));
            prev_sep = false;
        }
    }
    while (!out.empty() && out.back() == ' ') out.pop_back();
    return out;
}

// Mirror the tilde expansion the rest of ac9 uses (server.cpp:82).
std::string expand_home(std::string_view p) {
    if (p.size() >= 2 && p[0] == '~' && p[1] == '/') {
        if (const char * h = std::getenv("HOME")) {
            return std::string(h) + std::string(p.substr(1));
        }
    }
    return std::string(p);
}

// --- Filesystem: enumerate every image, exhaustively -------------------------

// Return true if `p` is a regular file with an image extension and
// size >= kMinImageBytes. Failure to stat -> false (not an error).
bool is_valid_image_file(const fs::path & p) {
    std::error_code ec;
    if (!fs::is_regular_file(p, ec) || ec) return false;
    const std::string ext = to_lower(p.extension().string());
    if (!image_exts().count(ext)) return false;
    const auto sz = fs::file_size(p, ec);
    if (ec) return false;
    return sz >= kMinImageBytes;
}

// Walk a directory tree recursively and append every valid image path.
// Deliberately unbounded: no depth cap, no file cap, no vendored-dir
// skip list. The user's directive is that ac9 must not give up while
// there is anything in the project tree it hasn't looked at yet.
void collect_images_under(const fs::path & root,
                          std::unordered_set<std::string> & seen,
                          std::vector<std::string> & out) {
    std::error_code ec;
    if (!fs::is_directory(root, ec) || ec) return;
    fs::recursive_directory_iterator it(
        root,
        fs::directory_options::skip_permission_denied | fs::directory_options::follow_directory_symlink,
        ec);
    fs::recursive_directory_iterator end;
    if (ec) return;
    while (it != end) {
        std::error_code iec;
        const fs::path & p = it->path();
        if (is_valid_image_file(p)) {
            const std::string abs = fs::weakly_canonical(p, iec).string();
            const std::string key = abs.empty() ? p.string() : abs;
            if (seen.insert(key).second) out.push_back(key);
        }
        it.increment(iec);
        if (iec) {
            // Directory unreadable or vanished mid-walk; keep going.
            iec.clear();
        }
    }
}

struct ImageEntry {
    std::string          path;
    std::uintmax_t       size  = 0;
    std::int64_t         mtime = 0;   // seconds since epoch
};

// Fill in stat details for a path; returns false if the file went away.
bool stat_image(const std::string & path, ImageEntry & out) {
    std::error_code ec;
    const auto sz = fs::file_size(path, ec);
    if (ec) return false;
    const auto ft = fs::last_write_time(path, ec);
    if (ec) return false;
    out.path = path;
    out.size = sz;
    // Raw file_clock ticks in seconds. We only need cache-invalidation
    // semantics (same file -> same key), not a real POSIX timestamp, so no
    // clock_cast is required — that would need C++20.
    out.mtime = std::chrono::duration_cast<std::chrono::seconds>(
        ft.time_since_epoch()).count();
    return true;
}

// Enumerate every image in the search scope. Priority order is
// preserved so callers can favor .ac9_images/ hits over deep-tree hits
// when scores tie: .ac9_images/<cwd>, then <cwd> recursively, then
// $HOME/.ac9_images (used when no project is open, matches the editor's
// out-dir fallback in server.cpp).
//
// No bounds. Every file gets scanned every time. Duplicates are dropped
// by canonical path.
std::vector<ImageEntry> enumerate_all_images(std::string_view cwd) {
    std::vector<std::string> ordered;
    std::unordered_set<std::string> seen;

    const std::string proj = expand_home(cwd);
    if (!proj.empty()) {
        collect_images_under(fs::path(proj) / ".ac9_images", seen, ordered);
        collect_images_under(fs::path(proj), seen, ordered);
    }
    if (const char * h = std::getenv("HOME")) {
        collect_images_under(fs::path(h) / ".ac9_images", seen, ordered);
    }

    std::vector<ImageEntry> out;
    out.reserve(ordered.size());
    for (const auto & p : ordered) {
        ImageEntry e;
        if (stat_image(p, e)) out.push_back(std::move(e));
    }
    return out;
}

// --- Filename / descriptor token extraction ----------------------------------

struct Hint {
    std::string token;
    bool        has_extension = false;  // strong signal: token ends with an image ext
};

// Common English "stop words" we don't want to treat as image descriptors.
// Intentionally short; overshooting causes real object nouns ("kitten",
// "sunset") to slip through, which is what we want.
const std::set<std::string> & stop_words() {
    static const std::set<std::string> s = {
        "a", "an", "the", "of", "on", "in", "at", "to", "for", "with",
        "and", "or", "but", "is", "are", "was", "were", "be", "been",
        "it", "them", "this", "that", "these", "those", "there", "here",
        "my", "your", "our", "their", "his", "her", "its",
        "image", "picture", "photo", "photograph", "png", "jpg", "jpeg",
        "webp", "gif", "bmp", "file", "one", "please",
        "edit", "modify", "change", "make", "recolor", "repaint", "turn",
        "update", "redo", "swap", "replace", "add", "remove", "erase",
        "color", "colour", "colored", "coloured",
    };
    return s;
}

std::vector<Hint> extract_hints(std::string_view prompt) {
    std::vector<Hint> hints;
    std::unordered_set<std::string> seen_tok;

    auto push = [&](std::string tok, bool has_ext) {
        if (tok.empty()) return;
        std::string key = to_lower(tok);
        if (!seen_tok.insert(key).second) return;
        hints.push_back({std::move(tok), has_ext});
    };

    // 1) Filename with recognized image extension: `foo.png`, `my-image-2.jpeg`,
    //    also full paths like `assets/foo.png`.
    static const std::regex file_re(
        R"(([A-Za-z0-9_.\-/]*[A-Za-z0-9_\-]\.(?:png|jpg|jpeg|webp|gif|bmp)))",
        std::regex::icase);
    {
        auto it  = std::cregex_iterator(prompt.data(),
                                        prompt.data() + prompt.size(), file_re);
        auto end = std::cregex_iterator();
        for (; it != end; ++it) push(it->str(1), true);
    }

    // 2) Quoted strings — user may quote a partial basename ("black-kitty").
    static const std::regex quote_re(R"(["'`]([^"'`]{2,})["'`])");
    {
        auto it  = std::cregex_iterator(prompt.data(),
                                        prompt.data() + prompt.size(), quote_re);
        auto end = std::cregex_iterator();
        for (; it != end; ++it) {
            std::string tok = it->str(1);
            const std::string lc = to_lower(tok);
            bool has_ext = false;
            for (const auto & ext : image_exts()) {
                if (lc.size() >= ext.size() &&
                    lc.compare(lc.size() - ext.size(), ext.size(), ext) == 0) {
                    has_ext = true;
                    break;
                }
            }
            push(std::move(tok), has_ext);
        }
    }

    // 3) Individual descriptor words. Grab hyphenated and single tokens of
    //    length >= 3 that aren't stop words. Order preserved.
    std::string cur;
    auto flush_word = [&]() {
        if (cur.size() >= 3) {
            const std::string lc = to_lower(cur);
            if (!stop_words().count(lc)) push(cur, false);
        }
        cur.clear();
    };
    for (char c : prompt) {
        const unsigned char uc = static_cast<unsigned char>(c);
        if (std::isalnum(uc) || uc == '-' || uc == '_') cur.push_back(c);
        else flush_word();
    }
    flush_word();

    return hints;
}

// --- Filename matching -------------------------------------------------------

// Score a single (image, hint) pair. Higher is better. 0 means no match.
double filename_score(const std::string & basename_norm,
                      const std::string & stem_norm,
                      const Hint & hint) {
    if (hint.token.empty()) return 0.0;
    const std::string ht  = to_lower(hint.token);
    const std::string htn = normalize_for_match(hint.token);
    if (htn.empty()) return 0.0;

    // Exact basename (with or without extension) — strongest.
    if (to_lower(basename_norm) == ht || stem_norm == htn) return 1.00;

    // Suffix match: image path ends with hint (e.g. hint "assets/foo.png",
    // image "/proj/x/assets/foo.png").
    if (hint.has_extension && basename_norm.size() >= ht.size() &&
        to_lower(basename_norm).compare(
            basename_norm.size() - ht.size(), ht.size(), ht) == 0) {
        return 0.95;
    }

    // Basename starts with hint (e.g. hint "black-kitty" matches
    // "black-kitty-20260707T034812Z.png").
    if (stem_norm.rfind(htn, 0) == 0) return 0.90;

    // Basename contains hint as a whole token (bounded by separators).
    const std::string padded = " " + stem_norm + " ";
    if (padded.find(" " + htn + " ") != std::string::npos) return 0.80;

    // Basename contains hint as a substring.
    if (stem_norm.find(htn) != std::string::npos) return 0.65;

    return 0.0;
}

// Rank every image by the best hint score it earns. Returns pairs of
// (index into images, score, why).
struct FilenameHit {
    std::size_t index;
    double      score;
    std::string why;
};

std::vector<FilenameHit> rank_by_filename(
    const std::vector<ImageEntry> & images,
    const std::vector<Hint> & hints)
{
    std::vector<FilenameHit> hits;
    for (std::size_t i = 0; i < images.size(); ++i) {
        const fs::path p(images[i].path);
        const std::string base = p.filename().string();
        const std::string stem = normalize_for_match(p.stem().string());
        double best = 0.0;
        std::string best_hint;
        for (const auto & h : hints) {
            const double s = filename_score(base, stem, h);
            if (s > best) {
                best = s;
                best_hint = h.token;
            }
        }
        if (best > 0.0) {
            std::string why = "matched \"" + best_hint + "\" against " + base;
            hits.push_back({i, best, std::move(why)});
        }
    }
    std::sort(hits.begin(), hits.end(),
              [](const FilenameHit & a, const FilenameHit & b) {
                  return a.score > b.score;
              });
    return hits;
}

// --- Session-state pick (existing behavior) ----------------------------------

std::string pick_session_last_image() {
    try {
        for (const auto & r : context::by_layer("image", 12)) {
            if (r.kind != "gen_path" && r.kind != "edit_path") continue;
            if (r.content.empty()) continue;
            std::error_code ec;
            if (!fs::exists(r.content, ec)) continue;
            const auto sz = fs::file_size(r.content, ec);
            if (ec) continue;
            if (sz < kMinImageBytes) continue;
            return r.content;
        }
    } catch (...) {}
    return {};
}

// --- Description cache (persistent JSON) -------------------------------------

// Cache location: <cwd>/.ac9_images/.descriptions.json. Living next to the
// generated images means a fresh clone with existing .ac9_images/ still
// has usable descriptions; a fresh session doesn't cost a describe pass.
// When there's no cwd we fall back to $HOME/.ac9_images/.
std::string cache_path_for(std::string_view cwd) {
    if (!cwd.empty()) {
        return (fs::path(expand_home(cwd)) / ".ac9_images" /
                ".descriptions.json").string();
    }
    if (const char * h = std::getenv("HOME")) {
        return (fs::path(h) / ".ac9_images" / ".descriptions.json").string();
    }
    return "/tmp/.ac9_images/.descriptions.json";
}

std::mutex g_cache_mtx;

json load_cache_json(const std::string & path) {
    std::ifstream f(path);
    if (!f.is_open()) return json{{"version", 1}, {"entries", json::object()}};
    try {
        json j;
        f >> j;
        if (!j.is_object() || !j.contains("entries") ||
            !j["entries"].is_object()) {
            return json{{"version", 1}, {"entries", json::object()}};
        }
        return j;
    } catch (...) {
        return json{{"version", 1}, {"entries", json::object()}};
    }
}

// Atomic write: temp file then rename. Prevents a crash from leaving a
// half-written JSON that later loads throw on.
void save_cache_json(const std::string & path, const json & j) {
    std::error_code ec;
    fs::create_directories(fs::path(path).parent_path(), ec);
    const std::string tmp = path + ".tmp";
    {
        std::ofstream f(tmp, std::ios::binary | std::ios::trunc);
        if (!f.is_open()) return;
        f << j.dump(2);
    }
    fs::rename(tmp, path, ec);
    if (ec) {
        // Rename failed (cross-device?); overwrite in place as a fallback.
        std::ofstream f(path, std::ios::binary | std::ios::trunc);
        if (f.is_open()) f << j.dump(2);
        fs::remove(tmp, ec);
    }
}

// Look up a cached description iff mtime+size still match. Returns empty
// string on miss (or invalid entry). Callers hold g_cache_mtx.
std::string cache_lookup(const json & j,
                         const std::string & abspath,
                         std::int64_t mtime,
                         std::uintmax_t size)
{
    const auto & entries = j.at("entries");
    auto it = entries.find(abspath);
    if (it == entries.end()) return {};
    if (!it->is_object()) return {};
    try {
        if (it->value("mtime", std::int64_t{0})     != mtime) return {};
        if (it->value("size",  std::uintmax_t{0})   != size)  return {};
        return it->value("description", std::string{});
    } catch (...) {
        return {};
    }
}

void cache_store(json & j,
                 const std::string & abspath,
                 std::int64_t mtime,
                 std::uintmax_t size,
                 const std::string & description)
{
    j["entries"][abspath] = {
        {"mtime",       mtime},
        {"size",        size},
        {"description", description},
    };
}

// Describe an image, using the cache when the file hasn't changed.
// Blocks; vision::describe is 5-15s CPU per image on a cold call, but
// caching makes every subsequent call for the same file free.
std::string describe_with_cache(std::string_view cwd, const ImageEntry & e) {
    const std::string cpath = cache_path_for(cwd);
    {
        std::lock_guard<std::mutex> lk(g_cache_mtx);
        const json j = load_cache_json(cpath);
        std::string hit = cache_lookup(j, e.path, e.mtime, e.size);
        if (!hit.empty()) return hit;
    }

    std::string desc;
    try {
        desc = vision::describe(
            e.path,
            "Describe the contents of this image concretely and specifically. "
            "Name the primary subject, its colors, notable objects, style, "
            "and setting. Keep the description under 60 words. "
            "Do NOT preface with 'The image shows'; go straight to the content.");
    } catch (const std::exception & ex) {
        return std::string("<vision failed: ") + ex.what() + ">";
    } catch (...) {
        return "<vision failed>";
    }

    if (desc.empty()) desc = "<empty description>";

    {
        std::lock_guard<std::mutex> lk(g_cache_mtx);
        json j = load_cache_json(cpath);
        cache_store(j, e.path, e.mtime, e.size, desc);
        save_cache_json(cpath, j);
    }
    return desc;
}

// --- Description-based ranking via the coder LLM -----------------------------

struct RankedDescription {
    std::size_t index;
    double      score;   // 0.0..1.0
    std::string reason;
};

// Extract the first `{...}` JSON object substring. The coder likes to
// wrap replies in prose or fenced blocks; find the outermost braced
// span and parse just that.
std::string extract_json_object(const std::string & s) {
    int depth = 0;
    std::size_t start = std::string::npos;
    for (std::size_t i = 0; i < s.size(); ++i) {
        const char c = s[i];
        if (c == '{') {
            if (depth == 0) start = i;
            ++depth;
        } else if (c == '}') {
            --depth;
            if (depth == 0 && start != std::string::npos) {
                return s.substr(start, i - start + 1);
            }
        }
    }
    return {};
}

std::vector<RankedDescription> rank_by_description(
    std::string_view user_prompt,
    const std::vector<ImageEntry> & images,
    const std::vector<std::string> & descriptions)
{
    std::vector<RankedDescription> ranked;
    if (images.empty()) return ranked;

    // Build the candidate list for the coder. Include basename so the
    // LLM can cross-reference against terms the user might have typed
    // even without an explicit filename hint.
    std::ostringstream body;
    body << "Candidates:\n";
    for (std::size_t i = 0; i < images.size(); ++i) {
        body << i << ". file=" << fs::path(images[i].path).filename().string()
             << "\n   desc=" << descriptions[i] << "\n";
    }
    body << "\nUser request: " << user_prompt << "\n";

    const std::string system_prompt =
        "You are a strict image-picker. Given a list of candidate images "
        "(each with a filename and a vision description) plus the user's edit "
        "request, pick the ONE candidate whose depicted content best matches "
        "what the user is asking to edit. Reply with a single JSON object and "
        "nothing else:\n"
        "{\n"
        "  \"index\": <integer index of the best match>,\n"
        "  \"confidence\": <number 0.0-1.0>,\n"
        "  \"runner_up\": <integer index or -1>,\n"
        "  \"runner_up_confidence\": <number 0.0-1.0>,\n"
        "  \"reason\": <short string explaining why>\n"
        "}\n"
        "confidence should reflect how well the match maps to the request; "
        "use LOW confidence (<0.6) when several candidates could plausibly "
        "match or when nothing matches well.";

    std::string raw;
    try {
        raw = coder::generate(system_prompt, body.str(),
                              /*max_new_tokens=*/512, /*truncated=*/nullptr);
    } catch (...) {
        return ranked;
    }

    const std::string j_txt = extract_json_object(raw);
    if (j_txt.empty()) return ranked;
    json j;
    try { j = json::parse(j_txt); } catch (...) { return ranked; }

    auto push_ranked = [&](const char * idx_key, const char * conf_key,
                           const std::string & why) {
        if (!j.contains(idx_key)) return;
        int idx = -1;
        try { idx = j.at(idx_key).get<int>(); } catch (...) { return; }
        if (idx < 0 || static_cast<std::size_t>(idx) >= images.size()) return;
        double conf = 0.0;
        try { conf = j.value(conf_key, 0.0); } catch (...) {}
        if (conf < 0.0) conf = 0.0;
        if (conf > 1.0) conf = 1.0;
        std::string reason = why;
        try {
            const std::string llm_reason = j.value("reason", std::string{});
            if (!llm_reason.empty()) reason += " — " + llm_reason;
        } catch (...) {}
        ranked.push_back({static_cast<std::size_t>(idx), conf, std::move(reason)});
    };
    push_ranked("index", "confidence", "coder pick: top-1");
    push_ranked("runner_up", "runner_up_confidence", "coder pick: runner-up");
    return ranked;
}

// --- Coordination helper: cascade wrapper -----------------------------------

Match make_not_found(std::vector<std::string> steps, std::string reason) {
    Match m;
    m.kind   = Match::Kind::NotFound;
    m.reason = std::move(reason);
    m.steps  = std::move(steps);
    return m;
}

Match make_found(std::string path,
                 std::vector<std::string> steps,
                 std::string reason) {
    Match m;
    m.kind   = Match::Kind::Found;
    m.path   = std::move(path);
    m.reason = std::move(reason);
    m.steps  = std::move(steps);
    return m;
}

}  // anonymous namespace

// --- Public API --------------------------------------------------------------

bool project_has_any_image(std::string_view cwd) {
    // Fast path — no describe, no cache work; just check the tree for one
    // valid image file. Used to relax the edit-intent gate.
    const std::string proj = expand_home(cwd);
    if (!proj.empty()) {
        std::unordered_set<std::string> seen;
        std::vector<std::string> hits;
        collect_images_under(fs::path(proj) / ".ac9_images", seen, hits);
        if (!hits.empty()) return true;
        collect_images_under(fs::path(proj), seen, hits);
        if (!hits.empty()) return true;
    }
    if (const char * h = std::getenv("HOME")) {
        std::unordered_set<std::string> seen;
        std::vector<std::string> hits;
        collect_images_under(fs::path(h) / ".ac9_images", seen, hits);
        if (!hits.empty()) return true;
    }
    return false;
}

Match resolve(std::string_view cwd, std::string_view user_prompt) {
    std::vector<std::string> steps;

    // ---- Step 1: filename tokens in the prompt ----
    const std::vector<Hint> hints = extract_hints(user_prompt);
    std::size_t explicit_hints = 0;
    for (const auto & h : hints) if (h.has_extension) ++explicit_hints;

    const std::vector<ImageEntry> all_images = enumerate_all_images(cwd);
    steps.push_back("scan: found " + std::to_string(all_images.size()) +
                    " image file(s) in project tree");

    if (!hints.empty() && !all_images.empty()) {
        const auto fhits = rank_by_filename(all_images, hints);
        if (!fhits.empty() && fhits.front().score >= 0.65) {
            const auto & top = fhits.front();
            // Ambiguity check on filenames too: two files with identical
            // top score means the user needs to disambiguate.
            std::size_t tied = 0;
            for (const auto & h : fhits) {
                if (h.score >= top.score - 1e-9) ++tied; else break;
            }
            if (tied == 1) {
                steps.push_back("filename: " + fhits.front().why +
                                " (score " + std::to_string(top.score) + ")");
                return make_found(all_images[top.index].path, std::move(steps),
                                  "resolved by filename hint");
            }
            // Tied — build ambiguous set and return.
            steps.push_back("filename: multiple files scored " +
                            std::to_string(top.score) +
                            " (" + std::to_string(tied) + " candidates)");
            Match m;
            m.kind   = Match::Kind::Ambiguous;
            m.reason = "multiple filename matches; please disambiguate";
            m.steps  = std::move(steps);
            for (std::size_t i = 0; i < tied && i < fhits.size(); ++i) {
                const auto & h  = fhits[i];
                const auto & im = all_images[h.index];
                Candidate c;
                c.path        = im.path;
                c.basename    = fs::path(im.path).filename().string();
                c.description = "";
                c.score       = h.score;
                c.why         = h.why;
                m.candidates.push_back(std::move(c));
            }
            return m;
        }
        if (!fhits.empty()) {
            steps.push_back("filename: best score " +
                            std::to_string(fhits.front().score) +
                            " below threshold; falling through");
        } else if (explicit_hints > 0) {
            // The prompt explicitly named a *.png but nothing in the tree
            // matched. Surface this before trying vision — the user gave us
            // a clear name.
            steps.push_back("filename: extension token(s) found but no image "
                            "matches; falling through to session/vision");
        }
    }

    // ---- Step 2: session state (existing behavior) ----
    const std::string sess = pick_session_last_image();
    if (!sess.empty()) {
        steps.push_back("session: newest prior gen/edit_path >=100 KB");
        return make_found(sess, std::move(steps), "resolved from session state");
    }
    steps.push_back("session: no prior gen/edit image in this session");

    // ---- Step 3: describe every image and rank by content ----
    if (all_images.empty()) {
        return make_not_found(
            std::move(steps),
            "no images in project (.ac9_images/, project tree, ~/.ac9_images/) "
            "and no prior session image");
    }

    steps.push_back("vision: describing " +
                    std::to_string(all_images.size()) +
                    " image(s) (cached where possible)");
    std::vector<std::string> descs;
    descs.reserve(all_images.size());
    for (const auto & e : all_images) {
        descs.push_back(describe_with_cache(cwd, e));
    }

    const std::vector<RankedDescription> ranked =
        rank_by_description(user_prompt, all_images, descs);
    if (ranked.empty()) {
        return make_not_found(std::move(steps),
            "vision described " + std::to_string(all_images.size()) +
            " image(s) but the coder could not pick a match");
    }

    const auto & top = ranked.front();
    const double runner = ranked.size() >= 2 ? ranked[1].score : 0.0;
    const bool confident = top.score >= kMinTopScore &&
                          (ranked.size() < 2 || (top.score - runner) >= kMinScoreDelta);

    if (confident) {
        steps.push_back("vision: confident match idx=" +
                        std::to_string(top.index) +
                        " score=" + std::to_string(top.score));
        return make_found(all_images[top.index].path, std::move(steps),
                          "resolved by vision description");
    }

    // Ambiguous — bundle up top candidates for the caller to hand back.
    steps.push_back("vision: ambiguous; top=" + std::to_string(top.score) +
                    " runner-up=" + std::to_string(runner));
    Match m;
    m.kind   = Match::Kind::Ambiguous;
    m.reason = "vision match is not confident; please pick one";
    m.steps  = std::move(steps);
    for (const auto & r : ranked) {
        const auto & im = all_images[r.index];
        Candidate c;
        c.path        = im.path;
        c.basename    = fs::path(im.path).filename().string();
        c.description = descs[r.index];
        c.score       = r.score;
        c.why         = r.reason;
        m.candidates.push_back(std::move(c));
    }
    return m;
}

// --- Canonical character storage ---------------------------------------------
//
// Level 1 of the subject-consistency ship plan. Every helper is
// best-effort: an empty return means "not present" and the caller falls
// back to legacy behavior — none of them throw. See the header for
// storage layout.

namespace {

// Filesystem-safe slug for a character name. Restrictive on purpose:
// a character token also feeds into sd-cli argv and prompt text, so
// only allow lowercase alphanumerics plus underscore. Leading/trailing
// underscores are trimmed. An empty result is a hard error at the
// caller — never pass an unsanitized name through.
std::string canonical_slug(std::string_view raw) {
    std::string out;
    out.reserve(raw.size());
    bool last_us = false;
    for (char c : raw) {
        const unsigned char uc = static_cast<unsigned char>(c);
        if (std::isalnum(uc)) {
            out.push_back(static_cast<char>(std::tolower(uc)));
            last_us = false;
        } else if (!last_us && !out.empty()) {
            out.push_back('_');
            last_us = true;
        }
    }
    while (!out.empty() && out.back() == '_') out.pop_back();
    return out;
}

fs::path canonical_root(std::string_view cwd, const std::string & slug) {
    const std::string proj = expand_home(cwd);
    if (proj.empty()) {
        if (const char * h = std::getenv("HOME")) {
            return fs::path(h) / ".ac9_images" / "canonical" / slug;
        }
        return fs::path("/tmp") / ".ac9_images" / "canonical" / slug;
    }
    return fs::path(proj) / ".ac9_images" / "canonical" / slug;
}

}  // anonymous namespace

std::string canonical_dir(std::string_view cwd, std::string_view char_name) {
    const std::string slug = canonical_slug(char_name);
    if (slug.empty()) return {};
    return canonical_root(cwd, slug).string();
}

bool canonical_exists(std::string_view cwd, std::string_view char_name) {
    const std::string slug = canonical_slug(char_name);
    if (slug.empty()) return false;
    const fs::path png = canonical_root(cwd, slug) / (slug + ".png");
    return is_valid_image_file(png);
}

std::string canonical_ref(std::string_view cwd, std::string_view char_name) {
    const std::string slug = canonical_slug(char_name);
    if (slug.empty()) return {};
    const fs::path png = canonical_root(cwd, slug) / (slug + ".png");
    if (!is_valid_image_file(png)) return {};
    return png.string();
}

std::uint64_t canonical_seed(std::string_view cwd, std::string_view char_name) {
    const std::string slug = canonical_slug(char_name);
    if (slug.empty()) return 0;
    const fs::path seed_file = canonical_root(cwd, slug) / (slug + ".seed");
    std::ifstream in(seed_file);
    if (!in) return 0;
    std::string line;
    std::getline(in, line);
    // Trim whitespace.
    while (!line.empty() && std::isspace(static_cast<unsigned char>(line.back())))
        line.pop_back();
    std::size_t start = 0;
    while (start < line.size() &&
           std::isspace(static_cast<unsigned char>(line[start]))) ++start;
    line = line.substr(start);
    if (line.empty()) return 0;
    try {
        return std::stoull(line);
    } catch (...) {
        return 0;
    }
}

std::string canonical_prompt_suffix(std::string_view cwd,
                                    std::string_view char_name) {
    const std::string slug = canonical_slug(char_name);
    if (slug.empty()) return {};
    const fs::path tag_file = canonical_root(cwd, slug) / (slug + ".txt");
    std::ifstream in(tag_file);
    if (!in) return {};
    std::stringstream ss;
    ss << in.rdbuf();
    std::string out = ss.str();
    // Collapse whitespace so the suffix is safe to append verbatim to a
    // subject: newlines / tabs -> single space, then trim.
    for (char & c : out) {
        if (c == '\n' || c == '\r' || c == '\t') c = ' ';
    }
    // Collapse runs of spaces.
    std::string compact;
    compact.reserve(out.size());
    bool last_space = false;
    for (char c : out) {
        if (c == ' ') {
            if (!last_space && !compact.empty()) compact.push_back(' ');
            last_space = true;
        } else {
            compact.push_back(c);
            last_space = false;
        }
    }
    while (!compact.empty() && compact.back() == ' ') compact.pop_back();
    return compact;
}

std::string canonical_lora(std::string_view cwd, std::string_view char_name) {
    const std::string slug = canonical_slug(char_name);
    if (slug.empty()) return {};
    const fs::path lora = canonical_root(cwd, slug) / (slug + ".lora.safetensors");
    std::error_code ec;
    if (!fs::is_regular_file(lora, ec) || ec) return {};
    return lora.string();
}

bool canonical_write_seed(std::string_view cwd,
                          std::string_view char_name,
                          std::uint64_t    seed) {
    const std::string slug = canonical_slug(char_name);
    if (slug.empty()) return false;
    const fs::path dir = canonical_root(cwd, slug);
    std::error_code ec;
    fs::create_directories(dir, ec);
    if (ec) return false;
    std::ofstream out(dir / (slug + ".seed"), std::ios::trunc);
    if (!out) return false;
    out << seed << "\n";
    return out.good();
}

bool canonical_write_prompt_suffix(std::string_view cwd,
                                   std::string_view char_name,
                                   std::string_view suffix) {
    const std::string slug = canonical_slug(char_name);
    if (slug.empty()) return false;
    const fs::path dir = canonical_root(cwd, slug);
    std::error_code ec;
    fs::create_directories(dir, ec);
    if (ec) return false;
    std::ofstream out(dir / (slug + ".txt"), std::ios::trunc);
    if (!out) return false;
    out << suffix;
    if (!suffix.empty() && suffix.back() != '\n') out << "\n";
    return out.good();
}

std::vector<std::string> canonical_training_images(
    std::string_view cwd, std::string_view char_name) {
    std::vector<std::string> results;
    const std::string slug = canonical_slug(char_name);
    if (slug.empty()) return results;
    const fs::path dir = canonical_root(cwd, slug);
    std::error_code ec;
    if (!fs::is_directory(dir, ec) || ec) return results;
    // The canonical PNG itself is a valid training image — LoRA training
    // benefits from the promoted "master" being in the set.
    for (const auto & entry : fs::directory_iterator(dir, ec)) {
        if (ec) break;
        if (!entry.is_regular_file()) continue;
        const fs::path p = entry.path();
        if (!is_valid_image_file(p)) continue;
        results.push_back(p.string());
    }
    std::sort(results.begin(), results.end());
    return results;
}

std::string canonical_promote(std::string_view cwd,
                              std::string_view char_name,
                              std::string_view source_image_path) {
    const std::string slug = canonical_slug(char_name);
    if (slug.empty()) return {};
    const std::string src = expand_home(source_image_path);
    std::error_code ec;
    if (!fs::is_regular_file(src, ec) || ec) return {};
    const fs::path dir = canonical_root(cwd, slug);
    fs::create_directories(dir, ec);
    if (ec) return {};
    const fs::path dst = dir / (slug + ".png");
    fs::copy_file(src, dst, fs::copy_options::overwrite_existing, ec);
    if (ec) return {};
    return dst.string();
}

}  // namespace image_resolver
