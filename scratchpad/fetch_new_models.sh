#!/usr/bin/env bash
# Sequentially fetch + chunk every new role listed in data/sources.json
# via `./ac9 fetch <role>`. Runs in the background so the main harness
# can watch progress via the log tail. Fails loud on any single role's
# error so the caller sees WHICH one broke.
set -e
cd /home/jwoods/work/Autoclank9001
log=scratchpad/fetch_new_models.log
: > "$log"
echo "== fetch start $(date -u +%Y-%m-%dT%H:%M:%SZ) ==" >> "$log"
for role in \
    qwen3-8b-huihui-v2 \
    qwen2.5-coder-7b-huihui \
    qwen3-4b-huihui-v2 \
    vision-8b-huihui \
    vision-8b-huihui-mmproj
do
    echo "== fetching $role $(date -u +%H:%M:%S) ==" | tee -a "$log"
    if ! ./ac9 fetch "$role" 2>&1 | tee -a "$log"; then
        echo "== FAILED at $role $(date -u +%H:%M:%S) ==" | tee -a "$log"
        exit 1
    fi
done
echo "== fetch done $(date -u +%Y-%m-%dT%H:%M:%SZ) ==" | tee -a "$log"
