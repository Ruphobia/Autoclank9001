#!/bin/bash
# Watch the ticket runner for the maze-game project. Polls every 30s.
# Prints a snapshot on every transition (ticket status changes or runner
# stops). Exits when running=false OR after 3h wall-clock, whichever first.
set -u
CWD="/home/jwoods/work/ac9_test_001"
STATUS_URL="http://127.0.0.1:8080/api/tickets/run/status?cwd=$CWD"
BOARD="$CWD/.tickets.agile"

state=""
end=$(( $(date +%s) + 10800 ))   # 3h max
i=0
while [ "$(date +%s)" -lt "$end" ]; do
  st=$(curl -s "$STATUS_URL" 2>/dev/null)
  running=$(echo "$st" | python3 -c 'import json,sys; d=json.load(sys.stdin); print(d.get("running", False))' 2>/dev/null || echo unknown)
  cur=$(echo "$st" | python3 -c 'import json,sys; d=json.load(sys.stdin); print(d.get("current_ticket_id",""))' 2>/dev/null || echo "")
  last_layer=$(echo "$st" | python3 -c 'import json,sys; d=json.load(sys.stdin); print(d.get("last_layer",""))' 2>/dev/null || echo "")
  # Build board snapshot signature so we detect any status change.
  sig=$(python3 -c "
import json
try:
    b = json.load(open('$BOARD'))
    rows = [t['id']+':'+t.get('status','') for t in b.get('tickets',[])]
    print('|'.join(rows))
except Exception as e:
    print('ERR:'+str(e))
")
  new_state="running=$running|cur=$cur|layer=$last_layer|$sig"
  if [ "$new_state" != "$state" ]; then
    echo "[$(date -u +%FT%TZ)] running=$running current=$cur last_layer=$last_layer"
    python3 -c "
import json
try:
    b = json.load(open('$BOARD'))
    for t in b.get('tickets',[]):
        print(f'    {t[\"id\"]:<5} {t.get(\"status\",\"?\"):<8} {t.get(\"title\",\"\")}' )
except Exception as e:
    print('    (board read failed:', e, ')')
"
    state="$new_state"
    echo
  fi
  if [ "$running" = "False" ]; then
    echo "[$(date -u +%FT%TZ)] runner stopped."
    exit 0
  fi
  i=$((i+1))
  sleep 30
done
echo "[$(date -u +%FT%TZ)] watch timeout (3h)."
exit 1
