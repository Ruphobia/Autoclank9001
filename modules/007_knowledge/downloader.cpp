#include "downloader.hpp"

#include <nlohmann/json.hpp>
#include <curl/curl.h>

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace kb {
namespace fs = std::filesystem;
using json   = nlohmann::json;

namespace {

struct WriteCtx {
    int             fd;
    std::uint64_t   file_offset;
    std::uint64_t   bytes_written;
    std::atomic<bool> * stop_flag;
};

std::size_t curl_write_cb(char * ptr, std::size_t size, std::size_t nmemb, void * userdata) {
    auto * ctx = static_cast<WriteCtx *>(userdata);
    if (ctx->stop_flag && ctx->stop_flag->load()) return 0;  // abort transfer
    const std::size_t n = size * nmemb;
    std::size_t remaining = n;
    const char * buf = ptr;
    while (remaining > 0) {
        const ssize_t w = ::pwrite(ctx->fd, buf,
                                   remaining,
                                   static_cast<off_t>(ctx->file_offset + ctx->bytes_written));
        if (w < 0) {
            if (errno == EINTR) continue;
            return 0;
        }
        ctx->bytes_written += static_cast<std::uint64_t>(w);
        buf       += w;
        remaining -= static_cast<std::size_t>(w);
    }
    return n;
}

int curl_progress_cb(void * userdata, curl_off_t, curl_off_t, curl_off_t, curl_off_t) {
    auto * stop_flag = static_cast<std::atomic<bool> *>(userdata);
    return (stop_flag && stop_flag->load()) ? 1 : 0;  // non-zero aborts
}

}

Downloader::Downloader()  = default;
Downloader::~Downloader() { stop(); if (dest_fd_ >= 0) { ::close(dest_fd_); dest_fd_ = -1; } }

void Downloader::start(std::string url,
                       fs::path dest_path,
                       fs::path state_path,
                       int num_workers,
                       std::uint64_t chunk_size_bytes) {
    {
        std::lock_guard<std::mutex> lk(mtx_);
        if (active_.load() || done_.load()) return;

        state_path_  = std::move(state_path);
        num_workers_ = num_workers;
        error_.clear();

        const bool resumed = load_state_locked();
        if (!resumed) {
            std::uint64_t total = 0;
            std::string head_err;
            if (!head_total_size(url, total, head_err)) {
                error_ = "HEAD failed: " + head_err;
                return;
            }
            state_.url         = std::move(url);
            state_.dest_path   = std::move(dest_path);
            state_.total_bytes = total;
            state_.chunk_size  = chunk_size_bytes;
            state_.chunks.clear();
            std::uint64_t offset = 0;
            while (offset < total) {
                Chunk c;
                c.offset   = offset;
                c.length   = std::min<std::uint64_t>(chunk_size_bytes, total - offset);
                c.complete = false;
                state_.chunks.push_back(c);
                offset += c.length;
            }
            save_state_locked();
        }

        // Preallocate / open destination.
        fs::create_directories(state_.dest_path.parent_path());
        dest_fd_ = ::open(state_.dest_path.c_str(),
                          O_WRONLY | O_CREAT,
                          0644);
        if (dest_fd_ < 0) {
            error_ = "open " + state_.dest_path.string() + ": " + std::strerror(errno);
            return;
        }
        // Make sure the file is at least total_bytes in size (sparse OK).
        if (::ftruncate(dest_fd_, static_cast<off_t>(state_.total_bytes)) != 0) {
            error_ = "ftruncate: " + std::string(std::strerror(errno));
            ::close(dest_fd_); dest_fd_ = -1;
            return;
        }

        next_idx_.store(0);
        stop_flag_.store(false);
        active_.store(true);
        done_.store(false);
    }

    // Launch workers outside the lock.
    workers_.clear();
    workers_.reserve(static_cast<std::size_t>(num_workers_));
    for (int i = 0; i < num_workers_; ++i) {
        workers_.emplace_back(&Downloader::worker_loop, this);
    }
}

void Downloader::stop() {
    stop_flag_.store(true);
    for (auto & t : workers_) {
        if (t.joinable()) t.join();
    }
    workers_.clear();
    active_.store(false);
}

Downloader::Status Downloader::status() const {
    std::lock_guard<std::mutex> lk(mtx_);
    Status s;
    s.total_bytes  = state_.total_bytes;
    s.chunks_total = state_.chunks.size();
    s.active       = active_.load();
    s.done         = done_.load();
    s.error        = error_;
    std::uint64_t down = 0;
    std::size_t   comp = 0;
    for (const auto & c : state_.chunks) {
        if (c.complete) { down += c.length; ++comp; }
    }
    s.downloaded_bytes = down;
    s.chunks_complete  = comp;
    return s;
}

void Downloader::worker_loop() {
    for (;;) {
        if (stop_flag_.load()) break;

        const std::size_t idx = next_idx_.fetch_add(1);
        std::size_t total_chunks;
        Chunk       my_chunk;
        {
            std::lock_guard<std::mutex> lk(mtx_);
            total_chunks = state_.chunks.size();
            if (idx >= total_chunks) break;
            my_chunk = state_.chunks[idx];
            if (my_chunk.complete) continue;
        }

        std::string err;
        if (!download_chunk(my_chunk, err)) {
            std::lock_guard<std::mutex> lk(mtx_);
            if (error_.empty()) error_ = "chunk " + std::to_string(idx) + ": " + err;
            stop_flag_.store(true);
            break;
        }

        {
            std::lock_guard<std::mutex> lk(mtx_);
            state_.chunks[idx].complete = true;
            save_state_locked();
        }
    }

    // Last worker to finish marks done and syncs/closes.
    bool any_active = false;
    {
        std::lock_guard<std::mutex> lk(mtx_);
        for (const auto & c : state_.chunks) {
            if (!c.complete) { any_active = true; break; }
        }
        if (!any_active && !stop_flag_.load()) {
            if (dest_fd_ >= 0) {
                ::fsync(dest_fd_);
                ::close(dest_fd_);
                dest_fd_ = -1;
            }
            done_.store(true);
        }
    }
}

bool Downloader::head_total_size(const std::string & url,
                                 std::uint64_t & out_size,
                                 std::string & err) {
    CURL * c = curl_easy_init();
    if (!c) { err = "curl_easy_init failed"; return false; }
    char errbuf[CURL_ERROR_SIZE] = {0};
    curl_easy_setopt(c, CURLOPT_URL,             url.c_str());
    curl_easy_setopt(c, CURLOPT_NOBODY,          1L);
    curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION,  1L);
    curl_easy_setopt(c, CURLOPT_FAILONERROR,     1L);
    curl_easy_setopt(c, CURLOPT_ERRORBUFFER,     errbuf);
    curl_easy_setopt(c, CURLOPT_USERAGENT,       "tool/kb-downloader");
    CURLcode rc = curl_easy_perform(c);
    if (rc != CURLE_OK) {
        err = errbuf[0] ? errbuf : curl_easy_strerror(rc);
        curl_easy_cleanup(c);
        return false;
    }
    curl_off_t cl = 0;
    if (curl_easy_getinfo(c, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &cl) != CURLE_OK || cl <= 0) {
        err = "no content-length";
        curl_easy_cleanup(c);
        return false;
    }
    out_size = static_cast<std::uint64_t>(cl);
    curl_easy_cleanup(c);
    return true;
}

