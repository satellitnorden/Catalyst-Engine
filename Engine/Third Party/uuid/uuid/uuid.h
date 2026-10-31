/*
 * uuid.h - Production-grade UUID library for C/C++
 *
 * Copyright (c) 2026 Muhammad Fiaz
 * SPDX-License-Identifier: MIT
 *
 * A complete, self-contained UUID library implementing RFC 9562.
 * Zero third-party runtime dependencies.
 * Supports C99/C11/C17/C23 and C++98 through C++23.
 *
 * https://github.com/muhammad-fiaz/uuid
 * https://muhammad-fiaz.github.io/uuid
 */

#ifndef UUID_UUID_H
#define UUID_UUID_H 1

#if defined(__cplusplus)
extern "C" {
#endif

/* ── Version ──────────────────────────────────────────────────────────── */

#define UUID_VERSION_MAJOR  0
#define UUID_VERSION_MINOR  1
#define UUID_VERSION_PATCH  0
#define UUID_VERSION_STRING "0.1.0"

/* ── Includes ─────────────────────────────────────────────────────────── */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* ── Export / Visibility ──────────────────────────────────────────────── */

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
  #if defined(UUID_BUILDING_SHARED)
    #if defined(__GNUC__) || defined(__clang__)
      #define UUID_EXPORT __attribute__((dllexport))
    #elif defined(_MSC_VER)
      #define UUID_EXPORT __declspec(dllexport)
    #else
      #define UUID_EXPORT
    #endif
  #elif defined(UUID_BUILDING_STATIC) || defined(UUID_STATIC)
    #define UUID_EXPORT
  #else
    #if defined(__GNUC__) || defined(__clang__)
      #define UUID_EXPORT __attribute__((dllimport))
    #elif defined(_MSC_VER)
      #define UUID_EXPORT __declspec(dllimport)
    #else
      #define UUID_EXPORT
    #endif
  #endif
#else
  #if defined(__GNUC__) || defined(__clang__)
    #define UUID_EXPORT __attribute__((visibility("default")))
  #else
    #define UUID_EXPORT
  #endif
#endif

/* ── Deprecation ──────────────────────────────────────────────────────── */

#if defined(__GNUC__) || defined(__clang__)
  #define UUID_DEPRECATED(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
  #define UUID_DEPRECATED(msg) __declspec(deprecated(msg))
#else
  #define UUID_DEPRECATED(msg)
#endif

/* ── Inline ───────────────────────────────────────────────────────────── */

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
  #define UUID_INLINE static inline
#elif defined(_MSC_VER)
  #define UUID_INLINE static __forceinline
#elif defined(__GNUC__) || defined(__clang__)
  #define UUID_INLINE static __inline__
#else
  #define UUID_INLINE static
#endif

/* ── Nullability (optional, Clang) ────────────────────────────────────── */

#if defined(__clang__) && defined(__has_feature)
  #if __has_feature(nullability_completeness)
    #define UUID_NULLABLE    _Nullable
    #define UUID_NONNULL     _Nonnull
  #else
    #define UUID_NULLABLE
    #define UUID_NONNULL
  #endif
#else
  #define UUID_NULLABLE
  #define UUID_NONNULL
#endif

/* ── Constants ────────────────────────────────────────────────────────── */

/** Length of a UUID string in canonical format (including null terminator). */
#define UUID_STRING_LEN     37

/** Length of a UUID string without null terminator. */
#define UUID_STRING_LEN_NO_NUL 36

/** Length of a compact UUID string (including null terminator). */
#define UUID_COMPACT_LEN    33

/** Length of a compact UUID string without null terminator. */
#define UUID_COMPACT_LEN_NO_NUL 32

/** Size of a UUID in bytes. */
#define UUID_SIZE            16

/** Size of a UUID in binary form. */
#define UUID_BINARY_SIZE     16

/* ── Error Codes ──────────────────────────────────────────────────────── */

/** Error codes returned by uuid functions. */
typedef enum {
    UUID_OK                 =  0,  /**< Success. */
    UUID_E_INVALID_ARG      = -1,  /**< Invalid argument. */
    UUID_E_INVALID_UUID     = -2,  /**< Invalid UUID value. */
    UUID_E_INVALID_FORMAT   = -3,  /**< Invalid string format. */
    UUID_E_INVALID_LENGTH   = -4,  /**< Invalid input length. */
    UUID_E_INVALID_VERSION  = -5,  /**< Invalid UUID version. */
    UUID_E_INVALID_VARIANT  = -6,  /**< Invalid UUID variant. */
    UUID_E_BUFFER_TOO_SMALL = -7,  /**< Provided buffer is too small. */
    UUID_E_NO_MEMORY        = -8,  /**< Memory allocation failed. */
    UUID_E_NOT_SUPPORTED    = -9,  /**< Operation not supported. */
    UUID_E_PLATFORM         = -10, /**< Platform/OS error. */
    UUID_E_RANDOM           = -11, /**< Random number generation failed. */
    UUID_E_OVERFLOW         = -12, /**< Counter overflow. */
    UUID_E_NAMESPACE        = -13, /**< Invalid namespace. */
    UUID_E_TIMEOUT          = -14  /**< Operation timed out. */
} uuid_error_t;

/* ── UUID Version Enum ────────────────────────────────────────────────── */

typedef enum {
    UUID_VERSION_NIL   = 0,  /**< Nil UUID (all zeros). */
    UUID_VERSION_1     = 1,  /**< Time-based (Gregorian). */
    UUID_VERSION_3     = 3,  /**< Name-based (MD5). */
    UUID_VERSION_4     = 4,  /**< Random. */
    UUID_VERSION_5     = 5,  /**< Name-based (SHA-1). */
    UUID_VERSION_6     = 6,  /**< Time-based (reordered, Gregorian). */
    UUID_VERSION_7     = 7,  /**< Time-based (Unix epoch). */
    UUID_VERSION_8     = 8,  /**< Custom/vendor-specific. */
    UUID_VERSION_MAX   = 15  /**< Max UUID (all 0xFF). */
} uuid_version_t;

/* ── UUID Variant Enum ────────────────────────────────────────────────── */

typedef enum {
    UUID_VARIANT_NCS        = 0x00,  /**< NCS backward compatibility. */
    UUID_VARIANT_RFC_4122   = 0x01,  /**< RFC 4122 / RFC 9562. */
    UUID_VARIANT_MICROSOFT  = 0x02,  /**< Microsoft backward compat. */
    UUID_VARIANT_MAX        = 0x03   /**< Reserved (max). */
} uuid_variant_t;

/* ── Namespace IDs ────────────────────────────────────────────────────── */

/** Well-known namespace UUIDs per RFC 9562 Section 6.6. */
typedef enum {
    UUID_NS_DNS    = 0,  /**< 6ba7b810-9dad-11d1-80b4-00c04fd430c8 */
    UUID_NS_URL    = 1,  /**< 6ba7b811-9dad-11d1-80b4-00c04fd430c8 */
    UUID_NS_OID    = 2,  /**< 6ba7b812-9dad-11d1-80b4-00c04fd430c8 */
    UUID_NS_X500   = 3   /**< 6ba7b814-9dad-11d1-80b4-00c04fd430c8 */
} uuid_namespace_t;

/* ── Format Options ───────────────────────────────────────────────────── */

typedef enum {
    UUID_FMT_CANONICAL = 0,  /**< 8-4-4-4-12 (default) */
    UUID_FMT_COMPACT   = 1,  /**< 32 hex characters, no separators */
    UUID_FMT_BRACES    = 2,  /**< {8-4-4-4-12} */
    UUID_FMT_UPPER     = 4,  /**< Uppercase hex */
    UUID_FMT_BINARY    = 8   /**< Raw 16-byte binary */
} uuid_format_t;

/* ── UUID Struct ──────────────────────────────────────────────────────── */

/**
 * UUID structure (16 bytes, native byte order internally).
 * The struct is a value type — pass by value for small copies,
 * by pointer for API calls.
 */
#if defined(_WIN32) && defined(uuid_t)
#undef uuid_t
#endif
typedef struct {
    uint8_t bytes[UUID_SIZE];  /**< Raw UUID bytes (network byte order). */
} uuid_t;

/** Nil UUID constant (all zeros). */
#define UUID_nil_init {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}

/** Max UUID constant (all 0xFF). */
#define UUID_max_init {{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}}

/**
 * Static nil UUID instance.
 * @code
 *   uuid_t nil = UUID_NIL;
 * @endcode
 */
static const uuid_t UUID_NIL = UUID_nil_init;

/**
 * Static max UUID instance.
 * @code
 *   uuid_t max = UUID_MAX_UUID;
 * @endcode
 */
static const uuid_t UUID_MAX_UUID = UUID_max_init;

/* ── UUID v1 State ────────────────────────────────────────────────────── */

/** State for UUID v1 generation. Call uuid_v1_init() before use. */
typedef struct {
    uint64_t timestamp;    /**< Last timestamp (100-ns intervals since Gregorian epoch). */
    uint16_t clock_seq;    /**< Clock sequence for this node. */
    uint8_t  node[6];      /**< Node identifier (MAC or random). */
    int      initialized;  /**< Non-zero if initialized. */
} uuid_v1_state_t;

/* ── UUID v7 State ────────────────────────────────────────────────────── */

/** State for UUID v7 generation with monotonic counter. */
typedef struct {
    uint64_t timestamp_ms; /**< Last timestamp (ms since Unix epoch). */
    uint16_t counter;      /**< Monotonic counter. */
    uint8_t  node[6];      /**< Optional node ID. */
    int      initialized;  /**< Non-zero if initialized. */
} uuid_v7_state_t;

/* ── Custom Allocator ─────────────────────────────────────────────────── */

/** Custom allocator function pointer type. */
typedef void *(*uuid_alloc_fn)(size_t size, void *user_data);

/** Custom deallocator function pointer type. */
typedef void  (*uuid_free_fn)(void *ptr, void *user_data);

/** Allocator configuration. */
typedef struct {
    uuid_alloc_fn alloc;    /**< Allocation function (NULL = malloc). */
    uuid_free_fn  free;     /**< Deallocation function (NULL = free). */
    void         *user_data;/**< User data passed to alloc/free. */
} uuid_allocator_t;

/* ── Error Handling ───────────────────────────────────────────────────── */

/**
 * Get a human-readable error message for an error code.
 * @param error Error code.
 * @return Pointer to a static string. Never returns NULL.
 */
UUID_EXPORT const char *uuid_strerror(uuid_error_t error);

/**
 * Get the last error message (thread-local).
 * @return Pointer to the last error message string.
 */
UUID_EXPORT const char *uuid_last_error(void);

/* ── Initialization ───────────────────────────────────────────────────── */

/**
 * Initialize a nil UUID (all zeros).
 * @param[out] uuid Pointer to UUID to initialize.
 * @return UUID_OK on success.
 */
UUID_EXPORT uuid_error_t uuid_init_nil(uuid_t *UUID_NONNULL uuid);

/**
 * Initialize a max UUID (all 0xFF).
 * @param[out] uuid Pointer to UUID to initialize.
 * @return UUID_OK on success.
 */
UUID_EXPORT uuid_error_t uuid_init_max(uuid_t *UUID_NONNULL uuid);

/**
 * Initialize a UUID from a 16-byte array.
 * @param[out] uuid   Pointer to UUID to initialize.
 * @param[in]  data   16-byte array (network byte order).
 * @return UUID_OK on success, UUID_E_INVALID_ARG if uuid or data is NULL.
 */
UUID_EXPORT uuid_error_t uuid_init_from_bytes(uuid_t *UUID_NONNULL uuid,
                                              const uint8_t data[UUID_SIZE]);

/**
 * Copy a UUID.
 * @param[out] dst Destination UUID.
 * @param[in]  src Source UUID.
 * @return UUID_OK on success.
 */
UUID_EXPORT uuid_error_t uuid_copy(uuid_t *UUID_NONNULL dst,
                                   const uuid_t *UUID_NONNULL src);

/**
 * Compare two UUIDs byte-by-byte.
 * @param[in] a First UUID.
 * @param[in] b Second UUID.
 * @return < 0 if a < b, 0 if a == b, > 0 if a > b.
 */
UUID_EXPORT int uuid_compare(const uuid_t *UUID_NONNULL a,
                             const uuid_t *UUID_NONNULL b);

/**
 * Check if a UUID is nil (all zeros).
 * @param[in] uuid UUID to check.
 * @return 1 if nil, 0 otherwise.
 */
UUID_EXPORT int uuid_is_nil(const uuid_t *UUID_NONNULL uuid);

/**
 * Check if a UUID is max (all 0xFF).
 * @param[in] uuid UUID to check.
 * @return 1 if max, 0 otherwise.
 */
UUID_EXPORT int uuid_is_max(const uuid_t *UUID_NONNULL uuid);

/**
 * Swap two UUIDs.
 * @param[in,out] a First UUID.
 * @param[in,out] b Second UUID.
 * @return UUID_OK on success.
 */
UUID_EXPORT uuid_error_t uuid_swap(uuid_t *UUID_NONNULL a,
                                   uuid_t *UUID_NONNULL b);

/**
 * Set all bytes of a UUID.
 * @param[out] uuid  UUID to set.
 * @param[in]  value Byte value for all 16 bytes.
 * @return UUID_OK on success.
 */
UUID_EXPORT uuid_error_t uuid_set(uuid_t *UUID_NONNULL uuid, uint8_t value);

/* ── Version & Variant ────────────────────────────────────────────────── */

/**
 * Get the version of a UUID.
 * @param[in] uuid UUID to inspect.
 * @return Version number (0-15), or UUID_E_INVALID_UUID on error.
 */
UUID_EXPORT int uuid_version(const uuid_t *UUID_NONNULL uuid);

/**
 * Get the variant of a UUID.
 * @param[in] uuid UUID to inspect.
 * @return Variant value, or UUID_E_INVALID_UUID on error.
 */
UUID_EXPORT int uuid_variant(const uuid_t *UUID_NONNULL uuid);

/**
 * Get the version as an enum.
 * @param[in] uuid UUID to inspect.
 * @param[out] ver Pointer to store version (may be NULL).
 * @return UUID_OK on success, UUID_E_INVALID_UUID if not a valid UUID.
 */
UUID_EXPORT uuid_error_t uuid_get_version(const uuid_t *UUID_NONNULL uuid,
                                          uuid_version_t *UUID_NULLABLE ver);

/**
 * Get the variant as an enum.
 * @param[in] uuid UUID to inspect.
 * @param[out] var Pointer to store variant (may be NULL).
 * @return UUID_OK on success, UUID_E_INVALID_UUID if not a valid UUID.
 */
UUID_EXPORT uuid_error_t uuid_get_variant(const uuid_t *UUID_NONNULL uuid,
                                          uuid_variant_t *UUID_NULLABLE var);

/**
 * Get the timestamp from a UUID v1.
 * @param[in]  uuid      UUID to extract from.
 * @param[out] timestamp Pointer to store timestamp (100-ns intervals since Gregorian epoch).
 * @return UUID_OK on success, UUID_E_INVALID_VERSION if not v1.
 */
UUID_EXPORT uuid_error_t uuid_v1_timestamp(const uuid_t *UUID_NONNULL uuid,
                                           uint64_t *UUID_NONNULL timestamp);

/**
 * Get the timestamp from a UUID v7.
 * @param[in]  uuid       UUID to extract from.
 * @param[out] timestamp_ms Pointer to store timestamp (ms since Unix epoch).
 * @return UUID_OK on success, UUID_E_INVALID_VERSION if not v7.
 */
UUID_EXPORT uuid_error_t uuid_v7_timestamp_ms(const uuid_t *UUID_NONNULL uuid,
                                              uint64_t *UUID_NONNULL timestamp_ms);

/**
 * Get the clock sequence from a UUID v1.
 * @param[in]  uuid       UUID to extract from.
 * @param[out] clock_seq  Pointer to store clock sequence.
 * @return UUID_OK on success, UUID_E_INVALID_VERSION if not v1.
 */
UUID_EXPORT uuid_error_t uuid_v1_clock_seq(const uuid_t *UUID_NONNULL uuid,
                                           uint16_t *UUID_NONNULL clock_seq);

/**
 * Get the node ID from a UUID v1.
 * @param[in]  uuid UUID to extract from.
 * @param[out] node 6-byte buffer to store node ID.
 * @return UUID_OK on success, UUID_E_INVALID_VERSION if not v1.
 */
UUID_EXPORT uuid_error_t uuid_v1_node(const uuid_t *UUID_NONNULL uuid,
                                      uint8_t node[6]);

/* ── Byte Access ──────────────────────────────────────────────────────── */

/**
 * Get a specific byte from a UUID.
 * @param[in]  uuid  UUID to inspect.
 * @param[in]  index Byte index (0-15).
 * @param[out] byte  Pointer to store byte value.
 * @return UUID_OK on success, UUID_E_INVALID_ARG if index out of range.
 */
UUID_EXPORT uuid_error_t uuid_byte_at(const uuid_t *UUID_NONNULL uuid,
                                      size_t index, uint8_t *UUID_NONNULL byte);

/**
 * Get a pointer to the raw bytes of a UUID.
 * @param[in] uuid UUID to inspect.
 * @return Pointer to the 16-byte array (never NULL for valid uuid).
 */
UUID_EXPORT const uint8_t *uuid_bytes(const uuid_t *UUID_NONNULL uuid);

/* ── Parsing ──────────────────────────────────────────────────────────── */

/**
 * Parse a UUID from a canonical string.
 *
 * Accepted formats:
 *   - "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
 *   - "{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}"
 *   - "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" (compact, 32 hex chars)
 *   - Mixed case is accepted.
 *   - Leading/trailing whitespace is trimmed in relaxed mode.
 *
 * @param[out] uuid   UUID to parse into.
 * @param[in]  str    Null-terminated string to parse.
 * @return UUID_OK on success, error code on failure.
 */
UUID_EXPORT uuid_error_t uuid_parse(uuid_t *UUID_NONNULL uuid,
                                    const char *UUID_NONNULL str);

/**
 * Parse a UUID from a string with length limit.
 * @param[out] uuid UUID to parse into.
 * @param[in]  str  String to parse (need not be null-terminated).
 * @param[in]  len  Length of str.
 * @return UUID_OK on success, error code on failure.
 */
UUID_EXPORT uuid_error_t uuid_parse_n(uuid_t *UUID_NONNULL uuid,
                                      const char *UUID_NONNULL str, size_t len);

/**
 * Parse a UUID from a binary (16-byte) buffer.
 * @param[out] uuid UUID to parse into.
 * @param[in]  data 16-byte buffer.
 * @param[in]  len  Length of data (must be 16).
 * @return UUID_OK on success, error code on failure.
 */
UUID_EXPORT uuid_error_t uuid_parse_binary(uuid_t *UUID_NONNULL uuid,
                                           const void *UUID_NONNULL data,
                                           size_t len);

/**
 * Validate a UUID string without parsing.
 * @param[in] str Null-terminated string to validate.
 * @return UUID_OK if valid, error code otherwise.
 */
UUID_EXPORT uuid_error_t uuid_validate(const char *UUID_NONNULL str);

/**
 * Validate a UUID string with length limit.
 * @param[in] str String to validate.
 * @param[in] len Length of str.
 * @return UUID_OK if valid, error code otherwise.
 */
UUID_EXPORT uuid_error_t uuid_validate_n(const char *UUID_NONNULL str, size_t len);

/* ── Formatting ───────────────────────────────────────────────────────── */

/**
 * Format a UUID as a canonical string.
 *
 * Output: "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx\0"
 *
 * @param[in]  uuid UUID to format.
 * @param[out] buf  Buffer to write to (must be at least UUID_STRING_LEN bytes).
 * @param[in]  size Size of buffer.
 * @return UUID_OK on success, UUID_E_BUFFER_TOO_SMALL if size < UUID_STRING_LEN.
 */
UUID_EXPORT uuid_error_t uuid_format(const uuid_t *UUID_NONNULL uuid,
                                     char *UUID_NONNULL buf, size_t size);

/**
 * Format a UUID as a compact string (no hyphens).
 *
 * Output: "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\0"
 *
 * @param[in]  uuid UUID to format.
 * @param[out] buf  Buffer to write to (must be at least UUID_COMPACT_LEN bytes).
 * @param[in]  size Size of buffer.
 * @return UUID_OK on success, UUID_E_BUFFER_TOO_SMALL if size < UUID_COMPACT_LEN.
 */
UUID_EXPORT uuid_error_t uuid_format_compact(const uuid_t *UUID_NONNULL uuid,
                                             char *UUID_NONNULL buf, size_t size);

/**
 * Format a UUID with braces.
 *
 * Output: "{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}\0"
 *
 * @param[in]  uuid UUID to format.
 * @param[out] buf  Buffer to write to (must be at least 39 bytes).
 * @param[in]  size Size of buffer.
 * @return UUID_OK on success, UUID_E_BUFFER_TOO_SMALL if too small.
 */
UUID_EXPORT uuid_error_t uuid_format_braces(const uuid_t *UUID_NONNULL uuid,
                                            char *UUID_NONNULL buf, size_t size);

/**
 * Format a UUID with custom options.
 * @param[in]  uuid    UUID to format.
 * @param[out] buf     Buffer to write to.
 * @param[in]  size    Size of buffer.
 * @param[in]  format  Format options (bitmask of uuid_format_t).
 * @return UUID_OK on success, error code on failure.
 */
UUID_EXPORT uuid_error_t uuid_format_options(const uuid_t *UUID_NONNULL uuid,
                                             char *UUID_NONNULL buf, size_t size,
                                             int format);

/**
 * Format a UUID to a binary buffer.
 * @param[in]  uuid UUID to format.
 * @param[out] buf  16-byte buffer.
 * @param[in]  size Size of buffer (must be >= 16).
 * @return UUID_OK on success, UUID_E_BUFFER_TOO_SMALL if too small.
 */
UUID_EXPORT uuid_error_t uuid_format_binary(const uuid_t *UUID_NONNULL uuid,
                                            void *UUID_NONNULL buf, size_t size);

/**
 * Format a UUID as a string using a caller-provided buffer.
 * Convenience wrapper around uuid_format().
 * @param[in]  uuid UUID to format.
 * @param[out] buf  Buffer (at least UUID_STRING_LEN bytes).
 * @param[in]  size Size of buffer.
 * @return Pointer to buf on success, NULL on failure.
 */
UUID_EXPORT char *uuid_to_string(const uuid_t *UUID_NONNULL uuid,
                                 char *UUID_NONNULL buf, size_t size);

/* ── UUID Generation: Version 1 (Time-based, Gregorian) ───────────────── */

/**
 * Initialize UUID v1 state with a random clock sequence.
 * @param[out] state State to initialize.
 * @return UUID_OK on success, UUID_E_RANDOM if random generation fails.
 */
UUID_EXPORT uuid_error_t uuid_v1_init(uuid_v1_state_t *UUID_NONNULL state);

/**
 * Generate a UUID v1 from the given state.
 *
 * Thread safety: caller must serialize access to state, or use separate
 * state instances per thread.
 *
 * @param[in]  state V1 state (must be initialized).
 * @param[out] uuid  Generated UUID.
 * @return UUID_OK on success, UUID_E_RANDOM if random generation fails.
 */
UUID_EXPORT uuid_error_t uuid_v1(uuid_v1_state_t *UUID_NONNULL state,
                                 uuid_t *UUID_NONNULL uuid);

/**
 * Generate a UUID v1 using a temporary state (not thread-safe).
 * Uses a static state internally. Not recommended for production.
 * @param[out] uuid Generated UUID.
 * @return UUID_OK on success.
 */
UUID_EXPORT uuid_error_t uuid_v1_generate(uuid_t *UUID_NONNULL uuid);

/* ── UUID Generation: Version 3 (Name-based, MD5) ────────────────────── */

/**
 * Generate a UUID v3 (name-based, MD5).
 * @param[in]  namespace_uuid Namespace UUID.
 * @param[in]  name           Name string.
 * @param[in]  name_len       Length of name.
 * @param[out] uuid           Generated UUID.
 * @return UUID_OK on success.
 */
UUID_EXPORT uuid_error_t uuid_v3(const uuid_t *UUID_NONNULL namespace_uuid,
                                 const void *UUID_NONNULL name, size_t name_len,
                                 uuid_t *UUID_NONNULL uuid);

/**
 * Generate a UUID v3 using a well-known namespace.
 * @param[in]  ns  Namespace enum (UUID_NS_DNS, UUID_NS_URL, etc.).
 * @param[in]  name Name string.
 * @param[in]  name_len Length of name.
 * @param[out] uuid Generated UUID.
 * @return UUID_OK on success, UUID_E_NAMESPACE if ns is invalid.
 */
UUID_EXPORT uuid_error_t uuid_v3_ns(uuid_namespace_t ns,
                                    const char *UUID_NONNULL name, size_t name_len,
                                    uuid_t *UUID_NONNULL uuid);

/* ── UUID Generation: Version 4 (Random) ─────────────────────────────── */

/**
 * Generate a random UUID v4.
 * Uses cryptographically secure platform random.
 * @param[out] uuid Generated UUID.
 * @return UUID_OK on success, UUID_E_RANDOM if random generation fails.
 */
UUID_EXPORT uuid_error_t uuid_v4(uuid_t *UUID_NONNULL uuid);

/* ── UUID Generation: Version 5 (Name-based, SHA-1) ──────────────────── */

/**
 * Generate a UUID v5 (name-based, SHA-1).
 * @param[in]  namespace_uuid Namespace UUID.
 * @param[in]  name           Name string.
 * @param[in]  name_len       Length of name.
 * @param[out] uuid           Generated UUID.
 * @return UUID_OK on success.
 */
UUID_EXPORT uuid_error_t uuid_v5(const uuid_t *UUID_NONNULL namespace_uuid,
                                 const void *UUID_NONNULL name, size_t name_len,
                                 uuid_t *UUID_NONNULL uuid);

/**
 * Generate a UUID v5 using a well-known namespace.
 * @param[in]  ns  Namespace enum.
 * @param[in]  name Name string.
 * @param[in]  name_len Length of name.
 * @param[out] uuid Generated UUID.
 * @return UUID_OK on success.
 */
UUID_EXPORT uuid_error_t uuid_v5_ns(uuid_namespace_t ns,
                                    const char *UUID_NONNULL name, size_t name_len,
                                    uuid_t *UUID_NONNULL uuid);

/* ── UUID Generation: Version 6 (Reordered time-based) ────────────────── */

/**
 * Initialize UUID v6 state.
 * @param[out] state State to initialize.
 * @return UUID_OK on success.
 */
UUID_EXPORT uuid_error_t uuid_v6_init(uuid_v1_state_t *UUID_NONNULL state);

/**
 * Generate a UUID v6 (reordered time-based, Gregorian).
 * @param[in]  state V1 state (shared with v1).
 * @param[out] uuid  Generated UUID.
 * @return UUID_OK on success.
 */
UUID_EXPORT uuid_error_t uuid_v6(uuid_v1_state_t *UUID_NONNULL state,
                                 uuid_t *UUID_NONNULL uuid);

/**
 * Generate a UUID v6 using thread-local state (convenience).
 * @param[out] uuid Generated UUID.
 * @return UUID_OK on success.
 * @note Uses thread-local state; fully thread-safe.
 */
UUID_EXPORT uuid_error_t uuid_v6_generate(uuid_t *UUID_NONNULL uuid);

/* ── UUID Generation: Version 7 (Unix epoch time-based) ───────────────── */

/**
 * Initialize UUID v7 state.
 * @param[out] state State to initialize.
 * @return UUID_OK on success.
 */
UUID_EXPORT uuid_error_t uuid_v7_init(uuid_v7_state_t *UUID_NONNULL state);

/**
 * Generate a UUID v7 (Unix epoch, millisecond precision, monotonic).
 * @param[in]  state V7 state (must be initialized).
 * @param[out] uuid  Generated UUID.
 * @return UUID_OK on success, UUID_E_OVERFLOW on counter overflow.
 */
UUID_EXPORT uuid_error_t uuid_v7(uuid_v7_state_t *UUID_NONNULL state,
                                 uuid_t *UUID_NONNULL uuid);

/**
 * Generate a UUID v7 using a temporary state (not thread-safe).
 * @param[out] uuid Generated UUID.
 * @return UUID_OK on success.
 */
UUID_EXPORT uuid_error_t uuid_v7_generate(uuid_t *UUID_NONNULL uuid);

/* ── UUID Generation: Version 8 (Custom/Vendor-specific) ──────────────── */

/**
 * Generate a UUID v8 from custom bytes.
 * The caller provides the full 16-byte content; version and variant
 * bits are set automatically.
 *
 * @param[out] uuid    Generated UUID.
 * @param[in]  custom  12 custom bytes (bytes 0-11 of the UUID).
 * @return UUID_OK on success.
 */
UUID_EXPORT uuid_error_t uuid_v8(uuid_t *UUID_NONNULL uuid,
                                 const uint8_t custom[12]);

/**
 * Generate a UUID v8 from fully custom bytes (no auto version/variant).
 * @param[out] uuid   Generated UUID.
 * @param[in]  custom 16 custom bytes.
 * @return UUID_OK on success.
 */
UUID_EXPORT uuid_error_t uuid_v8_custom(uuid_t *UUID_NONNULL uuid,
                                        const uint8_t custom[16]);

/* ── Nil / Max ────────────────────────────────────────────────────────── */

/** Convenience: generate a nil UUID. */
UUID_EXPORT uuid_error_t uuid_nil(uuid_t *UUID_NONNULL uuid);

/** Convenience: generate a max UUID. */
UUID_EXPORT uuid_error_t uuid_max(uuid_t *UUID_NONNULL uuid);

/* ── Namespace Helpers ────────────────────────────────────────────────── */

/**
 * Get a well-known namespace UUID by enum.
 * @param[in]  ns      Namespace enum.
 * @param[out] uuid    Namespace UUID.
 * @return UUID_OK on success, UUID_E_NAMESPACE if invalid.
 */
UUID_EXPORT uuid_error_t uuid_namespace(uuid_namespace_t ns,
                                        uuid_t *UUID_NONNULL uuid);

/* ── Hashing ──────────────────────────────────────────────────────────── */

/**
 * FNV-1a hash of a UUID (for hash tables).
 * @param[in] uuid UUID to hash.
 * @return 64-bit hash value.
 */
UUID_EXPORT uint64_t uuid_hash(const uuid_t *UUID_NONNULL uuid);

/**
 * Compute MD5 hash of a buffer (used internally for UUID v3).
 * @param[in]  data     Input data.
 * @param[in]  data_len Length of input data.
 * @param[out] digest   16-byte MD5 digest output.
 */
UUID_EXPORT void uuid_md5(const void *UUID_NONNULL data, size_t data_len,
                          uint8_t digest[16]);

/**
 * Compute SHA-1 hash of a buffer (used internally for UUID v5).
 * @param[in]  data     Input data.
 * @param[in]  data_len Length of input data.
 * @param[out] digest   20-byte SHA-1 digest output.
 */
UUID_EXPORT void uuid_sha1(const void *UUID_NONNULL data, size_t data_len,
                           uint8_t digest[20]);

/* ── Serialization ────────────────────────────────────────────────────── */

/**
 * Serialize a UUID to a byte buffer (big-endian).
 * @param[in]  uuid UUID to serialize.
 * @param[out] buf  Output buffer (16 bytes).
 * @param[in]  size Size of output buffer.
 * @return UUID_OK on success, UUID_E_BUFFER_TOO_SMALL if too small.
 */
UUID_EXPORT uuid_error_t uuid_serialize(const uuid_t *UUID_NONNULL uuid,
                                        uint8_t *UUID_NONNULL buf, size_t size);

/**
 * Deserialize a UUID from a byte buffer (big-endian).
 * @param[out] uuid UUID to deserialize into.
 * @param[in]  buf  Input buffer (16 bytes).
 * @param[in]  size Size of input buffer.
 * @return UUID_OK on success, UUID_E_BUFFER_TOO_SMALL if too small.
 */
UUID_EXPORT uuid_error_t uuid_deserialize(uuid_t *UUID_NONNULL uuid,
                                          const uint8_t *UUID_NONNULL buf,
                                          size_t size);

/* ── Sorting ──────────────────────────────────────────────────────────── */

/**
 * Compare function for qsort (ascending byte order).
 * @param[in] a Pointer to first uuid_t.
 * @param[in] b Pointer to second uuid_t.
 * @return < 0, 0, or > 0.
 */
UUID_EXPORT int uuid_compare_asc(const void *UUID_NONNULL a,
                                 const void *UUID_NONNULL b);

/**
 * Compare function for qsort (descending byte order).
 * @param[in] a Pointer to first uuid_t.
 * @param[in] b Pointer to second uuid_t.
 * @return < 0, 0, or > 0.
 */
UUID_EXPORT int uuid_compare_desc(const void *UUID_NONNULL a,
                                  const void *UUID_NONNULL b);

/**
 * Sort an array of UUIDs in ascending order.
 * @param[in,out] uuids Array of UUIDs.
 * @param[in]     count Number of UUIDs.
 */
UUID_EXPORT void uuid_sort_asc(uuid_t *UUID_NONNULL uuids, size_t count);

/**
 * Sort an array of UUIDs in descending order.
 * @param[in,out] uuids Array of UUIDs.
 * @param[in]     count Number of UUIDs.
 */
UUID_EXPORT void uuid_sort_desc(uuid_t *UUID_NONNULL uuids, size_t count);

/* ── Utility ──────────────────────────────────────────────────────────── */

/**
 * Get the library version string.
 * @return Pointer to static version string (e.g. "0.1.0").
 */
UUID_EXPORT const char *uuid_version_string(void);

/**
 * Get the library version as integers.
 * @param[out] major Major version (may be NULL).
 * @param[out] minor Minor version (may be NULL).
 * @param[out] patch Patch version (may be NULL).
 */
UUID_EXPORT void uuid_version_info(int *major, int *minor, int *patch);

/**
 * Get the namespace UUID for a well-known namespace.
 * @param[in]  ns   Namespace enum.
 * @param[out] uuid Namespace UUID.
 * @return UUID_OK on success.
 */
UUID_EXPORT uuid_error_t uuid_get_namespace(uuid_namespace_t ns,
                                            uuid_t *UUID_NONNULL uuid);

#if defined(__cplusplus)
}
#endif

#endif /* UUID_UUID_H */
