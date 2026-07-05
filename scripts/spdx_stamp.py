#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""Prepend SPDX-License-Identifier headers to source files that lack them.

Additive: never modifies a file that already has SPDX or an existing
copyright block in its first 10 lines. Skips vendored third-party
embeds and files owned by other agents.
"""
import os, re, sys

ROOT = os.path.abspath(sys.argv[1] if len(sys.argv) > 1 else ".")
SPDX_ID = "GPL-3.0-or-later"

# Exact repo-relative paths to skip (vendored third-party)
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
}

# Prefix directories to skip
EXCLUDE_PREFIX = (
    "kicad/",
    ".git/",
    "scratchpad/",
    "modules/010_interface/eda/",       # other agent
    "modules/353_editor_session/",      # other agent
    "modules/354_annotator/",           # other agent
    "modules/355_pcb_ops/",             # other agent
    "modules/356_lib_editor/",          # other agent
    "data/",                             # runtime data
    "resources/",                        # runtime data
    "005_context/",                      # runtime sessions
    "_deps/",
    "CMakeFiles/",
    "bin/",
    "interface_gen/",
    "build/",
    "scripts/",                          # this script
)

# Also skip these specific other-agent files under testing/
EXCLUDE_TESTING = {
    "testing/test_annotator.cpp",
    "testing/test_lib_editor.cpp",
    "testing/test_pcb_ops.cpp",
}

INCLUDE_ROOTS = ("modules", "testing")
TOP_FILES = ("main.cpp", "CMakeLists.txt", "README.md")

# If we see any of these in the first 10 lines, the file already has some
# form of attribution or SPDX; don't touch it.
SKIP_RE = re.compile(
    r"(SPDX-License-Identifier|Copyright |\(c\) [12]\d\d\d|@license|"
    r"@toast-ui|SQLite is in the|Yuji Hirose|BSD-3-Clause|MIT License|"
    r"Apache License)"
)

# Extension -> (open, close, pad). Empty close = line comment.
STYLES = {
    ".cpp": ("//", "", ""),  ".hpp": ("//", "", ""),
    ".h":   ("//", "", ""),  ".c":   ("//", "", ""),
    ".cc":  ("//", "", ""),  ".hh":  ("//", "", ""),
    ".js":  ("//", "", ""),  ".mjs": ("//", "", ""),
    ".py":  ("#",  "", ""),
    ".css": ("/*", "*/", " "),
    ".html":("<!--", "-->", " "),
    ".md":  ("<!--", "-->", " "),
    ".sh":  ("#",  "", ""),
    ".cmake":("#", "", ""),
}

def header_for(path):
    name = os.path.basename(path)
    ext = os.path.splitext(path)[1]
    if name == "CMakeLists.txt":
        o, c, p = "#", "", ""
    elif ext in STYLES:
        o, c, p = STYLES[ext]
    else:
        return None
    body = f"SPDX-License-Identifier: {SPDX_ID}"
    if c:
        return f"{o}{p}{body}{p}{c}\n"
    return f"{o} {body}\n"

def stamp(path):
    try:
        with open(path, "r", encoding="utf-8", errors="replace") as f:
            data = f.read()
    except (OSError, UnicodeDecodeError):
        return False
    if not data:
        return False
    head_lines = data.splitlines(keepends=True)[:10]
    head = "".join(head_lines)
    if SKIP_RE.search(head):
        return False
    hdr = header_for(path)
    if hdr is None:
        return False
    lines = data.splitlines(keepends=True)
    insert_at = 0
    # Shebang stays on line 1 for interpreted scripts.
    if lines and lines[0].startswith("#!"):
        insert_at = 1
    # HTML: SPDX goes on line 1 (before doctype/comments); no BOM handling
    # needed for our files.
    new = "".join(lines[:insert_at]) + hdr + "".join(lines[insert_at:])
    with open(path, "w", encoding="utf-8") as f:
        f.write(new)
    return True

def candidates():
    for tf in TOP_FILES:
        p = os.path.join(ROOT, tf)
        if os.path.isfile(p):
            yield p
    for r in INCLUDE_ROOTS:
        root_p = os.path.join(ROOT, r)
        if not os.path.isdir(root_p):
            continue
        for dp, _, fs in os.walk(root_p):
            for fn in fs:
                p = os.path.join(dp, fn)
                rel = os.path.relpath(p, ROOT)
                if rel in EXCLUDE_FILES or rel in EXCLUDE_TESTING:
                    continue
                if any(rel.startswith(pref) for pref in EXCLUDE_PREFIX):
                    continue
                ext = os.path.splitext(fn)[1]
                if ext in STYLES or fn == "CMakeLists.txt":
                    yield p

n_stamped = 0
n_skipped = 0
for p in candidates():
    if stamp(p):
        n_stamped += 1
    else:
        n_skipped += 1
print(f"stamped: {n_stamped}   skipped-had-header-or-non-source: {n_skipped}")
