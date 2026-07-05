#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""Rename 'tool' to 'ac9' (binary/paths) or 'AutoClank 9001' (branding)
across the repo, using a curated substitution table.

Excludes vendored third-party, kicad/, other-agent-owned dirs, and any
runtime data dirs.
"""
import os, re, sys

ROOT = os.path.abspath(sys.argv[1] if len(sys.argv) > 1 else ".")

EXCLUDE_PREFIX = (
    "kicad/",
    ".git/",
    "scratchpad/",
    "modules/010_interface/eda/",
    "modules/353_editor_session/",
    "modules/354_annotator/",
    "modules/355_pcb_ops/",
    "modules/356_lib_editor/",
    "data/",
    "resources/",
    "005_context/",
    "_deps/",
    "CMakeFiles/",
    "bin/",
    "interface_gen/",
    "build/",
    "scripts/",
)

EXCLUDE_FILES = {
    "modules/005_context/sqlite3.c",
    "modules/005_context/sqlite3.h",
    "modules/010_interface/httplib.h",
    "modules/010_interface/xterm.js",
    "modules/010_interface/xterm.css",
    "modules/010_interface/xterm-addon-fit.js",
    "modules/010_interface/toastui-editor.min.js",
    "modules/010_interface/toastui-editor.min.css",
    "modules/010_interface/toastui-editor-dark.min.css",
    "modules/010_interface/toastui-editor-plugin-code-syntax-highlight.min.js",
    "modules/010_interface/toastui-editor-plugin-code-syntax-highlight.min.css",
    "modules/010_interface/prism-all.min.js",
    "modules/010_interface/prism-tomorrow.min.css",
    "modules/010_interface/codejar.min.js",
    "LICENSE",
    ".gitignore",
    "todo.txt",  # other agent editing
    "testing/test_annotator.cpp",
    "testing/test_lib_editor.cpp",
    "testing/test_pcb_ops.cpp",
}

# Text file extensions to sweep. Binary or unrelated types skipped.
TEXT_EXTS = {
    ".cpp", ".hpp", ".h", ".c", ".cc", ".hh",
    ".js", ".mjs",
    ".py", ".sh",
    ".css", ".html",
    ".md", ".txt",
    ".cmake",
}

# Substitutions applied in order. Each entry: (pattern_type, pattern, replacement)
# Types: "literal" (exact string), "re" (regex, MULTILINE).
# Applied to every file's full text unless the file is excluded.
SUBS = [
    # --- CMake / project identity ---
    ("literal", "project(tool LANGUAGES C CXX)", "project(ac9 LANGUAGES C CXX)"),
    ("literal", "add_executable(tool_test", "add_executable(ac9_test"),
    ("re", r"\btool_test\b", "ac9_test"),
    ("re", r"\bTOOL_(CPP|CUDA|TEST_CPP)_SOURCES\b", r"AC9_\1_SOURCES"),
    ("literal", "_TOOL_EXCLUDE_REGEX", "_AC9_EXCLUDE_REGEX"),

    # --- Environment variables and per-user paths ---
    ("literal", "TOOL_PERSONAL_DIR", "AC9_PERSONAL_DIR"),
    ("literal", "$HOME/.tool/", "$HOME/.ac9/"),
    ("literal", "<project>/.tool/", "<project>/.ac9/"),
    ("literal", ".tool_runs", ".ac9_runs"),
    ("literal", ".toolai.cfg", ".ac9ai.cfg"),

    # --- HTML title + JS title strings ---
    ("literal", "<title>tool</title>", "<title>AutoClank 9001</title>"),
    ("literal", "'tool — ready'", "'AutoClank 9001 — ready'"),
    ("literal", "`tool — ${s.headline}`", "`AutoClank 9001 — ${s.headline}`"),
    ("literal", "'tool — (server unreachable)'", "'AutoClank 9001 — (server unreachable)'"),
    ("literal", "Open in tool browser", "Open in AutoClank 9001 browser"),

    # --- stderr banners: only match the exact "tool: " prefix used at line
    #     start of fprintf format strings, so we don't stomp English "tool: ..." ---
    ("re", r'"tool: pipeline load error:', '"ac9: pipeline load error:'),
    ("re", r'"tool: shutting down\.\.\.\\n"', '"ac9: shutting down...\\n"'),
    ("re", r'"tool: web ui listening on', '"ac9: web ui listening on'),
    ("re", r'"tool: web server stopped"', '"ac9: web server stopped"'),
    ("re", r'"tool: %s\\n"', '"ac9: %s\\n"'),

    # --- KiCad s-expr generator field: literal "tool" in narrow contexts ---
    ("literal", 'std::string generator = "tool";', 'std::string generator = "ac9";'),
    ("literal", 'SExpr::make_string("tool")', 'SExpr::make_string("ac9")'),
    ("literal", '"tool:placeholder"', '"ac9:placeholder"'),

    # --- SBOM auditor test: rename generator name in emit calls ---
    ("re", r'emit_(cyclonedx_json|spdx_[a-z_]+)\(cs, "tool"',
        r'emit_\1(cs, "ac9"'),

    # --- File-banner comments (line 1 comment describing the project) ---
    #    Narrow: match only if the file starts with the banner form.
    ("re", r'\A// tool — main entry\.',
        r'// ac9 — main entry.'),
    ("re", r'\A// tool web UI - vanilla JS,',
        r'// ac9 web UI - vanilla JS,'),

    # --- Prose in code comments: `tool` in backticks (rare, narrow) ---
    #    Match `tool` where it's a full backticked token, only in .cpp/.hpp/.md
    ("re", r"`tool`", "`AutoClank 9001`"),

    # --- Comment references to "tool" as project name (careful, ordered last) ---
    ("literal", "// The tool runs on the user's server in the basement",
        "// AutoClank 9001 runs on the user's server in the basement"),
    ("literal", "rebuilds `tool` we want", "rebuilds `ac9` we want"),
]

def process_file(path):
    try:
        with open(path, "r", encoding="utf-8", errors="replace") as f:
            data = f.read()
    except (OSError, UnicodeDecodeError):
        return False, 0
    orig = data
    change_count = 0
    for kind, pat, rep in SUBS:
        if kind == "literal":
            if pat in data:
                new = data.replace(pat, rep)
                if new != data:
                    change_count += data.count(pat)
                    data = new
        else:  # regex
            new, n = re.subn(pat, rep, data, flags=re.MULTILINE)
            if n:
                change_count += n
                data = new
    if data != orig:
        with open(path, "w", encoding="utf-8") as f:
            f.write(data)
        return True, change_count
    return False, 0

def candidates():
    for tf in ("main.cpp", "CMakeLists.txt", "README.md"):
        p = os.path.join(ROOT, tf)
        if os.path.isfile(p):
            yield p
    for dp, _, fs in os.walk(ROOT):
        rel_dp = os.path.relpath(dp, ROOT).replace(os.sep, "/") + "/"
        if any(rel_dp.startswith(pref) for pref in EXCLUDE_PREFIX):
            continue
        for fn in fs:
            p = os.path.join(dp, fn)
            rel = os.path.relpath(p, ROOT)
            if rel in EXCLUDE_FILES:
                continue
            if any(rel.startswith(pref) for pref in EXCLUDE_PREFIX):
                continue
            ext = os.path.splitext(fn)[1]
            if ext in TEXT_EXTS or fn == "CMakeLists.txt":
                yield p

n_files = 0
n_edits = 0
for p in candidates():
    changed, count = process_file(p)
    if changed:
        n_files += 1
        n_edits += count
print(f"files-touched: {n_files}   substitutions: {n_edits}")
