#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

// Stack trace analyzer: parses traceback / panic output from common
// runtimes into structured frames. The pipeline can then resolve
// frames to source locations and ask the answering specialists for a
// ranked list of likely fixes.
namespace stack_trace {

enum class Language {
    Unknown = 0,
    Python  = 1,
    NodeJs  = 2,
    Java    = 3,
    Go      = 4,
    Rust    = 5,
    Cxx     = 6,    // gcc/clang style "file:line: message" or "at ::function (file:line)"
};
const char * to_string(Language);

struct Frame {
    Language     language = Language::Unknown;
    std::string  file;
    int          line = 0;        // 0 when unknown
    int          column = 0;
    std::string  function;
    std::string  raw;             // the original line text
};

struct Trace {
    Language             language = Language::Unknown;
    std::string          error;            // best-guess error message line
    std::vector<Frame>   frames;
};

void init();
void shutdown();

// Sniff the language by line patterns, then parse.
Trace parse(std::string_view text);

// Language-specific parsers; useful when the caller already knows
// the runtime.
Trace parse_python (std::string_view text);
Trace parse_nodejs (std::string_view text);
Trace parse_java   (std::string_view text);
Trace parse_go     (std::string_view text);
Trace parse_rust   (std::string_view text);
Trace parse_cxx    (std::string_view text);

}
