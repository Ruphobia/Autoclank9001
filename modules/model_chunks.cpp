#include "model_chunks.hpp"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace model_chunks {
namespace fs = std::filesystem;

bool ensure(const fs::path & final_path) {
    if (fs::exists(final_path) && fs::file_size(final_path) > 0) {
        return true;
    }

    const fs::path dir = final_path.parent_path();
    const std::string base = final_path.filename().string();
    const std::string prefix = base + ".part-";

    if (!fs::exists(dir)) {
        std::fprintf(stderr,
            "model_chunks: directory %s does not exist\n",
            dir.string().c_str());
        return false;
    }

    std::vector<fs::path> chunks;
    for (const auto & e : fs::directory_iterator(dir)) {
        if (!e.is_regular_file()) continue;
        const std::string name = e.path().filename().string();
        if (name.size() <= prefix.size()) continue;
        if (name.compare(0, prefix.size(), prefix) != 0) continue;
        if (name.size() < 4 ||
            name.compare(name.size() - 4, 4, ".bin") != 0) continue;
        chunks.push_back(e.path());
    }

    if (chunks.empty()) {
        std::fprintf(stderr,
            "model_chunks: %s missing and no chunks found at %s/%s*.bin\n",
            final_path.string().c_str(),
            dir.string().c_str(),
            prefix.c_str());
        return false;
    }

    std::sort(chunks.begin(), chunks.end());

    std::fprintf(stderr,
        "model_chunks: reassembling %s from %zu chunks (one-time)...\n",
        final_path.string().c_str(), chunks.size());

    const fs::path tmp_path = final_path.string() + ".reassembling";
    {
        std::ofstream out(tmp_path, std::ios::binary | std::ios::trunc);
        if (!out) {
            throw std::runtime_error(
                "model_chunks: cannot open output " + tmp_path.string());
        }
        std::array<char, 1 << 20> buf;  // 1 MiB
        for (const auto & c : chunks) {
            std::ifstream in(c, std::ios::binary);
            if (!in) {
                throw std::runtime_error(
                    "model_chunks: cannot open chunk " + c.string());
            }
            while (in) {
                in.read(buf.data(), buf.size());
                std::streamsize n = in.gcount();
                if (n > 0) out.write(buf.data(), n);
            }
            if (!out) {
                throw std::runtime_error(
                    "model_chunks: write failed at chunk " + c.string());
            }
        }
        out.flush();
        if (!out) {
            throw std::runtime_error("model_chunks: final flush failed");
        }
    }
    fs::rename(tmp_path, final_path);

    std::fprintf(stderr,
        "model_chunks: ready -> %s (%llu bytes)\n",
        final_path.string().c_str(),
        static_cast<unsigned long long>(fs::file_size(final_path)));
    return true;
}

}
