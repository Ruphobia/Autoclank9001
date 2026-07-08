---
title: Mouser integration
parent: Features
nav_order: 6
---

# Mouser integration
{: .no_toc }

Real electronic-component lookup wired into the chat pipeline. When
the user is asking to **find** a specific part ("I need a 3.3 V SMD
regulator, 1 A, switching, low cost") instead of asking a general
electronics question, the `components` module queries the Mouser
Search API and returns real parts with real stock, real datasheets,
and — most importantly — a health analysis.

## Health analysis

Every part comes back with an `analyze_health()` verdict. Anomalies
are surfaced as `Flag { code, severity, message, field }` records:

- `EOL` — the part is end-of-life or obsolete.
- `LOW_STOCK` — availability is below a threshold that would break
  a small build.
- `LONG_LEAD` — the lead time is measured in months.
- `NCNR` — non-cancellable, non-returnable. Do not spec into a
  hobby project without noticing.
- `ROHS` / restriction messages — regulatory anomalies the operator
  will want to see up front.

The flags are emitted verbatim on the SSE `component_health` layer
frame and folded into the answer as a callout, so the operator sees
the anomaly **before** the literal answer to "how many are in
stock?".

## Credentials

The Mouser API key lives in `settings/credentials.json` under
`mouser.api_key`, populated through the web UI at
Settings → API Credentials. When the key is missing the pipeline
degrades gracefully — the chat falls through to the regular
electronics LLM answer instead of failing hard.

## Why it exists

Chat models are great at "how does a buck converter work" and
useless at "what specifically is in stock this week for less than
a dollar." Anything that requires a hit to a live catalog belongs
on a real tool, not on model weights. The Mouser tool is that
real tool.
