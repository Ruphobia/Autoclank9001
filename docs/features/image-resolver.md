---
title: Image resolver
parent: Features
nav_order: 4
---

# Image resolver
{: .no_toc }

Answers a deceptively hard question: **which image did the user
mean?** The editor and the LoRA trainer both need a concrete
absolute path, but the operator is going to phrase it however they
please — by filename ("edit `foo.png`"), by nickname ("edit the
black kitty"), or by leaving the reference implicit ("make it more
stormy") and expecting `ac9` to work out which prior generation was
being talked about.

## Cascade

The resolver runs a first-hit-wins cascade:

1. **Explicit filename token** in the prompt (`foo.png`,
   `black-kitty`). Search `.ac9_images/` first, then the entire
   project tree — no depth cap, no file cap, no skip-list of
   vendored directories.
2. **Session state.** The newest `gen_path` or `edit_path` record
   with a real file at least 100 KB. Skips blank sd-cli failure
   PNGs.
3. **Vision description match.** Enumerate every image in the
   project, describe each with `vision::describe()`
   (Qwen3-VL-8B), then let the coder LLM pick the best match
   against the user's prompt. Descriptions are cached to
   `.ac9_images/.descriptions.json` keyed by absolute path plus
   mtime plus size so the vision pass runs once per file.

## Ambiguity

If step 3 turns up two or more plausible matches with similar
scores the resolver returns `Kind::Ambiguous` with the top
candidates. The caller then asks the user which one they meant
instead of guessing wrong.

## Why it matters

Every silent "wrong image" edit is a landmine. The resolver's job
is to trade a small amount of latency (one vision pass per new
image in the project) for a very low miss-rate on ambiguous
references.
