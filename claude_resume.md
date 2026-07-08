# Claude Resume - ac9 session handoff

*Written 2026-07-08. Read this first when you resume. It is the state of everything I was doing when the operator asked me to pause. Do not skim - the exact resume steps at the bottom depend on knowing what's staged and what's shipped.*

---

## The project in one paragraph

Autoclank 9001 (ac9) is a local-first C++17 orchestration workbench that runs many small abliterated LLMs on the operator's own hardware (two Tesla P100s, 16 GB each) and hands them real tools so they finish jobs instead of describing them. It exposes an HTTP server on port 8080, embeds all UI assets into the binary at CMake configure time, and drives external subprocesses (llama.cpp, sd-cli, coolwsd, ai-toolkit) as needed. The operator (jwoods, GitHub `Ruphobia`) uses ac9 to build OTHER projects: it plans them into tickets, then runs each ticket through a coder LLM until the target project builds and runs.

Repo: `/home/jwoods/work/Autoclank9001/`. Current binary was built from commit `033ac8e8` (before the mid-session fixes below).

---

## The long-running test we are inside

Test name: **maze-game test.** Target directory: `/home/jwoods/work/ac9_test_001/`. Operator directive (paraphrase): "use ac9 to build a browser-based maze game with a battery-draining robot, a C++ web server on port 9002, art via image_gen, and a smoke test. I never touch the code. You fix ac9 when it gets in the way."

The maze-game test IS the primary loop. Every ac9 fix in this session was surfaced by running it. Iteration cadence: send the bare planning prompt, kick off the runner, watch for blocks, fix the underlying scaffolding gap, nuclear reset (wipe target project + session), rerun.

Latest run: **v11** (nine nuclear resets so far). Details in "Where the maze-game test left off" below.

---

## Cumulative commits shipped this session (chronological, `main`)

Read `git log --oneline` for the full list. Grouped by theme:

**Image editor scaffolding** (`b56d5324`, `271756b9`, `9380b47f`, `c785515f`)
- Ticket router beats image router when both fire
- Image router bails on ac9-tool-name references
- Image router honors `save to <path>` phrasing
- Empty classify defaults to command/code
- Coder strips `<think>...</think>` blocks before returning
- Layer-2 decompose depth capped at 1
- Triple-backtick fences stripped from WRITEFILE bodies and comment_code output

**Tool router + prompt shaper** (`fa422ccd`, `4c161fa5`, `3884023c`, `2161b50d`)
- LLM-based intent classifier as new module `modules/009_tools/tool_router/`
- 13 default tools registered at startup with per-tool `prompt_template`
- Bare `language-name` line at coder output start rewrites to `WRITEFILE <default>`
- `router_decided` flag suppresses legacy regex routers when router picked confidently
- Image gen and image edit dispatch wired

**Docs and site infrastructure** (`6d57b4de`, `7a4f8f90`, `596e1b93`)
- Just the Docs Jekyll theme + landing + sidebar categories in `docs/`
- baseurl fix so `ruphobia.github.io/Autoclank9001/` loads styled
- em/en dash strip from docs and root

