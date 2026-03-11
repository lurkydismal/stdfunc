<!-- :toc: macro -->
<!-- :toc-title: -->
<!-- :toclevels: 99 -->

# stdfunc <!-- omit from toc -->

> A collection of C++ 23 utilities I reuse across projects - like assertions, hashing, compression wrappers, literal helpers, random utilities, etc.

## Table of Contents <!-- omit from toc -->

* [General Information](#general-information)
* [Technologies Used](#technologies-used)
* [Features](#features)
* [Setup](#setup)
* [Usage](#usage)
* [Project Status](#project-status)
* [Room for Improvement](#room-for-improvement)
* [Acknowledgements](#acknowledgements)
* [License](#license)

## General Information

A C++ 23 utility library. It’s intentionally opinionated and minimal.

Why I made it:

* I wanted a single place for safe, well-documented utility code that compiles with modern C++ and fits the *no surprises* philosophy.

## Technologies Used

<!--
clang version 20.1.8
Target: x86_64-pc-linux-gnu
Thread model: posix

Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
-->
* clang 20.1.8
* snappy 1.2.2-2
* zstd 1.5.7-2
* ctre 3.10.0-1 (my fork of it with fixed includes)
* glaze 5.7.2-1

## Features

* `stdfunc`:
  * `makeVariantContainer` to make specified container with passed values.
  * `bitsToBytes`, `lengthOfNumber`, `isSpace` `constexpr` helpers.
  * `sanitizeString` `constexpr` to trim spaces and comment and return view to trimmed part of passed string.
* `float16_t` type and `std::numeric_limits<float16_t>` and `std::formatter<float16_t>`.
* `uint128_t` type and `makeU128` to convert string.
* Literal helpers under `stdfunc::literals`:
  * `_b` byte literal, `_bytes` compile-time byte arrays, `_u128` string to `uint128_t`.
* Colored terminal ASCII-code constants in `stdfunc::color`.
* Debug helpers under `stdfunc`:
  * `trap` and `assert` that are active with `DEBUG` define (thread ID, stack trace, colored output).
* Compression wrappers under `stdfunc::compress`:
  * `compress::text` uses `snappy` compression.
  * `compress::data` uses `zstd`.
* Decompression wrappers under `stdfunc::decompress`:
  * `decompress::text` uses `snappy` compression.
  * `decompress::data` uses `zstd`.
* Concepts under `stdfunc`:
  * `has_common_type`, `is_container`, `is_struct`, `is_lambda`, `is_formattable`.
* File system helpers under `stdfunc::filesystem`:
  * `getApplicationDirectoryAbsolutePath`
  * `getPathsByRegexp` - runtime `std::regex` and compile-time `ctre` overload.
* Hashing under `stdfunc::hash`:
  * `hash::weak` (`FNV-1A`) for 32bits, 64bits and 128bits.
  * `hash::balanced` (`rapidhash`) for 64bits and (`xxHash3`) for 128bits.
* Random utilities under `stdfunc::random`:
  * `random::number::weak` (`constexpr`-friendly `xor-shift*` generator for 32bits, 64bits and 128bits).
  * `random::number::balanced` (runtime 32bits or 64bits depending on build target).
  * `random::value` for random value reference from container.
  * `random::view` for infinite view of random values reference from container.
  * `random::fill` to fill container with random values.
* Meta/ reflection for aggregate `struct`s under `stdfunc::meta`:
  * `is_reflectable` concept.
  * `iterateStructTopMostFields` to iterate reflectable `struct` with callback.
  * `hasMemberWithName` `consteval` to check if reflectable `struct` has a member with name.
* A set of macros for attributes (`FORCE_INLINE`, `CONST`, `PACKED`, etc.).

Mostly `constexpr` where feasible.

### Hash Strength Levels

This project provides multiple hashing tiers optimized for different goals
such as compile-time evaluation, runtime speed, and cryptographic security.

| Level | Goal | Properties | Example Algorithms |
|------|------|------------|-------------------|
| **Weak** | Compile-time / minimal overhead | `constexpr`, extremely fast, tiny binary size, not collision resistant | FNV-1a |
| **Balanced** | General-purpose hashing | Very fast, good distribution, suitable for most applications | xxHash3, rapidhash |
| **Strong** | Cryptographic hashing | Collision resistant, suitable for security-sensitive hashing | BLAKE2 |
| **Robust** | Password / key derivation | Memory-hard, resistant to brute-force and GPU attacks | Argon2 |

#### Notes

- **Weak** hashes prioritize compile-time evaluation and minimal overhead.
- **Balanced** hashes provide excellent speed and quality for general use.
- **Strong** hashes provide cryptographic guarantees such as collision resistance.
- **Robust** algorithms are designed for password hashing and key derivation and intentionally trade speed for security.

### Random Generator Strength Levels

This project provides multiple tiers of random number generators (RNGs)
optimized for different goals, such as compile-time evaluation, runtime speed,
statistical quality, and cryptographic security.

| Level | Goal | Properties | Example Generators |
|-------|------|------------|------------------|
| **Weak** | Compile-time / lightweight | `constexpr`, extremely fast, minimal state, small binary size, not cryptographically secure | xorshift* (xorshift multiply) |
| **Balanced** | General-purpose RNG | High-quality randomness, suitable for simulations and games, platform-portable | `std::mt19937_64` (64-bit), fallback to `std::mt19937` if 64-bit not supported |
| **Strong** | Cryptographically secure RNG | Produces unpredictable, high-entropy values suitable for cryptography, keys, nonces, and security-sensitive tasks | OpenSSL `RAND_bytes()` |
| **Robust** | Cryptographically secure / system RNG | Resistant to prediction and attacks, suitable for security-critical tasks | `/dev/urandom` on Linux (platform-specific secure sources for other OSes) |

#### Notes

- **Weak** RNGs are ideal for quick computations, games, or compile-time randomness, but should not be used where security or high-quality randomness is required.
- **Balanced** RNGs offer a good compromise between speed, quality, and portability; suitable for simulations, procedural generation, and most general tasks.
- **Strong** RNGs provide excellent statistical quality and extremely long periods; useful for simulations and scientific computations requiring high-quality randomness.
- **Robust** RNGs use system entropy or cryptographically secure sources to ensure unpredictability; intended for security-sensitive applications like key generation or cryptography.

## Setup

Requirements/ dependencies:

* C++ 23-capable compiler.
* Add the following libraries if you use the related subsystems:
  * `rapidhash` or `xxhash` for 64bits `hash::balanced`
  * `xxhash` for 128bits `hash::balanced`
  * `snappy` for `compress::text`/ `decompress::text`
  * `zstd` library for `compress::data`/ `decompress::data`
  * `ctre`/ `ctll`  for compile-time `getPathsByRegexp`
  * `glaze` for `meta::*`

Installation:

* See `config.sh` for what to include and what to compile.

## Usage

Quick examples demonstrating the most common helpers.

Parsing decimal into 128bits:

```cpp
#include "std128.hpp"

uint128_t v = stdfunc::makeU128( "6C62272E07BB014262B821756295C58D" ); // Hex without 0x
```

Colored output:

```cpp
#include "stdcolor.hpp"

std::println( "{}GREEN BOLD TEXT{}", stdfunc::color::g_green, stdfunc::color::g_reset );
```

Debug trap/assert:

```cpp
#include "stddebug.hpp" // only active with DEBUG define

stdfunc::assert( false, "Always {}", "fail" );
stdfunc::trap( "Fatal: {}", l_message );
```

Compression (text - fast and good byte-wise compression; data - good data compression):

```cpp
#include "stdcompress.hpp"
#include "stddecompress.hpp"

auto l_compressed = stdfunc::compress::text( "Hello, World!\n" );

if ( l_compressed ) {
    writeBinaryFile( "hello.txt", l_compressed.value() );

    auto l_decompressed = stdfunc::decompress::text( l_compressed.value() );

    std::println( "{}", l_decompressed );
}
```

Hashing:

```cpp
#include "stdhash.hpp"

const std::vector< std::byte > data = /* ... */;

auto l_v32 = stdfunc::hash::weak< uint32_t >( std::span( data ) );
auto l_v128 = stdfunc::hash::balanced< uint128_t >( std::span( data ) );
```

RNG helpers:

```cpp
#include "stdrandom.hpp"

stdfunc::random::g_engine.seed( 12345 ); // Will affect only balanced

int128_t l_number = stdfunc::random::number::weak< uint128_t >( 0, 100 );

int l_number2 = stdfunc::random::number::balanced< int >( 0, 100 );
```

Utility helpers:

```cpp
#include "stdfunc.hpp"

auto l_container = stdfunc::makeVariantContainer< std::vector >( 1, "str", 3.0f ); // Will hold std::variant of all passed types

size_t l_bytes = stdfunc::bitsToBytes( 13 ); // -> 2
```

## Project Status

Project is: _in progress_.

## Room for Improvement

Room for improvement:

* Make `hash::balanced` `constexpr`
* Add hex input support to `makeU128` and fix edge-case parsing/ overflow handling.
* Make `compress::text`/ `compress::data`/ `decompress::*` `constexpr` where practical or provide no-runtime stubs for compile-time builds.

To do:

* Add small examples in `examples/` demonstrating common integration patterns.
* Implement `stdfunc::system`

## Acknowledgements

* Compression choices are inspired by practical, fast compressors (`snappy`, `zstd`).
* Reflection helpers lean on `glaze` reflection.
* Compile-time regexp code uses `ctre`/ `ctll`.

## License

This project is open source and available under the
[GNU Affero General Public License v3.0](LICENSE).
