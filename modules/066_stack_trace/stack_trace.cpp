#include "stack_trace.hpp"

#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>

namespace stack_trace {
namespace {

std::vector<std::string> split_lines(std::string_view text) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : text) {
        if (c == '\n') { out.push_back(cur); cur.clear(); }
        else cur.push_back(c);
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

std::string trim(std::string_view in) {
    size_t a = 0, b = in.size();
    while (a < b && std::isspace(static_cast<unsigned char>(in[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(in[b - 1]))) --b;
    return std::string(in.substr(a, b - a));
}

}

const char * to_string(Language l) {
    switch (l) {
        case Language::Python:  return "python";
        case Language::NodeJs:  return "nodejs";
        case Language::Java:    return "java";
        case Language::Go:      return "go";
        case Language::Rust:    return "rust";
        case Language::Cxx:     return "cxx";
        case Language::Unknown: return "unknown";
    }
    return "unknown";
}

void init()     {}
void shutdown() {}

Trace parse_python(std::string_view text) {
    Trace t;
    t.language = Language::Python;
    auto lines = split_lines(text);
    static const std::regex frame_re(R"RX(\s*File "([^"]+)", line (\d+), in (\S+))RX");
    std::string last_error;
    for (size_t i = 0; i < lines.size(); ++i) {
        std::smatch m;
        if (std::regex_match(lines[i], m, frame_re)) {
            Frame f;
            f.language = Language::Python;
            f.file     = m[1].str();
            f.line     = std::stoi(m[2].str());
            f.function = m[3].str();
            f.raw      = lines[i];
            t.frames.push_back(f);
        } else {
            std::string tr = trim(lines[i]);
            if (!tr.empty() && tr.find("Error") != std::string::npos &&
                tr.find("Traceback") == std::string::npos) {
                last_error = tr;
            }
        }
    }
    t.error = last_error;
    return t;
}

Trace parse_nodejs(std::string_view text) {
    Trace t;
    t.language = Language::NodeJs;
    auto lines = split_lines(text);
    // First non-frame line typically holds the error message ("TypeError: ...").
    static const std::regex frame_re(
        R"(\s*at (?:([^\s(]+) \()?([^):]+):(\d+):(\d+)\)?)");
    for (size_t i = 0; i < lines.size(); ++i) {
        std::smatch m;
        if (std::regex_match(lines[i], m, frame_re)) {
            Frame f;
            f.language = Language::NodeJs;
            f.function = m[1].str();
            f.file     = m[2].str();
            f.line     = std::stoi(m[3].str());
            f.column   = std::stoi(m[4].str());
            f.raw      = lines[i];
            t.frames.push_back(f);
        } else if (t.error.empty()) {
            std::string tr = trim(lines[i]);
            if (!tr.empty()) t.error = tr;
        }
    }
    return t;
}

Trace parse_java(std::string_view text) {
    Trace t;
    t.language = Language::Java;
    auto lines = split_lines(text);
    static const std::regex frame_re(R"(\s*at ([\w$.<>]+)\(([^:)]+)(?::(\d+))?\))");
    for (size_t i = 0; i < lines.size(); ++i) {
        std::smatch m;
        if (std::regex_match(lines[i], m, frame_re)) {
            Frame f;
            f.language = Language::Java;
            f.function = m[1].str();
            f.file     = m[2].str();
            if (m[3].matched) f.line = std::stoi(m[3].str());
            f.raw      = lines[i];
            t.frames.push_back(f);
        } else if (t.error.empty()) {
            std::string tr = trim(lines[i]);
            if (!tr.empty()) t.error = tr;
        }
    }
    return t;
}

Trace parse_go(std::string_view text) {
    Trace t;
    t.language = Language::Go;
    auto lines = split_lines(text);
    // Go panics look like:
    //   panic: ...
    //   goroutine 1 [running]:
    //   main.foo(args)
    //           /path/file.go:42 +0xab
    static const std::regex func_re(R"(([\w/.\-]+\.[\w.\-]+)(?:\(.*\))?)");
    static const std::regex loc_re (R"(\s+([^\s:]+):(\d+)( \+0x[0-9a-fA-F]+)?)");
    std::string pending_func;
    for (size_t i = 0; i < lines.size(); ++i) {
        std::smatch m;
        if (lines[i].rfind("panic:", 0) == 0 && t.error.empty()) {
            t.error = lines[i];
            continue;
        }
        if (std::regex_match(lines[i], m, loc_re)) {
            Frame f;
            f.language = Language::Go;
            f.function = pending_func;
            f.file     = m[1].str();
            f.line     = std::stoi(m[2].str());
            f.raw      = lines[i];
            t.frames.push_back(f);
            pending_func.clear();
        } else if (std::regex_search(lines[i], m, func_re)) {
            pending_func = m[1].str();
        }
    }
    return t;
}

Trace parse_rust(std::string_view text) {
    Trace t;
    t.language = Language::Rust;
    auto lines = split_lines(text);
    // Pattern:
    //   thread 'main' panicked at <message>, src/main.rs:42:5
    //     N: <function>
    //                at <path>:<line>
    static const std::regex panic_re(
        R"(thread '[^']+' panicked at ([^,]+), ([^:]+):(\d+):(\d+))");
    static const std::regex func_re (R"(\s+\d+:\s+(.+))");
    static const std::regex at_re   (R"(\s+at\s+([^\s:]+):(\d+)(?::(\d+))?)");
    std::string pending_func;
    for (size_t i = 0; i < lines.size(); ++i) {
        std::smatch m;
        if (std::regex_match(lines[i], m, panic_re)) {
            t.error = trim(m[1].str());
            Frame f;
            f.language = Language::Rust;
            f.file     = m[2].str();
            f.line     = std::stoi(m[3].str());
            f.column   = std::stoi(m[4].str());
            f.function = "panic";
            f.raw      = lines[i];
            t.frames.push_back(f);
            continue;
        }
        if (std::regex_match(lines[i], m, at_re)) {
            Frame f;
            f.language = Language::Rust;
            f.function = pending_func;
            f.file     = m[1].str();
            f.line     = std::stoi(m[2].str());
            if (m[3].matched) f.column = std::stoi(m[3].str());
            f.raw      = lines[i];
            t.frames.push_back(f);
            pending_func.clear();
        } else if (std::regex_match(lines[i], m, func_re)) {
            pending_func = m[1].str();
        }
    }
    return t;
}

Trace parse_cxx(std::string_view text) {
    Trace t;
    t.language = Language::Cxx;
    auto lines = split_lines(text);
    // libbacktrace / addr2line style:
    //   #0  0xdeadbeef in some_function (args) at /path/file.cpp:42
    static const std::regex frame_re(
        R"(#\d+\s+0x[0-9a-fA-F]+\s+in\s+([\w:~<>]+).*at\s+([^:]+):(\d+))");
    for (size_t i = 0; i < lines.size(); ++i) {
        std::smatch m;
        if (std::regex_search(lines[i], m, frame_re)) {
            Frame f;
            f.language = Language::Cxx;
            f.function = m[1].str();
            f.file     = m[2].str();
            f.line     = std::stoi(m[3].str());
            f.raw      = lines[i];
            t.frames.push_back(f);
        } else if (t.error.empty()) {
            std::string tr = trim(lines[i]);
            if (!tr.empty() && tr.find("terminate") != std::string::npos) t.error = tr;
        }
    }
    return t;
}

Trace parse(std::string_view text) {
    std::string s{text};
    if (s.find("Traceback (most recent call last)") != std::string::npos ||
        (s.find("File \"") != std::string::npos && s.find("line ") != std::string::npos))
        return parse_python(text);
    if (s.find("thread '") != std::string::npos && s.find("panicked at") != std::string::npos)
        return parse_rust(text);
    if (s.rfind("panic:", 0) == 0 ||
        s.find("\npanic:") != std::string::npos)
        return parse_go(text);
    if (s.find("\n\tat ") != std::string::npos && s.find("(\n") == std::string::npos)
        return parse_java(text);
    if (s.find("\n    at ") != std::string::npos)
        return parse_nodejs(text);
    if (s.find("#0 ") != std::string::npos || s.find(" in ") != std::string::npos)
        return parse_cxx(text);
    Trace t;
    return t;
}

}
