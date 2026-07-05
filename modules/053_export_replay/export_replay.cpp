#include "export_replay.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace export_replay {
namespace {

namespace fs = std::filesystem;

constexpr const char * kCrLf = "\r\n";

std::string b64_encode(std::string_view in) {
    static const char * t =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    out.reserve(((in.size() + 2) / 3) * 4);
    size_t i = 0;
    while (i + 3 <= in.size()) {
        uint32_t v = ((uint32_t)(unsigned char)in[i] << 16)
                   | ((uint32_t)(unsigned char)in[i+1] << 8)
                   |  (uint32_t)(unsigned char)in[i+2];
        out.push_back(t[(v >> 18) & 0x3F]);
        out.push_back(t[(v >> 12) & 0x3F]);
        out.push_back(t[(v >>  6) & 0x3F]);
        out.push_back(t[ v        & 0x3F]);
        i += 3;
    }
    if (i < in.size()) {
        uint32_t v = (uint32_t)(unsigned char)in[i] << 16;
        if (i + 1 < in.size()) v |= (uint32_t)(unsigned char)in[i+1] << 8;
        out.push_back(t[(v >> 18) & 0x3F]);
        out.push_back(t[(v >> 12) & 0x3F]);
        if (i + 1 < in.size()) out.push_back(t[(v >> 6) & 0x3F]); else out.push_back('=');
        out.push_back('=');
    }
    return out;
}

std::string b64_decode(std::string_view in) {
    static int8_t d[256];
    static bool   ready = false;
    if (!ready) {
        for (int i = 0; i < 256; ++i) d[i] = -1;
        const char * t = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        for (int i = 0; i < 64; ++i) d[(unsigned char)t[i]] = (int8_t)i;
        ready = true;
    }
    std::string out;
    uint32_t buf = 0;
    int bits = 0;
    for (char c : in) {
        if (c == '=' || std::isspace(static_cast<unsigned char>(c))) continue;
        int v = d[(unsigned char)c];
        if (v < 0) continue;
        buf = (buf << 6) | (uint32_t)v;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out.push_back((char)((buf >> bits) & 0xFF));
        }
    }
    return out;
}

void write_record(std::ofstream & o, const Record & r) {
    std::ostringstream hdr;
    hdr << "WARC/1.0" << kCrLf;
    hdr << "WARC-Type: "         << (r.warc_type.empty()    ? "resource" : r.warc_type)   << kCrLf;
    hdr << "WARC-Date: "         << r.date_iso << kCrLf;
    hdr << "WARC-Record-ID: "    << (r.record_id.empty()    ? "<urn:uuid:0>" : r.record_id) << kCrLf;
    if (!r.target_uri.empty())   hdr << "WARC-Target-URI: " << r.target_uri << kCrLf;
    hdr << "Content-Type: "      << (r.content_type.empty() ? "application/octet-stream" : r.content_type) << kCrLf;
    hdr << "Content-Length: "    << r.body.size() << kCrLf;
    hdr << kCrLf;     // blank line between headers and body
    std::string s = hdr.str();
    o.write(s.data(), (std::streamsize)s.size());
    if (!r.body.empty()) o.write(r.body.data(), (std::streamsize)r.body.size());
    o.write(kCrLf, 2);
    o.write(kCrLf, 2);
}

bool read_record(std::ifstream & in, Record & out) {
    std::string line;
    // Skip leading blank lines.
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!line.empty()) break;
    }
    if (in.eof() && line.empty()) return false;
    if (line.rfind("WARC/", 0) != 0) return false;

    size_t body_len = 0;
    out = Record{};
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) break;     // end of headers
        auto colon = line.find(':');
        if (colon == std::string::npos) continue;
        std::string k = line.substr(0, colon);
        std::string v = line.substr(colon + 1);
        while (!v.empty() && (v.front() == ' ' || v.front() == '\t')) v.erase(0, 1);
        if (k == "WARC-Type")        out.warc_type    = v;
        else if (k == "WARC-Date")   out.date_iso     = v;
        else if (k == "WARC-Record-ID") out.record_id = v;
        else if (k == "WARC-Target-URI") out.target_uri = v;
        else if (k == "Content-Type")   out.content_type = v;
        else if (k == "Content-Length") body_len = (size_t)std::stoull(v);
    }
    out.body.resize(body_len);
    if (body_len) in.read(out.body.data(), (std::streamsize)body_len);
    // Eat the two trailing CRLFs (or whatever whitespace).
    int eaten = 0;
    while (eaten < 4 && in.peek() != EOF) {
        char c = (char)in.get();
        if (c == '\r' || c == '\n') ++eaten; else { in.unget(); break; }
    }
    return true;
}

}

void init()     {}
void shutdown() {}

size_t write_warc(const std::vector<Record> & records, std::string_view path) {
    fs::path p{std::string(path)};
    fs::create_directories(p.parent_path());
    std::ofstream o(p, std::ios::binary | std::ios::trunc);
    if (!o) throw std::runtime_error("export_replay: cannot write " + p.string());
    for (const auto & r : records) write_record(o, r);
    return records.size();
}

std::vector<Record> read_warc(std::string_view path) {
    std::vector<Record> out;
    std::ifstream in{std::string(path), std::ios::binary};
    if (!in) return out;
    Record r;
    while (read_record(in, r)) out.push_back(r);
    return out;
}

size_t replay_warc(std::string_view path,
                   const std::function<void(const Record &)> & cb) {
    std::ifstream in{std::string(path), std::ios::binary};
    if (!in) return 0;
    size_t n = 0;
    Record r;
    while (read_record(in, r)) { cb(r); ++n; }
    return n;
}

size_t write_jsonl(const std::vector<Record> & records, std::string_view path) {
    fs::path p{std::string(path)};
    fs::create_directories(p.parent_path());
    std::ofstream o(p, std::ios::binary | std::ios::trunc);
    if (!o) throw std::runtime_error("export_replay: cannot write jsonl " + p.string());
    for (const auto & r : records) {
        nlohmann::json j = {
            {"warc_type",    r.warc_type},
            {"date",         r.date_iso},
            {"record_id",    r.record_id},
            {"uri",          r.target_uri},
            {"content_type", r.content_type},
            {"body_b64",     b64_encode(r.body)},
        };
        o << j.dump() << "\n";
    }
    return records.size();
}

std::vector<Record> read_jsonl(std::string_view path) {
    std::vector<Record> out;
    std::ifstream in{std::string(path)};
    if (!in) return out;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        try {
            auto j = nlohmann::json::parse(line);
            Record r;
            r.warc_type    = j.value("warc_type", "");
            r.date_iso     = j.value("date",      "");
            r.record_id    = j.value("record_id", "");
            r.target_uri   = j.value("uri",       "");
            r.content_type = j.value("content_type", "");
            r.body         = b64_decode(j.value("body_b64", ""));
            out.push_back(r);
        } catch (...) {}
    }
    return out;
}

}
