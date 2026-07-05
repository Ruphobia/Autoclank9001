// Smoke test for 013_hex_editor: format detection, entropy, CRC32,
// diff and patch round trip.

#include "test_runner.hpp"
#include "../modules/013_hex_editor/hex_editor.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_hex";
    return fs::path("/tmp") / "tool_test_hex";
}

testing::TestOutcome run() {
    // Format detection on canonical magic bytes.
    {
        uint8_t png[]  = {0x89,'P','N','G',0x0D,0x0A,0x1A,0x0A, 'x','x'};
        uint8_t gguf[] = {'G','G','U','F', 0,0,0,0};
        uint8_t elf[]  = {0x7F,'E','L','F', 2, 1, 1, 0};
        uint8_t junk[] = {0x42, 0x42, 0x42, 0x42, 0x42};
        if (hex_editor::detect_format(png,  sizeof(png)).name  != "PNG")  return testing::fail("PNG magic miss");
        if (hex_editor::detect_format(gguf, sizeof(gguf)).name != "GGUF") return testing::fail("GGUF magic miss");
        if (hex_editor::detect_format(elf,  sizeof(elf)).name  != "ELF")  return testing::fail("ELF magic miss");
        if (hex_editor::detect_format(junk, sizeof(junk)).name != "unknown") return testing::fail("unknown miss");
    }

    // Entropy: constant buffer = 0; balanced = ~8.
    {
        std::vector<uint8_t> zeros(1024, 0);
        if (hex_editor::shannon_entropy(zeros.data(), zeros.size()) > 1e-9)
            return testing::fail("entropy of zeros not zero");
        std::vector<uint8_t> balanced(256 * 16);
        for (size_t i = 0; i < balanced.size(); ++i) balanced[i] = (uint8_t)(i & 0xFF);
        double h = hex_editor::shannon_entropy(balanced.data(), balanced.size());
        if (h < 7.99 || h > 8.0 + 1e-9)
            return testing::fail("entropy of uniform not ~8 bits, got " + std::to_string(h));
    }

    // CRC32: "123456789" -> 0xCBF43926 (the canonical reference vector).
    {
        const char * s = "123456789";
        if (hex_editor::crc32(reinterpret_cast<const uint8_t *>(s), 9) != 0xCBF43926u)
            return testing::fail("CRC32 reference vector mismatch");
    }

    // Read values at offset.
    {
        uint8_t  buf[8] = {0x78, 0x56, 0x34, 0x12, 0x11, 0x22, 0x33, 0x44};
        uint32_t v_le = 0, v_be = 0;
        hex_editor::read_u32(buf, sizeof(buf), 0, v_le, false);
        hex_editor::read_u32(buf, sizeof(buf), 0, v_be, true);
        if (v_le != 0x12345678u) return testing::fail("LE u32 wrong");
        if (v_be != 0x78563412u) return testing::fail("BE u32 wrong");
        uint8_t out;
        if (hex_editor::read_u8(buf, sizeof(buf), 8, out))
            return testing::fail("read past EOF should fail");
    }

    // Diff.
    {
        uint8_t a[] = {1,2,3,4,5,6,7,8};
        uint8_t b[] = {1,2,9,4,5,6,7,8};
        auto segs = hex_editor::diff(a, sizeof(a), b, sizeof(b));
        if (segs.size() != 1 || segs[0].offset != 2 ||
            segs[0].a.size() != 1 || segs[0].a[0] != 3 || segs[0].b[0] != 9)
            return testing::fail("diff single-byte segment wrong");

        uint8_t c[] = {0,0,0,0};
        uint8_t d[] = {0,0,0,0,7,7};
        auto trail = hex_editor::diff(c, sizeof(c), d, sizeof(d));
        if (trail.size() != 1 || trail[0].offset != 4 || trail[0].b.size() != 2)
            return testing::fail("diff trailing-bytes wrong");
    }

    // Hex dump produces a non-empty multi-line string with the offset
    // prefix the renderer promises.
    {
        uint8_t buf[3] = {'a','b','c'};
        std::string s = hex_editor::dump(buf, sizeof(buf), 0x100);
        if (s.find("00000100") == std::string::npos || s.find("|abc|") == std::string::npos)
            return testing::fail("hex dump missing expected pieces: " + s);
    }

    // Patch round trip: write a file, patch two ranges, read back.
    {
        fs::path root = scratch_root();
        std::error_code ec;
        fs::remove_all(root, ec);
        fs::create_directories(root);
        fs::path f = root / "patch.bin";

        std::vector<uint8_t> orig(16);
        for (size_t i = 0; i < orig.size(); ++i) orig[i] = (uint8_t)i;
        {
            std::ofstream o(f, std::ios::binary);
            o.write(reinterpret_cast<const char *>(orig.data()), (std::streamsize)orig.size());
        }

        std::vector<hex_editor::Patch> patches = {
            {4, {0xAA, 0xBB}},
            {12, {0xCC}}
        };
        hex_editor::apply_patches(f.string(), patches, true);
        if (!fs::exists(fs::path(f.string() + ".bak"))) return testing::fail("backup missing");

        std::vector<uint8_t> readback(16);
        {
            std::ifstream in(f, std::ios::binary);
            in.read(reinterpret_cast<char *>(readback.data()), (std::streamsize)readback.size());
        }
        if (readback[4] != 0xAA || readback[5] != 0xBB || readback[12] != 0xCC ||
            readback[6] != 6 || readback[11] != 11)
            return testing::fail("patch not applied as expected");

        fs::remove_all(root, ec);
    }

    return testing::ok();
}

const int _r = testing::register_test(
    "hex_editor",
    "013_hex_editor: format detection, CRC32/entropy, diff, hex dump, patch",
    &run);

}
