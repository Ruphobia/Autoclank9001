# SPDX-License-Identifier: GPL-3.0-or-later
cmake_minimum_required(VERSION 3.18)
project(ac9 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")

file(GLOB_RECURSE AC9_CPP_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/*.cpp"
    "${CMAKE_SOURCE_DIR}/*.cxx"
    "${CMAKE_SOURCE_DIR}/*.cc"
    "${CMAKE_SOURCE_DIR}/*.c"
)

file(GLOB_RECURSE TOOL_CPP_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/*.hpp"
    "${CMAKE_SOURCE_DIR}/*.hxx"
    "${CMAKE_SOURCE_DIR}/*.hh"
    "${CMAKE_SOURCE_DIR}/*.h"
)

file(GLOB_RECURSE AC9_CUDA_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/*.cu"
)

file(GLOB_RECURSE TOOL_CUDA_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/*.cuh"
)

# Exclude anything under build/CMake artifact dirs or hidden dirs (e.g. .git)
# so in-source configures don't pick up CMake's compiler-probe sources, etc.
# The `kicad/` subtree is KiCad 10.0.4 source, vendored into this repo as
# permanent tool source (GPL-3.0-or-later, same license as tool itself).
# It is excluded from the tool glob for now: KiCad has its own CMake tree
# with dozens of external deps (wxWidgets, OpenCascade, ngspice, ...) and
# grafting it into a single glob-recursive build is follow-up work. The
# integration approach today is subprocess (see modules/340_kicad_bridge/
# and scratchpad/kicad_integration.md).
set(_AC9_EXCLUDE_REGEX "/(build|cmake-build-[^/]*|CMakeFiles|CMakeScripts|_deps|Testing|kicad|llama_cpp|nlohmann_json|scratchpad|\\.[^/]+)/")
list(FILTER AC9_CPP_SOURCES  EXCLUDE REGEX "${_AC9_EXCLUDE_REGEX}")
list(FILTER TOOL_CPP_HEADERS  EXCLUDE REGEX "${_AC9_EXCLUDE_REGEX}")
list(FILTER AC9_CUDA_SOURCES EXCLUDE REGEX "${_AC9_EXCLUDE_REGEX}")
list(FILTER TOOL_CUDA_HEADERS EXCLUDE REGEX "${_AC9_EXCLUDE_REGEX}")

# Vendored SQLite amalgamation is compiled separately (own warning flags,
# own defines); exclude it from the main target's globbed sources.
list(FILTER AC9_CPP_SOURCES  EXCLUDE REGEX "/005_context/sqlite3\\.c$")
list(FILTER TOOL_CPP_HEADERS  EXCLUDE REGEX "/005_context/sqlite3\\.h$")
# Vendored cpp-httplib single header lives next to the web UI; not source.
list(FILTER TOOL_CPP_HEADERS  EXCLUDE REGEX "/010_interface/httplib\\.h$")

# ac9_test owns its own main(); compute its source list now (same tree
# minus tool's main and the web UI), then strip the testing harness out
# of the main tool target so its main() doesn't collide with main.cpp.
set(AC9_TEST_CPP_SOURCES ${AC9_CPP_SOURCES})
list(FILTER AC9_TEST_CPP_SOURCES EXCLUDE REGEX "/main\\.cpp$")
list(FILTER AC9_TEST_CPP_SOURCES EXCLUDE REGEX "/010_interface/")
# status.cpp is UI-adjacent but dependency-free, and the model runtimes
# (coder/physics/chemistry) call status::pulse() per decoded token; link
# it back in so ac9_test resolves.
list(APPEND AC9_TEST_CPP_SOURCES
    "${CMAKE_SOURCE_DIR}/modules/010_interface/status.cpp")
list(FILTER AC9_CPP_SOURCES      EXCLUDE REGEX "/testing/")
list(FILTER TOOL_CPP_HEADERS      EXCLUDE REGEX "/testing/")

if(AC9_CUDA_SOURCES)
    enable_language(CUDA)
    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
    if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
        set(CMAKE_CUDA_ARCHITECTURES native)
    endif()
endif()

if(NOT AC9_CPP_SOURCES AND NOT AC9_CUDA_SOURCES)
    message(FATAL_ERROR "No source files found under ${CMAKE_SOURCE_DIR}")
endif()

add_executable(${PROJECT_NAME}
    ${AC9_CPP_SOURCES}
    ${AC9_CUDA_SOURCES}
    ${TOOL_CPP_HEADERS}
    ${TOOL_CUDA_HEADERS}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/modules"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Wpedantic
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
endif()

if(AC9_CUDA_SOURCES)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
    )
endif()

# =====================================================================
# llama.cpp integration (vendored under modules/llama_cpp/, CUDA sm_60)
# =====================================================================
# Was FetchContent; now vendored in-repo so a fresh clone builds
# without network access. Source pinned at ggml-org/llama.cpp tag b9833,
# stripped of docs/examples/media/tests. See modules/llama_cpp/LICENSE.

# P100 = compute 6.0 = sm_60. Must be in the cache BEFORE
# add_subdirectory, since ggml-cuda calls enable_language(CUDA).
set(CMAKE_CUDA_ARCHITECTURES "60-real" CACHE STRING
    "CUDA arches; 60 = Tesla P100 (sm_60)" FORCE)

set(GGML_CUDA       ON  CACHE BOOL "" FORCE)
set(GGML_CUDA_NCCL  OFF CACHE BOOL "" FORCE)

set(LLAMA_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_TOOLS    OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_SERVER   OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_COMMON   OFF CACHE BOOL "" FORCE)
set(GGML_BUILD_TESTS     OFF CACHE BOOL "" FORCE)
set(GGML_BUILD_EXAMPLES  OFF CACHE BOOL "" FORCE)

# Standalone libmtmd build (vision / multimodal support for 009_tools/vision).
# Skips ffmpeg-dependent video features; image-only is fine for our needs.
set(LLAMA_BUILD_MTMD     ON  CACHE BOOL "" FORCE)
set(MTMD_VIDEO           OFF CACHE BOOL "" FORCE)

add_subdirectory(modules/llama_cpp EXCLUDE_FROM_ALL)

target_link_libraries(${PROJECT_NAME} PRIVATE llama mtmd)
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/modules/llama_cpp/tools/mtmd"
)

# =====================================================================
# nlohmann/json (header-only, MIT) vendored at modules/nlohmann_json/
# =====================================================================
# Was FetchContent; now vendored (single-header, ~900 KB). Pinned to
# nlohmann/json tag v3.11.3.
add_library(nlohmann_json INTERFACE)
add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)
target_include_directories(nlohmann_json INTERFACE
    "${CMAKE_SOURCE_DIR}/modules/nlohmann_json/single_include")

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

# =====================================================================
# SQLite (vendored amalgamation) for 005_context
# =====================================================================
find_package(Threads REQUIRED)

add_library(sqlite3_amalgamation STATIC
    "${CMAKE_SOURCE_DIR}/modules/005_context/sqlite3.c"
)
target_include_directories(sqlite3_amalgamation PUBLIC
    "${CMAKE_SOURCE_DIR}/modules/005_context"
)
target_compile_definitions(sqlite3_amalgamation PRIVATE
    SQLITE_ENABLE_FTS5
    SQLITE_THREADSAFE=1
    SQLITE_DQS=0
    SQLITE_OMIT_LOAD_EXTENSION
    SQLITE_DEFAULT_MEMSTATUS=0
    SQLITE_DEFAULT_WAL_SYNCHRONOUS=1
    SQLITE_LIKE_DOESNT_MATCH_BLOBS
    SQLITE_OMIT_DECLTYPE
    SQLITE_OMIT_PROGRESS_CALLBACK
    SQLITE_OMIT_SHARED_CACHE
    SQLITE_USE_ALLOCA
)
# Silence amalgamation warnings; suppress thread-sanitizer-style noise.
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(sqlite3_amalgamation PRIVATE -w)
endif()
target_link_libraries(sqlite3_amalgamation PRIVATE Threads::Threads)

target_link_libraries(${PROJECT_NAME} PRIVATE sqlite3_amalgamation)

# =====================================================================
# 007_knowledge: libcurl (HTTP) + libzim (Wikipedia ZIM reader)
# =====================================================================
find_package(CURL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)

# =====================================================================
# data: OpenSSL libcrypto for SHA-256 (content-addressed chunk store)
# =====================================================================
find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::Crypto)

# =====================================================================
# 012_hardware: CUDA runtime for enumerate_gpus() (cudaGetDeviceCount,
# cudaMemGetInfo, cudaGetDeviceProperties). cudart is already fetched
# in via ggml-cuda; we just need the link import for the ac9 target.
# =====================================================================
find_package(CUDAToolkit QUIET)
if(CUDAToolkit_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE CUDA::cudart)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HW_HAVE_CUDA=1)
    target_include_directories(${PROJECT_NAME} PRIVATE ${CUDAToolkit_INCLUDE_DIRS})
endif()

# libzim ships as a plain shared library on Ubuntu (libzim-dev). No CMake
# package config in the Noble package, so use find_library/find_path.
find_path(LIBZIM_INCLUDE_DIR NAMES zim/zim.h zim/archive.h
    HINTS /usr/include /usr/local/include)
find_library(LIBZIM_LIB NAMES zim
    HINTS /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib)
if(NOT LIBZIM_INCLUDE_DIR OR NOT LIBZIM_LIB)
    message(FATAL_ERROR
        "libzim not found. Install with: sudo apt install libzim-dev")
endif()
message(STATUS "libzim include: ${LIBZIM_INCLUDE_DIR}")
message(STATUS "libzim library: ${LIBZIM_LIB}")
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBZIM_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBZIM_LIB})

# =====================================================================
# 010_interface: cpp-httplib (vendored) + asset embedding
# =====================================================================
# cpp-httplib uses std::thread; no extra libs needed beyond pthreads
# (already pulled in via Threads::Threads from sqlite block).

set(_IFACE_DIR     "${CMAKE_SOURCE_DIR}/modules/010_interface")
set(_IFACE_GEN_DIR "${CMAKE_BINARY_DIR}/interface_gen")
set(_IFACE_GEN_CPP "${_IFACE_GEN_DIR}/assets_gen.cpp")
set(_IFACE_GEN_HPP "${_IFACE_GEN_DIR}/assets_gen.hpp")

file(GLOB _IFACE_ASSETS CONFIGURE_DEPENDS
    "${_IFACE_DIR}/*.html"
    "${_IFACE_DIR}/*.css"
    "${_IFACE_DIR}/*.js"
    "${_IFACE_DIR}/*.json"
    "${_IFACE_DIR}/*.svg"
    "${_IFACE_DIR}/*.png"
    "${_IFACE_DIR}/*.ico"
)

file(MAKE_DIRECTORY "${_IFACE_GEN_DIR}")
add_custom_command(
    OUTPUT  "${_IFACE_GEN_CPP}" "${_IFACE_GEN_HPP}"
    COMMAND ${CMAKE_COMMAND} -E env python3
            "${_IFACE_DIR}/embed_assets.py"
            "${_IFACE_DIR}"
            "${_IFACE_GEN_CPP}"
            "${_IFACE_GEN_HPP}"
    DEPENDS ${_IFACE_ASSETS}
            "${_IFACE_DIR}/embed_assets.py"
    COMMENT "Embedding 010_interface assets into binary"
    VERBATIM
)

target_sources(${PROJECT_NAME} PRIVATE
    "${_IFACE_GEN_CPP}"
    "${_IFACE_GEN_HPP}"
)
target_include_directories(${PROJECT_NAME} PRIVATE "${_IFACE_GEN_DIR}")
# cpp-httplib needs threads (already linked) and gets included as a header.
target_include_directories(${PROJECT_NAME} PRIVATE "${_IFACE_DIR}")

# =====================================================================
# ac9_test: standalone smoke-test runner. Same source tree minus tool's
# main and the web UI; links the same libraries so any module can
# register its own CLI smoke test from a static initializer under
# testing/. No model loads at startup; the per-feature test is what
# decides whether to call its module's init().
# =====================================================================
add_executable(ac9_test
    ${AC9_TEST_CPP_SOURCES}
    ${AC9_CUDA_SOURCES}
    ${TOOL_CPP_HEADERS}
    ${TOOL_CUDA_HEADERS}
)

target_include_directories(ac9_test PRIVATE
    "${CMAKE_SOURCE_DIR}"
    ${LIBZIM_INCLUDE_DIR}
    "${llama_cpp_SOURCE_DIR}/tools/mtmd"
)

target_link_libraries(ac9_test PRIVATE
    llama
    mtmd
    nlohmann_json::nlohmann_json
    sqlite3_amalgamation
    CURL::libcurl
    OpenSSL::Crypto
    ${LIBZIM_LIB}
    Threads::Threads
)

# 012_hardware pulls in the CUDA runtime (cudaGetDeviceProperties etc.);
# ac9_test compiles the same hardware.cpp, so it needs cudart too — mirror
# the ac9 target's CUDA link (was missing, breaking the test-target link).
if(CUDAToolkit_FOUND)
    target_link_libraries(ac9_test PRIVATE CUDA::cudart)
    target_compile_definitions(ac9_test PRIVATE HW_HAVE_CUDA=1)
    target_include_directories(ac9_test PRIVATE ${CUDAToolkit_INCLUDE_DIRS})
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ac9_test PRIVATE -Wall -Wextra -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(ac9_test PRIVATE /W4)
endif()

if(AC9_CUDA_SOURCES)
    set_target_properties(ac9_test PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
    )
endif()
