#!/bin/bash
# Send a chat message to ac9 and stream the SSE response.
# Usage: chat.sh <cwd> <message>
# Message is passed via env var so shell quoting doesn't leak into Python.
set -eu
export AC9_CWD="${1:?cwd}"
export AC9_MSG="${2:?message}"
ts=$(date -u +'%Y%m%dT%H%M%SZ')
log="/home/jwoods/work/Autoclank9001/scratchpad/ac9_test_001_logs/chat-${ts}.log"
mkdir -p "$(dirname "$log")"
{
  echo "=== $(date -u +%FT%TZ) ==="
  echo "cwd:     $AC9_CWD"
  echo "message: (see body below)"
  echo "---"
} | tee "$log"

python3 - <<'PYEOF' | tee -a "$log"
import json, os, sys, urllib.request
body = json.dumps({
    "message": os.environ["AC9_MSG"],
    "cwd":     os.environ["AC9_CWD"],
}).encode()
req = urllib.request.Request(
    "http://127.0.0.1:8080/api/chat",
    data=body,
    headers={"Content-Type": "application/json"},
)
seen_final = False
with urllib.request.urlopen(req, timeout=1800) as r:
    for line in r:
        s = line.decode("utf-8", errors="replace").rstrip("\r\n")
        print(s, flush=True)
        if seen_final and s.startswith("data:"):
            # We printed the final's data line; drain rest quickly and exit.
            break
        if s.startswith("event: final"):
            seen_final = True
PYEOF
