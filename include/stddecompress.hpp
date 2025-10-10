#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace stdfunc::decompress {

/**
 * @brief Decompress a text frame produced by `compress::text`.
 *
 * Attempts to decompress the binary frame in `_data` and return the original
 * text as `std::string`. Returns `std::nullopt` on failure (invalid frame,
 * corruption, unsupported format, or other decompression error).
 *
 * @param _data Compressed frame (binary). The function treats the bytes as
 *              opaque input for the decompressor.
 *
 * @return `std::optional<std::string>`:
 *         - decompressed original string on success;
 *         - `std::nullopt` on failure.
 *
 * @threadsafe Safe to call concurrently from multiple threads provided the
 *            underlying library does not require exclusive initialization.
 *
 * @complexity Time: roughly linear in the size of the decompressed data.
 *
 * @note If `compress::text` is implemented with Snappy (no framing for original
 *       size), the decompressor will use the framing/format produced by that
 *       compressor. If your compressed frames do not include the original
 *       size, the decompressor must rely on the compressor frame metadata.
 *
 * @example
 * auto de = decompress::text(compressed_string);
 * if (de) { std::string s = std::move(*de); }
 */
// TODO: Make constexpr
[[nodiscard]] auto text( std::string_view _data )
    -> std::optional< std::string >;

/**
 * @brief Decompress binary data produced by `compress::data`.
 *
 * Decompress a binary compressed frame into a `std::vector<std::byte>` of the
 * original size `_originalSize`. Some compressors (like zstd) may store the
 * original size in the frame; however many APIs expect the caller to provide
 * the expected decompressed size. If your compressor stores the original size
 * in the frame, `_originalSize` can be unused — consult your implementation.
 *
 * @param _data         Compressed frame bytes.
 * @param _originalSize Expected size of the decompressed output in bytes.
 *                      The function may use this to allocate the output buffer
 *                      and to validate the decompressed result. Passing an
 *                      incorrect `_originalSize` may cause decompression to
 *                      fail or return truncated/incorrect data.
 *
 * @return `std::optional<std::vector<std::byte>>`:
 *         - decompressed byte vector on success;
 *         - `std::nullopt` on failure (bad frame, corruption, mismatch with
 *           `_originalSize`, etc.).
 *
 * @threadsafe Safe to call concurrently, subject to underlying library rules.
 *
 * @complexity Time: approximately linear in `_originalSize` (or the
 *             decompressed amount).
 *
 * @example
 * auto outOpt = decompress::data(std::span(compressedBytes),
 * expected_original_size); if (outOpt) { write(outOpt->data(), outOpt->size());
 * }
 */
// TODO: Make constexpr
[[nodiscard]] auto data( std::span< const std::byte > _data,
                         size_t _originalSize )
    -> std::optional< std::vector< std::byte > >;

} // namespace stdfunc::decompress
