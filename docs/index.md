---
title: Home
layout: home
nav_order: 1
description: "Autoclank 9001 - a local-first, multi-model orchestration workbench."
permalink: /
---

# Autoclank 9001
{: .fs-9 }

Local-first AI orchestrator. Small abliterated specialists arranged in a
pipeline, each doing one job well, all running on your own hardware.
{: .fs-6 .fw-300 }

[Get Started](install.html){: .btn .btn-primary .fs-5 .mb-4 .mb-md-0 .mr-2 }
[GitHub Repo](https://github.com/Ruphobia/Autoclank9001){: .btn .fs-5 .mb-4 .mb-md-0 }

---

## What it is

`ac9` is a many-model orchestration workbench for real work, not chat
theater. A loose prompt walks through cleanup, entity recognition,
intent classification, dictionary grounding, precise rewrites, an
expertise router, and a disambiguation step before it ever reaches an
answering model. Every stage is visible in the chat trail. The
answering model is then handed real tools - an image generator, an
image editor, a KiCad-aware component lookup, a self-healing ticket
runner - so it can finish a job instead of describing one.

## Why it exists

Hosted chat models conflate two different jobs: being agreeable, and
being correct. For engineering and science only the second one matters.
When the empirical answer is inconvenient the alignment layer files the
corners off; the result reads fluently but is no longer reliable input
to real work. `ac9` separates the jobs. Local uncensored weights answer
factually. Any phrasing the user wants on top is added by the user, not
by the model. Nothing leaves the box. No provider system prompt, no
output-side filter, no silently swapped-in "safer" model.

## Who it's for

Engineers, researchers, and hobbyists who want a multi-GPU box on a
desk to be their own agentic coding lab. Anyone who has run into the
"model got helpful right up to the point the work was about to do
something real, then it sabotaged" pattern. If you have a pair of
P100s (or better) sitting in a Linux workstation and want to run a
30B-class coder with a 14B understanding stack and Chroma1-HD image
generation, all cooperating through one web UI, `ac9` is the workbench
you point at your project directory.

---

## Explore

The sidebar on the left is the map. **Features** is the current tour
of what `ac9` actually does today. **Demos** is where end-to-end runs
land as they get recorded. **Install** walks you through the build.
**FAQ** answers the questions the operator gets asked most often.
