---
title: Tool router
parent: Features
nav_order: 1
---

# Tool router
{: .no_toc }

The newest and most load-bearing piece of the pipeline. As of the
`tool_router` + `prompt_shaper` landing this replaced a growing pile
of per-tool regex intent detectors - `detect_ticket_intent`,
`detect_image_gen_intent`, `detect_image_edit_intent`, the Mouser
keyword sniffer, and so on - with **one** call into a small
classifier model (`planner-30b` / qwen35) that reads the cleaned user
prompt plus optional wiki and dictionary blocks and emits strict JSON
naming which registered tool to invoke, what arguments to pass, how
confident it is, and why.

## How dispatch works

The router produces a `Choice { tool, args, confidence, reason }`.
The chat dispatch layer in `server.cpp` acts on the confidence:

- `confidence >= 0.7` - dispatch to the picked tool with the shaped
  prompt.
- `confidence <  0.7` - fall through to the legacy regex routers as
  a safety net for crystal-clear cases (a bare `T-3` ticket id, a
  `*.png` file reference).
- `tool == "none"` - fall through to `classify::analyze` and the
  full understanding stack.

## Prompt shaper

The second half of the same design. Every registered tool carries a
`prompt_template` describing the ideal shape of the LLM input for
that tool ("one sprite = one ticket, no tool names, save to
`assets/<name>.png`"). When the router picks a tool the shaper
substitutes `{user_prompt}` and named args into the template and
returns the system + user message pair that the tool's downstream
LLM call will actually receive. The router never dispatches anything
itself; it only decides.

## Adding a tool

Register a `tool_router::Tool` with a one-line description, a
JSON-shape hint for the args, and the prompt template. See
[the FAQ](../faq.html) for the concrete steps.