**Em-dash policy** (`7703dd5d`, `033ac8e8`, and the workflow's swept files)
- Bulk snapshot: 46 em dashes + 1 en dash replaced with hyphen across 12 doc files
- Then a multi-agent workflow (`wpy72fvnf`) enumerated every em/en/h-bar dash across source, docs, memory, scratchpad, chat logs and swept them all
- Same workflow installed defensive strips in `coder.cpp`, `planner.cpp`, `shell.cpp write_one_file`, `server.cpp handle_chat` outgoing final frame
- System-prompt em-dash-ban bullet added to every LLM's system prompt (coder, planner, tool_router templates, vision, mouser, classify, disambiguate, stylize, entities, expertise, etc.)

**Coder OOM + crash visibility** (`20f7876d`)
- `coder::generate` retries the model load 3 times with 3s / 6s / 9s sleeps before throwing (fixes the CUDA memory pool release lag after sd-cli exits)
- Loud `!!!! CODER MODEL FAILED !!!!` stderr markers at every `coder::generate` catch site
- `ticket_run_execute` teaches the SSE frame parser to treat `event: error` as a hard block (no more silent ghost-passes when the LLM is dead)

**Subject consistency (image continuity)** (`1306d701`, `6fd4212e`, `1b003f34`, `b4e76f49`, `e6018200`, `b981c4d7`)
- `image_generator::generate()` gains `Options { seed, init_img_path, strength, lora_refs }` - legacy two-arg signature preserved as thin forwarder
- `image_resolver` extended with canonical character storage: `canonical_dir / _exists / _ref / _seed / _prompt_suffix / _lora / _write_seed / _write_prompt_suffix / _promote / _training_images`. Layout: `<cwd>/.ac9_images/canonical/<char>/{<char>.png, .txt, .seed, .lora.safetensors, sheet/}`
- `image_gen` tool_router dispatch is reference-aware: loads canonical seed + LoRA + init PNG + prompt suffix when a `character_name` is set
- New `image_promote` and `train_canonical` tool_router tools
- New module `modules/009_tools/lora_trainer/` wraps `ostris/ai-toolkit run.py` for Chroma1-HD LoRA training (Week 2 scope)
- Design + rationale in `scratchpad/subject_consistency_research.md` (40 KB report from a sub-agent)

**Collabora Online vendorization** (`3998ea7a`, `afeba418`, `3c38cb95`)
- New module `modules/1720_office_suite/` supervises coolwsd (fork+execv, adopts existing if alive, /dev/urandom access token, SIGINT/SIGTERM reaps)
- Native C++ WOPI endpoints in `server.cpp` (`/wopi/files/<id>`, `/wopi/files/<id>/contents` GET+POST, `/api/office/config`, `/api/office/ensure`)
- UI: Office menu in the menubar with Writer / Calc / Impress / Draw dropdown, four center-pane tabs, hide-not-destroy iframes
- `data/manifest.json` gets a `collabora` role note listing required runtime paths
- Replaces the earlier `/home/jwoods/work/cool-runtime/wopi_host.py` Python stub

**Office AI integration** (`b97bf460`)
- New module `modules/009_tools/office_docs/` with soffice-driven read/write/edit/summarize/structure primitives (cached by path+mtime, per-call profile isolation)
- 9 new tool_router entries: `doc_read`, `doc_summarize`, `doc_write`, `doc_edit`, `doc_structure`, `sheet_read`, `sheet_write`, `slide_read`, `slide_write`
- Dispatch cases in `server.cpp` + `/api/office/preview` endpoint
- File-tree dblclick on `.odt/.ods/.odp/.odg/.docx/.xlsx/.pptx` opens the corresponding Office tab; hover shows first ~200 chars via `office_docs::read()`
- `data/manifest.json` gets a `soffice` role note

**HEAD**: `b97bf460`. Everything above this line is on `origin/main`. Everything below is not.

---

## Uncommitted work (WORKING TREE ONLY - not on origin)

`modules/010_interface/server.cpp` has three defensive fixes I added right before the operator asked to pause. They are staged in the file but never committed and never built. Each fix has verbose comments in the file explaining WHY - grep for the marker strings.

**Fix 1: Router code-smell guard on image_gen picks.** Grep server.cpp for `code_smell`. Rejects the tool_router's `image_gen` decision when the extracted subject contains code tokens like `std::`, `#include`, `.cpp`, `.html`, `algorithm`, `main()`. Falls through to the coder path via `router_decided = false` + `goto after_router_dispatch;`. Motivated by v11 T-8 ('Implement maze generation algorithm') and T-9 ('Create HTML page with embedded JS') being routed to Chroma with the ticket body as the subject, producing garbage PNGs and marking the tickets "done."

**Fix 2: Ghost-pass detection in `ticket_run_execute`.** Grep server.cpp for `GHOST PASS DETECTED`. When the shell handler reports `exit_code=0`, additionally check whether the command contains any write-shaped op (`WRITEFILE`, `touch`, `mkdir`, `echo`, `printf`, `tee`, `>`, `>>`, `cp`, `mv`, `chmod`, `cat <<`, `cat >`, `dd`, `curl`, `wget`, `sed -i`, `git`, `cmake`, `make`, `pip`, `apt`, `python`, `node`). If not, log a loud marker and force `success = false`. Motivated by v11 T-7 (main.cpp) where the coder emitted `ls -la && find . -name '*.h*'` (pure exploration), exit 0, and the ticket was marked done despite writing no source file.

**Fix 3: `save_to` auto-defaulting for asset-shaped subjects.** Grep server.cpp for `save_to_effective`. When `save_to` is empty AND the subject mentions `sprite`, `tile`, `icon`, `asset`, `artwork`, `texture`, `pickup`, `backdrop`, or `logo`, derive `save_to = assets/<slugified-first-noun>.png` and emit a diagnostic layer. Downstream references to `save_to` in the same dispatch branch have been updated to use `save_to_effective`. Motivated by v11 T-1 where the robot sprite PNG landed in `.ac9_images/four-animated-robot-sprites-in-cute-pixel-art-*.png` instead of `assets/robot.png`.

**Build state:** last `cmake --build` run failed because of an unrelated compile error in `modules/009_tools/office_docs/office_docs.cpp` from Sub-Agent B (Office AI integration). The error was `no matching function for call: std::string read(const std::string & path)` - likely a name collision between the module function and something else. Not from my fixes. Small triage: rename the function to `read_text` or wrap in `office_docs::` fully qualified use everywhere. First thing to fix on resume.

---

## Where the maze-game test left off (v11)

**Plan:** 11 tickets from a bare user prompt (planner produced the shape rules automatically because they live in `tool_router.cpp::register_defaults_unlocked()`'s `ticket_plan.prompt_template`). Backed up at `/home/jwoods/work/.ac9_test_001-safekeeping/tickets.agile.plan_v11.pristine` and inside the project as `.tickets.agile.plan_v11.bak`.

**Board when the watcher stopped the runner:**
- T-1 done   Create robot sprite animation frames        [GHOST - image in .ac9_images/, NOT in assets/]
- T-2 done   Create wall tile asset                      [OK - assets/wall.png]
- T-3 done   Create floor tile asset                     [OK - assets/floor.png]
- T-4 done   Create exit tile asset                      [GHOST for a different reason - PNG in .ac9_images/ only]
- T-5 done   Create trap tile asset                      [OK - assets/trap.png]
- T-6 done   Create battery pickup item sprite           [OK - assets/battery_pickup.png]
- T-7 done   Write main.cpp web server entry point       [GHOST - coder ran `ls && find`, no main.cpp on disk]
- T-8 done   Implement maze generation algorithm         [GHOST - misfired to image_gen, PNG in .ac9_images/]
- T-9 done   Create HTML page with embedded JS engine    [GHOST - misfired to image_gen, PNG in .ac9_images/]
- T-10 done  Write CMakeLists.txt build configuration    [OK - CMakeLists.txt on disk]
- T-11 blocked  Add smoke test script for game server    (Layer-2 decompose fired: T-12/T-13/T-14/T-15)
- T-12 blocked  Create tests/smoke_test.sh skeleton      (depth cap fired here, no further decompose)
- T-13/T-14/T-15 todo (never ran)

**Real files on disk** (post-run):
```
/home/jwoods/work/ac9_test_001/CMakeLists.txt
/home/jwoods/work/ac9_test_001/assets/battery_pickup.png
/home/jwoods/work/ac9_test_001/assets/floor.png
/home/jwoods/work/ac9_test_001/assets/trap.png
/home/jwoods/work/ac9_test_001/assets/wall.png
```

The three uncommitted fixes in `server.cpp` target the exact three failure modes visible above (misroute code -> image_gen, ghost pass on exploration shell, wrong save location for art). After they build clean, v12 should produce the actual source files.

**Verified NOT crashing anymore:** `!!!! CODER MODEL FAILED !!!!` marker count in `scratchpad/ac9_test_001_logs/ac9.log` during v11 = zero. The OOM retry loop from commit `20f7876d` is working.

---

## Live services (all shut down as of pause)

At the moment of pause:
- ac9 process: killed
- coolwsd (unsupervised, PID 585716): killed
- coolforkit-ns children: killed
- `~/work/cool-runtime/wopi_host.py` (Python stub, PID 604491): killed
- Ports 8080, 9980, 9998: clear

On resume the FIRST fresh ac9 launch will:
1. Bind :8080
2. Spawn its own supervised coolwsd via `office_suite::init(9980, "/home/jwoods/work/cool-runtime/docs")` from `main.cpp` (after the rebuild lands the Collabora commits)
3. Native WOPI endpoints replace the Python stub - do NOT restart the stub

**Files the operator asked to delete but I have not:**
- `/home/jwoods/work/cool-runtime/wopi_host.py` (safe to `rm`, replaced by native handlers)
- The `~/work/CollaboraOnline/` shallow clone is fine to leave; it is reference material

---

## Sub-agents that ran this session and their deliverables

**Research reports** (all in `scratchpad/`, all committed to git via untracked scratch dir - the reports themselves are NOT in the repo, they are local artifacts):
- `office_suite_research.md` - 30 KB, 8 options compared, recommends Collabora Online + drawio hybrid
- `sound_tts_research.md` - 10 KB, recommends Kokoro-82M via TTS.cpp for TTS + Stable Audio Open for SFX
- `subject_consistency_research.md` - 40 KB, recommends Level 1 (seed lock + img2img + character sheet) then Level 3 (LoRA training via ostris/ai-toolkit)
- `mouser_routing_bug.md` - from an earlier session, diagnosis of Mouser routing miss + always-on health analysis
- `sdcpp_build/` - build cache from an earlier session, ignore

**Implementation sub-agents** (deliverables on `origin/main` unless noted):
- Docs site build: `6d57b4de` + `7a4f8f90`
- Subject consistency Level 1+3: `1306d701` through `b981c4d7` (six commits)
- Collabora vendorization: `3998ea7a` + `afeba418` + `3c38cb95`
- Office AI integration: `b97bf460`
- Em-dash purge workflow: 13-agent workflow, results across `7703dd5d` + `033ac8e8`
- Collabora demo standup + Python WOPI stub: NOT in repo (was superseded by native vendorization)

---

## Operator policies (memory files - `/home/jwoods/.claude/projects/-home-jwoods-work-Autoclank9001/memory/`)

Read the memory files before doing anything. Non-negotiable:

- `feedback_do_what_asked.md` - do not add hidden caps to searches, walks, scans. Exhaustive by default. Cost is not a stop signal.
- `feedback_stop_asking.md` - if a design has 2-3 sensible defaults and you would mark one "Recommended", skip AskUserQuestion and just build.
- `feedback_dont_stop.md` - long autonomous tasks keep grinding through conversational turns. Side quests do not pause the primary loop. Commit + push under single-user identity (no Co-Authored-By footer).
- `feedback_abliterated_everywhere.md` - every model plugged into ac9 must be abliterated / uncensored / de-safetied. LLM, image, TTS, sound, vision. Verify per-model when researching.
- `feedback_no_em_dash.md` - ac9 never emits em dash (U+2014), en dash (U+2013), or horizontal bar (U+2015). Plain hyphen only. Enforcement stack is documented in the memory file itself.

Also read `/home/jwoods/work/Autoclank9001/CLAUDE.md` (the golden rule: Claude never touches ac9's work product under `~/work/Quantiprize/**` or the target project `~/work/ac9_test_001/`).

---

## Resume steps (do these in order)

**Step 1 - Triage the office_docs compile error.**
```bash
cd /home/jwoods/work/Autoclank9001/build && cmake --build . -j$(nproc) 2>&1 | grep 'error:' | head
```
Look for the `std::string read` collision. Likely fix: fully qualify the call site (`office_docs::read(...)` instead of `read(...)`) or rename the function to `read_text`. Do NOT revert commit `b97bf460`.

**Step 2 - Verify the three uncommitted server.cpp fixes still make sense.**
```bash
cd /home/jwoods/work/Autoclank9001
git diff modules/010_interface/server.cpp | less
```
The three fixes are self-contained blocks with `!!!! ROUTER MISFIRE !!!!`, `!!!! GHOST PASS DETECTED !!!!`, and `auto-defaulting save_to` diagnostic emits. Read the surrounding comments to remember why each exists (also captured above in "Uncommitted work").

**Step 3 - Build and commit.**
```bash
cmake --build build -j$(nproc)   # after fixing office_docs
git add modules/010_interface/server.cpp
git commit -m "server.cpp: router code-smell guard + ghost-pass detection + save_to auto-default

Three fixes that surfaced when v11 completed with ghost passes:
- image_gen misfire on 'Implement maze generation algorithm' style ticket
  bodies (Chroma rendered garbage PNG from ticket body as subject); reject
  when subject contains code tokens and fall through to coder path.
- Coder emitted 'ls -la && find' on T-7 'Write main.cpp' and exited 0;
  ticket marked done despite no source file written. ticket_run_execute
  now checks for write-shaped ops in the shell command and forces
  success=false on pure exploration.
- Robot sprite PNG landed in .ac9_images/ instead of assets/ because
  router did not extract save_to. Default save_to to assets/<slug>.png
  when subject mentions sprite/tile/icon/asset/artwork/texture/pickup."
git push
```

**Step 4 - Nuclear reset target project.**
```bash
rm -rf /home/jwoods/work/ac9_test_001
mkdir -p /home/jwoods/work/ac9_test_001
rm -f /home/jwoods/work/Autoclank9001/005_context/sessions/active.txt
```

**Step 5 - Restart ac9 (which will supervise its own coolwsd).**
```bash
cd /home/jwoods/work/Autoclank9001
nohup ./ac9 > scratchpad/ac9_test_001_logs/ac9.log 2>&1 &
```
Watch for `!!!! OFFICE SUITE FAILED TO START !!!!` in the log - if present, coolwsd auto-detect failed and needs `AC9_COLLABORA_BINARY` env override. Default path: `/home/jwoods/work/collabora-prefix/usr/bin/coolwsd`.

**Step 6 - Create and activate a fresh session (`v12`).**
```bash
sid=$(curl -s -X POST http://127.0.0.1:8080/api/sessions \
  -H 'Content-Type: application/json' \
  -d '{"name":"ac9_test_001_v12","root_dir":"/home/jwoods/work/ac9_test_001"}' \
  | python3 -c 'import json,sys; print(json.load(sys.stdin)["id"])')
curl -s -X POST http://127.0.0.1:8080/api/sessions/$sid/activate -H 'Content-Type: application/json' -d '{}'
```

**Step 7 - Send the bare planning prompt.**
Use `scratchpad/ac9_test_001_logs/chat.sh` with the operator's original prompt text. The planner prompt SHOULD NOT include hand-typed shape rules - those live in `tool_router.cpp::register_defaults_unlocked()`'s `ticket_plan.prompt_template` template now.

Prompt to send (verbatim):
```
Plan a browser-based maze game project. The game is a C++ web server on port 9002, bound to 0.0.0.0. It serves one HTML page with embedded JavaScript. Player is an animated robot sprite controlled by arrow keys. The maze is grid-based with walls, dead ends, and traps. The robot has a battery that always drains and drains faster while moving. Battery pickups on the maze recharge the robot. Game ends when the battery reaches zero or the robot reaches the exit tile. Use CMake and include a smoke test.
```

**Step 8 - Backup + kick off runner.**
```bash
cp /home/jwoods/work/ac9_test_001/.tickets.agile /home/jwoods/work/ac9_test_001/.tickets.agile.plan_v12.bak
cp /home/jwoods/work/ac9_test_001/.tickets.agile /home/jwoods/work/.ac9_test_001-safekeeping/tickets.agile.plan_v12.pristine
curl -s -X POST http://127.0.0.1:8080/api/tickets/run/start -H 'Content-Type: application/json' -d '{"cwd":"/home/jwoods/work/ac9_test_001"}'
```

**Step 9 - Start the watcher (background, notifies on runner stop).**
```bash
/home/jwoods/work/Autoclank9001/scratchpad/ac9_test_001_logs/watch_runner.sh > /home/jwoods/work/Autoclank9001/scratchpad/ac9_test_001_logs/watch_runner.log 2>&1 &
```

**Step 10 - Verify Office is reachable BEFORE the runner grinds too long.**
Point the operator's laptop browser at:
```
http://192.168.1.229:8080/            # ac9 main UI - click Office menu
http://192.168.1.229:9980/hosting/discovery   # coolwsd health, must return WOPI XML
```
The Office menu in ac9's own UI now opens iframes pointed at `<ac9_own_url>/wopi/files/Untitled.<ext>&access_token=<office_suite::access_token()>`. If it works, the vendorization landed cleanly.

---

## Iteration hypothesis for v12

If the three uncommitted fixes work:

- T-8 and T-9 (code tickets) will no longer misroute to image_gen because the code-smell guard rejects the router's pick and falls back to coder.
- T-7 will no longer ghost-pass because ticket_run_execute forces `success=false` on shell commands with no write op.
- T-1 will land the robot sprite at `assets/robot.png` because save_to auto-defaults.

Expected outcome for v12: 6 art tickets + main.cpp + maze algo + index.html + CMakeLists + smoke test all writing real files. If everything lands, the operator can then `cd ~/work/ac9_test_001 && cmake -B build . && cmake --build build && ./build/maze_game` and browse `http://192.168.1.229:9002/` to actually play the game.

If T-11 (smoke test) still blocks: this run is the first real test of the smoke-test shell path with the ghost-pass detector active. If the coder writes a real `smoke_test.sh` it will pass; if it writes only exploration commands it will now block correctly instead of ghost-passing.

---

## Things worth remembering that are not obviously in the code

- Nuclear reset semantics: `rm -rf <target>`, `mkdir -p <target>`, `rm active.txt` (session pointer). Do NOT `rm` the session SQLite files themselves - that risks other sessions the operator uses. Also do NOT touch `~/work/.ac9_test_001-safekeeping/` - the pristine backups live there.
- `ticket_run_execute` posts to `http://127.0.0.1:<port>/api/chat` internally to run each ticket. The runner and the chat handler are in the same process, but the network hop is real - a bug in one shows up as a mysterious blocking behavior in the other.
- The tool router's `ticket_plan` prompt_template embeds every shape rule the operator once had to type by hand. When you tweak how tickets look, edit that template.
- Image gen's `save_to` was the operator's original directive. Auto-defaulting is a safety net; the tool_router should ideally always emit one for asset requests. If v12 shows the LLM still not extracting save_to for asset tickets, tighten the tool description in `register_defaults_unlocked()`.
- Every LLM call in ac9 delegates to `coder::generate` OR `planner::generate`. If either OOMs, everything downstream (classify, stylize, entities, etc.) OOMs. The retry loop at `coder.cpp` load-time is the current mitigation. If v12 shows OOM again, look at `hardware::pick_placement` for qwen35 - the layer-split (`LLAMA_SPLIT_MODE_LAYER`) fights the CLAUDE.md hardware discipline (`LLAMA_SPLIT_MODE_NONE` + UVA). The operator may want to revisit that trade-off.
- The em-dash policy is aggressive. Any new string literal, comment, or log message you write MUST use plain hyphen. The workflow's system-prompt bans are cumulative - the runner cache may replay old model outputs; if you see an em dash post-restart, grep `_events.log` files and delete the offender before rerunning.

---

## Contact points if you get stuck

- Operator: `jwoods` (GitHub `Ruphobia`). Machine `jiraiya` at `192.168.1.229` on the LAN.
- Domain (owned by operator): `autoclank` (TLD to be decided). Docs site currently at `https://ruphobia.github.io/Autoclank9001/`.
- Collabora runtime installed at `/home/jwoods/work/collabora-prefix/`. Source clone at `/home/jwoods/work/CollaboraOnline/`.
- `~/work/cool-runtime/` contains coolwsd's runtime state; do not delete the child-roots dir mid-run.

---

*End of resume doc. If you make substantive changes during your session, append them here so the next resume starts closer to reality.*
