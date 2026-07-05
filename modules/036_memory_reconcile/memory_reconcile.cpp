#include "memory_reconcile.hpp"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace memory_reconcile {
namespace {

std::string fnv64_hex(std::string_view bytes) {
    uint64_t h = 0xcbf29ce484222325ULL;
    for (unsigned char c : bytes) { h ^= c; h *= 0x100000001b3ULL; }
    char buf[24];
    std::snprintf(buf, sizeof(buf), "fnv64:%016llx", (unsigned long long)h);
    return buf;
}

}

void init()     {}
void shutdown() {}

std::string fingerprint(std::string_view category,
                        std::string_view content,
                        std::vector<std::string> tags) {
    std::sort(tags.begin(), tags.end());
    std::ostringstream os;
    os << "cat=" << category << "\ncontent=" << content << "\ntags=";
    for (size_t i = 0; i < tags.size(); ++i) {
        if (i) os << ",";
        os << tags[i];
    }
    return fnv64_hex(os.str());
}

Snapshot load_jsonl(std::string_view src_path) {
    Snapshot out;
    std::ifstream in{std::string(src_path)};
    if (!in) return out;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        try {
            auto j = nlohmann::json::parse(line);
            Record r;
            r.ts       = j.value("ts", (int64_t)0);
            r.category = j.value("category", "");
            r.content  = j.value("content",  "");
            if (j.contains("tags") && j["tags"].is_array())
                r.tags = j["tags"].get<std::vector<std::string>>();
            r.source   = j.value("source", "");
            r.fingerprint = fingerprint(r.category, r.content, r.tags);
            out[r.fingerprint] = std::move(r);
        } catch (...) {}
    }
    return out;
}

TwoWayDiff diff(const Snapshot & left, const Snapshot & right) {
    TwoWayDiff d;
    for (const auto & [fp, r] : left) {
        if (right.count(fp)) d.common.push_back(r);
        else                 d.left_only.push_back(r);
    }
    for (const auto & [fp, r] : right) {
        if (!left.count(fp)) d.right_only.push_back(r);
    }
    return d;
}

ThreeWayClassification classify(const Snapshot & base,
                                const Snapshot & left,
                                const Snapshot & right) {
    ThreeWayClassification c;
    // Build the union of fingerprints so we touch each record once.
    std::map<std::string, const Record *> all;
    for (const auto & [fp, r] : base)  all[fp] = &r;
    for (const auto & [fp, r] : left)  all[fp] = &r;
    for (const auto & [fp, r] : right) all[fp] = &r;

    for (const auto & [fp, rp] : all) {
        bool in_b = base .count(fp);
        bool in_l = left .count(fp);
        bool in_r = right.count(fp);
        const Record & r = *rp;
        if (in_b && in_l && in_r)            c.unchanged    .push_back(r);
        else if (!in_b && in_l && !in_r)     c.added_left   .push_back(r);
        else if (!in_b && !in_l && in_r)     c.added_right  .push_back(r);
        else if (!in_b && in_l && in_r)      c.added_both   .push_back(r);
        else if (in_b && !in_l && in_r)      c.removed_left .push_back(r);
        else if (in_b && in_l && !in_r)      c.removed_right.push_back(r);
        // base-only (deleted everywhere) is silently dropped; nothing
        // to merge.
    }
    return c;
}

}
