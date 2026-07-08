---
title: Install
nav_order: 4
---

# Install
{: .no_toc }

`ac9` is a single C++ binary. Build it out of the repo, drop a
credentials file into `settings/`, and run it. No package repo, no
installer script — CMake and a C++20 toolchain are the whole story.

## Requirements

- Linux (Ubuntu 22.04 / 24.04 tested).
- A C++20 compiler (`g++` 12 or newer).
- CMake 3.20+.
- CUDA 12.x if you want the GPU pathway (you do; the CPU fallback
  exists but is not the point of the project).
- One or more NVIDIA GPUs. The bench box is a pair of P100s. Any
  card with CUDA UVA support will host any model — the unified
  memory pathway carries VRAM overflow to system RAM at the cost of
  some throughput.

Model weights (Qwen3-Coder-30B-A3B, Qwen2.5-14B, Chroma1-HD,
Qwen3-VL-8B and friends) are pulled at first run by the `data::`
bootstrap subsystem — you do not have to fetch them by hand.

## Build

```bash
git clone https://github.com/Ruphobia/Autoclank9001
cd Autoclank9001

cmake -B build .
cmake --build build -j
```

The binary lands at `build/ac9`. Run it from the repo root:

```bash
./build/ac9
```

The web UI opens on `http://127.0.0.1:8787` by default. Point a
browser at it. Every session lives per browser tab.

## What CMakeLists.txt does

The top-level `CMakeLists.txt` (see the repo root) walks every
subdirectory under `modules/` and pulls in whichever ones declare
themselves as `ac9` modules. That is why adding a new tool is
mostly a matter of creating a folder under `modules/` — the build
system finds it automatically.

## Docker

A container image is a plausible future path, especially for the
GPU pathway with the CUDA runtime already baked in. It is **not**
supported today. If you want a reproducible environment right now,
build in a fresh Ubuntu VM against the requirements list above.
