#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

// Attachment Deduper: detects duplicate PDFs / figures / supplementary
// files in a reference library's attachments folder. Content
// duplicates use FNV-1a 64 over the raw bytes (proven adequate for
// byte-identical files; collision-improbable at the small library
// scales we deal with). Near-duplicate detection over text-bearing
// files reuses 044_dedup_ingest::simhash64 on a text excerpt the
// caller supplies (e.g. the OCR layer of a PDF), since perceptual
// image hashing needs an image decoder that lives outside this layer.
namespace attachment_deduper {

struct FileFp {
    std::string path;
    std::string content_hash;       // "fnv64:<hex>"
    uint64_t    simhash       = 0;  // 0 means "no text excerpt provided"
    size_t      size_bytes    = 0;
};

struct Group {
    std::vector<std::string> paths;
    bool                     byte_identical = false;
    int                      hamming_within = 0;     // worst pairwise SimHash distance
};

void init();
void shutdown();

// Reads the file and computes the byte hash plus size. text_excerpt
// is the OPTIONAL secondary signal: when non-empty, simhash is also
// computed so near-duplicate clustering becomes possible alongside
// byte-exact clustering.
FileFp fingerprint(std::string_view path, std::string_view text_excerpt = {});

// Group `fps` into duplicate clusters. byte_identical groups always
// win; the remaining files are then clustered by SimHash within
// `near_dup_hamming_threshold` bits when simhash is non-zero. Files
// without a SimHash never join a near-dup group.
std::vector<Group> cluster(const std::vector<FileFp> & fps,
                           int near_dup_hamming_threshold = 3);

}
