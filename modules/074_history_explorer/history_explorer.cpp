#include "history_explorer.hpp"

#include "../011_scripting/scripting.hpp"

#include <algorithm>
#include <ctime>
#include <sstream>
#include <unordered_map>

namespace history_explorer {
namespace {

std::string shell_quote(std::string_view s) {
    std::string out;
    out.push_back('\'');
    for (char c : s) {
        if (c == '\'') out.append("'\\''");
        else           out.push_back(c);
    }
    out.push_back('\'');
    return out;
}

std::string format_iso_from_unix(int64_t ts) {
    std::time_t t = (std::time_t)ts;
    std::tm tm{}; ::gmtime_r(&t, &tm);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tm);
    return buf;
}

}

void init()     {}
void shutdown() {}

std::vector<BlameEntry> blame(std::string_view repo, std::string_view path,
                              size_t line_only) {
    std::vector<BlameEntry> out;
    std::string cmd = "git -C ";
    cmd += shell_quote(repo);
    cmd += " blame --line-porcelain";
    if (line_only > 0) {
        cmd += " -L ";
        cmd += std::to_string(line_only);
        cmd += ",";
        cmd += std::to_string(line_only);
    }
    cmd += " -- ";
    cmd += shell_quote(path);
    auto r = scripting::execute(cmd, {}, 30);
    if (r.exit_code != 0) return out;

    // Per-line porcelain blocks look like:
    //   <sha> orig_line final_line [count]
    //   author Name
    //   author-time 1700000000
    //   ...
    //   \t<line text>
    std::istringstream is(r.stdout_text);
    std::string line;
    BlameEntry cur;
    bool have_header = false;
    while (std::getline(is, line)) {
        if (line.empty()) continue;
        if (line[0] == '\t') {
            cur.text = line.substr(1);
            out.push_back(cur);
            cur = BlameEntry{};
            have_header = false;
            continue;
        }
        // sha line: 40-hex + space + orig + space + final + (optional count)
        if (!have_header && line.size() >= 41 && line[40] == ' ') {
            cur.sha = line.substr(0, 7);
            int orig = 0, finalln = 0;
            std::sscanf(line.c_str() + 41, "%d %d", &orig, &finalln);
            cur.line_number = (size_t)finalln;
            have_header = true;
            continue;
        }
        // metadata lines
        if (line.rfind("author ", 0) == 0) {
            cur.author = line.substr(7);
        } else if (line.rfind("author-time ", 0) == 0) {
            int64_t ts = 0;
            try { ts = std::stoll(line.substr(12)); } catch (...) {}
            cur.date = format_iso_from_unix(ts);
        }
    }
    return out;
}

namespace {

std::vector<git_workbench::Commit> parse_commit_list(const std::string & out_text) {
    std::vector<git_workbench::Commit> out;
    std::istringstream is(out_text);
    std::string line;
    while (std::getline(is, line)) {
        if (line.empty()) continue;
        size_t t1 = line.find('\t');
        if (t1 == std::string::npos) continue;
        size_t t2 = line.find('\t', t1 + 1);
        if (t2 == std::string::npos) continue;
        size_t t3 = line.find('\t', t2 + 1);
        if (t3 == std::string::npos) continue;
        git_workbench::Commit c;
        c.sha     = line.substr(0, t1);
        c.author  = line.substr(t1 + 1, t2 - t1 - 1);
        c.date    = line.substr(t2 + 1, t3 - t2 - 1);
        c.subject = line.substr(t3 + 1);
        out.push_back(c);
    }
    return out;
}

}

std::vector<git_workbench::Commit> file_history(std::string_view repo,
                                                std::string_view path,
                                                std::string_view range) {
    std::string cmd = "git -C ";
    cmd += shell_quote(repo);
    cmd += " log --no-color --pretty=format:%h%x09%an%x09%aI%x09%s";
    if (!range.empty()) { cmd += " "; cmd.append(range.data(), range.size()); }
    cmd += " -- ";
    cmd += shell_quote(path);
    auto r = scripting::execute(cmd, {}, 30);
    return parse_commit_list(r.stdout_text);
}

std::vector<git_workbench::Commit> pickaxe(std::string_view repo,
                                           std::string_view term,
                                           std::string_view range) {
    std::string cmd = "git -C ";
    cmd += shell_quote(repo);
    cmd += " log --no-color --pretty=format:%h%x09%an%x09%aI%x09%s -S";
    cmd += shell_quote(term);
    if (!range.empty()) { cmd += " "; cmd.append(range.data(), range.size()); }
    auto r = scripting::execute(cmd, {}, 30);
    return parse_commit_list(r.stdout_text);
}

std::vector<AuthorCount> commits_by_author(std::string_view repo,
                                           std::string_view range) {
    std::string cmd = "git -C ";
    cmd += shell_quote(repo);
    cmd += " log --no-color --pretty=format:%an";
    if (!range.empty()) { cmd += " "; cmd.append(range.data(), range.size()); }
    auto r = scripting::execute(cmd, {}, 30);
    std::unordered_map<std::string, size_t> counts;
    std::istringstream is(r.stdout_text);
    std::string line;
    while (std::getline(is, line)) {
        if (!line.empty()) ++counts[line];
    }
    std::vector<AuthorCount> out;
    for (auto & [k, v] : counts) out.push_back({k, v});
    std::sort(out.begin(), out.end(),
        [](const AuthorCount & a, const AuthorCount & b) {
            if (a.count != b.count) return a.count > b.count;
            return a.author < b.author;
        });
    return out;
}

}
