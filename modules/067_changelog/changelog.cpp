#include "changelog.hpp"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstring>
#include <map>
#include <sstream>
#include <unordered_map>

namespace changelog {
namespace {

const std::array<std::pair<const char *, const char *>, 9> kKnownTypes = {{
    {"feat",     "Features"},
    {"fix",      "Bug fixes"},
    {"perf",     "Performance"},
    {"refactor", "Refactors"},
    {"docs",     "Documentation"},
    {"test",     "Tests"},
    {"build",    "Build"},
    {"ci",       "CI"},
    {"chore",    "Chores"},
}};

}

const char * to_string(SemverBump b) {
    switch (b) {
        case SemverBump::Major: return "major";
        case SemverBump::Minor: return "minor";
        case SemverBump::Patch: return "patch";
    }
    return "patch";
}

void init()     {}
void shutdown() {}

std::string detect_type(std::string_view subject) {
    // Match `<type>(<scope>)?: <subject>` at the start. Lowercase the type.
    std::string s{subject};
    auto colon = s.find(':');
    if (colon == std::string::npos) return {};
    std::string prefix = s.substr(0, colon);
    auto bang = prefix.find('!');
    if (bang != std::string::npos) prefix.erase(bang);
    auto paren = prefix.find('(');
    if (paren != std::string::npos) prefix.erase(paren);
    std::string type;
    for (char c : prefix) {
        if (std::isalpha(static_cast<unsigned char>(c)))
            type.push_back((char)std::tolower((unsigned char)c));
        else
            return {};
    }
    return type;
}

std::vector<Group> group_by_type(const std::vector<Commit> & commits) {
    std::vector<Group> out;
    std::unordered_map<std::string, size_t> ix;
    for (const auto & [k, label] : kKnownTypes) {
        Group g; g.ccm_type = k; g.label = label;
        ix[k] = out.size();
        out.push_back(std::move(g));
    }
    Group other; other.label = "Other"; other.ccm_type = "";
    out.push_back(std::move(other));
    size_t other_ix = out.size() - 1;

    for (const auto & c : commits) {
        std::string t = detect_type(c.subject);
        auto it = ix.find(t);
        if (t.empty() || it == ix.end()) out[other_ix].commits.push_back(c);
        else                              out[it->second].commits.push_back(c);
    }
    // Drop empty groups.
    out.erase(std::remove_if(out.begin(), out.end(),
        [](const Group & g){ return g.commits.empty(); }), out.end());
    return out;
}

SemverBump suggest_bump(const std::vector<Commit> & commits) {
    bool any_feat = false;
    for (const auto & c : commits) {
        if (c.body.find("BREAKING CHANGE") != std::string::npos)
            return SemverBump::Major;
        // type! marker
        auto colon = c.subject.find(':');
        if (colon != std::string::npos) {
            std::string prefix = c.subject.substr(0, colon);
            if (prefix.find('!') != std::string::npos) return SemverBump::Major;
        }
        std::string t = detect_type(c.subject);
        if (t == "feat") any_feat = true;
    }
    return any_feat ? SemverBump::Minor : SemverBump::Patch;
}

std::string render_markdown(const std::vector<Commit> & commits,
                            std::string_view title) {
    auto groups = group_by_type(commits);
    std::ostringstream os;
    os << title << "\n\n";
    for (const auto & g : groups) {
        os << "### " << g.label << "\n\n";
        for (const auto & c : g.commits) {
            os << "- " << c.subject;
            if (!c.sha.empty()) os << " (" << c.sha << ")";
            os << "\n";
        }
        os << "\n";
    }
    return os.str();
}

std::vector<Commit> read_git_log(std::string_view repo_path, std::string_view range) {
    std::vector<Commit> out;
    // Format: SHA <tab> SUBJECT <NUL> BODY <RS>
    std::string cmd = "git -C '";
    for (char c : repo_path) { if (c == '\'') cmd += "'\\''"; else cmd.push_back(c); }
    cmd += "' log --no-color --pretty=format:%h%x09%s%x00%b%x1e ";
    cmd.append(range.data(), range.size());
    FILE * p = ::popen(cmd.c_str(), "r");
    if (!p) return out;
    std::string buf;
    char chunk[4096];
    while (size_t n = ::fread(chunk, 1, sizeof(chunk), p)) buf.append(chunk, n);
    ::pclose(p);

    size_t start = 0;
    for (size_t i = 0; i <= buf.size(); ++i) {
        if (i == buf.size() || buf[i] == '\x1e') {
            std::string rec = buf.substr(start, i - start);
            start = i + 1;
            if (rec.empty()) continue;
            // Split sha <tab> subject <NUL> body
            auto tab = rec.find('\t');
            if (tab == std::string::npos) continue;
            std::string sha = rec.substr(0, tab);
            std::string after = rec.substr(tab + 1);
            auto nul = after.find('\0');
            Commit c;
            c.sha     = sha;
            c.subject = (nul == std::string::npos) ? after : after.substr(0, nul);
            c.body    = (nul == std::string::npos) ? std::string{} : after.substr(nul + 1);
            // strip leading whitespace from the body (git includes a newline)
            while (!c.body.empty() && (c.body.front() == '\n' || c.body.front() == ' '))
                c.body.erase(0, 1);
            out.push_back(c);
        }
    }
    return out;
}

}
