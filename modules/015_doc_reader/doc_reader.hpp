#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Document reader (RAG, retrieval flavor): ingests user documents into
// a per-project SQLite FTS5 index so the pipeline can ground answers
// in the user's own files. Distinct from 007_knowledge (Wikipedia) and
// from 005_context (per-session chat log).
//
// Plain text, Markdown, and HTML are handled in-process. Other formats
// (PDF, DOCX, etc.) are delegated to pandoc via 014_format_translators
// when it is on PATH; when it is not the ingest call returns false and
// leaves the index untouched.
//
// The index lives at <project>/.tool/rag/index.sqlite.
namespace doc_reader {

struct Document {
    int64_t      id          = 0;     // sqlite rowid
    std::string  path;                // absolute path, as ingested
    std::string  format;              // "text", "markdown", "html", "pandoc", ...
    int64_t      ingested_at = 0;     // unix seconds
    size_t       chunk_count = 0;     // number of chunks stored for this doc
};

struct Chunk {
    int64_t      doc_id   = 0;
    int64_t      chunk_id = 0;        // chunks_fts rowid
    size_t       chunk_ix = 0;        // 0-based ordinal within doc
    std::string  text;                // body of the chunk
};

struct Hit {
    Document      document;
    Chunk         chunk;
    double        score = 0.0;        // FTS5 bm25 score (lower is better)
    std::string   snippet;            // FTS5 snippet() with [[ ]] highlights
};

// Idempotent. Opens / creates the index. project_root may be empty to
// use the current working directory.
void open(std::string_view project_root);
void close();

// Ingest one file. Returns true if the file was indexed, false if the
// format is unsupported or pandoc is needed but missing. Throws on IO
// errors or sqlite errors.
bool ingest(std::string_view file_path);

// Remove a document (and its chunks) from the index by id.
void forget(int64_t doc_id);

// Drop the entire index. Useful between tests and for "rebuild it".
void reset();

std::vector<Document> list();

// Full-text search over ingested chunks. `top_k` caps the number of
// returned hits; default is reasonable for a chat grounding step.
std::vector<Hit> search(std::string_view query, size_t top_k = 8);

}
