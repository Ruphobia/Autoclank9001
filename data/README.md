<!-- SPDX-License-Identifier: GPL-3.0-or-later -->
# data/

Everything ac9 needs at runtime that isn't source code lives here as
content-addressed chunks. Nothing under this directory is committed to
git (see the repo's `.gitignore`).

Every file in this directory is named `<sha256>.bin` and is at most
1.5 GB. There are two kinds of files:

- **`<sha>.bin`** chunks are pieces of larger assets (models,
  dictionaries, thesaurus, wikipedia archive). The `<sha>` is the
  SHA-256 of that chunk's own bytes.
- **`manifest.json`** maps semantic roles (`coder`, `physics`,
  `planner-4b`, `dictionary`, `wikipedia`, ...) to (a) the SHA-256 of
  the reassembled full file, (b) the ordered list of chunk SHAs that
  reassemble it, (c) size and a human-readable filename.

## Why content-addressed chunks?

The tool loads big things: a coder LLM is around 10 GB, wikipedia is
around 50 GB, thesaurus and dictionary are together around 70 MB. All
of them come from external sources and change slowly. Content addressing
buys us four things at once:

- **Verification.** A chunk's filename IS its SHA-256, so a bit-flip on
  disk or a corrupted download can't hide. Reassembly checks each
  chunk's hash and then the reassembled full-file hash against the
  manifest before anything else runs.
- **Deduplication.** If two roles resolve to the same underlying file
  (say a coder and a fallback planner share the same base model quant),
  they share the same chunk on disk. No copies.
- **Idempotence.** Running the pipeline twice on the same input is a
  no-op: every chunk that already exists on disk is left alone.
- **Cache-friendliness.** A local cache or a downstream mirror keyed
  by SHA-256 is trivial to serve: the URL and the filename are the same
  hash, no state, no metadata lookup, no coordination.

## Why 1.5 GB per chunk?

Two limits pushed us here. GitHub Releases caps a single asset at
2 GiB (2,147,483,648 bytes). Any bigger and the release upload fails.
And smaller chunks make retries cheaper: a broken download at 90% loses
1.4 GB, not 10 GB. We could go smaller (say, 100 MB) but then a
10 GB model becomes 100 chunks instead of 7, which bloats the manifest
and multiplies HTTP roundtrips. 1.5 GB is the practical sweet spot
that stays well under the 2 GiB GitHub cap and keeps chunk counts low.

## Where do these files come from?

Right now, from the pipeline in `ac9`'s data subcommand:

```
ac9 chunk <role> <human_name> /path/to/input.gguf
```

This computes the input's SHA-256, splits it into 1.5 GB pieces,
computes each piece's SHA-256, and moves them into `data/<sha>.bin`.
It updates `manifest.json` atomically with the new role entry.

## How they'll be served in the future

The plan is **GitHub Releases**. GitHub Releases has effectively unlimited
storage and bandwidth (fair-use, undocumented, historically forgiving),
zero cost, and CDN-backed downloads via Fastly. Every ac9 release
version will publish a matching `models-v<N>` release with the SHA-named
chunks as assets. The manifest gets a `sources` array added:

```json
{
  "coder": {
    "human_name": "Qwen2.5-Coder-14B-Instruct-abliterated.Q5_K_M.gguf",
    "sha256": "abc123...",
    "size_bytes": 10508874240,
    "chunk_count": 7,
    "chunks": ["e91a...", "b73d...", "..."],
    "sources": [
      "https://github.com/Ruphobia/Autoclank9001/releases/download/models-v1/{chunk}.bin",
      "magnet:?xt=urn:btih:..."
    ]
  }
}
```

On first run the tool checks `manifest.json`, then for each missing
chunk pulls from `sources` in order (SHA-verifying each). If GitHub
Releases ever goes away, we add another source URL and everyone still
works. Any user who already has the chunks on disk keeps working
forever regardless of what happens to the hosting.

## Roles

The manifest is the source of truth. As of this writing, the
canonical role names are:

| Role | What it is |
|------|------------|
| `coder` | Codegen model (currently Qwen2.5-Coder-14B) |
| `qwen14b` | Understanding / classify / expertise (Qwen2.5-14B) |
| `physics` | Physics tool model (Qwen3-14B) |
| `chemistry` | Chemistry tool model (ChemLLM-20B) |
| `cleanup` | Prompt cleanup copy-editor (Qwen2.5-1.5B) |
| `vision` | Vision LLM (Qwen3-VL-8B) |
| `vision-mmproj` | Multimodal projector for vision |
| `planner-4b` | Reasoning planner, small (Qwen3-4B-Thinking abliterated) |
| `planner-30b` | Reasoning planner, large (Qwen3-30B-A3B-Thinking abliterated) |
| `dictionary` | Webster + WordNet + Moby thesaurus |
| `wikipedia` | Kiwix ZIM Wikipedia archive |
| `safety` | Hazards and legal JSON |

New roles get added by running `ac9 chunk <role> ...` against a new
asset. Existing role entries are updated by running the same command
with a new SHA (deprecates the old chunks, which can be garbage-collected
via `ac9 data prune`).
