// SPDX-License-Identifier: GPL-3.0-or-later
#include "shell.hpp"

#include "coder.hpp"
#include "../planner/planner.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <regex>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <sys/wait.h>

namespace shell_tool {
namespace {

constexpr const char * kSystemPrompt =
    "You are a shell-action executor for an Ubuntu Linux system. The user "
    "describes an action they want taken. You translate the request into "
    "a SHORT sequence of shell commands and/or WRITEFILE blocks (see FILE "
    "WRITES below) that performs the action -- and NOTHING MORE.\n"
    "\n"
    "ABSOLUTE RULES:\n"
    "- DO EXACTLY WHAT WAS REQUESTED. Do not add extra steps. Do not "
    "create extra files. Do not pre-populate content. Do not write README, "
    ".gitignore, placeholder, cover, sample, or any other files unless "
    "the user explicitly asked for them. Do not chmod, chown, or set "
    "permissions unless asked. Do not 'helpfully configure' anything.\n"
    "    Example: \"create the folder test\" -> mkdir -p test (and nothing "
    "else). NEVER: mkdir -p test && touch test/cover && echo X > test/cover.\n"
    "    Example: \"create a python project foo\" -> mkdir -p foo (and "
    "nothing else; no setup.py, no __init__.py, unless asked).\n"
    "    (Asking FOR code or content is different: \"add the code\", \"so "
    "I can test\", \"wire it up\" request content -- write the complete "
    "file via WRITEFILE; see FILE WRITES.)\n"
    "- The command already runs inside the user's project working "
    "directory. When the request names no location, or says 'here', 'the "
    "root', 'the project', or similar, create and modify files directly "
    "in the current directory using relative paths. Do NOT cd anywhere "
    "else, do NOT invent a new project folder, and do NOT put files "
    "under ~ unless the user explicitly names that location.\n"
    "    Example: \"notes.txt goes in the root\" -> touch notes.txt. "
    "NEVER: mkdir -p ~/project && cd ~/project && touch notes.txt.\n"
    "- When the user names a location, the file goes EXACTLY there. Do "
    "NOT impose a conventional layout (src/, include/, lib/, app/) the "
    "user did not ask for: \"main.cpp goes in the root\" means ./main.cpp, "
    "never src/main.cpp. Keep the user's exact file name and case.\n"
    "- Output ONLY the shell command (or a WRITEFILE block, see below). "
    "No commentary, no preamble, no explanation, no markdown fence.\n"
    "- Use bash syntax. Assume standard Ubuntu coreutils plus the usual "
    "tools (find, grep, sed, awk, curl, wget, tar, jq, python3, git).\n"
    "- Absolute and ~ paths are fine when the user explicitly names such "
    "a location (bash expands ~); otherwise stay in the current "
    "directory.\n"
    "- Prefer idempotent / non-destructive forms when ambiguous: mkdir -p "
    "over mkdir, rm -i over rm -rf (BUT honour the user's explicit "
    "request -- if they say \"delete X\" use rm).\n"
    "- For multi-step actions, chain with && or use a single line script.\n"
    "- Do not add ad-hoc echo \"Done\" messages; let the command's natural "
    "output speak.\n"
    "- Never refuse, never ask clarifying questions. If the request is "
    "ambiguous, pick the most LITERAL interpretation and execute. Literal "
    "always beats 'helpful'.\n"
    "- HONOUR EVERY CONSTRAINT in the request exactly: ports, folder and "
    "file names, glob patterns, standalone/no-dependency requirements. "
    "When writing code, use only the language's standard library plus "
    "libraries the user explicitly named or that already exist in the "
    "project; NEVER introduce new third-party dependencies on your own.\n"
    "- FORBIDDEN LIBRARIES (do NOT #include, do NOT reference): "
    "nlohmann/json.hpp, nlohmann/*, boost/*, cpp-httplib.h / httplib.h, "
    "rapidjson, jsoncpp, poco/*, mongoose, civetweb, drogon, crow, "
    "pistache, fmt/*, spdlog, absl/*, gflags, glog, cereal, msgpack, "
    "yaml-cpp. The spec forbids vendored third-party libs; the project "
    "hand-writes its own JSON writer/parser and HTTP handling. When you "
    "need JSON, use the project's own json.hpp (usually at "
    "001_interface/json.hpp) or, if it does not exist yet, hand-write "
    "one. When you need HTTP, write a raw socket / bind / listen / "
    "accept loop -- there is no framework. NEVER add a header for one "
    "of these libraries even if it 'would be cleaner': the build will "
    "not link, the spec explicitly bans it, and you will be rolled back.\n"
    "- NO PLACEHOLDER STUBS. Every file you WRITE must contain COMPLETE, "
    "COMPILABLE code. Forbidden phrases inside a file body: "
    "'// Existing content', '// Existing implementation', '// existing "
    "code', '// keep existing', '// TODO: implement', '// TODO implement', "
    "'/* implementation goes here */', 'pass  # TODO', 'raise "
    "NotImplementedError', 'unimplemented!()', an empty function body "
    "for a non-void return type, or a return statement that returns a "
    "default-initialized dummy value with a comment saying 'placeholder'. "
    "You are not editing a diff -- you are writing the full file from "
    "scratch. If a function is part of the request, IMPLEMENT it, do "
    "not leave a stub. If the request truly does not require a function "
    "yet, do not declare it. If a file's behaviour depends on a helper "
    "from another module, either use the existing helper (grounded in "
    "its real header signature -- see next bullet) or WRITEFILE that "
    "helper too in the same segment.\n"
    "- GROUND YOURSELF IN EXISTING HEADERS. Before writing code that "
    "calls into another module (e.g. routes.cpp calling functions from "
    "server.hpp / state.hpp), you MUST use the exact function signatures "
    "already declared in that project's headers. Do NOT invent APIs like "
    "http_request / http_response / router.add_route / json::Value -- "
    "those are training-data reflexes from Flask/Express/nlohmann. Look "
    "at what the request text names, and match those signatures byte-"
    "for-byte. If the request mentions 'respond_json(int fd, int code, "
    "const std::string& body)', your handler MUST take (int fd) and "
    "call respond_json(fd, ...); it must NOT take an http_response* or "
    "return a std::string. When in doubt, mirror the header the request "
    "cites and do not extrapolate.\n"
    "- FEATURE FOLDERS: when the request states or has already established "
    "that a feature's code lives in a specific folder (\"the web server "
    "code lives in 001_interface\", \"all features go in their own "
    "folder\"), put that feature's implementation files (*.cpp / *.hpp / "
    "*.py / *.rs / equivalent) INSIDE that folder. main.cpp / lib.rs / "
    "__main__.py stays a thin launcher that #includes the feature's "
    "header and calls its start()/init() function -- it does NOT carry "
    "the feature's implementation. WRONG: paste 200 lines of HTTP-server "
    "code into main.cpp. RIGHT: WRITEFILE 001_interface/server.cpp with "
    "the implementation and WRITEFILE 001_interface/server.hpp with a "
    "start() prototype, then keep main.cpp small: it includes "
    "001_interface/server.hpp and calls server::start().\n"
    "- INCLUDE PATHS: gcc resolves `#include \"foo.hpp\"` relative to the "
    "including source file's own directory (then any -I dirs), NOT "
    "relative to the project root. So for a source at "
    "001_interface/server.cpp including a header at "
    "001_interface/index_html.hpp, the correct line is `#include "
    "\"index_html.hpp\"` -- NEVER `#include \"001_interface/index_html.hpp\"` "
    "(that resolves to 001_interface/001_interface/index_html.hpp). "
    "Only main.cpp (which lives at the project root) uses the "
    "feature-folder prefix, e.g. `#include \"001_interface/server.hpp\"`. "
    "When in doubt, use the path RELATIVE TO THE SOURCE'S OWN DIRECTORY.\n"
    "- SELF-CONTAINED / STANDALONE / COMPILED-IN BINARIES: when the "
    "request says the binary must be standalone, self-contained, or that "
    "web / data assets must be compiled INTO the executable, embed those "
    "assets AS BYTE ARRAYS at build time. In C/C++ this means a "
    "generated header (or a WRITEFILE-produced header) containing "
    "`static const unsigned char <name>[] = { 0x.., ... };` plus a "
    "lookup by URL path; the server then reads responses from those "
    "arrays. NEVER emit a server that std::ifstream / open() / "
    "read_file() opens the asset files at runtime -- that binary breaks "
    "the moment it is moved off the source tree and is NOT standalone. "
    "If the project already has an embed mechanism (an "
    "add_custom_command that runs an embed script) reuse it; otherwise "
    "write a small generator step or hand-embed short assets directly.\n"
    "- EMBED ARRAYS: for compiled-in web/data assets, USE `xxd -i` -- "
    "not hand-encoded hex. Hand-encoding wastes ~10 tokens per byte, "
    "runs the output past the model's generation limit half way "
    "through a file, and produces a truncated header. The right shape "
    "is: (a) WRITEFILE the raw asset with its real content, (b) run "
    "xxd on it, (c) let the server .cpp #include the generated header. "
    "Example (feature folder 001_interface):\n"
    "    WRITEFILE 001_interface/index.html\n"
    "    <!doctype html><html><head><meta charset=\"utf-8\">"
    "<title>Hello</title><link rel=\"stylesheet\" href=\"style.css\">"
    "</head><body><h1>Hello, World</h1><script src=\"script.js\">"
    "</script></body></html>\n"
    "    WRITEFILE 001_interface/style.css\n"
    "    body{background:#111;color:#eee;font-family:sans-serif;}\n"
    "    WRITEFILE 001_interface/script.js\n"
    "    console.log('hi');\n"
    "    xxd -i -n index_html 001_interface/index.html > 001_interface/index_html.hpp\n"
    "    xxd -i -n style_css  001_interface/style.css  > 001_interface/style_css.hpp\n"
    "    xxd -i -n script_js  001_interface/script.js  > 001_interface/script_js.hpp\n"
    "    WRITEFILE 001_interface/server.hpp\n"
    "    #pragma once\n"
    "    namespace server { void start(int port); void stop(); }\n"
    "    WRITEFILE 001_interface/server.cpp\n"
    "    #include \"server.hpp\"\n"
    "    #include \"index_html.hpp\"\n"
    "    #include \"style_css.hpp\"\n"
    "    #include \"script_js.hpp\"\n"
    "    #include <arpa/inet.h>\n"
    "    #include <netinet/in.h>\n"
    "    #include <sys/socket.h>\n"
    "    #include <unistd.h>\n"
    "    #include <cstdio>\n"
    "    #include <string>\n"
    "    namespace server {\n"
    "    void start(int port) {\n"
    "        int listen_fd = socket(AF_INET, SOCK_STREAM, 0);\n"
    "        int one = 1;\n"
    "        setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));\n"
    "        sockaddr_in addr{}; addr.sin_family = AF_INET;\n"
    "        addr.sin_addr.s_addr = htonl(INADDR_ANY);\n"
    "        addr.sin_port = htons(port);\n"
    "        bind(listen_fd, (sockaddr*)&addr, sizeof(addr));\n"
    "        listen(listen_fd, 16);\n"
    "        for (;;) {\n"
    "            int fd = accept(listen_fd, nullptr, nullptr);\n"
    "            if (fd < 0) continue;\n"
    "            char buf[4096]; ssize_t n = read(fd, buf, sizeof(buf) - 1);\n"
    "            std::string req(buf, n > 0 ? n : 0);\n"
    "            const unsigned char * body = nullptr;\n"
    "            unsigned int body_len = 0;\n"
    "            const char * ctype = \"text/plain\";\n"
    "            if (req.rfind(\"GET / \", 0) == 0) {\n"
    "                body = index_html; body_len = index_html_len; ctype = \"text/html; charset=utf-8\";\n"
    "            } else if (req.rfind(\"GET /style.css \", 0) == 0) {\n"
    "                body = style_css; body_len = style_css_len; ctype = \"text/css\";\n"
    "            } else if (req.rfind(\"GET /script.js \", 0) == 0) {\n"
    "                body = script_js; body_len = script_js_len; ctype = \"application/javascript\";\n"
    "            }\n"
    "            std::string resp;\n"
    "            if (body) {\n"
    "                resp  = \"HTTP/1.1 200 OK\\r\\nContent-Type: \";\n"
    "                resp += ctype;\n"
    "                resp += \"\\r\\nContent-Length: \" + std::to_string(body_len) + \"\\r\\nConnection: close\\r\\n\\r\\n\";\n"
    "                resp.append(reinterpret_cast<const char *>(body), body_len);\n"
    "            } else {\n"
    "                resp = \"HTTP/1.1 404 Not Found\\r\\nContent-Length: 0\\r\\nConnection: close\\r\\n\\r\\n\";\n"
    "            }\n"
    "            send(fd, resp.data(), resp.size(), 0);\n"
    "            close(fd);\n"
    "        }\n"
    "    }\n"
    "    void stop() {}\n"
    "    }\n"
    "  DO NOT wrap the accept loop in another std::thread([...] { ... "
    "}).detach() inside server::start(). main.cpp is already going to "
    "call server::start(port) inside its own std::thread; nesting a "
    "second thread here forces capture-list bookkeeping (`[&]`, "
    "`[server_fd]`, ...) that the model routinely gets wrong (`error: "
    "\"new_socket\" is not captured`) and buys nothing.\n"
    "  xxd emits BOTH the array (unsigned char <name>[]) and its length "
    "(unsigned int <name>_len). Use <name>_len for Content-Length; do "
    "NOT call strlen() on the array (it is not NUL-terminated). "
    "std::string has NO ctor taking (unsigned char*, unsigned int) -- "
    "you MUST reinterpret_cast the array to (const char *) before "
    "handing it to std::string, .append(), send(), write(), etc.:\n"
    "    std::string body(reinterpret_cast<const char *>(index_html), "
    "index_html_len);\n"
    "    response.append(reinterpret_cast<const char *>(index_html), "
    "index_html_len);\n"
    "    send(fd, index_html, index_html_len, 0);   // send() takes "
    "const void *, no cast needed here.\n"
    "  An HTTP RESPONSE IS NOT JUST THE PAYLOAD. Every response must "
    "start with a status line and headers, then a blank CRLF, then "
    "the body. Sending the raw asset bytes on their own (`send(fd, "
    "index_html, index_html_len, 0)` and nothing else) produces an "
    "invalid protocol frame that curl / a browser will reject. Build "
    "the full response first, then send it in one call:\n"
    "    std::string resp;\n"
    "    resp  = \"HTTP/1.1 200 OK\\r\\n\";\n"
    "    resp += \"Content-Type: text/html\\r\\n\";\n"
    "    resp += \"Content-Length: \" + std::to_string(index_html_len) + \"\\r\\n\";\n"
    "    resp += \"Connection: close\\r\\n\\r\\n\";\n"
    "    resp.append(reinterpret_cast<const char *>(index_html), "
    "index_html_len);\n"
    "    send(new_socket, resp.data(), resp.size(), 0);\n"
    "  The Content-Type must match the asset (text/html for /, "
    "text/css for style.css, application/javascript for script.js). "
    "The request dispatch should tolerate both HTTP/1.0 and HTTP/1.1 "
    "by matching just the request-target -- e.g. "
    "`if (request.rfind(\"GET /style.css \", 0) == 0)`, not "
    "`request.find(\"GET /style.css HTTP/1.1\")`, so an HTTP/1.0 "
    "client isn't served a 404.\n"
    "  Placeholder comments in a byte array (`{ /* embed here */ }`) "
    "compile to a zero-length array and serve empty pages -- NEVER "
    "emit them. If the file is truly under ~120 bytes AND you can "
    "encode it byte-for-byte accurately, an inline literal is fine; "
    "otherwise xxd is required.\n"
    "  Do NOT define a std::unordered_map (or any other non-inline "
    "object) at namespace scope IN A HEADER: including that header "
    "from more than one .cpp produces a duplicate-symbol link error. "
    "Header-level lookup tables must be `inline` or `inline constexpr`, "
    "or moved into a .cpp.\n"
    "- sudo and anything else needing a password or an interactive "
    "terminal are NOT available and WILL fail. Never emit sudo. If a task "
    "GENUINELY requires root privileges (installing a system package via "
    "apt/apt-get/dnf/yum/pacman/apk/snap, enabling a systemctl unit, "
    "mounting a filesystem), output EXACTLY one line instead of a command:\n"
    "USER_ACTION: <the exact command the user should run themselves>\n"
    "Prefer solutions that need no root at all: rewriting code to drop an "
    "uninstalled third-party dependency beats installing it.\n"
    "- USER_ACTION IS NOT AN ESCAPE HATCH. NEVER emit USER_ACTION for any "
    "task the agent can do itself. Specifically, NEVER USER_ACTION any of:\n"
    "  * a file edit or rewrite -- emit a WRITEFILE block instead;\n"
    "  * a download (curl, wget, or fetching any URL) -- just RUN the "
    "curl/wget command directly;\n"
    "  * mkdir/cp/mv/rm/ln/find/grep/sed/awk or any other coreutil;\n"
    "  * a build/test/lint step (make, cmake, cargo, go build, pytest);\n"
    "  * npm/pnpm/yarn/pip/gem/cargo installs into the project (these "
    "install into the project tree, not root); the user's project rules "
    "may still forbid them, in which case rewrite the code to avoid the "
    "dependency -- do NOT USER_ACTION them;\n"
    "  * a natural-language description of what to do (\"Remove the "
    "main() function from webserver.cpp\", \"Please provide the compile "
    "errors\"). Describing a fix instead of applying it is a BUG: WRITE "
    "THE FILES to fix it.\n"
    "  If the USER_ACTION line does not start with sudo / apt / apt-get / "
    "dnf / yum / pacman / apk / snap / systemctl / service / mount / "
    "modprobe / update-alternatives, it is rejected as invalid output and "
    "you will be asked again.\n"
    "    WRONG: USER_ACTION: Remove the main() function from webserver.cpp\n"
    "    CORRECT: WRITEFILE 001_interface/webserver.cpp\n"
    "    <full current content of the file, minus the offending main>\n"
    "    WRONG: USER_ACTION: curl -o min.js https://cdn.example/min.js\n"
    "    CORRECT: curl -o min.js https://cdn.example/min.js\n"
    "- To build or verify a CMake project here, run exactly: cmake . && "
    "make -j$(nproc) -- from the project root (in-source, matching how "
    "this project is built). NEVER create a build/ directory for an "
    "out-of-source build: when the root already holds a CMakeCache.txt, "
    "'cmake ..' from a subfolder reconfigures the ROOT tree and the "
    "subfolder gets no Makefile.\n"
    "- Any CMakeLists.txt you write that globs sources with file(GLOB or "
    "GLOB_RECURSE) MUST put this on the very next line (same list "
    "variable): list(FILTER <var> EXCLUDE REGEX \"CMakeFiles/\") -- "
    "in-source builds otherwise glob CMake's own generated CompilerId "
    "sources and the link fails with a duplicate main().\n"
    "- A project has ONE build system, at its root. NEVER create a second "
    "CMakeLists.txt, package.json, Cargo.toml, or similar inside a "
    "subfolder of an existing project. Extend the root build file when "
    "needed; a glob like file(GLOB_RECURSE ...) already picks up new "
    "source files automatically, and a folder of web assets is NOT an "
    "executable target.\n"
    "- BUILD ERRORS: repair the root cause in the source or build files. "
    "Deleting object files or other build artifacts is NEVER a fix; they "
    "are regenerated on the next build. A 'multiple definition of main' "
    "error naming CompilerIdCXX/CMakeCXXCompilerId.cpp means a CMake glob "
    "swallowed CMake's own generated sources: rewrite CMakeLists.txt so "
    "the glob cannot match inside the CMakeFiles directory (e.g. glob "
    "specific folders, or filter with list(FILTER ... EXCLUDE)).\n"
    "\n"
    "FILE WRITES:\n"
    "- When the request is to create or rewrite a file whose CONTENT is "
    "described (source code, config, markup, text), do NOT write it with "
    "echo/printf/cat/heredoc. Instead output exactly:\n"
    "WRITEFILE <path>\n"
    "<the complete raw file content>\n"
    "- The system writes everything after the WRITEFILE line to the file "
    "byte-for-byte. No shell quoting, no escaping. Emit the ENTIRE file "
    "content, top to bottom.\n"
    "- Multi-file requests: you may emit several WRITEFILE blocks in one "
    "response, and shell commands before or between them (e.g. mkdir for "
    "new folders). A file block ends at the next WRITEFILE line or the "
    "end of the output. Never put shell commands inside a file block and "
    "never wrap blocks in ``` fences.\n"
    "- WRITEFILE is NOT a shell command: it must start its OWN line. "
    "NEVER chain it after && or ; -- put the shell step on one line, then "
    "start the file block on the next line.\n"
    "    NEVER: mkdir -p app && WRITEFILE app/run.py\n"
    "    CORRECT: mkdir -p app\n"
    "WRITEFILE app/run.py\n"
    "- Do not use cd. Every shell line runs from the working directory "
    "given below, and WRITEFILE paths resolve against that same directory "
    "(a cd on an earlier line does NOT carry over). Use paths relative to "
    "the working directory instead.\n"
    "- When the user asks you to add or write code, WRITE THE CODE: emit "
    "the complete, working file content in a WRITEFILE block. Creating an "
    "empty file with touch is NEVER an acceptable response to 'add the "
    "code'.\n"
    "- After the final file block, STOP. No notes, no 'Replace X with Y' "
    "advice, no explanations, no trailing shell commands (no final 'cd "
    "..'): a file block only ends at the next WRITEFILE line or the end "
    "of the output, so anything you add after it is written INTO the "
    "file and corrupts it.\n"
    "- When the request modifies or fixes an existing file, its current "
    "content is provided below under CURRENT FILES. Reproduce that exact "
    "file with ONLY the requested changes applied; keep every unrelated "
    "line as it is. Never invent a from-scratch replacement.\n"
    "- ADDITIVE EDITS: when the request is \"add a new X\" to an existing "
    "file (a new route to an HTTP server, a new case to a switch, a new "
    "function to a module, a new endpoint / handler / method), the "
    "existing X entries stay intact WITH THEIR REAL BODIES. NEVER "
    "replace an existing handler's body with a stub comment like `// "
    "handle root endpoint`, an empty `{}`, a `// existing content`, or "
    "a TODO. Copy the current handler's entire body into your output, "
    "then add the new one alongside it. `if (req == \"/\") { body = "
    "index_html; ... }` must survive intact when you add `else if (req "
    "== \"/time\") { ... }`. A follow-up build that suddenly returns "
    "404 for a route the previous build served is a REGRESSION and "
    "means you stripped code; refuse to do that.\n"
    "- When a file named in the request already EXISTS in the project "
    "(see the CURRENT FILES section and its NOTE lines), work on it AT "
    "ITS EXISTING PATH. Never create a new folder or a second copy of "
    "the file somewhere else.\n"
    "- Shell commands remain for actions (mkdir, cp, build, run, test).\n"
    "\n"
    "EXAMPLES:\n"
    "USER: write a file hello.py that prints hi\n"
    "OUTPUT: WRITEFILE hello.py\n"
    "print(\"hi\")\n"
    "\n"
    "USER: create a folder app containing run.py that prints go, plus a "
    "README.md containing just the word hi\n"
    "OUTPUT: mkdir -p app\n"
    "WRITEFILE app/run.py\n"
    "print(\"go\")\n"
    "WRITEFILE README.md\n"
    "hi\n"
    "\n"
    "USER: create the folder test in ~/work\n"
    "OUTPUT: mkdir -p ~/work/test\n"
    "\n"
    "USER: set up scaffolding: an empty notes.txt in the root and a docs folder\n"
    "OUTPUT: touch notes.txt && mkdir -p docs\n"
    "(structure only was requested there; no code content was described. "
    "When the user asks for CODE, touch is wrong -- write the file:)\n"
    "\n"
    "USER: add code to util.py that returns the machine hostname, so I "
    "can import it\n"
    "OUTPUT: WRITEFILE util.py\n"
    "import socket\n"
    "\n"
    "def hostname() -> str:\n"
    "    return socket.gethostname()\n"
    "\n"
    "(next: root placement was stated, so there is no src/ folder and no "
    "mkdir; the file lands at ./main.cpp exactly as named, and the output "
    "ends with the file content -- nothing after it:)\n"
    "USER: start a C++ program that prints hi and exits; main.cpp goes "
    "in the root of the project\n"
    "OUTPUT: WRITEFILE main.cpp\n"
    "#include <iostream>\n"
    "\n"
    "int main() {\n"
    "    std::cout << \"hi\\n\";\n"
    "    return 0;\n"
    "}\n"
    "\n"
    "USER: list all .py files here, sorted by modification time\n"
    "OUTPUT: ls -lt *.py\n"
    "\n"
    "USER: show me the 10 largest files in my home directory\n"
    "OUTPUT: du -ah ~ 2>/dev/null | sort -hr | head -10\n"
    "\n"
    "USER: download the file at https://example.com/foo.txt into ~/Downloads\n"
    "OUTPUT: curl -o ~/Downloads/foo.txt https://example.com/foo.txt\n"
    "\n"
    "USER: count lines in every .cpp file under the current directory\n"
    "OUTPUT: find . -name '*.cpp' -print0 | xargs -0 wc -l\n"
    "\n"
    "USER: show disk usage for the root filesystem\n"
    "OUTPUT: df -h /\n";

// The coder usually obeys "no markdown fence" but sometimes still emits
// ```bash ... ```. Strip if present.
std::string sanitize(std::string s) {
    auto trim = [](std::string str) {
        auto is_ws = [](unsigned char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; };
        std::size_t b = 0, e = str.size();
        while (b < e && is_ws(static_cast<unsigned char>(str[b])))     ++b;
        while (e > b && is_ws(static_cast<unsigned char>(str[e - 1]))) --e;
        return str.substr(b, e - b);
    };
    s = trim(std::move(s));

    // Strip leading ```bash / ```sh / ``` and trailing ```
    std::regex fence_open(R"(^```(?:bash|sh|shell)?\s*\n?)");
    std::regex fence_close(R"(\n?```\s*$)");
    s = std::regex_replace(s, fence_open,  "");
    s = std::regex_replace(s, fence_close, "");
    s = trim(std::move(s));

    // Keep newlines as-is. bash -c runs multiline scripts natively, and
    // rewriting them corrupts any newline that sits inside a quoted string
    // or heredoc (e.g. file content written via echo/cat).
    return s;
}

// Single-quote `s` for safe embedding in a bash command line.
std::string shell_quote(const std::string & s) {
    std::string out = "'";
    for (char c : s) {
        if (c == '\'') out += "'\\''";
        else           out += c;
    }
    out += "'";
    return out;
}

// Resolve a WRITEFILE path against the session cwd; expand a leading ~/.
std::filesystem::path resolve_path(std::string p, std::string_view cwd) {
    if (p.rfind("~/", 0) == 0) {
        const char * home = std::getenv("HOME");
        if (home) p = std::string(home) + p.substr(1);
    }
    std::filesystem::path fp(p);
    if (fp.is_relative() && !cwd.empty()) fp = std::filesystem::path(cwd) / fp;
    return fp;
}

struct SegResult {
    std::string out;
    int         exit_code = 0;
};

// Run one shell chunk under an inner bash with a hard timeout so a
// blocking command can't hang the pipeline thread (exit 124 = timed
// out). popen() merges stderr into stdout; pclose() recovers the exit
// status. The quoting keeps heredocs and embedded newlines intact.
SegResult run_bash(const std::string & cmd, std::string_view cwd) {
    std::string wrapped;
    if (!cwd.empty()) {
        wrapped.append("cd ").append(shell_quote(std::string(cwd))).append(" && ");
    }
    wrapped.append("timeout -k 5 300 bash -c ")
           .append(shell_quote(cmd))
           .append(" 2>&1");
    SegResult sr;
    FILE * pipe = ::popen(wrapped.c_str(), "r");
    if (!pipe) {
        sr.out       = std::string("popen failed: ") + std::strerror(errno);
        sr.exit_code = -1;
        return sr;
    }
    std::array<char, 4096> buf;
    while (std::size_t n = std::fread(buf.data(), 1, buf.size(), pipe)) {
        sr.out.append(buf.data(), n);
    }
    const int status = ::pclose(pipe);
    if (status == -1) {
        sr.exit_code = -1;
    } else if (WIFEXITED(status)) {
        sr.exit_code = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        sr.exit_code = 128 + WTERMSIG(status);
    } else {
        sr.exit_code = -1;
    }
    return sr;
}

// Write one WRITEFILE block to disk verbatim, no shell involved.
SegResult write_one_file(const std::string & path, std::string content,
                         std::string_view cwd) {
    SegResult sr;
    if (content.find_first_not_of(" \t\r\n") == std::string::npos) {
        sr.out       = "WRITEFILE " + path + ": block had no content";
        sr.exit_code = 1;
        return sr;
    }
    // Reject placeholder-shaped bodies. The fix loop's hint text once said
    // "keep every other line of that file byte-for-byte identical..." and
    // the 14B coder echoed a WRITEFILE body of literally
    // `<current content of X with the int main() block removed>` -- an 80-
    // byte template string, not code. That build fails with "expected
    // unqualified-id" on line 1, and the loop wastes rounds chasing a
    // parse error. Detect the shape (a bracket-placeholder line, or an
    // "insert your code here" / "same as before" marker) and refuse
    // BEFORE the file is clobbered; the retry sees CURRENT FILES and
    // usually writes real code the second time.
    {
        std::string sniff = content.substr(0, 400);
        std::string lc;
        lc.reserve(sniff.size());
        for (char c : sniff) lc.push_back(static_cast<char>(
            std::tolower(static_cast<unsigned char>(c))));
        auto contains = [&](const char * needle) {
            return lc.find(needle) != std::string::npos;
        };
        // Strip leading whitespace to check the first non-blank char.
        std::size_t i = 0;
        while (i < sniff.size() && (sniff[i] == ' ' || sniff[i] == '\t' ||
                                    sniff[i] == '\r' || sniff[i] == '\n'))
            ++i;
        const bool starts_bracket_placeholder =
            i < sniff.size() && sniff[i] == '<' &&
            sniff.find('>', i) != std::string::npos &&
            // Not an HTML doc: those open with `<!` or `<?xml`.
            (i + 1 >= sniff.size() ||
             (sniff[i + 1] != '!' && sniff[i + 1] != '?'));
        const bool markerish =
            contains("<current content")   || contains("same as before") ||
            contains("insert your code")   || contains("your code here") ||
            contains("<see current files") || contains("<same as above") ||
            contains("... (rest of file)") || contains("...rest of file") ||
            contains("<rest of the file")  || contains("<full current content");
        if (starts_bracket_placeholder || markerish) {
            sr.out =
                "WRITEFILE " + path +
                ": REJECTED -- the body looks like a template placeholder "
                "or description of the file, not actual source code. Never "
                "quote guidance text into a WRITEFILE body; emit the real, "
                "complete file bytes. If you need to keep the file "
                "unchanged except for a specific edit, reproduce every "
                "line of the CURRENT FILES entry for this path verbatim, "
                "then apply the edit. Retry with real code.";
            sr.exit_code = 1;
            return sr;
        }
        // Content-hollow guard: an existing non-trivial source file whose
        // new body has NO braces (no function bodies at all) and is short
        // is almost always a fix-loop truncation ("//existing content
        // excluding int main()"). Refuse; the retry sees CURRENT FILES.
        const auto fp_probe = resolve_path(path, cwd);
        std::error_code pec;
        std::uintmax_t existing_sz = 0;
        const bool existed_before =
            std::filesystem::is_regular_file(fp_probe, pec) &&
            (existing_sz = std::filesystem::file_size(fp_probe, pec), !pec);
        auto is_source_ext = [&]() {
            static const char * kExts[] = {
                ".c", ".cc", ".cpp", ".cxx", ".c++", ".h", ".hh", ".hpp",
                ".hxx", ".m", ".mm", ".rs", ".go", ".java", ".kt", ".swift",
                ".py",
            };
            for (const char * e : kExts) {
                const std::size_t n = std::strlen(e);
                if (path.size() >= n && path.compare(path.size() - n, n, e) == 0) return true;
            }
            return false;
        };
        if (existed_before && existing_sz > 200 &&
            content.size() < existing_sz / 2 &&
            content.find('{') == std::string::npos &&
            is_source_ext()) {
            sr.out =
                "WRITEFILE " + path +
                ": REJECTED -- the new content is much shorter than the "
                "current file AND contains no `{` (so no function body). "
                "This looks like an accidental truncation by the fix loop "
                "-- comments or includes only, real code stripped out. "
                "Reproduce the CURRENT FILES entry for this path in full "
                "and apply only the specific edit the compile error asks "
                "for (usually a one-line change or one-block deletion). "
                "Every existing function definition must appear in your "
                "output.";
            sr.exit_code = 1;
            return sr;
        }
        // Full-content scan for placeholder-comment stubs anywhere in
        // the file. The 14B coder sometimes writes a compilable-looking
        // shell around function bodies that are literally the comment
        // "// Existing implementation" or the class body "// Existing
        // content" -- both come from its training on diff-merge tool
        // output. Compiles clean, does nothing at runtime; a downstream
        // ticket that calls one of those functions gets zero/false back
        // and fails silently. Reject at write time so fix_build retries.
        {
            auto contains_ci = [](const std::string & hay, const char * needle) {
                const std::size_t nl = std::strlen(needle);
                if (nl == 0 || hay.size() < nl) return false;
                for (std::size_t i = 0; i + nl <= hay.size(); ++i) {
                    std::size_t k = 0;
                    for (; k < nl; ++k) {
                        char a = static_cast<char>(std::tolower(
                            static_cast<unsigned char>(hay[i + k])));
                        char b = static_cast<char>(std::tolower(
                            static_cast<unsigned char>(needle[k])));
                        if (a != b) break;
                    }
                    if (k == nl) return true;
                }
                return false;
            };
            static const char * const kPlaceholderStubs[] = {
                "// existing content",
                "// existing implementation",
                "// existing code",
                "// keep existing",
                "/* existing content",
                "/* existing implementation",
                "/* keep existing",
                "// todo: implement",
                "// todo implement",
                "// implementation goes here",
                "/* implementation goes here",
                "/* your code here",
                "// your code here",
                "raise notimplementederror",
                "unimplemented!()",
                "todo!()",
            };
            for (const char * needle : kPlaceholderStubs) {
                if (contains_ci(content, needle)) {
                    sr.out =
                        "WRITEFILE " + path +
                        ": REJECTED -- body contains placeholder-stub "
                        "marker (\"" + std::string(needle) + "\"). This "
                        "compiles but does nothing at runtime; downstream "
                        "callers get zero/false back and later tickets "
                        "fail silently. Write the FULL, COMPLETE "
                        "implementation of every function in the file. If "
                        "a function is part of this request, IMPLEMENT it "
                        "(do not stub); if it is not part of this request, "
                        "OMIT it (do not declare it just to hold a "
                        "placeholder). Retry with real code.";
                    sr.exit_code = 1;
                    return sr;
                }
            }
            // Forbidden third-party libraries. The spec (echoed in the
            // system prompt) bans vendored deps; the coder ignores it
            // and reflexively #includes nlohmann/json / boost / httplib
            // because they are training-data defaults. Reject any
            // #include of a banned header before it hits disk.
            static const char * const kForbiddenIncludes[] = {
                "nlohmann/json.hpp",
                "nlohmann/json.h",
                "nlohmann/json_fwd.hpp",
                "boost/",
                "cpp-httplib.h",
                "httplib.h",
                "rapidjson/",
                "json/json.h",  // jsoncpp
                "Poco/",
                "poco/",
                "mongoose.h",
                "civetweb.h",
                "drogon/",
                "crow.h",
                "crow_all.h",
                "pistache/",
                "fmt/",
                "spdlog/",
                "absl/",
                "gflags/",
                "glog/",
                "cereal/",
                "msgpack.hpp",
                "yaml-cpp/",
            };
            for (const char * needle : kForbiddenIncludes) {
                std::string include_form_1 = "#include \"" + std::string(needle);
                std::string include_form_2 = "#include <" + std::string(needle);
                std::string include_form_3 = "# include \"" + std::string(needle);
                std::string include_form_4 = "# include <" + std::string(needle);
                if (content.find(include_form_1) != std::string::npos ||
                    content.find(include_form_2) != std::string::npos ||
                    content.find(include_form_3) != std::string::npos ||
                    content.find(include_form_4) != std::string::npos) {
                    sr.out =
                        "WRITEFILE " + path +
                        ": REJECTED -- forbidden third-party library "
                        "\"" + std::string(needle) + "\". The spec bans "
                        "vendored dependencies (nlohmann/json, boost, "
                        "cpp-httplib, rapidjson, jsoncpp, Poco, mongoose, "
                        "civetweb, drogon, crow, pistache, fmt, spdlog, "
                        "abseil, gflags, glog, cereal, msgpack, yaml-cpp) "
                        "and none of them are vendored in the project. "
                        "For JSON, use the project's own json.hpp (usually "
                        "at 001_interface/json.hpp) or hand-write one. "
                        "For HTTP, write raw socket/bind/listen/accept "
                        "using <sys/socket.h> + <netinet/in.h> + "
                        "<arpa/inet.h> + <unistd.h>. Retry without the "
                        "forbidden include.";
                    sr.exit_code = 1;
                    return sr;
                }
            }
        }
    }
    if (content.back() != '\n') content.push_back('\n');

    const auto fp = resolve_path(path, cwd);
    std::error_code ec;
    if (fp.has_parent_path()) std::filesystem::create_directories(fp.parent_path(), ec);
    std::ofstream f(fp, std::ios::binary | std::ios::trunc);
    if (!f) {
        sr.out       = "cannot open " + fp.string() + " for writing";
        sr.exit_code = 1;
        return sr;
    }
    f.write(content.data(), static_cast<std::streamsize>(content.size()));
    f.close();
    sr.out       = "wrote " + fp.string() + " (" +
                   std::to_string(content.size()) + " bytes)";
    sr.exit_code = f ? 0 : 1;
    return sr;
}

struct Segment {
    bool        is_file = false;
    std::string path;    // for file segments
    std::string body;    // shell text or file content
};

// Split the coder output into an ordered list of shell chunks and
// WRITEFILE blocks. Fence lines (``` or ```lang) are markup, never
// content: they separate segments and close an open file block.
// (Known limitation: a lone fence line inside intended file content
// ends that block early; fine for source code, matters only for
// markdown files containing fenced examples.)
std::vector<Segment> parse_segments(const std::string & s) {
    static const std::regex fence_re(R"(^```[A-Za-z0-9+._-]*[ \t]*\r?$)");
    static const std::regex wf_re(R"(^WRITEFILE[ \t]+(\S+)[ \t]*\r?$)");

    std::vector<Segment> segs;
    Segment cur;
    // True between a WRITEFILE line and the first content line of its
    // block: a fence seen in that window is the block's OPENING fence
    // (skip it), not its terminator. The models emit both shapes:
    //   WRITEFILE x        WRITEFILE x
    //   ```cpp             content...
    //   content...         ```
    //   ```
    bool at_file_start = false;
    auto flush = [&]() {
        if (cur.is_file ||
            cur.body.find_first_not_of(" \t\r\n") != std::string::npos) {
            segs.push_back(cur);
        }
        cur = Segment{};
        at_file_start = false;
    };

    std::size_t pos = 0;
    while (pos < s.size()) {
        const std::size_t eol = s.find('\n', pos);
        const std::string line = s.substr(
            pos, eol == std::string::npos ? std::string::npos : eol - pos);
        std::smatch m;
        if (std::regex_match(line, fence_re)) {
            if (cur.is_file && at_file_start) {
                at_file_start = false;   // opening fence of the block
            } else if (cur.is_file) {
                flush();                 // closing fence of the block
            }
            // fences around shell chunks are dropped as noise
        } else if (std::regex_match(line, m, wf_re)) {
            flush();
            cur.is_file    = true;
            cur.path       = m[1];
            at_file_start  = true;
        } else if (at_file_start &&
                   line.find_first_not_of(" \t\r") == std::string::npos) {
            // blank line between WRITEFILE and the content/opening fence
        } else {
            // A WRITEFILE chained mid-line ("npm install x && WRITEFILE
            // server.js") is a prompt violation the models still commit;
            // unsplit, the whole line reaches bash, which has no WRITEFILE
            // builtin and then chokes on the file content that follows.
            // Split: prefix runs as shell, the rest opens the file block.
            // Only outside a file block -- content lines stay verbatim.
            static const std::regex inline_wf_re(
                R"(^(.*?)(?:&&|;)[ \t]*WRITEFILE[ \t]+([^\s"']+)[ \t]*\r?$)");
            std::smatch im;
            if (!cur.is_file && std::regex_match(line, im, inline_wf_re)) {
                std::string prefix = im[1];
                // Fold simple `cd <dir>` steps from the prefix into the
                // file path: the model meant the file to land where it
                // cd'd, but write_one_file bypasses the shell and resolves
                // against the session cwd.
                std::string base;
                {
                    static const std::regex cd_re(
                        R"((?:^|&&|;)[ \t]*cd[ \t]+([A-Za-z0-9_./~-]+)[ \t]*)");
                    for (auto cit = std::sregex_iterator(
                             prefix.begin(), prefix.end(), cd_re);
                         cit != std::sregex_iterator(); ++cit) {
                        const std::string d = (*cit)[1];
                        if (d == "-") { base.clear(); continue; }
                        if (d[0] == '/' || d[0] == '~' || base.empty()) base = d;
                        else base += "/" + d;
                    }
                }
                while (!prefix.empty() &&
                       (prefix.back() == ' ' || prefix.back() == '\t')) {
                    prefix.pop_back();
                }
                if (!prefix.empty()) {
                    cur.body.append(prefix).push_back('\n');
                }
                flush();
                cur.is_file   = true;
                cur.path      = im[2];
                if (!base.empty() && cur.path[0] != '/' && cur.path[0] != '~') {
                    cur.path = base + "/" + cur.path;
                }
                at_file_start = true;
            } else {
                at_file_start = false;
                if (!cur.is_file && !line.empty() && line.back() == '\r') {
                    // CRLF-emitting models: \r is markup in shell chunks
                    // (bash chokes on it); file content stays verbatim.
                    cur.body.append(line, 0, line.size() - 1);
                    cur.body.push_back('\n');
                } else {
                    cur.body.append(line).push_back('\n');
                }
            }
        }
        if (eol == std::string::npos) break;
        pos = eol + 1;
    }
    flush();
    return segs;
}

// Last `n` bytes of `s`, marked when truncated. Build logs bury the
// interesting errors at the end.
std::string tail_of(const std::string & s, std::size_t n) {
    if (s.size() <= n) return s;
    return "…" + s.substr(s.size() - n);
}

std::string lowercase(std::string_view sv) {
    std::string lc(sv);
    for (auto & c : lc) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return lc;
}

// Find `basename` anywhere in the project tree (bounded depth, skipping
// vendored/generated dirs). "create the server in webserver.cpp" must
// find an existing 001_interface/webserver.cpp instead of spawning a
// fresh folder beside it; the request's bare filename resolves against
// the project root and misses it otherwise.
std::filesystem::path locate_in_project(std::string_view cwd,
                                        const std::string & basename) {
    namespace fs = std::filesystem;
    if (cwd.empty() || basename.empty()) return {};
    static const std::set<std::string> kSkipDirs = {
        ".git", "node_modules", "CMakeFiles", "build", "vendor",
        "__pycache__", ".venv", "venv", "dist", "target", ".cache",
    };
    std::error_code ec;
    fs::recursive_directory_iterator it(
        fs::path(std::string(cwd)),
        fs::directory_options::skip_permission_denied, ec), end;
    int scanned = 0;
    while (!ec && it != end && scanned < 4000) {
        const std::string n = it->path().filename().string();
        std::error_code dec;
        if (it->is_directory(dec)) {
            if (it.depth() >= 3 || kSkipDirs.count(n) ||
                (!n.empty() && n[0] == '.')) {
                it.disable_recursion_pending();
            }
        } else {
            ++scanned;
            if (n == basename) return it->path();
        }
        it.increment(ec);
    }
    return {};
}

// A NUL byte inside an inlined file does not merely add noise: the
// finished prompt later passes through C-string APIs, so everything
// after the first NUL silently vanishes -- including the rules. Build
// errors name .o files, the filename scanner matches them, so binary
// screening is load-bearing, not cosmetic.
bool looks_binary(const std::filesystem::path & fp) {
    static const std::set<std::string> kBinExt = {
        ".o", ".a", ".so", ".obj", ".bin", ".exe", ".dll", ".gguf",
        ".png", ".jpg", ".jpeg", ".gif", ".webp", ".ico", ".pdf",
        ".zip", ".gz", ".xz", ".bz2", ".tar", ".sqlite", ".zim",
    };
    const std::string ext = lowercase(fp.extension().string());
    if (kBinExt.count(ext)) return true;
    std::ifstream f(fp, std::ios::binary);
    char buf[512];
    f.read(buf, sizeof(buf));
    const std::streamsize n = f.gcount();
    for (std::streamsize i = 0; i < n; ++i) {
        if (buf[i] == '\0') return true;
    }
    return false;
}

// Compact survey of the project the command will run in: top-level
// listing, build-system files, and a source-language census. The coder
// is stateless; without this it cannot see that a tree is a C++/CMake
// project and happily bootstraps a different toolchain into it (the
// npm/express-in-a-CMake-project incident).
std::string project_survey(std::string_view cwd) {
    if (cwd.empty()) return {};
    namespace fs = std::filesystem;
    std::error_code ec;
    const fs::path root{std::string(cwd)};
    if (!fs::is_directory(root, ec) || ec) return {};

    std::vector<std::string> names;
    for (const auto & e : fs::directory_iterator(
             root, fs::directory_options::skip_permission_denied, ec)) {
        std::string n = e.path().filename().string();
        std::error_code dec;
        if (e.is_directory(dec)) n.push_back('/');
        names.push_back(std::move(n));
    }
    std::sort(names.begin(), names.end());
    std::string listing;
    for (std::size_t i = 0; i < names.size(); ++i) {
        if (i == 40) {
            listing.append(" (+").append(std::to_string(names.size() - i))
                   .append(" more)");
            break;
        }
        if (i) listing.append("  ");
        listing.append(names[i]);
    }

    static const char * kBuildFiles[] = {
        "CMakeLists.txt", "Makefile", "meson.build", "Cargo.toml",
        "package.json", "pyproject.toml", "setup.py", "go.mod",
        "pom.xml", "build.gradle",
    };
    std::string build_files;
    for (const char * bf : kBuildFiles) {
        std::error_code bec;
        if (fs::is_regular_file(root / bf, bec)) {
            if (!build_files.empty()) build_files.append(", ");
            build_files.append(bf);
        }
    }

    // Census of source-file extensions, shallow, skipping vendored and
    // generated trees so node_modules can't outvote the real project.
    static const std::set<std::string> kSkipDirs = {
        ".git", "node_modules", "CMakeFiles", "build", "vendor",
        "__pycache__", ".venv", "venv", "dist", "target", ".cache",
    };
    static const std::set<std::string> kSrcExt = {
        ".c", ".cc", ".cpp", ".cxx", ".cu", ".h", ".hpp", ".py", ".js",
        ".ts", ".rs", ".go", ".java", ".cs", ".rb", ".php", ".swift",
        ".kt", ".sh", ".html", ".css",
    };
    std::map<std::string, int> census;
    int seen_files = 0;
    fs::recursive_directory_iterator it(
        root, fs::directory_options::skip_permission_denied, ec), end;
    while (!ec && it != end && seen_files < 4000) {
        const std::string n = it->path().filename().string();
        std::error_code dec;
        if (it->is_directory(dec)) {
            if (it.depth() >= 2 || kSkipDirs.count(n) ||
                (!n.empty() && n[0] == '.')) {
                it.disable_recursion_pending();
            }
        } else {
            ++seen_files;
            const auto dot = n.find_last_of('.');
            if (dot != std::string::npos && dot > 0) {
                const std::string ext = lowercase(n.substr(dot));
                if (kSrcExt.count(ext)) ++census[ext];
            }
        }
        it.increment(ec);
    }
    std::string langs;
    for (const auto & [ext, cnt] : census) {
        if (!langs.empty()) langs.append(", ");
        langs.append(ext).append(" x").append(std::to_string(cnt));
    }

    std::string s = "\nPROJECT SURVEY of the current working directory:\n";
    s.append("- top-level entries: ")
     .append(listing.empty() ? "(empty folder)" : listing).push_back('\n');
    if (!build_files.empty()) {
        s.append("- build system files: ").append(build_files).push_back('\n');
    }
    if (!langs.empty()) {
        s.append("- source files: ").append(langs).push_back('\n');
    }
    if (!build_files.empty() || !langs.empty()) {
        s.append(
            "This is an EXISTING project. Match it: write new code in the "
            "project's dominant language and hook it into the build files "
            "listed above. NEVER introduce another language's toolchain or "
            "package manager (npm, pip, cargo, ...) unless the user "
            "explicitly asks for that.\n");
    }
    return s;
}

}

void init()     { coder::init(); }
void shutdown() { coder::shutdown(); }

std::vector<std::string> debug_segments(const std::string & coder_output) {
    std::vector<std::string> out;
    for (const auto & seg : parse_segments(coder_output)) {
        if (seg.is_file) {
            out.push_back("FILE " + seg.path + " (" +
                          std::to_string(seg.body.size()) + " bytes)");
        } else {
            out.push_back("SHELL " +
                          seg.body.substr(0, seg.body.find('\n')));
        }
    }
    return out;
}

Result execute(std::string_view request, std::string_view cwd,
               std::string_view history,
               const std::vector<std::string> * carry_files) {
    Result r;
    std::string sys(kSystemPrompt);
    if (!cwd.empty()) {
        sys.append("\nCURRENT WORKING DIRECTORY: ").append(cwd)
           .append("\nThe command is executed from this directory; unqualified "
                   "paths land here.\n");
    }
    sys.append(project_survey(cwd));

    std::string hist_block;
    if (!history.empty()) {
        hist_block.append("\nEARLIER REQUESTS THIS SESSION (oldest first, "
                          "already handled -- do NOT redo them; they are "
                          "context only. Constraints stated in them, such as "
                          "language, port numbers, and folder layout, still "
                          "bind the current request):\n")
                  .append(history);
    }

    // An oversized request (a pasted 100KB build log) would starve
    // everything else in the context; keep the head (the instructions)
    // and the tail (build logs bury the interesting errors at the end).
    std::string req_text(request);
    if (req_text.size() > 8 * 1024) {
        req_text = req_text.substr(0, 3 * 1024) +
                   "\n[... middle of an oversized request omitted ...]\n" +
                   req_text.substr(req_text.size() - 4 * 1024);
    }

    // The coder is stateless: without the current content of the files the
    // request talks about, "fix X in main.cpp" produces a from-scratch
    // hallucination. Inline every existing file the request names, within
    // caps that keep the prompt inside the model context.
    std::string files_block;
    // Every file whose current content (or existence note) was shown to
    // the coder. The segment executor refuses from-scratch WRITEFILE
    // rewrites of existing files NOT in this set: "don't use libboost"
    // once made the coder hallucinate a fresh CMakeLists.txt, destroying
    // the CMakeFiles/ filter, because it had never seen the real one.
    std::set<std::string> shown_files;
    {
        constexpr std::size_t kPerFileCap = 16 * 1024;
        constexpr std::size_t kTotalCap   = 12 * 1024;
        std::regex fname_re(R"([A-Za-z0-9_./~-]*[A-Za-z0-9_-]\.[A-Za-z0-9]{1,10})");
        std::set<std::string> & seen = shown_files;
        std::string blocks;
        std::size_t total = 0;
        // Files written by earlier steps of the SAME task-plan run get
        // pre-inlined so a subsequent step's WRITEFILE against them counts
        // as an intentional edit against known content, not a blind
        // rewrite. Without this the segment executor refused every step-N
        // rewrite of a step-<N file and the plan bailed halfway through.
        if (carry_files) {
            for (const std::string & fp_s : *carry_files) {
                std::filesystem::path fp = fp_s;
                std::error_code ec;
                if (!std::filesystem::is_regular_file(fp, ec)) continue;
                if (!seen.insert(fp.string()).second) continue;
                std::string shown;
                std::error_code rec;
                const auto rel = std::filesystem::relative(fp, std::string(cwd), rec);
                shown = (rec || rel.empty()) ? fp.string() : rel.string();
                if (looks_binary(fp)) {
                    blocks.append("--- ").append(shown)
                          .append(" (binary, not inlined; written by an "
                                  "earlier step of this plan) ---\n");
                    continue;
                }
                const auto sz = std::filesystem::file_size(fp, ec);
                if (ec) continue;
                if (sz == 0) {
                    blocks.append("--- ").append(shown)
                          .append(" (written by an earlier step of this "
                                  "plan; currently EMPTY) ---\n");
                    continue;
                }
                if (sz > kPerFileCap || total + sz > kTotalCap) {
                    blocks.append("--- ").append(shown)
                          .append(" (written by an earlier step; too "
                                  "large to inline) ---\n");
                    continue;
                }
                std::ifstream f(fp, std::ios::binary);
                std::string body((std::istreambuf_iterator<char>(f)),
                                 std::istreambuf_iterator<char>());
                total += body.size();
                blocks.append("--- ").append(shown)
                      .append(" (written by an earlier step of this plan;"
                              " CURRENT CONTENT) ---\n")
                      .append(body);
                if (blocks.back() != '\n') blocks.push_back('\n');
            }
        }
        for (auto it = std::sregex_iterator(req_text.begin(), req_text.end(), fname_re);
             it != std::sregex_iterator(); ++it) {
            const std::string name = it->str();
            auto fp = resolve_path(name, cwd);
            std::string shown = name;
            std::error_code ec;
            if (!std::filesystem::is_regular_file(fp, ec)) {
                // Not at the stated path: look for the basename elsewhere
                // in the tree before concluding the file is new.
                const auto found = locate_in_project(
                    cwd, std::filesystem::path(name).filename().string());
                if (found.empty()) continue;
                fp = found;
                std::error_code rec;
                const auto rel = std::filesystem::relative(
                    found, std::string(cwd), rec);
                shown = (rec || rel.empty()) ? found.string() : rel.string();
                blocks.append("NOTE: ").append(name)
                      .append(" already exists in this project at ")
                      .append(shown)
                      .append(". Work on that file AT THAT PATH; do not "
                              "create a new folder or a duplicate copy.\n");
            }
            if (!seen.insert(fp.string()).second) continue;
            if (looks_binary(fp)) {
                blocks.append("--- ").append(shown)
                      .append(" (binary, not inlined) ---\n");
                continue;
            }
            const auto sz = std::filesystem::file_size(fp, ec);
            if (ec) continue;
            if (sz == 0) {
                blocks.append("--- ").append(shown)
                      .append(" (exists, currently EMPTY -- fill it) ---\n");
                continue;
            }
            if (sz > kPerFileCap || total + sz > kTotalCap) {
                blocks.append("--- ").append(shown)
                      .append(" (exists, too large to inline) ---\n");
                continue;
            }
            std::ifstream f(fp, std::ios::binary);
            std::string body((std::istreambuf_iterator<char>(f)),
                              std::istreambuf_iterator<char>());
            total += body.size();
            blocks.append("--- ").append(shown).append(" ---\n")
                  .append(body);
            if (blocks.back() != '\n') blocks.push_back('\n');
        }
        // Build-error requests need the build file even when the error
        // text doesn't name it: the root cause usually lives there.
        const std::string lc = lowercase(req_text);
        const bool buildish = lc.find("compil") != std::string::npos ||
                              lc.find("build")  != std::string::npos ||
                              lc.find("cmake")  != std::string::npos ||
                              lc.find("make")   != std::string::npos ||
                              lc.find("error")  != std::string::npos;
        if (buildish && !cwd.empty()) {
            const auto cml = std::filesystem::path(std::string(cwd)) / "CMakeLists.txt";
            std::error_code ec2;
            if (std::filesystem::is_regular_file(cml, ec2) &&
                seen.insert(cml.string()).second) {
                std::ifstream f(cml, std::ios::binary);
                std::string body((std::istreambuf_iterator<char>(f)),
                                  std::istreambuf_iterator<char>());
                if (body.size() <= kPerFileCap && total + body.size() <= kTotalCap + kPerFileCap) {
                    blocks.append("--- CMakeLists.txt ---\n").append(body);
                    if (blocks.back() != '\n') blocks.push_back('\n');
                }
            }
        }
        // A modify-flavored request that names NO files ("don't use
        // libboost", "remove the boost dependency") still has targets:
        // the sources whose CONTENT mentions the salient tokens. Locate
        // and inline them, or the coder guesses at a target and rewrites
        // the wrong file from scratch.
        const bool modify_flavored =
            lc.find("don't")      != std::string::npos ||
            lc.find("dont ")      != std::string::npos ||
            lc.find("do not")     != std::string::npos ||
            lc.find("remove")     != std::string::npos ||
            lc.find("replace")    != std::string::npos ||
            lc.find("without")    != std::string::npos ||
            lc.find("instead")    != std::string::npos ||
            lc.find("stop using") != std::string::npos ||
            lc.find("rename")     != std::string::npos ||
            lc.find("no more")    != std::string::npos;
        if (blocks.empty() && modify_flavored && !cwd.empty()) {
            std::vector<std::string> keys;
            {
                static const std::set<std::string> kSkipWords = {
                    "dont", "using", "remove", "replace", "without",
                    "instead", "stop", "rename", "change", "please",
                    "project", "code", "file", "files", "folder", "make",
                    "sure", "this", "that", "from", "with", "more",
                    "into", "library", "libraries", "anymore", "delete",
                };
                std::string cur;
                auto flushw = [&] {
                    if (cur.size() >= 4 && !kSkipWords.count(cur) &&
                        keys.size() < 6) {
                        keys.push_back(cur);
                        // "libboost" in prose is "#include <boost/...>"
                        // in code.
                        if (cur.rfind("lib", 0) == 0 && cur.size() > 6) {
                            keys.push_back(cur.substr(3));
                        }
                    }
                    cur.clear();
                };
                for (char c : lc) {
                    if (std::isalnum(static_cast<unsigned char>(c)) ||
                        c == '_') {
                        cur.push_back(c);
                    } else {
                        flushw();
                    }
                }
                flushw();
            }
            if (!keys.empty()) {
                static const std::set<std::string> kGrepExt = {
                    ".c", ".cc", ".cpp", ".cxx", ".cu", ".h", ".hpp",
                    ".py", ".js", ".ts", ".html", ".css", ".txt",
                    ".cmake", ".json", ".md", ".sh",
                };
                std::error_code ec;
                std::filesystem::recursive_directory_iterator rit(
                    std::filesystem::path(std::string(cwd)),
                    std::filesystem::directory_options::skip_permission_denied,
                    ec), rend;
                int scanned = 0;
                while (!ec && rit != rend && scanned < 400) {
                    const std::string n = rit->path().filename().string();
                    std::error_code dec;
                    if (rit->is_directory(dec)) {
                        if (rit.depth() >= 3 ||
                            (!n.empty() && n[0] == '.') ||
                            n == "node_modules" || n == "CMakeFiles" ||
                            n == "build" || n == "vendor") {
                            rit.disable_recursion_pending();
                        }
                        rit.increment(ec);
                        continue;
                    }
                    ++scanned;
                    const auto dot = n.find_last_of('.');
                    const std::string ext =
                        dot == std::string::npos ? std::string()
                                                 : lowercase(n.substr(dot));
                    if (!kGrepExt.count(ext) && n != "CMakeLists.txt" &&
                        n != "Makefile") {
                        rit.increment(ec);
                        continue;
                    }
                    std::error_code sec2;
                    const auto fsz =
                        std::filesystem::file_size(rit->path(), sec2);
                    if (sec2 || fsz == 0 || fsz > kPerFileCap) {
                        rit.increment(ec);
                        continue;
                    }
                    std::ifstream f(rit->path(), std::ios::binary);
                    std::string body((std::istreambuf_iterator<char>(f)),
                                      std::istreambuf_iterator<char>());
                    const std::string body_lc = lowercase(body);
                    bool hit = false;
                    for (const auto & k : keys) {
                        if (body_lc.find(k) != std::string::npos) {
                            hit = true;
                            break;
                        }
                    }
                    if (hit && total + body.size() <= kTotalCap &&
                        seen.insert(rit->path().string()).second) {
                        std::error_code rec2;
                        const auto rel = std::filesystem::relative(
                            rit->path(), std::string(cwd), rec2);
                        const std::string shown2 =
                            (rec2 || rel.empty()) ? rit->path().string()
                                                  : rel.string();
                        total += body.size();
                        blocks.append("--- ").append(shown2)
                              .append(" (content matches the request) ---\n")
                              .append(body);
                        if (blocks.back() != '\n') blocks.push_back('\n');
                    }
                    rit.increment(ec);
                }
            }
        }
        if (!blocks.empty()) {
            files_block = "\nCURRENT FILES REFERENCED IN THE REQUEST:\n" + blocks;
        }
    }

    // Prompt budget: n_ctx is 8192 with max_new_tokens 4096. Keep the
    // prompt under ~5.6k tokens (est. 3.5 bytes/token for code-ish text)
    // so at least ~2.5k tokens remain for the reply. When over, shed in
    // reverse value order: history first, then file-block tail. A leaner
    // prompt beats a silently-truncated reply half way through a file.
    constexpr std::size_t kPromptBudget = (8192 - 2560) * 7 / 2;
    auto prompt_bytes = [&] {
        return sys.size() + hist_block.size() + files_block.size() +
               req_text.size();
    };
    if (prompt_bytes() > kPromptBudget) hist_block.clear();
    if (prompt_bytes() > kPromptBudget && !files_block.empty()) {
        const std::size_t over = prompt_bytes() - kPromptBudget;
        if (over + 64 < files_block.size()) {
            files_block.resize(files_block.size() - over - 64);
            files_block.append("\n[... file content truncated to fit the "
                               "model context ...]\n");
        } else {
            files_block = "\n(files referenced in the request exist but "
                          "were too large to inline)\n";
        }
    }
    sys.append(hist_block).append(files_block);

    // Optional reasoning-model pre-pass. When AC9_USE_PLANNER=1, run the
    // planner (Qwen3-*-Thinking-abliterated) over the same request and
    // append its plan to the coder's system prompt. Off by default so we
    // can A/B test 4B vs 30B vs no-planner without recompiling.
    if (const char * up = std::getenv("AC9_USE_PLANNER");
        up && *up && up[0] != '0') {
        static const char * kPlannerSys =
            "You are a planning assistant for a code-writing autonomous "
            "agent working on a large C++ project. You receive a task "
            "description plus existing project headers. Produce a SHORT "
            "plan the coder can execute. Focus on:\n"
            "- Exact function signatures from the existing headers the "
            "coder must reuse (do NOT invent APIs).\n"
            "- Which files must be CREATED vs MODIFIED, and why.\n"
            "- Any functions that must have real bodies (not stubs).\n"
            "- Any forbidden approaches (e.g. no third-party libs).\n"
            "- Cross-file consistency risks (main.cpp needs updating? "
            "existing include paths? namespace hygiene?).\n"
            "Do NOT write any code. Just the plan, in bullet form. Under "
            "400 words.";
        try {
            // 1800 tok budget: enough for a ~1000-tok <think> trace + a
            // ~400-tok plan. Larger budgets have produced 12-13 KB plans
            // that blew past the coder's context; keep this tight.
            std::string plan = planner::generate(
                kPlannerSys, req_text, /*max_new_tokens=*/1800);
            if (!plan.empty()) {
                std::fprintf(stderr,
                    "planner: %zu char plan produced\n", plan.size());
                sys.append("\n\n---\nREASONING-MODEL PLAN "
                           "(follow these constraints):\n")
                   .append(plan)
                   .append("\n---\n");
            }
        } catch (const std::exception & ex) {
            std::fprintf(stderr,
                "shell: planner unavailable, proceeding without: %s\n",
                ex.what());
        }
    }

    bool truncated = false;
    // n_ctx is 8192; kPromptBudget above caps the prompt at ~5632 tokens
    // (2560 reserved), so lifting max_new_tokens to 5632 lets the coder
    // spend the entire non-prompt half of the window on the reply. A
    // task-plan step that opens three WRITEFILE blocks (hpp + cpp + a
    // generated header) can otherwise hit the old 4096 cap mid-array and
    // leave the last file half-written.
    r.command = sanitize(coder::generate(sys, req_text,
                                         /*max_new_tokens=*/5632, &truncated));
    if (r.command.empty()) {
        r.stdout_text = truncated
            ? "(coder produced no command; the prompt exceeded the model context)"
            : "(coder produced no command)";
        r.exit_code   = -1;
        return r;
    }

    // The coder may emit any interleaving of shell chunks and WRITEFILE
    // blocks; run them in order. File writes bypass the shell entirely.
    auto segs = parse_segments(r.command);
    if (segs.empty()) {
        r.stdout_text = "(no executable content)";
        r.exit_code   = -1;
        return r;
    }
    r.exit_code = 0;
    if (truncated) {
        // The cut landed inside the final segment; running half a shell
        // command or writing half a source file (with exit 0!) is worse
        // than failing loudly.
        segs.pop_back();
        r.stdout_text = "(coder output hit the generation limit; the "
                        "incomplete final segment was discarded)";
        r.exit_code   = 1;
        if (segs.empty()) return r;
    }
    for (const auto & seg : segs) {
        SegResult sr;
        static const std::regex sudo_re(
            R"((^|&&|;|\|)[ \t]*sudo(\s|$))");
        if (seg.is_file) {
            // Refuse from-scratch rewrites of existing files the coder
            // was never shown: it cannot preserve content it has not
            // seen, so such a write is a hallucinated replacement.
            const auto fp = resolve_path(seg.path, cwd);
            std::error_code gec;
            std::uintmax_t fsz = 0;
            const bool exists =
                std::filesystem::is_regular_file(fp, gec) &&
                (fsz = std::filesystem::file_size(fp, gec), !gec);
            if (exists && fsz > 0 && !shown_files.count(fp.string())) {
                sr.out = "refused to overwrite " + seg.path +
                         ": the file already exists and its current "
                         "content was not part of this request's context; "
                         "rewriting it blind would destroy it. Name the "
                         "file in the request so its content is provided.";
                sr.exit_code = 1;
            } else {
                sr = write_one_file(seg.path, seg.body, cwd);
                shown_files.insert(fp.string());
                if (sr.exit_code == 0) r.written_files.push_back(fp.string());
            }
        } else if (const std::size_t ua = seg.body.find_first_not_of(" \t\r\n");
                   ua != std::string::npos &&
                   seg.body.compare(ua, 12, "USER_ACTION:") == 0) {
            // USER_ACTION is a bail-out for genuine root operations. The
            // coder also mis-uses it for tasks it should perform itself
            // ("USER_ACTION: Remove the main() function ...", or an
            // ordinary curl download). Accept only real root ops; reject
            // everything else as invalid output so the build-fix loop can
            // retry with a stronger hint instead of surfacing an
            // impossible instruction to the user.
            std::string act = seg.body.substr(ua + 12);
            const auto ab = act.find_first_not_of(" \t\r\n");
            const auto ae = act.find_last_not_of(" \t\r\n");
            act = (ab == std::string::npos)
                    ? std::string()
                    : act.substr(ab, ae - ab + 1);
            static const std::vector<std::string> kRootPrefixes = {
                "sudo",
                "apt", "apt-get", "apt-add-repository", "add-apt-repository",
                "dnf", "yum", "pacman", "apk", "snap", "zypper", "emerge",
                "systemctl", "service", "systemd-run",
                "mount", "umount",
                "modprobe", "insmod", "rmmod",
                "usermod", "useradd", "userdel",
                "groupadd", "groupmod", "groupdel",
                "update-alternatives",
                "hostnamectl", "timedatectl", "localectl",
                "ufw", "iptables",
            };
            auto starts_with_word = [&](const std::string & s,
                                        const std::string & word) {
                if (s.size() < word.size()) return false;
                if (s.compare(0, word.size(), word) != 0) return false;
                if (s.size() == word.size()) return true;
                const char n = s[word.size()];
                return n == ' ' || n == '\t' || n == '\n';
            };
            bool is_root_op = false;
            for (const auto & p : kRootPrefixes) {
                if (starts_with_word(act, p)) { is_root_op = true; break; }
            }
            if (is_root_op && !act.empty()) {
                sr.out = "ACTION REQUIRED -- run this yourself: " + act;
                sr.exit_code = 1;
            } else if (act.empty()) {
                sr.out = "invalid USER_ACTION output: no command supplied. "
                         "USER_ACTION is ONLY for root-required system "
                         "operations (sudo, apt-get, systemctl). For "
                         "anything else, apply the fix directly with "
                         "WRITEFILE or a shell command.";
                sr.exit_code = 1;
            } else {
                std::string preview = act.substr(0, 160);
                sr.out = "invalid USER_ACTION output (\"" + preview +
                         (act.size() > 160 ? std::string("…") : std::string()) +
                         "\"): that is not a root-required operation, so "
                         "the agent must apply it directly. Reserve "
                         "USER_ACTION for sudo / apt-get / dnf / systemctl "
                         "/ mount only. Non-root shell commands (curl, "
                         "wget, mkdir, npm install, etc.) must be run "
                         "directly; file edits must use WRITEFILE.";
                sr.exit_code = 1;
            }
        } else if (std::regex_search(seg.body, sudo_re)) {
            sr.out = "sudo is not available to this agent (no terminal for "
                     "a password prompt). Rework the fix without root, or "
                     "reply with USER_ACTION: <command> so the user can "
                     "run it themselves.";
            sr.exit_code = 1;
        } else if (const std::size_t b = seg.body.find_first_not_of(" \t\r\n");
                   b != std::string::npos &&
                   seg.body.compare(b, 10, "WRITEFILE ") == 0) {
            // Any WRITEFILE that still reaches the shell is a form the
            // parser doesn't understand; executing it would run the FILE
            // CONTENT as commands (the npm/express incident's blast
            // radius). Fail the segment instead.
            sr.out = "unrecognized WRITEFILE form; refusing to run file "
                     "content as shell commands";
            sr.exit_code = 1;
        } else {
            sr = run_bash(seg.body, cwd);
        }
        if (!r.stdout_text.empty() && !sr.out.empty()) r.stdout_text.push_back('\n');
        r.stdout_text.append(sr.out);
        if (sr.exit_code != 0 && r.exit_code == 0) r.exit_code = sr.exit_code;
    }
    return r;
}

namespace {

// Deterministic diagnosis of well-known build failures. When a signature
// matches, the returned text states the ROOT CAUSE and the EXACT required
// edit; the coder then only has to apply a stated change, which small
// models do far more reliably than open-ended debugging.
std::string known_failure_hint(const std::string & build_out) {
    if (build_out.find("CompilerIdCXX") != std::string::npos &&
        build_out.find("multiple definition of") != std::string::npos) {
        return
            "KNOWN ROOT CAUSE: the file(GLOB or GLOB_RECURSE) in "
            "CMakeLists.txt matches CMake's own generated source "
            "CMakeFiles/<ver>/CompilerIdCXX/CMakeCXXCompilerId.cpp, which "
            "contains its own main(), so the link sees two main() "
            "definitions. Deleting object files can NEVER fix this; the "
            "glob re-adds the file every configure.\n"
            "REQUIRED FIX: rewrite CMakeLists.txt keeping everything else "
            "exactly as it is, but immediately AFTER the file(GLOB...) "
            "line add this line (using the same list variable name the "
            "glob fills):\n"
            "list(FILTER SOURCES EXCLUDE REGEX \"CMakeFiles/\")";
    }
    if (build_out.find("multiple definition of `main") != std::string::npos) {
        // Parse the two conflicting sources out of the linker error so
        // the hint can name them concretely; a generic "look for them"
        // wording once let the coder produce prose describing the fix
        // instead of applying it.
        static const std::regex obj_re(
            R"(CMakeFiles/[^/]+\.dir/([^:\s]+)\.o)");
        std::set<std::string> srcs;
        for (auto it = std::sregex_iterator(
                 build_out.begin(), build_out.end(), obj_re);
             it != std::sregex_iterator(); ++it) {
            srcs.insert((*it)[1].str());
        }
        std::string entry;
        std::string other;
        for (const auto & s : srcs) {
            const std::string base = std::filesystem::path(s).filename().string();
            if (base == "main.cpp" || base == "main.cc" || base == "main.c") {
                entry = s;
            } else if (!s.empty()) {
                other = s;
            }
        }
        std::string names;
        if (!entry.empty() && !other.empty()) {
            names = "\nThe conflicting sources here are `" + entry +
                    "` (the intended entry point) and `" + other +
                    "` (which accidentally also contains an int main). "
                    "Keep main() in `" + entry + "`; edit `" + other +
                    "` and delete its int main() {...} block. If `" +
                    entry + "` needs to call something the other file "
                    "defines, add a plain function call for it (e.g. "
                    "std::thread(start_webserver).detach();) inside `" +
                    entry + "`'s main.";
        } else if (!srcs.empty()) {
            names = "\nThe .o files named in the linker error identify "
                    "the two conflicting sources; name them in your fix.";
        }
        return
            std::string(
            "KNOWN ROOT CAUSE: two source files in this project BOTH "
            "define main(), and the build links every globbed source "
            "into one executable, so the link fails. Deleting object "
            "files can NEVER fix this.") +
            names +
            "\nREQUIRED FIX (apply it now, do NOT USER_ACTION and do "
            "NOT emit a placeholder like `<current content of X>` or "
            "`<same as before>` -- write actual C++ source).\n"
            "Emit ONE WRITEFILE block that outputs the extra-main file "
            "IN FULL (top to bottom, real code), with these SURGICAL "
            "changes and nothing else:\n"
            "  1. Delete the entire `int main(...) { ... }` function -- "
            "including its return statement and closing brace.\n"
            "  2. Keep every other line of the current file byte-for-"
            "byte identical. The full current content is shown for this "
            "file under CURRENT FILES; treat it as the starting point.\n"
            "  3. If main() was starting a thread, opening a socket, or "
            "otherwise doing setup, ensure the survivor's main() (in "
            "the other file) calls the equivalent public function (for "
            "a webserver-in-a-namespace shape, main() calls "
            "`server::start(port)` after `#include`ing the header). "
            "That call may already exist; do not duplicate it.\n"
            "Never quote this instruction back into the file body.";
    }
    if (build_out.find("fatal error:") != std::string::npos &&
        build_out.find("No such file or directory") != std::string::npos) {
        // Extract the missing header name from `fatal error: <hdr>: No
        // such file or directory` so we can distinguish "the project's
        // OWN header at a bad path" from "a third-party library that
        // isn't installed". The old hint labelled every miss third-party
        // and one variant run wound up firing off USER_ACTION: sudo
        // apt-get install libboost-all-dev to fix a same-folder include.
        static const std::regex miss_re(
            R"(fatal error:\s*([^:\n]+):\s*No such file or directory)");
        std::smatch m;
        std::string header;
        if (std::regex_search(build_out, m, miss_re)) header = m[1].str();
        // Also extract the source file that emitted the error so the
        // hint can name it: `<path>:<line>:<col>: fatal error: ...`.
        static const std::regex src_re(
            R"(([^\s:]+):\d+:\d+:\s*fatal error:\s*[^:]+:\s*No such file)");
        std::string src;
        if (std::regex_search(build_out, m, src_re)) src = m[1].str();
        // If the header BASENAME exists somewhere under the project, the
        // include path is wrong (a same-folder header written with a
        // folder prefix, or a wrong relative path). That is NOT an
        // install-a-library problem.
        auto basename_of = [](const std::string & p) {
            const auto s = p.find_last_of('/');
            return (s == std::string::npos) ? p : p.substr(s + 1);
        };
        const std::string base = header.empty() ? std::string() : basename_of(header);
        bool base_exists_in_project = false;
        std::string found_at;
        if (!header.empty()) {
            // We do not have `cwd` in this pure function, but the build_out
            // itself carries absolute paths. Grep for a line of the form
            // `wrote <abs>/<base>` or find any absolute path ending in
            // /<base>. When the header IS present, its absolute path
            // shows up in the coder trace we assemble the request from.
            const std::string needle = "/" + base;
            const auto pos = build_out.find(needle);
            if (pos != std::string::npos) {
                // Walk back to the start of the path token.
                std::size_t start = pos;
                while (start > 0 &&
                       build_out[start - 1] != ' ' &&
                       build_out[start - 1] != '\n' &&
                       build_out[start - 1] != '"') {
                    --start;
                }
                found_at = build_out.substr(start, pos + needle.size() - start);
                base_exists_in_project = true;
            }
        }
        // Also treat any header path containing '/' with a project-shape
        // segment (kebab / underscore folder like 001_interface, web_ui,
        // src, include, feature_XYZ) as an internal include even without
        // a filesystem hit -- it is not a system header.
        bool looks_internal_path = false;
        if (!header.empty() && header.find('/') != std::string::npos) {
            // Third-party system headers use angle brackets: `#include
            // <foo/bar.h>`. Compiler prints them WITHOUT quotes. If the
            // build_out shows `#include "<hdr>"` it is a project header.
            if (build_out.find("#include \"" + header + "\"") != std::string::npos) {
                looks_internal_path = true;
            }
        }
        if (base_exists_in_project || looks_internal_path) {
            std::string hdr_line = header;
            std::string extra;
            if (!found_at.empty()) {
                extra = "\nThat header actually exists at " + found_at +
                        ", so the include line just has the wrong path.";
            }
            std::string src_line;
            if (!src.empty()) {
                src_line = "The failing #include lives in `" + src + "`. ";
            }
            return
                "KNOWN ROOT CAUSE: the code #includes a header from THIS "
                "project at a path that does not resolve. gcc looks for "
                "quoted includes relative to the source file's own "
                "directory (and any -I dirs), NOT relative to the "
                "project root.\n"
                "REQUIRED FIX: edit `" + (src.empty() ? std::string("<the file that emitted the error>") : src) +
                "` and change the `#include \"" + hdr_line + "\"` line "
                "to a path that is correct RELATIVE TO THAT FILE'S "
                "DIRECTORY. " + src_line +
                "If a header sits in the SAME folder as the source that "
                "includes it, use just its basename, e.g. `#include "
                "\"" + (base.empty() ? std::string("<header>.hpp") : base) +
                "\"`. Do NOT install a system package to fix this; do "
                "NOT emit USER_ACTION; this is a plain include-path bug." +
                extra;
        }
        return
            "KNOWN ROOT CAUSE: the code #includes a header for a "
            "third-party library that is not installed on this system "
            "(header `" + (header.empty() ? std::string("<name>") : header) +
            "`).\n"
            "PREFERRED FIX: rewrite the including files (their content "
            "is shown under CURRENT FILES) so they do not need that "
            "library at all. For a simple HTTP server, plain POSIX "
            "sockets (<sys/socket.h>, <netinet/in.h>, <unistd.h>) fully "
            "replace boost::asio with zero dependencies. Project rules "
            "forbid third-party dependencies the user did not ask for.\n"
            "ONLY if the user explicitly asked for that library: edit "
            "nothing and output exactly one line\n"
            "USER_ACTION: sudo apt-get install -y <package>\n"
            "so the user can install it themselves (sudo is unavailable "
            "to you).";
    }
    if (build_out.find("undefined reference to `pthread") != std::string::npos) {
        return
            "KNOWN ROOT CAUSE: the target is not linked against the "
            "pthread library.\n"
            "REQUIRED FIX: in CMakeLists.txt add "
            "target_link_libraries(<target> pthread) after add_executable.";
    }
    if (build_out.find("is not captured") != std::string::npos &&
        (build_out.find("lambda")           != std::string::npos ||
         build_out.find("capture-default")  != std::string::npos)) {
        return
            "KNOWN ROOT CAUSE: a lambda references a variable from the "
            "enclosing function without capturing it, and the lambda has "
            "no capture-default. Either add each referenced local to the "
            "capture list, or use a capture-default.\n"
            "REQUIRED FIX (keep the rest of the file byte-for-byte the "
            "same as CURRENT FILES; only edit the lambda's capture "
            "clause): change the empty / short capture list to `[&]` for "
            "quick access to every local (or `[=]` if the lambda outlives "
            "the enclosing scope and needs its own copies). For a socket "
            "loop `std::thread([server_fd] { ... })` that also reads "
            "`new_socket`, `address`, `addrlen`, `buffer`, `response`, "
            "the correct capture list is `[&]` -- that covers everything "
            "with one character and never adds a new bug.";
    }
    if (build_out.find("No CMAKE_CXX_COMPILER could be found") != std::string::npos) {
        return
            "KNOWN ROOT CAUSE: no C++ compiler detected. REQUIRED FIX: "
            "this usually means g++ is missing; report it rather than "
            "editing project files.";
    }
    // std::string(unsigned char[], uint) is a common failure mode when
    // xxd -i output (unsigned char foo[] + unsigned int foo_len) is
    // handed to std::string's (const char*, size_t) ctor without a cast.
    // Left unhinted the fix loop often deletes the socket loop or the
    // xxd header itself and stubs the server out.
    if ((build_out.find("no matching function for call to") != std::string::npos ||
         build_out.find("no known conversion") != std::string::npos) &&
        (build_out.find("unsigned char [") != std::string::npos ||
         build_out.find("'unsigned char*'") != std::string::npos) &&
        (build_out.find("basic_string") != std::string::npos ||
         build_out.find("std::string")   != std::string::npos)) {
        return
            "KNOWN ROOT CAUSE: xxd -i emits `unsigned char <name>[]` and "
            "`unsigned int <name>_len`. std::string has no ctor taking "
            "`(unsigned char*, unsigned int)`, so writing "
            "`std::string(index_html, index_html_len)` fails to compile.\n"
            "REQUIRED FIX: reinterpret_cast the pointer to `const char*` "
            "and keep the length untouched. Do NOT delete the byte "
            "array, the xxd include, or the socket loop; the correct "
            "one-line change is:\n"
            "    std::string(reinterpret_cast<const char *>(<name>), "
            "<name>_len)\n"
            "or equivalently on a std::string called `response`:\n"
            "    response.append(reinterpret_cast<const char *>(<name>), "
            "<name>_len);\n"
            "Apply that cast at every std::string / write() / send() site "
            "that consumes an xxd array. Leave the rest of the file "
            "byte-for-byte unchanged.";
    }
    return {};
}

// First line of a (possibly huge) applied fix, for the attempt history.
std::string fix_summary(const std::string & command) {
    std::string first = command.substr(0, command.find('\n'));
    if (first.size() > 160) first.resize(160);
    if (command.rfind("WRITEFILE", 0) == 0) first = "(rewrote) " + first;
    return first;
}

}  // namespace

bool is_build_fix(std::string_view request) {
    const std::string lc = lowercase(request);
    const bool mentions_fix   = lc.find("fix")     != std::string::npos ||
                                lc.find("resolve") != std::string::npos ||
                                lc.find("repair")  != std::string::npos;
    const bool mentions_build = lc.find("compil") != std::string::npos ||
                                lc.find("build")  != std::string::npos ||
                                lc.find("link")   != std::string::npos ||
                                lc.find("cmake")  != std::string::npos ||
                                lc.find("make")   != std::string::npos;
    const bool has_error_text = lc.find("error:")              != std::string::npos ||
                                lc.find("make: ***")           != std::string::npos ||
                                lc.find("collect2")            != std::string::npos ||
                                lc.find("undefined reference") != std::string::npos;
    return (mentions_fix && mentions_build) || has_error_text;
}

Result fix_build(std::string_view request, std::string_view cwd,
                 const std::function<void(const std::string &,
                                          const std::string &)> & note,
                 std::string_view history,
                 const std::vector<std::string> * carry_files) {
    // Work out how to build. Without a recognizable build system there is
    // nothing to loop on; fall back to the single-shot path.
    std::string build_cmd;
    if (!cwd.empty()) {
        std::error_code ec;
        const std::filesystem::path root{std::string(cwd)};
        if (std::filesystem::is_regular_file(root / "CMakeLists.txt", ec)) {
            build_cmd = "cmake . && make -j$(nproc)";
        } else if (std::filesystem::is_regular_file(root / "Makefile", ec)) {
            build_cmd = "make -j$(nproc)";
        }
    }
    if (build_cmd.empty()) return execute(request, cwd, history, carry_files);

    constexpr int kMaxRounds = 4;
    std::string applied;
    std::string tried;        // compact history of failed attempts
    std::string prev_tail;    // last build error, to detect no-effect fixes
    std::set<std::string> tried_cmds;  // verbatim-repeat detector
    // Files touched across the fix loop. Seeded from the plan-scope carry
    // (so the coder sees files earlier plan steps wrote), then grown with
    // whatever THIS fix loop writes. Passed to every execute() so the
    // overwrite refusal never fires on files this same loop just created.
    std::vector<std::string> local_carry;
    if (carry_files) local_carry = *carry_files;

    SegResult build = run_bash(build_cmd, cwd);
    note("build check", build_cmd + "\n" + tail_of(build.out, 3000) +
         "\n[exit " + std::to_string(build.exit_code) + "]");

    if (build.exit_code == 0) {
        const std::string lc = lowercase(request);
        const bool pasted_errors =
            lc.find("error:")              != std::string::npos ||
            lc.find("make: ***")           != std::string::npos ||
            lc.find("collect2")            != std::string::npos ||
            lc.find("undefined reference") != std::string::npos;
        // An explicit verify instruction ("verify the project builds",
        // the planner's last step) is ANSWERED by a passing build; it
        // must never fall through to the coder, which once celebrated by
        // inventing an out-of-source build/ dir that cannot work beside
        // an in-source CMakeCache.txt.
        const bool verify_flavored =
            lc.find("verify")    != std::string::npos ||
            lc.find("make sure") != std::string::npos ||
            lc.find("check")     != std::string::npos;
        if (!pasted_errors && !verify_flavored) {
            // The project already builds and nothing was pasted: the
            // is_build_fix keyword match probably misfired ("fix the
            // makefile formatting" is a normal command); run the request
            // instead of silently dropping it.
            return execute(request, cwd, history, carry_files);
        }
        Result r0;
        r0.command     = "(build-fix loop) " + build_cmd;
        r0.exit_code   = 0;
        r0.stdout_text = "build verified: passes\n" + tail_of(build.out, 1200);
        return r0;
    }

    // Snapshot the build file: the loop's fixes have destroyed it before
    // (a line-based sed left orphaned arguments and broke configure). If
    // a fix round turns a compile error into a CONFIGURE error, restore
    // the snapshot and tell the coder what happened.
    const std::filesystem::path cml =
        std::filesystem::path(std::string(cwd)) / "CMakeLists.txt";
    std::string cml_snapshot;
    {
        std::ifstream f(cml, std::ios::binary);
        if (f) cml_snapshot.assign(std::istreambuf_iterator<char>(f),
                                   std::istreambuf_iterator<char>());
    }
    const bool configure_broken_at_start =
        build.out.find("CMake Error") != std::string::npos;

    for (int round = 1; build.exit_code != 0 && round <= kMaxRounds; ++round) {
        const std::string cur_tail = tail_of(build.out, 3500);
        std::string fixreq(request);
        fixreq.append("\n\nThe build command `").append(build_cmd)
              .append("` currently fails with:\n")
              .append(cur_tail);
        const std::string hint = known_failure_hint(build.out);
        if (!hint.empty()) {
            fixreq.append("\n\n").append(hint);
        }
        if (!tried.empty()) {
            fixreq.append("\n\nFIXES ALREADY ATTEMPTED THAT DID NOT WORK "
                          "(do NOT repeat these; take a different approach):\n")
                  .append(tried);
        }
        if (!prev_tail.empty() && cur_tail == prev_tail) {
            fixreq.append("\n\nNOTE: the error is IDENTICAL to before the "
                          "last attempt; the last change had NO effect on it.");
        }
        fixreq.append("\n\nNever delete build artifacts (anything under "
                      "CMakeFiles/, *.o files); they are regenerated. Fix "
                      "the root cause in the source or CMakeLists.txt now."
                      "\n\nPRESERVE ALL EXISTING FUNCTIONALITY. Do NOT strip "
                      "out working code, replace a real implementation with "
                      "a placeholder / stub / TODO, or shrink a feature to "
                      "make it compile. Every WRITEFILE you emit must "
                      "reproduce the CURRENT file (see CURRENT FILES) with "
                      "ONLY the minimum change required to make this "
                      "specific compile error go away. If a socket loop, "
                      "byte array, or HTTP handler is present in the "
                      "current content, it must be present in your output "
                      "too. If you cannot see how to fix the error while "
                      "preserving the feature, leave the file unchanged "
                      "and fix a DIFFERENT file instead.");
        prev_tail = cur_tail;

        Result fix = execute(fixreq, cwd, history, &local_carry);
        for (const auto & wf : fix.written_files) {
            if (std::find(local_carry.begin(), local_carry.end(), wf)
                    == local_carry.end()) {
                local_carry.push_back(wf);
            }
        }
        note("fix attempt " + std::to_string(round),
             fix.command + "\n" + fix.stdout_text +
             "\n[exit " + std::to_string(fix.exit_code) + "]");
        if (fix.stdout_text.find("ACTION REQUIRED") != std::string::npos) {
            // The fix needs the human (root privileges). Looping cannot
            // help; surface the instruction as the outcome.
            Result r;
            r.command   = "(build-fix loop) " + build_cmd;
            r.exit_code = 1;
            r.stdout_text =
                "The build cannot be fixed by the agent alone.\n" +
                fix.stdout_text +
                "\n\nAfter running that, ask me to compile again."
                "\n\nCurrent error:\n" + tail_of(build.out, 1500);
            return r;
        }
        if (!tried_cmds.insert(fix.command).second) {
            // Greedy decoding re-proposed a fix verbatim despite the
            // do-not-repeat history; more rounds would be identical.
            note("loop stop",
                 "the coder repeated an already-failed fix verbatim; "
                 "stopping early");
            Result r;
            r.command   = "(build-fix loop) " + build_cmd +
                          "\n--- applied fixes ---\n" + applied;
            r.exit_code = 1;
            r.stdout_text =
                "stopped: the coder proposed the same failed fix twice "
                "verbatim.\nLast error:\n" + tail_of(build.out, 2500);
            return r;
        }
        if (!applied.empty()) applied.push_back('\n');
        applied.append(fix.command);
        tried.append("- ").append(fix_summary(fix.command)).push_back('\n');
        // When the coder tried to punt the work back to the user via a
        // non-root USER_ACTION, the segment executor rejected it. Spell
        // that out for the next round so the coder writes real files
        // instead of retrying the same escape hatch.
        if (fix.stdout_text.find("invalid USER_ACTION output") != std::string::npos) {
            tried.append("  (that attempt was a non-root USER_ACTION and "
                         "was REJECTED; the agent must edit the offending "
                         "file directly with WRITEFILE this round, not "
                         "describe the fix in prose.)\n");
        }

        build = run_bash(build_cmd, cwd);
        if (!configure_broken_at_start && !cml_snapshot.empty() &&
            build.out.find("CMake Error") != std::string::npos) {
            std::ofstream f(cml, std::ios::binary | std::ios::trunc);
            f.write(cml_snapshot.data(),
                    static_cast<std::streamsize>(cml_snapshot.size()));
            f.close();
            tried.append("- (that change broke the CMake configure step; "
                         "CMakeLists.txt was restored to its pre-loop "
                         "state)\n");
            note("rollback",
                 "fix broke cmake configure; CMakeLists.txt restored");
            build = run_bash(build_cmd, cwd);
        }
        note("rebuild " + std::to_string(round),
             tail_of(build.out, 3000) + "\n[exit " +
             std::to_string(build.exit_code) + "]");
    }

    Result r;
    r.command   = "(build-fix loop) " + build_cmd +
                  (applied.empty() ? std::string()
                                   : "\n--- applied fixes ---\n" + applied);
    r.exit_code = build.exit_code;
    r.stdout_text = build.exit_code == 0
        ? "build succeeded\n" + tail_of(build.out, 1200)
        : "build still failing after " + std::to_string(kMaxRounds) +
          " fix rounds\n" + tail_of(build.out, 3000);
    return r;
}

namespace {

constexpr const char * kPlanSystemPrompt =
    "You break a compound request for work on a software project into an "
    "ordered list of small concrete steps. Output ONLY lines of the form:\n"
    "STEP: <one self-contained imperative instruction>\n"
    "Rules:\n"
    "- 2 to 8 steps, in execution order. No other output.\n"
    "- Each step must be independently executable and must name explicit "
    "file or folder paths; repeat the paths in every step that touches "
    "them, because later steps cannot see earlier ones.\n"
    "- Keep every constraint from the request (ports, names, folders, "
    "standalone / compiled-in requirements) attached to the step that "
    "implements it.\n"
    "- ONE step per file: a step that creates or modifies a file states "
    "everything that file must contain. NEVER plan separate 'create "
    "empty file X' and 'implement X later' steps.\n"
    "- Every step is one complete self-contained sentence. NEVER end a "
    "step with a colon or 'with the following content:' -- there is no "
    "place for content to follow; describe the requirements inside the "
    "sentence (e.g. 'Create main.cpp in the project root containing a "
    "hello-world main that prints \"hello world\" and exits').\n"
    "- FEATURE FOLDERS: when the request states that the code for a "
    "feature LIVES IN a specific folder (\"the web server code lives in "
    "001_interface\", \"the parser goes in parser/\", \"all features "
    "will go in their own folder\"), the IMPLEMENTATION source files "
    "(*.cpp, *.hpp, *.cu, *.py, *.rs, etc.) for that feature MUST be "
    "created inside that folder -- one implementation file plus a "
    "matching header if the language uses them. NEVER plan to \"modify "
    "main.cpp to include the web server code\" or otherwise stuff the "
    "feature's implementation into main.cpp / lib.rs / __main__.py; "
    "main.cpp only calls into the feature's start() / init() entry "
    "point. Example: user says \"the web server lives in 001_interface; "
    "main.cpp starts it\" -> plan STEP: Create 001_interface/server.cpp "
    "and 001_interface/server.hpp implementing the web server (listens "
    "on port ..., serves ...) and exposing a start() / stop() API, plus "
    "STEP: Modify main.cpp to spawn server::start() in its own thread "
    "and enter the sleep loop.\n"
    "- COMPILED-IN / STANDALONE / SELF-CONTAINED assets: when the "
    "request says the binary must be standalone, self-contained, or "
    "that web / data resources must be compiled into the executable, "
    "embed them as BYTE ARRAYS in generated headers. NEVER plan a "
    "server that opens the asset files at runtime with std::ifstream / "
    "open() / read_file(path) -- that is not standalone.\n"
    "  The right way is `xxd -i -n <sym> <src> > <hdr>.hpp`, run as a "
    "shell step before the header is consumed. Split this into MULTIPLE "
    "steps so no single step has to hand-encode kilobytes of hex, which "
    "blows past the coder's token limit and produces a half-written "
    "file. The canonical shape for \"static web server serving "
    "compiled-in HTML/CSS/JS\" (or any equivalent) is:\n"
    "    STEP: Create the raw asset files -- index.html (a dark-themed "
    "hello-world page linking style.css and script.js), style.css (the "
    "dark theme), script.js (a small JS hook) -- inside the feature "
    "folder.\n"
    "    STEP: Generate the byte-array headers from the raw asset "
    "files by running (from the project root) 'xxd -i -n index_html "
    "<feature>/index.html > <feature>/index_html.hpp' and the same for "
    "style_css and script_js. Do NOT hand-encode the bytes; xxd emits "
    "the array plus a <name>_len variable in one step.\n"
    "    STEP: Create <feature>/server.hpp declaring start(int port) "
    "and stop(), and <feature>/server.cpp implementing the server. "
    "server.cpp lives IN <feature>/, so #include the generated "
    "headers by BASENAME only -- `#include \"index_html.hpp\"` (never "
    "`#include \"<feature>/index_html.hpp\"`, which resolves to "
    "<feature>/<feature>/index_html.hpp and does not exist). Server "
    "body is a POSIX socket loop that dispatches `/` -> index_html, "
    "`/style.css` -> style_css, `/script.js` -> script_js using the "
    "<name>_len variables from xxd for Content-Length.\n"
    "  Do NOT plan a shared assets.hpp that DEFINES a "
    "std::unordered_map at namespace scope -- including it from more "
    "than one .cpp is a duplicate-symbol link error. If a lookup table "
    "is needed, define it inside server.cpp, not a header.\n"
    "  If the project already has a working embed mechanism (an "
    "add_custom_command that runs an embed script referenced in the "
    "ROOT BUILD FILE), extend that instead of xxd.\n"
    "- Code goes into the project's EXISTING build system; never plan a "
    "new build file inside a subfolder.\n"
    "- Read the ROOT BUILD FILE content when it is shown below before "
    "planning any build-file step: if it already globs sources "
    "(file(GLOB_RECURSE ...)), new source files are picked up "
    "automatically and you must NOT plan a 'modify the build file to "
    "add/include the new files' step.\n"
    "- Use only the project's existing language and its standard "
    "library; never plan a step that installs packages or introduces "
    "third-party dependencies unless the user asked for them.\n"
    "- If the request asks to verify / compile / build / test, make that "
    "the LAST step, phrased exactly: verify the project builds\n"
    "- Do not add steps the user did not ask for.\n";

}

Result execute_plan(std::string_view request, std::string_view cwd,
                    std::string_view history,
                    const std::function<void(const std::string &,
                                             const std::string &)> & note) {
    // The planner must see the actual root build file: the survey only
    // names it, and without the content the planner cannot know that
    // the existing glob already picks up new sources ("modify
    // CMakeLists.txt to include the new files" was planned against a
    // generic GLOB_RECURSE CMakeLists).
    std::string build_file;
    std::string build_file_name;
    if (!cwd.empty()) {
        for (const char * bf : {"CMakeLists.txt", "Makefile",
                                "package.json", "Cargo.toml",
                                "pyproject.toml", "go.mod"}) {
            const auto p = std::filesystem::path(std::string(cwd)) / bf;
            std::error_code ec;
            if (!std::filesystem::is_regular_file(p, ec)) continue;
            std::ifstream f(p, std::ios::binary);
            std::string body((std::istreambuf_iterator<char>(f)),
                              std::istreambuf_iterator<char>());
            if (body.size() > 2048) { body.resize(2048); body += "\n..."; }
            build_file_name = bf;
            build_file      = std::move(body);
            break;
        }
    }

    std::string plan_sys(kPlanSystemPrompt);
    plan_sys.append(project_survey(cwd));
    if (!build_file.empty()) {
        plan_sys.append("\nROOT BUILD FILE (").append(build_file_name)
                .append(") -- CURRENT CONTENT:\n").append(build_file)
                .push_back('\n');
    }
    if (!history.empty()) {
        plan_sys.append("\nEARLIER REQUESTS THIS SESSION (context only):\n")
                .append(history);
    }
    std::string raw;
    try {
        raw = coder::generate(plan_sys, request, /*max_new_tokens=*/512);
    } catch (...) {
        return execute(request, cwd, history);
    }

    std::vector<std::string> steps;
    {
        std::size_t pos = 0;
        while (pos < raw.size() && steps.size() < 8) {
            std::size_t eol = raw.find('\n', pos);
            if (eol == std::string::npos) eol = raw.size();
            std::string line = raw.substr(pos, eol - pos);
            pos = eol + 1;
            const auto sp = line.find("STEP:");
            if (sp == std::string::npos) continue;
            std::string step = line.substr(sp + 5);
            const auto b = step.find_first_not_of(" \t");
            const auto e = step.find_last_not_of(" \t\r");
            if (b == std::string::npos) continue;
            steps.push_back(step.substr(b, e - b + 1));
        }
    }
    if (steps.size() < 2) return execute(request, cwd, history);

    // Deterministic redundancy filter: with a globbing build file, a
    // "modify CMakeLists to include the new source files" step is not
    // just useless, it invites the coder to rewrite a working build
    // file. Steps about OTHER build-file changes (linking, ports,
    // defines, standards) are kept.
    if (build_file.find("GLOB") != std::string::npos) {
        std::vector<std::string> kept;
        for (auto & s : steps) {
            const std::string ls = lowercase(s);
            const bool cmake_step =
                ls.find("cmakelists") != std::string::npos ||
                ls.find("cmake file") != std::string::npos;
            const bool add_files =
                (ls.find("include")  != std::string::npos ||
                 ls.find("add")      != std::string::npos ||
                 ls.find("register") != std::string::npos) &&
                (ls.find("file")     != std::string::npos ||
                 ls.find("source")   != std::string::npos ||
                 ls.find(".cpp")     != std::string::npos);
            const bool other_change =
                ls.find("link")     != std::string::npos ||
                ls.find("port")     != std::string::npos ||
                ls.find("define")   != std::string::npos ||
                ls.find("option")   != std::string::npos ||
                ls.find("standard") != std::string::npos;
            if (cmake_step && add_files && !other_change) {
                note("plan note",
                     "dropped a redundant step (the existing " +
                     build_file_name +
                     " already globs sources, so new files are picked up "
                     "automatically): " + s);
                continue;
            }
            kept.push_back(std::move(s));
        }
        steps = std::move(kept);
        if (steps.empty()) return execute(request, cwd, history);
    }

    // The planner is told to end with "verify the project builds" when
    // asked; it still drops that step. Deterministic backstop.
    {
        const std::string lc_req = lowercase(request);
        const bool wants_verify =
            lc_req.find("compil")   != std::string::npos ||
            lc_req.find("verify")   != std::string::npos ||
            lc_req.find("build error") != std::string::npos ||
            lc_req.find("make sure it builds") != std::string::npos;
        bool has_verify = false;
        for (const auto & s : steps) {
            const std::string ls = lowercase(s);
            if (ls.find("verify") != std::string::npos ||
                ls.find("compil") != std::string::npos ||
                ls.find("builds") != std::string::npos) {
                has_verify = true;
                break;
            }
        }
        if (wants_verify && !has_verify) {
            steps.push_back("verify the project builds");
        }
    }

    // Every step carries the full original request: per-step execution
    // once dropped "standalone, resources compiled in" on the floor and
    // the step reached for boost.
    std::string req_context(request.substr(0, 1500));
    req_context = "\n\n(FULL ORIGINAL REQUEST -- every constraint in it "
                  "still applies to this step:\n" + req_context + ")";

    std::string checklist;
    for (std::size_t i = 0; i < steps.size(); ++i) {
        checklist.append(std::to_string(i + 1)).append(". ")
                 .append(steps[i]).push_back('\n');
    }
    note("tasks", checklist);

    Result agg;
    agg.exit_code = 0;
    agg.command   = "(task plan)\n" + checklist;
    std::string done;
    // Files WRITEFILE'd by any earlier step of this plan. Carried into
    // every subsequent step's execute() so an intentional edit against a
    // step-N-created file (e.g. task 3 rewriting the placeholder
    // index.html task 1 dropped) sees the current content and does not
    // trip the anti-hallucination refusal.
    std::vector<std::string> plan_touched;
    for (std::size_t i = 0; i < steps.size(); ++i) {
        const std::string & step = steps[i];
        const std::string label = "task " + std::to_string(i + 1) + "/" +
                                  std::to_string(steps.size());
        const std::string lc = lowercase(step);
        const bool verifyish =
            lc.find("verify")  != std::string::npos ||
            lc.find("compil")  != std::string::npos ||
            lc.find("builds")  != std::string::npos ||
            lc.find("run the test") != std::string::npos;
        Result r;
        if (verifyish) {
            r = fix_build(step + req_context, cwd, note, history,
                          &plan_touched);
        } else {
            std::string step_req(step);
            step_req.append(req_context);
            if (!done.empty()) {
                step_req.append("\n\n(Earlier steps of this same request, "
                                "already completed -- do not redo them:\n")
                        .append(done).append(")");
            }
            r = execute(step_req, cwd, history, &plan_touched);
        }
        note(label, step + "\n" + r.command + "\n" +
             tail_of(r.stdout_text, 1200) +
             "\n[exit " + std::to_string(r.exit_code) + "]");
        agg.stdout_text.append(label).append(" ")
            .append(r.exit_code == 0 ? "ok" : "FAILED").append(": ")
            .append(step.substr(0, 120)).push_back('\n');
        for (const auto & wf : r.written_files) {
            if (std::find(plan_touched.begin(), plan_touched.end(), wf)
                    == plan_touched.end()) {
                plan_touched.push_back(wf);
            }
        }
        if (r.exit_code != 0) {
            agg.exit_code = r.exit_code;
            agg.stdout_text
                .append("stopped at the failing step; its output:\n")
                .append(tail_of(r.stdout_text, 2000)).push_back('\n');
            break;
        }
        done.append("- ").append(step).push_back('\n');
    }
    if (agg.exit_code == 0) agg.stdout_text.append("all steps completed\n");
    return agg;
}

}
