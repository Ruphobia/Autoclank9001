// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 009_tools/shell coder-output segmentation. Pins the
// parser against the exact failure shape from the Quantiprize session:
// a WRITEFILE chained mid-line after && reached bash verbatim and died
// with "WRITEFILE: command not found" while the file content that
// followed was executed as shell.

#include "test_runner.hpp"
#include "../modules/009_tools/shell/shell.hpp"

#include <string>

namespace {

testing::TestOutcome run() {
    using shell_tool::debug_segments;

    // Plain shell only.
    auto s1 = debug_segments("mkdir -p app && ls app\n");
    if (s1.size() != 1 || s1[0] != "SHELL mkdir -p app && ls app")
        return testing::fail("plain shell chunk misparsed");

    // Canonical WRITEFILE block.
    auto s2 = debug_segments("WRITEFILE hello.py\nprint(\"hi\")\n");
    if (s2.size() != 1 || s2[0].rfind("FILE hello.py", 0) != 0)
        return testing::fail("canonical WRITEFILE misparsed");

    // Shell, then two file blocks.
    auto s3 = debug_segments(
        "mkdir -p app\n"
        "WRITEFILE app/run.py\nprint(\"go\")\n"
        "WRITEFILE README.md\nhi\n");
    if (s3.size() != 3 ||
        s3[0] != "SHELL mkdir -p app" ||
        s3[1].rfind("FILE app/run.py", 0) != 0 ||
        s3[2].rfind("FILE README.md", 0) != 0)
        return testing::fail("multi-block output misparsed");

    // THE regression: WRITEFILE chained after && must split into a shell
    // prefix and a file block, never reach bash.
    auto s4 = debug_segments(
        "mkdir -p web && npm install express && WRITEFILE web/server.js\n"
        "const express = require('express');\n"
        "app.listen(3000);\n");
    if (s4.size() != 2)
        return testing::fail("inline && WRITEFILE did not split into 2 segments");
    if (s4[0] != "SHELL mkdir -p web && npm install express")
        return testing::fail("shell prefix wrong: " + s4[0]);
    if (s4[1].rfind("FILE web/server.js", 0) != 0)
        return testing::fail("file segment wrong: " + s4[1]);

    // Same with ';' as the separator.
    auto s5 = debug_segments("mkdir -p a; WRITEFILE a/x.txt\nhello\n");
    if (s5.size() != 2 ||
        s5[0] != "SHELL mkdir -p a" ||
        s5[1].rfind("FILE a/x.txt", 0) != 0)
        return testing::fail("inline ; WRITEFILE did not split");

    // The inline pattern INSIDE a file block is content, never a split.
    auto s6 = debug_segments(
        "WRITEFILE notes.md\n"
        "run this: mkdir x && WRITEFILE x/y\n"
        "more\n");
    if (s6.size() != 1 || s6[0].rfind("FILE notes.md", 0) != 0)
        return testing::fail("file content containing && WRITEFILE was split");

    // Fenced file block: fences are markup, not content.
    auto s7 = debug_segments("WRITEFILE main.cpp\n```cpp\nint main(){}\n```\n");
    if (s7.size() != 1 || s7[0].rfind("FILE main.cpp", 0) != 0)
        return testing::fail("fenced WRITEFILE misparsed");

    // cd steps in the split prefix fold into the file path (write_one_file
    // bypasses the shell, so the cd would otherwise be silently ignored).
    auto s8 = debug_segments(
        "mkdir -p 001_interface && cd 001_interface && npm init -y && "
        "WRITEFILE server.js\nconsole.log(1);\n");
    if (s8.size() != 2 || s8[1].rfind("FILE 001_interface/server.js", 0) != 0)
        return testing::fail("cd folding into file path failed: " +
                             (s8.size() > 1 ? s8[1] : std::string("(missing)")));

    // CRLF output: \r is stripped from shell lines (bash chokes on it),
    // never from file content.
    auto s9 = debug_segments("ls -la\r\npwd\r\n");
    if (s9.size() != 1 || s9[0] != "SHELL ls -la")
        return testing::fail("CRLF shell line not stripped: [" + s9[0] + "]");

    return testing::ok();
}

const int _reg = testing::register_test(
    "shell_segments",
    "coder-output segmentation: WRITEFILE blocks, inline && WRITEFILE split, fences",
    &run);

}  // namespace
