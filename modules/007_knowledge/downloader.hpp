#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace kb {

// Parallel chunked HTTP downloader with on-disk per-chunk resume state.
// Each chunk is downloaded via libcurl with a Range: header, written to the
// destination file at its exact byte offset using pwrite() (no locking).
// Completion of each chunk is persisted to a JSON state file -- killing the
// process at any moment loses at most one in-flight chunk per worker.
class Downloader {
public:
    struct Status {
        std::uint64_t total_bytes      = 0;
        std::uint64_t downloaded_bytes = 0;
        std::size_t   chunks_total     = 0;
        std::size_t   chunks_complete  = 0;
        bool          active           = false;
        bool          done             = false;
        std::string   error;
    };

    Downloader();
    ~Downloader();

    // Start (or resume) the download. Returns immediately; workers run in
    // background threads. Calling start() again while already active is a
    // no-op.
    void start(std::string url,
               std::filesystem::path dest_path,
               std::filesystem::path state_path,
               int num_workers      = 6,
               std::uint64_t chunk_size_bytes = 200ULL * 1024 * 1024);

    // Signal workers to stop and join them. Safe to call any time. Partial
    // state is persisted; next start() with the same state_path resumes.
    void stop();

    Status status() const;

private:
    struct Chunk {
        std::uint64_t offset = 0;
        std::uint64_t length = 0;
        bool          complete = false;
    };

    struct State {
        std::string           url;
        std::filesystem::path dest_path;
        std::uint64_t         total_bytes = 0;
        std::uint64_t         chunk_size  = 0;
        std::vector<Chunk>    chunks;
    };

    void worker_loop();
    bool head_total_size(const std::string & url, std::uint64_t & out_size, std::string & err);
    bool download_chunk(const Chunk & c, std::string & err);
    void save_state_locked() const;
    bool load_state_locked();
    void rebuild_status_locked();

    mutable std::mutex         mtx_;
    State                      state_;
    std::filesystem::path      state_path_;
    int                        num_workers_ = 0;
    std::atomic<std::size_t>   next_idx_{0};
    std::atomic<bool>          stop_flag_{false};
    std::atomic<bool>          active_{false};
    std::atomic<bool>          done_{false};
    std::string                error_;
    std::vector<std::thread>   workers_;
    int                        dest_fd_ = -1;
};

}
