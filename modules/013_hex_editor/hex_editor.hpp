#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

// Hex editor engine. UI-free: this exposes the operations a binary
// editor needs (hex dump, value-at-offset, structural format detection,
// diff, patch, checksum, entropy). The web UI in 010_interface can
// wrap these later. Built deliberately without new dependencies; CRC32
// is the only checksum because stronger hashes belong in a dedicated
// crypto module if and when we add one.
namespace hex_editor {

// Render a region of `data` as the classic three-column hex dump:
// offset, hex bytes (bytes_per_line wide), printable-ASCII gutter.
// `base_offset` is added to every printed offset so dumps of mapped
// sub-ranges can show their real file address.
std::string dump(const uint8_t * data,
                 size_t          size,
                 uint64_t        base_offset = 0,
                 size_t          bytes_per_line = 16);

// Read a fixed-width integer or float at `offset`. Returns false if the
// read would go past `size`. Endianness defaults to little.
bool read_u8 (const uint8_t * d, size_t n, size_t off, uint8_t  & out);
bool read_u16(const uint8_t * d, size_t n, size_t off, uint16_t & out, bool big_endian = false);
bool read_u32(const uint8_t * d, size_t n, size_t off, uint32_t & out, bool big_endian = false);
bool read_u64(const uint8_t * d, size_t n, size_t off, uint64_t & out, bool big_endian = false);
bool read_i8 (const uint8_t * d, size_t n, size_t off, int8_t   & out);
bool read_i16(const uint8_t * d, size_t n, size_t off, int16_t  & out, bool big_endian = false);
bool read_i32(const uint8_t * d, size_t n, size_t off, int32_t  & out, bool big_endian = false);
bool read_i64(const uint8_t * d, size_t n, size_t off, int64_t  & out, bool big_endian = false);
bool read_f32(const uint8_t * d, size_t n, size_t off, float    & out, bool big_endian = false);
bool read_f64(const uint8_t * d, size_t n, size_t off, double   & out, bool big_endian = false);

// Magic-byte based format identification. Covers the common containers
// the workbench is likely to bump into (ELF, PE, PNG, JPEG, GIF, GGUF,
// ZIP, GZIP, TAR ustar, PDF, BMP). Returns {"unknown", ""} on miss.
struct FormatId {
    std::string name;       // short identifier, e.g. "PNG"
    std::string detail;     // human-facing one-liner
};
FormatId detect_format(const uint8_t * data, size_t size);

// Shannon entropy of `data` in bits per byte, range [0, 8].
double shannon_entropy(const uint8_t * data, size_t size);

// CRC32 (IEEE polynomial 0xEDB88320), the same flavour ZIP and PNG use.
uint32_t crc32(const uint8_t * data, size_t size);

// Byte-level diff. Each segment is the smallest contiguous run where
// the two buffers differ; runs are coalesced when the gap of equal
// bytes between them is shorter than `coalesce_gap`. If the buffers
// have different lengths, the trailing extra bytes are reported as a
// single segment at the shorter length.
struct DiffSegment {
    uint64_t              offset;
    std::vector<uint8_t>  a;
    std::vector<uint8_t>  b;
};
std::vector<DiffSegment> diff(const uint8_t * a, size_t sa,
                              const uint8_t * b, size_t sb,
                              size_t coalesce_gap = 4);

// Write-then-rename byte patches. Each patch overwrites `bytes.size()`
// bytes starting at `offset`. When `make_backup` is true the original
// file is copied to `<path>.bak` before any write. Throws on IO error
// or if any patch would extend past the file length.
struct Patch {
    uint64_t              offset = 0;
    std::vector<uint8_t>  bytes;
};
void apply_patches(std::string_view              path,
                   const std::vector<Patch>    & patches,
                   bool                          make_backup = true);

}
