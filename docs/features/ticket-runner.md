---
title: Ticket runner
parent: Features
nav_order: 5
---

# Ticket runner
{: .no_toc }

`ac9`'s agile-board runner reads a `.tickets.agile` file at the root
of the target project and walks the todo list one entry at a time.
Each ticket has a body (what to do) and a verify step (usually
`cmake --build .`). The coder model produces a patch, the runner
applies it, the verify step decides `ok` or `fail`. What makes the
runner interesting is what happens on `fail`.

## The self-healing pipeline

Five progressive layers, only escalating to the human at the last
step.

### Layer 0 - checkpoint on success

After every ticket completes with `ok=true`, snapshot the entire
target project directory into a last-good backup at
`<project>/.backup/`. Overwrite atomically. Bounds any single
failure's blast radius to one ticket's worth of change.

### Layer 1 - auto-stop on block

When a ticket ends with `ok=false`, the runner halts immediately.
It does **not** roll forward to the next todo. Every downstream
ticket that runs the same verify would otherwise cascade-fail on
the same broken file. Emits `event: run_paused` on the SSE stream
so the client shows a "blocked, awaiting operator" state.

### Layer 2 - auto-decompose

On block, restore the target project from the last-good backup.
Feed the failing ticket's body to the thinking model (`planner-30b`
or whatever `AC9_PLANNER_ROLE` names) asking it to split the ticket
into 2-5 smaller sub-tickets a small coder can knock out one at a
time. Insert the sub-tickets into `.tickets.agile` in place of the
failing ticket. Resume the run.

### Layer 3 - self-repair (one retry)

If a sub-ticket fails, the planner analyzes the failure log and
produces a fix hypothesis. The coder reads the hypothesis plus
current code state and emits a targeted patch. Runner applies the
patch and re-runs the sub-ticket. One retry, no infinite loops.

### Layer 4 - hard halt

If Layer 3 still fails, the sub-ticket stays blocked and the
runner stops. At this point the operator can read the code, prompt
`ac9` to have its own AI repair the issue through the repair
endpoint, or wipe and restart the pipeline if the failure looks
structural.