bool Downloader::download_chunk(const Chunk & c, std::string & err) {
    CURL * curl = curl_easy_init();
    if (!curl) { err = "curl_easy_init failed"; return false; }

    char errbuf[CURL_ERROR_SIZE] = {0};
    WriteCtx wctx{ dest_fd_, c.offset, 0, &stop_flag_ };

    char range[64];
    std::snprintf(range, sizeof(range), "%llu-%llu",
                  static_cast<unsigned long long>(c.offset),
                  static_cast<unsigned long long>(c.offset + c.length - 1));

    curl_easy_setopt(curl, CURLOPT_URL,            state_.url.c_str());
    curl_easy_setopt(curl, CURLOPT_RANGE,          range);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,      &wctx);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR,    1L);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER,    errbuf);
    curl_easy_setopt(curl, CURLOPT_USERAGENT,      "tool/kb-downloader");
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS,     0L);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, curl_progress_cb);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA,     &stop_flag_);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT,  1024L);   // bytes/sec
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME,   60L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT,   30L);

    CURLcode rc = curl_easy_perform(curl);
    if (rc != CURLE_OK) {
        err = errbuf[0] ? errbuf : curl_easy_strerror(rc);
        curl_easy_cleanup(curl);
        return false;
    }
    if (wctx.bytes_written != c.length) {
        err = "short read: got " + std::to_string(wctx.bytes_written) +
              " want " + std::to_string(c.length);
        curl_easy_cleanup(curl);
        return false;
    }
    curl_easy_cleanup(curl);
    return true;
}

void Downloader::save_state_locked() const {
    json j;
    j["url"]         = state_.url;
    j["dest_path"]   = state_.dest_path.string();
    j["total_bytes"] = state_.total_bytes;
    j["chunk_size"]  = state_.chunk_size;
    json arr = json::array();
    for (const auto & c : state_.chunks) {
        arr.push_back({{"offset", c.offset}, {"length", c.length}, {"complete", c.complete}});
    }
    j["chunks"] = std::move(arr);

    const fs::path tmp = state_path_;
    fs::path tmp_with_suffix = tmp;
    tmp_with_suffix += ".tmp";
    {
        std::ofstream out(tmp_with_suffix);
        out << j.dump();
    }
    fs::rename(tmp_with_suffix, state_path_);
}

bool Downloader::load_state_locked() {
    if (!fs::exists(state_path_)) return false;
    std::ifstream in(state_path_);
    if (!in) return false;
    json j;
    try { in >> j; } catch (...) { return false; }
    try {
        state_.url         = j.at("url").get<std::string>();
        state_.dest_path   = j.at("dest_path").get<std::string>();
        state_.total_bytes = j.at("total_bytes").get<std::uint64_t>();
        state_.chunk_size  = j.at("chunk_size").get<std::uint64_t>();
        state_.chunks.clear();
        for (const auto & c : j.at("chunks")) {
            Chunk ch;
            ch.offset   = c.at("offset").get<std::uint64_t>();
            ch.length   = c.at("length").get<std::uint64_t>();
            ch.complete = c.at("complete").get<bool>();
            state_.chunks.push_back(ch);
        }
    } catch (...) {
        return false;
    }
    return true;
}

}
