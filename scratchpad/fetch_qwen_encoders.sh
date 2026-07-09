#!/usr/bin/env bash
# Wait for the qwen-image fetch (pid 1001078) to finish, then pull the
# abliterated encoder + mmproj sequentially. Serialised so bandwidth
# stays saturated by ONE request instead of split between three.
set -e
cd /home/jwoods/work/Autoclank9001
log=scratchpad/fetch_qwen_encoders.log
: > "$log"

echo "== waiting for qwen-image fetch (pid 1001078) $(date -u +%H:%M:%S) ==" | tee -a "$log"
while kill -0 1001078 2>/dev/null; do sleep 10; done
echo "== qwen-image fetch process ended $(date -u +%H:%M:%S) ==" | tee -a "$log"

for role in qwen2.5-vl-7b-abliterated qwen2.5-vl-7b-abliterated-mmproj; do
    echo "== fetching $role $(date -u +%H:%M:%S) ==" | tee -a "$log"
    if ! ./ac9 fetch "$role" 2>&1 | tee -a "$log"; then
        echo "== FAILED at $role $(date -u +%H:%M:%S) ==" | tee -a "$log"
        exit 1
    fi
done
echo "== all encoders done $(date -u +%Y-%m-%dT%H:%M:%SZ) ==" | tee -a "$log"
