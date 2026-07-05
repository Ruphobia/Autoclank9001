#include "wikipedia.hpp"

#include <zim/archive.h>
#include <zim/item.h>
#include <zim/search.h>
#include <zim/suggestion.h>

#include <algorithm>
#include <cctype>
#include <regex>
#include <stdexcept>

namespace kb {
namespace fs = std::filesystem;

struct WikipediaArchive::Impl {
    std::unique_ptr<zim::Archive> archive;
    fs::path                      path;
};

WikipediaArchive::WikipediaArchive() : impl_(std::make_unique<Impl>()) {}
WikipediaArchive::~WikipediaArchive() = default;

bool WikipediaArchive::open(const fs::path & zim_path) {
    std::lock_guard<std::mutex> lk(mtx_);
    try {
        auto archive = std::make_unique<zim::Archive>(zim_path.string());
        impl_->archive = std::move(archive);
        impl_->path    = zim_path;
        return true;
    } catch (const std::exception &) {
        impl_->archive.reset();
        impl_->path.clear();
        return false;
    }
}

void WikipediaArchive::close() {
    std::lock_guard<std::mutex> lk(mtx_);
    impl_->archive.reset();
    impl_->path.clear();
}

bool WikipediaArchive::is_open() const {
    std::lock_guard<std::mutex> lk(mtx_);
    return impl_->archive != nullptr;
}

fs::path WikipediaArchive::path() const {
    std::lock_guard<std::mutex> lk(mtx_);
    return impl_->path;
}

namespace {

// Drop the CONTENT of non-text regions before tag stripping: <style>,
// <script>, <head> (which carries a duplicate <title>), and comments.
// A plain tag stripper keeps their text, so every snippet used to open
// with the article's inline CSS ("/* start https://en.wikipedia.org */
// .mw-parser-output{...}") instead of prose.
std::string strip_nontext_blocks(const std::string & html) {
    std::string out;
    out.reserve(html.size());
    auto ci_match = [&](std::size_t p, const char * s) {
        for (std::size_t k = 0; s[k]; ++k) {
            if (p + k >= html.size()) return false;
            if (std::tolower(static_cast<unsigned char>(html[p + k])) != s[k])
                return false;
        }
        return true;
    };
    // "<head" must not swallow "<header>": require a delimiter after the
    // tag name.
    auto tag_open = [&](std::size_t p, const char * name, std::size_t len) {
        if (!ci_match(p + 1, name)) return false;
        const std::size_t after = p + 1 + len;
        return after >= html.size() || html[after] == '>' ||
               html[after] == ' ' || html[after] == '\t' ||
               html[after] == '\n' || html[after] == '\r' ||
               html[after] == '/';
    };
    auto skip_past_close = [&](std::size_t from, const char * closer) {
        for (std::size_t p = from; p < html.size(); ++p) {
            if (html[p] == '<' && ci_match(p, closer)) {
                const std::size_t gt = html.find('>', p);
                return gt == std::string::npos ? html.size() : gt + 1;
            }
        }
        return html.size();
    };
    std::size_t i = 0;
    while (i < html.size()) {
        if (html[i] == '<') {
            if (tag_open(i, "style", 5)) {
                i = skip_past_close(i + 6, "</style");
                continue;
            }
            if (tag_open(i, "script", 6)) {
                i = skip_past_close(i + 7, "</script");
                continue;
            }
            if (tag_open(i, "head", 4)) {
                i = skip_past_close(i + 5, "</head");
                continue;
            }
            if (ci_match(i, "<!--")) {
                const std::size_t e = html.find("-->", i);
                i = e == std::string::npos ? html.size() : e + 3;
                continue;
            }
        }
        out.push_back(html[i]);
        ++i;
    }
    return out;
}

// Strip HTML tags and collapse whitespace; truncate to ~max_len chars
// at a word boundary. Cheap, not a real parser.
std::string extract_snippet(const std::string & raw_html, std::size_t max_len = 350) {
    const std::string html = strip_nontext_blocks(raw_html);
    std::string out;
    out.reserve(html.size());
    bool in_tag = false;
    bool last_ws = false;
    for (char c : html) {
        if (c == '<') { in_tag = true;  continue; }
        if (c == '>') { in_tag = false; continue; }
        if (in_tag)    continue;
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!last_ws && !out.empty()) { out.push_back(' '); last_ws = true; }
        } else {
            out.push_back(c);
            last_ws = false;
        }
    }
    // Trim trailing whitespace.
    while (!out.empty() && out.back() == ' ') out.pop_back();
    // Truncate at word boundary near max_len.
    if (out.size() > max_len) {
        std::size_t cut = max_len;
        while (cut > 0 && out[cut] != ' ') --cut;
        if (cut == 0) cut = max_len;
        out.resize(cut);
        out.append(" ...");
    }
    return out;
}

std::string read_item_snippet(const zim::Archive & arch, const std::string & path) {
    try {
        zim::Entry entry = arch.getEntryByPath(path);
        zim::Item  item  = entry.getItem(/*follow=*/true);
        std::string body = item.getData();
        return extract_snippet(body);
    } catch (const std::exception &) {
        return {};
    }
}

}

std::vector<WikiHit> WikipediaArchive::suggest(std::string_view query, int max_results) const {
    std::lock_guard<std::mutex> lk(mtx_);
    std::vector<WikiHit> out;
    if (!impl_->archive) return out;
    try {
        zim::SuggestionSearcher searcher(*impl_->archive);
        auto search = searcher.suggest(std::string(query));
        auto results = search.getResults(0, static_cast<int>(max_results));
        for (const auto & r : results) {
            WikiHit h;
            h.title   = r.getTitle();
            h.path    = r.getPath();
            h.snippet = read_item_snippet(*impl_->archive, h.path);
            out.push_back(std::move(h));
            if (static_cast<int>(out.size()) >= max_results) break;
        }
    } catch (const std::exception &) {
        // Library may throw on weird input; return what we have.
    }
    return out;
}

std::vector<WikiHit> WikipediaArchive::search(std::string_view query, int max_results) const {
    std::lock_guard<std::mutex> lk(mtx_);
    std::vector<WikiHit> out;
    if (!impl_->archive) return out;
    try {
        zim::Searcher searcher(*impl_->archive);
        zim::Query    q;
        q.setQuery(std::string(query));
        auto search  = searcher.search(q);
        auto results = search.getResults(0, static_cast<int>(max_results));
        for (const auto & r : results) {
            WikiHit h;
            h.title   = r.getTitle();
            h.path    = r.getPath();
            h.snippet = read_item_snippet(*impl_->archive, h.path);
            out.push_back(std::move(h));
            if (static_cast<int>(out.size()) >= max_results) break;
        }
    } catch (const std::exception &) {
    }
    return out;
}

}
