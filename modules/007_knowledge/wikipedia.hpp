#pragma once

#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

namespace kb {

struct WikiHit {
    std::string title;
    std::string path;     // ZIM path (article URL)
    std::string snippet;  // short summary extracted from the article
};

// Thread-safe wrapper around a libzim archive. open()/close() can be
// called at any time; lookup() returns empty if not open.
class WikipediaArchive {
public:
    WikipediaArchive();
    ~WikipediaArchive();

    // Open (or replace) the mounted archive. Returns true on success.
    bool open(const std::filesystem::path & zim_path);
    void close();
    bool is_open() const;
    std::filesystem::path path() const;

    // Title-suggestion search (fast, fuzzy). Use for entity resolution.
    std::vector<WikiHit> suggest(std::string_view query, int max_results = 5) const;

    // Full-text search over article body. Slower; use sparingly.
    std::vector<WikiHit> search(std::string_view query, int max_results = 5) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    mutable std::mutex   mtx_;
};

}
