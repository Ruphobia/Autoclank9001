#include "attachment_deduper.hpp"

#include "../044_dedup_ingest/dedup_ingest.hpp"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace attachment_deduper {
namespace {

namespace fs = std::filesystem;

std::string fnv64_hex_bytes(const std::string & bytes) {
    uint64_t h = 0xcbf29ce484222325ULL;
    for (unsigned char c : bytes) { h ^= c; h *= 0x100000001b3ULL; }
    char buf[24];
    std::snprintf(buf, sizeof(buf), "fnv64:%016llx", (unsigned long long)h);
    return buf;
}

}

void init()     {}
void shutdown() {}

FileFp fingerprint(std::string_view path, std::string_view text_excerpt) {
    FileFp f;
    f.path = std::string(path);
    fs::path p{f.path};
    if (!fs::exists(p)) return f;
    std::ifstream in(p, std::ios::binary);
    std::ostringstream b; b << in.rdbuf();
    std::string body = b.str();
    f.size_bytes   = body.size();
    f.content_hash = fnv64_hex_bytes(body);
    if (!text_excerpt.empty()) f.simhash = dedup_ingest::simhash64(text_excerpt);
    return f;
}

std::vector<Group> cluster(const std::vector<FileFp> & fps, int threshold) {
    std::vector<Group> out;
    if (fps.empty()) return out;

    // Byte-identical first.
    std::unordered_map<std::string, std::vector<size_t>> bybytes;
    for (size_t i = 0; i < fps.size(); ++i) {
        if (fps[i].content_hash.empty()) continue;
        bybytes[fps[i].content_hash].push_back(i);
    }
    std::vector<bool> taken(fps.size(), false);
    for (auto & [hash, ixs] : bybytes) {
        if (ixs.size() < 2) continue;
        Group g;
        g.byte_identical = true;
        for (auto i : ixs) { g.paths.push_back(fps[i].path); taken[i] = true; }
        out.push_back(std::move(g));
    }

    // Then SimHash near-dup clustering over the remainder (skip files
    // without a SimHash).
    auto popcount = [](uint64_t x){
        int n = 0; while (x) { n += (int)(x & 1); x >>= 1; } return n;
    };
    std::vector<size_t> remaining;
    for (size_t i = 0; i < fps.size(); ++i) {
        if (taken[i]) continue;
        if (fps[i].simhash == 0) continue;
        remaining.push_back(i);
    }
    // Union-find by pairwise threshold.
    std::vector<int> parent(remaining.size());
    for (size_t k = 0; k < parent.size(); ++k) parent[k] = (int)k;
    auto find = [&](auto && self, int x) -> int {
        return parent[x] == x ? x : parent[x] = self(self, parent[x]);
    };
    std::unordered_map<int, int> worst;
    for (size_t i = 0; i < remaining.size(); ++i) {
        for (size_t j = i + 1; j < remaining.size(); ++j) {
            int d = popcount(fps[remaining[i]].simhash ^ fps[remaining[j]].simhash);
            if (d > threshold) continue;
            int ri = find(find, (int)i), rj = find(find, (int)j);
            if (ri != rj) parent[ri] = rj;
            int r = find(find, (int)j);
            if (d > worst[r]) worst[r] = d;
        }
    }
    std::unordered_map<int, std::vector<size_t>> nearby;
    for (size_t i = 0; i < remaining.size(); ++i) {
        int r = find(find, (int)i);
        nearby[r].push_back(remaining[i]);
    }
    for (auto & [r, ids] : nearby) {
        if (ids.size() < 2) continue;
        Group g;
        g.byte_identical = false;
        g.hamming_within = worst[r];
        for (auto i : ids) g.paths.push_back(fps[i].path);
        out.push_back(std::move(g));
    }
    return out;
}

}
