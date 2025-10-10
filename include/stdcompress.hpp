#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace stdfunc::compress {

/**
 * @brief Compress a UTF-8 (or arbitrary) text string.
 *
 * This is a convenience wrapper around a fast, stream-oriented text compressor
 * (current implementation: **Snappy**). The function returns a `std::string`
 * containing the compressed frame. The compressed output is binary data and
 * may contain NUL bytes — treat it as opaque.
 *
 * @param _text  Input text to compress. Treated as a sequence of bytes; no
 *               string encoding validation is performed.
 * @param _level Compression level: Level 1 is the fastest Level 2 is a little
 *               slower but provides better compression.
 *
 * @return `std::optional<std::string>`:
 *         - contains compressed bytes on success;
 *         - `std::nullopt` on failure (e.g. underlying library error,
 *           out-of-memory, or other API-level failure).
 *
 * @threadsafe Safe to call concurrently from multiple threads as long as the
 *            underlying compressor library is initialized appropriately and
 *            you do not mutate any shared global compressor state. The
 *            implementation should allocate thread-local temporaries.
 *
 * @complexity Time: roughly linear in `_text.size()`; exact constant factors
 *             depend on compressor. Memory: allocates an output buffer sized
 *             to the compressor's worst-case output (implementation dependent).
 *
 * @example
 * std::optional<std::string> c = compress::text("hello world");
 * if (c) {
 *     // c->data() contains compressed bytes (not a printable string)
 * }
 */
// TODO: Make constexpr
[[nodiscard]] auto text( std::string_view _text, size_t _level = 1 )
    -> std::optional< std::string >;

/**
 * @brief Compress arbitrary binary data.
 *
 * This function compresses a block of bytes using a general-purpose binary
 * compressor (current implementation: **Zstandard (zstd)**). It returns a
 * vector of bytes containing the compressed frame. The result should be
 * treated as opaque binary data.
 *
 * @param _data  Input data to compress (view into caller-owned memory).
 * @param _level Compression level. For zstd typical valid values are
 *               1..22 (implementation-dependent). Lower values favor speed,
 *               higher values favor compression ratio. Default is `3`
 *               (a reasonable speed/ratio tradeoff).
 *
 *               Rough guideline (zstd):
 *                 - 1 : fastest, lowest compression
 *                 - 3 : fast, good default
 *                 - 9 : slower, noticeably better compression
 *                 - 19-22 : slowest, best compression (may be very slow/memory
 * heavy)
 *
 * @return `std::optional<std::vector<std::byte>>`:
 *         - contains compressed bytes on success;
 *         - `std::nullopt` on failure (invalid arguments, compression error,
 *           or underlying library failure).
 *
 * @threadsafe Thread-safe to call concurrently unless the implementation uses
 *            a shared mutable compressor context (it typically won't). If you
 *            plan high-concurrency workloads, benchmark and consider per-thread
 *            contexts or streaming APIs.
 *
 * @complexity Time: roughly linear in `_data.size()` with constants depending
 *             on `_level`. Memory: allocates output buffer of size proportional
 *             to compressor worst-case.
 *
 * @example
 * std::vector<std::byte> in = ...;
 * auto outOpt = compress::data(std::span(in), level=5);
 * if (outOpt) { write outOpt->data() to disk/network  }
 */
// TODO: Make constexpr
[[nodiscard]] auto data( std::span< const std::byte > _data, size_t _level = 3 )
    -> std::optional< std::vector< std::byte > >;

} // namespace stdfunc::compress
