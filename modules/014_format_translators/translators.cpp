#include "translators.hpp"

#include "../011_scripting/scripting.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <stdexcept>

namespace translators {
namespace {

namespace fs = std::filesystem;

std::string lower_ext(std::string_view path) {
    fs::path p{std::string(path)};
    auto    ext = p.extension().string();
    if (!ext.empty() && ext.front() == '.') ext.erase(0, 1);
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    return ext;
}

std::string shell_quote(std::string_view s) {
    std::string out;
    out.reserve(s.size() + 2);
    out.push_back('\'');
    for (char c : s) {
        if (c == '\'') out.append("'\\''");
        else           out.push_back(c);
    }
    out.push_back('\'');
    return out;
}

// {src} -> source, {dst} -> target, expanded with single-quote escapes.
std::string fill_template(std::string_view tmpl,
                          std::string_view src,
                          std::string_view dst) {
    std::string out;
    out.reserve(tmpl.size() + 64);
    for (size_t i = 0; i < tmpl.size();) {
        if (tmpl[i] == '{' && i + 4 < tmpl.size() &&
            tmpl.compare(i, 5, "{src}") == 0) {
            out += shell_quote(src); i += 5;
        } else if (tmpl[i] == '{' && i + 4 < tmpl.size() &&
                   tmpl.compare(i, 5, "{dst}") == 0) {
            out += shell_quote(dst); i += 5;
        } else {
            out.push_back(tmpl[i++]);
        }
    }
    return out;
}

struct Rule {
    const char * src_ext;
    const char * dst_ext;
    const char * tool;
    const char * cmd_template;
};

// Curated set. Identity rules are excluded; symmetric pairs are spelled
// out explicitly so the table doubles as documentation. Tools that
// commonly need a flag to emit a different format are written in the
// command template so what runs is what the table says.
const Rule kRules[] = {
    // Compression
    {"",     "gz",   "gzip",     "gzip -c -- {src} > {dst}"},
    {"gz",   "",     "gunzip",   "gunzip -c -- {src} > {dst}"},
    {"",     "bz2",  "bzip2",    "bzip2 -c -- {src} > {dst}"},
    {"bz2",  "",     "bunzip2",  "bunzip2 -c -- {src} > {dst}"},
    {"",     "xz",   "xz",       "xz -c -- {src} > {dst}"},
    {"xz",   "",     "unxz",     "unxz -c -- {src} > {dst}"},
    {"",     "zst",  "zstd",     "zstd -q -c -- {src} > {dst}"},
    {"zst",  "",     "unzstd",   "unzstd -q -c -- {src} > {dst}"},

    // Archives (src/dst are dirs for create, dirs for extract)
    {"",     "zip",  "zip",      "(cd {src} && zip -qr - .) > {dst}"},
    {"zip",  "",     "unzip",    "unzip -q -d {dst} -- {src}"},
    {"",     "tar",  "tar",      "tar -cf {dst} -C {src} ."},
    {"tar",  "",     "tar",      "mkdir -p {dst} && tar -xf {src} -C {dst}"},
    {"tgz",  "",     "tar",      "mkdir -p {dst} && tar -xzf {src} -C {dst}"},
    {"7z",   "",     "7z",       "7z x -y -o{dst} -- {src} >/dev/null"},

    // Documents (pandoc covers most pairs in one binary)
    {"md",   "html", "pandoc",   "pandoc -f markdown -t html -o {dst} -- {src}"},
    {"md",   "pdf",  "pandoc",   "pandoc -f markdown -o {dst} -- {src}"},
    {"md",   "docx", "pandoc",   "pandoc -f markdown -o {dst} -- {src}"},
    {"html", "md",   "pandoc",   "pandoc -f html -t markdown -o {dst} -- {src}"},
    {"docx", "md",   "pandoc",   "pandoc -f docx -t markdown -o {dst} -- {src}"},
    {"rst",  "md",   "pandoc",   "pandoc -f rst  -t markdown -o {dst} -- {src}"},
    {"tex",  "pdf",  "pandoc",   "pandoc -f latex -o {dst} -- {src}"},

    // Images (ImageMagick)
    {"png",  "jpg",  "convert",  "convert -- {src} {dst}"},
    {"jpg",  "png",  "convert",  "convert -- {src} {dst}"},
    {"png",  "webp", "convert",  "convert -- {src} {dst}"},
    {"webp", "png",  "convert",  "convert -- {src} {dst}"},
    {"bmp",  "png",  "convert",  "convert -- {src} {dst}"},
    {"tif",  "png",  "convert",  "convert -- {src} {dst}"},
    {"tiff", "png",  "convert",  "convert -- {src} {dst}"},
    {"svg",  "png",  "convert",  "convert -density 192 -- {src} {dst}"},
    {"heic", "png",  "convert",  "convert -- {src} {dst}"},

    // Audio / video (ffmpeg)
    {"wav",  "mp3",  "ffmpeg",   "ffmpeg -y -i {src} -- {dst}"},
    {"mp3",  "wav",  "ffmpeg",   "ffmpeg -y -i {src} -- {dst}"},
    {"flac", "wav",  "ffmpeg",   "ffmpeg -y -i {src} -- {dst}"},
    {"mp4",  "mp3",  "ffmpeg",   "ffmpeg -y -i {src} -vn -- {dst}"},
    {"mov",  "mp4",  "ffmpeg",   "ffmpeg -y -i {src} -c copy -- {dst}"},
    {"webm", "mp4",  "ffmpeg",   "ffmpeg -y -i {src} -- {dst}"},
};

std::vector<Pairing> build_supported() {
    std::vector<Pairing> v;
    v.reserve(sizeof(kRules) / sizeof(kRules[0]));
    for (const auto & r : kRules) {
        v.push_back({r.src_ext ? r.src_ext : "", r.dst_ext ? r.dst_ext : "", r.tool});
    }
    return v;
}

const Rule * find_rule(const std::string & src_ext, const std::string & dst_ext) {
    const Rule * wildcard = nullptr;
    for (const auto & r : kRules) {
        const std::string rs = r.src_ext ? r.src_ext : "";
        const std::string rd = r.dst_ext ? r.dst_ext : "";
        if (rs == src_ext && rd == dst_ext) return &r;          // exact
        const bool src_ok = rs.empty() || rs == src_ext;
        const bool dst_ok = rd.empty() || rd == dst_ext;
        if (src_ok && dst_ok && !wildcard) wildcard = &r;
    }
    return wildcard;
}

}

void init()     {}
void shutdown() {}

bool tool_available(std::string_view tool) {
    // POSIX `command -v` is the most portable existence probe.
    std::string cmd = "command -v ";
    cmd += shell_quote(tool);
    cmd += " >/dev/null 2>&1";
    return std::system(cmd.c_str()) == 0;
}

const std::vector<Pairing> & supported_pairings() {
    static const std::vector<Pairing> v = build_supported();
    return v;
}

std::optional<TranslationPlan> plan(std::string_view src, std::string_view dst) {
    auto src_ext = lower_ext(src);
    auto dst_ext = lower_ext(dst);
    const Rule * rule = find_rule(src_ext, dst_ext);
    if (!rule) return std::nullopt;
    if (!tool_available(rule->tool)) return std::nullopt;
    TranslationPlan p;
    p.tool        = rule->tool;
    p.source_path = std::string(src);
    p.target_path = std::string(dst);
    p.command     = fill_template(rule->cmd_template, src, dst);
    return p;
}

Result translate(std::string_view src, std::string_view dst, int timeout_seconds) {
    auto p = plan(src, dst);
    if (!p) {
        throw std::runtime_error("translators::translate: no rule (or tool missing) for " +
                                 lower_ext(src) + " -> " + lower_ext(dst));
    }
    Result r;
    r.plan = *p;
    auto er = scripting::execute(p->command, {}, timeout_seconds);
    r.stdout_text = er.stdout_text;
    r.stderr_text = er.stderr_text;
    r.exit_code   = er.exit_code;
    r.timed_out   = er.timed_out;
    return r;
}

}
