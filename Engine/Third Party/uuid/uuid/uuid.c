/*
 * uuid.c - Production-grade UUID library implementation
 *
 * Copyright (c) 2026 Muhammad Fiaz
 * SPDX-License-Identifier: MIT
 *
 * Implements RFC 9562 UUID generation for all versions.
 * Zero third-party runtime dependencies.
 */

#if defined(__linux__) && !defined(_POSIX_C_SOURCE)
  #define _POSIX_C_SOURCE 200809L
#endif

#include "uuid/uuid.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ── Platform Detection ───────────────────────────────────────────────── */

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
  #define UUID_PLATFORM_WINDOWS 1
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #ifndef UUID_NO_WINDOWS_HEADERS
    #include <windows.h>
    #include <bcrypt.h>
  #endif
  #if defined(_MSC_VER)
    #pragma comment(lib, "bcrypt.lib")
  #endif
#elif defined(__APPLE__)
  #define UUID_PLATFORM_APPLE 1
  #include <Availability.h>
  #include <TargetConditionals.h>
  #include <sys/random.h>
  #include <unistd.h>
#elif defined(__linux__)
  #define UUID_PLATFORM_LINUX 1
  #include <sys/random.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <time.h>
  #include <pthread.h>
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
  #define UUID_PLATFORM_BSD 1
  #include <sys/random.h>
  #include <unistd.h>
  #include <time.h>
#else
  #define UUID_PLATFORM_OTHER 1
  #include <stdlib.h>
  #include <time.h>
#endif

/* ── Thread-local storage ─────────────────────────────────────────────── */

#if defined(_MSC_VER)
  #define UUID_TLS __declspec(thread)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
  #define UUID_TLS _Thread_local
#elif defined(__GNUC__) || defined(__clang__)
  #define UUID_TLS __thread
#else
  #define UUID_TLS
#endif

/* ── Static Error Message ─────────────────────────────────────────────── */

static UUID_TLS char tls_error_msg[256] = "No error";

const char *uuid_last_error(void) {
    return tls_error_msg;
}

static void uuid_set_error(const char *msg) {
    size_t len;
    if (!msg) { tls_error_msg[0] = '\0'; return; }
    len = strlen(msg);
    if (len >= sizeof(tls_error_msg)) len = sizeof(tls_error_msg) - 1;
    memcpy(tls_error_msg, msg, len);
    tls_error_msg[len] = '\0';
}

/* ── Error Messages ───────────────────────────────────────────────────── */

const char *uuid_strerror(uuid_error_t error) {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#endif
    switch (error) {
        case UUID_OK:                 return "Success";
        case UUID_E_INVALID_ARG:      return "Invalid argument";
        case UUID_E_INVALID_UUID:     return "Invalid UUID value";
        case UUID_E_INVALID_FORMAT:   return "Invalid string format";
        case UUID_E_INVALID_LENGTH:   return "Invalid input length";
        case UUID_E_INVALID_VERSION:  return "Invalid UUID version";
        case UUID_E_INVALID_VARIANT:  return "Invalid UUID variant";
        case UUID_E_BUFFER_TOO_SMALL: return "Buffer too small";
        case UUID_E_NO_MEMORY:        return "Memory allocation failed";
        case UUID_E_NOT_SUPPORTED:    return "Operation not supported";
        case UUID_E_PLATFORM:         return "Platform error";
        case UUID_E_RANDOM:           return "Random generation failed";
        case UUID_E_OVERFLOW:         return "Counter overflow";
        case UUID_E_NAMESPACE:        return "Invalid namespace";
        case UUID_E_TIMEOUT:          return "Operation timed out";
        default:                      return "Unknown error";
    }
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}

/* ── Hex Helpers ──────────────────────────────────────────────────────── */

static const char hex_lower[] = "0123456789abcdef";
static const char hex_upper[] = "0123456789ABCDEF";

UUID_INLINE int hex_to_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

UUID_INLINE int is_hex(char c) {
    return hex_to_val(c) >= 0;
}

/* ── Namespace UUIDs (RFC 9562 Section 6.6) ───────────────────────────── */

static const uuid_t ns_uuids[4] = {
    /* DNS:    6ba7b810-9dad-11d1-80b4-00c04fd430c8 */
    {{0x6b,0xa7,0xb8,0x10,0x9d,0xad,0x11,0xd1,0x80,0xb4,0x00,0xc0,0x4f,0xd4,0x30,0xc8}},
    /* URL:    6ba7b811-9dad-11d1-80b4-00c04fd430c8 */
    {{0x6b,0xa7,0xb8,0x11,0x9d,0xad,0x11,0xd1,0x80,0xb4,0x00,0xc0,0x4f,0xd4,0x30,0xc8}},
    /* OID:    6ba7b812-9dad-11d1-80b4-00c04fd430c8 */
    {{0x6b,0xa7,0xb8,0x12,0x9d,0xad,0x11,0xd1,0x80,0xb4,0x00,0xc0,0x4f,0xd4,0x30,0xc8}},
    /* X500:   6ba7b814-9dad-11d1-80b4-00c04fd430c8 */
    {{0x6b,0xa7,0xb8,0x14,0x9d,0xad,0x11,0xd1,0x80,0xb4,0x00,0xc0,0x4f,0xd4,0x30,0xc8}}
};

/* ── Gregorian Offset (UUID v1/v6 epoch) ─────────────────────────────── */

/* Number of 100-ns intervals between 1582-10-15 00:00:00 and Unix epoch 1970-01-01 00:00:00 */
#define UUID_GREGORIAN_OFFSET ((uint64_t)0x01b21dd213814000ULL)

/* ── Platform Random ──────────────────────────────────────────────────── */

#if defined(UUID_PLATFORM_WINDOWS)

static uuid_error_t uuid_platform_random(void *buf, size_t len) {
    NTSTATUS status = BCryptGenRandom(NULL, (PUCHAR)buf, (ULONG)len,
                                      BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    if (status >= 0) return UUID_OK;
    uuid_set_error("BCryptGenRandom failed");
    return UUID_E_RANDOM;
}

#elif defined(UUID_PLATFORM_APPLE)

static uuid_error_t uuid_platform_random(void *buf, size_t len) {
    if (arc4random_buf(buf, len) == 0) return UUID_OK;
    uuid_set_error("arc4random_buf failed");
    return UUID_E_RANDOM;
}

#elif defined(UUID_PLATFORM_LINUX)

static uuid_error_t uuid_platform_random(void *buf, size_t len) {
    size_t total = 0;
    while (total < len) {
        ssize_t n = getrandom((uint8_t *)buf + total, len - total, 0);
        if (n < 0) {
            /* Fallback to /dev/urandom */
            int fd = open("/dev/urandom", O_RDONLY);
            if (fd < 0) {
                uuid_set_error("Failed to open /dev/urandom");
                return UUID_E_RANDOM;
            }
            total = 0;
            while (total < len) {
                n = read(fd, (uint8_t *)buf + total, len - total);
                if (n <= 0) {
                    close(fd);
                    uuid_set_error("Failed to read /dev/urandom");
                    return UUID_E_RANDOM;
                }
                total += (size_t)n;
            }
            close(fd);
            return UUID_OK;
        }
        total += (size_t)n;
    }
    return UUID_OK;
}

#elif defined(UUID_PLATFORM_BSD)

static uuid_error_t uuid_platform_random(void *buf, size_t len) {
    ssize_t n = getrandom(buf, len, 0);
    if (n < 0 || (size_t)n != len) {
        uuid_set_error("getrandom failed");
        return UUID_E_RANDOM;
    }
    return UUID_OK;
}

#else

/* Fallback: poor man's entropy from time + pid (NOT cryptographically secure) */
static uuid_error_t uuid_platform_random(void *buf, size_t len) {
    size_t i;
    unsigned int seed = (unsigned int)time(NULL);
#if defined(_MSC_VER)
    seed ^= (unsigned int)_getpid();
    srand(seed);
#else
    seed ^= (unsigned int)getpid();
    srand(seed);
#endif
    for (i = 0; i < len; i++) {
        ((uint8_t *)buf)[i] = (uint8_t)(rand() & 0xFF);
    }
    uuid_set_error("Using non-cryptographic random fallback");
    return UUID_OK;
}

#endif

/* ── Platform Time ────────────────────────────────────────────────────── */

static uint64_t uuid_current_time_100ns(void) {
    uint64_t ticks;
#if defined(UUID_PLATFORM_WINDOWS)
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ticks = ((uint64_t)ft.dwHighDateTime << 32) | (uint64_t)ft.dwLowDateTime;
#elif defined(UUID_PLATFORM_LINUX) || defined(UUID_PLATFORM_BSD)
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ticks = (uint64_t)ts.tv_sec * 10000000ULL + (uint64_t)(ts.tv_nsec / 100);
#elif defined(UUID_PLATFORM_APPLE)
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ticks = (uint64_t)ts.tv_sec * 10000000ULL + (uint64_t)(ts.tv_nsec / 100);
#else
    ticks = (uint64_t)time(NULL) * 10000000ULL;
#endif
    return ticks + UUID_GREGORIAN_OFFSET;
}

static uint64_t uuid_current_time_ms(void) {
#if defined(UUID_PLATFORM_WINDOWS)
    FILETIME ft;
    uint64_t ticks;
    GetSystemTimeAsFileTime(&ft);
    ticks = ((uint64_t)ft.dwHighDateTime << 32) | (uint64_t)ft.dwLowDateTime;
    /* Convert 100-ns to ms, subtract Gregorian offset to get Unix time */
    return (ticks - UUID_GREGORIAN_OFFSET) / 10000ULL;
#elif defined(UUID_PLATFORM_LINUX) || defined(UUID_PLATFORM_BSD) || defined(UUID_PLATFORM_APPLE)
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)(ts.tv_nsec / 1000000ULL);
#else
    return (uint64_t)time(NULL) * 1000ULL;
#endif
}

/* ── Basic UUID Operations ────────────────────────────────────────────── */

uuid_error_t uuid_init_nil(uuid_t *uuid) {
    if (!uuid) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    memset(uuid->bytes, 0, UUID_SIZE);
    return UUID_OK;
}

uuid_error_t uuid_init_max(uuid_t *uuid) {
    if (!uuid) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    memset(uuid->bytes, 0xFF, UUID_SIZE);
    return UUID_OK;
}

uuid_error_t uuid_init_from_bytes(uuid_t *uuid, const uint8_t data[UUID_SIZE]) {
    if (!uuid || !data) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    memcpy(uuid->bytes, data, UUID_SIZE);
    return UUID_OK;
}

uuid_error_t uuid_copy(uuid_t *dst, const uuid_t *src) {
    if (!dst || !src) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    memcpy(dst->bytes, src->bytes, UUID_SIZE);
    return UUID_OK;
}

int uuid_compare(const uuid_t *a, const uuid_t *b) {
    if (!a || !b) return 0;
    return memcmp(a->bytes, b->bytes, UUID_SIZE);
}

int uuid_is_nil(const uuid_t *uuid) {
    static const uint8_t zeros[UUID_SIZE] = {0};
    if (!uuid) return 0;
    return memcmp(uuid->bytes, zeros, UUID_SIZE) == 0;
}

int uuid_is_max(const uuid_t *uuid) {
    static const uint8_t maxes[UUID_SIZE] = {
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    };
    if (!uuid) return 0;
    return memcmp(uuid->bytes, maxes, UUID_SIZE) == 0;
}

uuid_error_t uuid_swap(uuid_t *a, uuid_t *b) {
    uuid_t tmp;
    if (!a || !b) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    memcpy(&tmp, a, sizeof(uuid_t));
    memcpy(a, b, sizeof(uuid_t));
    memcpy(b, &tmp, sizeof(uuid_t));
    return UUID_OK;
}

uuid_error_t uuid_set(uuid_t *uuid, uint8_t value) {
    if (!uuid) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    memset(uuid->bytes, value, UUID_SIZE);
    return UUID_OK;
}

/* ── Version & Variant ────────────────────────────────────────────────── */

int uuid_version(const uuid_t *uuid) {
    if (!uuid) return -1;
    /* Version is stored in bits 48-51 of byte 6 */
    return (int)((uuid->bytes[6] >> 4) & 0x0F);
}

int uuid_variant(const uuid_t *uuid) {
    if (!uuid) return -1;
    /* Variant is stored in the top bits of byte 8 */
    if ((uuid->bytes[8] & 0x80) == 0x00) return UUID_VARIANT_NCS;
    if ((uuid->bytes[8] & 0xC0) == 0x80) return UUID_VARIANT_RFC_4122;
    if ((uuid->bytes[8] & 0xE0) == 0xC0) return UUID_VARIANT_MICROSOFT;
    return UUID_VARIANT_MAX;
}

uuid_error_t uuid_get_version(const uuid_t *uuid, uuid_version_t *ver) {
    int v;
    if (!uuid) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    v = uuid_version(uuid);
    if (v < 0) {
        uuid_set_error("Invalid UUID");
        return UUID_E_INVALID_UUID;
    }
    if (ver) *ver = (uuid_version_t)v;
    return UUID_OK;
}

uuid_error_t uuid_get_variant(const uuid_t *uuid, uuid_variant_t *var) {
    int v;
    if (!uuid) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    v = uuid_variant(uuid);
    if (v < 0) {
        uuid_set_error("Invalid UUID");
        return UUID_E_INVALID_UUID;
    }
    if (var) *var = (uuid_variant_t)v;
    return UUID_OK;
}

uuid_error_t uuid_v1_timestamp(const uuid_t *uuid, uint64_t *timestamp) {
    if (!uuid || !timestamp) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    if (uuid_version(uuid) != 1) {
        uuid_set_error("Not a UUID v1");
        return UUID_E_INVALID_VERSION;
    }
    /* v1 timestamp: 60 bits spread across bytes 0-7 */
    *timestamp = ((uint64_t)uuid->bytes[7] << 48) |
                 ((uint64_t)uuid->bytes[6] << 40) |
                 ((uint64_t)uuid->bytes[5] << 32) |
                 ((uint64_t)uuid->bytes[4] << 24) |
                 ((uint64_t)uuid->bytes[3] << 16) |
                 ((uint64_t)uuid->bytes[2] << 8)  |
                 ((uint64_t)uuid->bytes[1])        |
                 (((uint64_t)uuid->bytes[0] & 0x0F) << 56);
    return UUID_OK;
}

uuid_error_t uuid_v7_timestamp_ms(const uuid_t *uuid, uint64_t *timestamp_ms) {
    if (!uuid || !timestamp_ms) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    if (uuid_version(uuid) != 7) {
        uuid_set_error("Not a UUID v7");
        return UUID_E_INVALID_VERSION;
    }
    /* v7 timestamp: 48 bits in bytes 0-5 (big-endian) */
    *timestamp_ms = ((uint64_t)uuid->bytes[0] << 40) |
                    ((uint64_t)uuid->bytes[1] << 32) |
                    ((uint64_t)uuid->bytes[2] << 24) |
                    ((uint64_t)uuid->bytes[3] << 16) |
                    ((uint64_t)uuid->bytes[4] << 8)  |
                    ((uint64_t)uuid->bytes[5]);
    return UUID_OK;
}

uuid_error_t uuid_v1_clock_seq(const uuid_t *uuid, uint16_t *clock_seq) {
    if (!uuid || !clock_seq) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    if (uuid_version(uuid) != 1) {
        uuid_set_error("Not a UUID v1");
        return UUID_E_INVALID_VERSION;
    }
    /* Clock sequence: 14 bits in bytes 8-9 (variant bits masked) */
    *clock_seq = (uint16_t)(((uint16_t)(uuid->bytes[8] & 0x3F) << 8) |
                 (uint16_t)uuid->bytes[9]);
    return UUID_OK;
}

uuid_error_t uuid_v1_node(const uuid_t *uuid, uint8_t node[6]) {
    if (!uuid || !node) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    if (uuid_version(uuid) != 1) {
        uuid_set_error("Not a UUID v1");
        return UUID_E_INVALID_VERSION;
    }
    memcpy(node, &uuid->bytes[10], 6);
    return UUID_OK;
}

/* ── Byte Access ──────────────────────────────────────────────────────── */

uuid_error_t uuid_byte_at(const uuid_t *uuid, size_t index, uint8_t *byte) {
    if (!uuid || !byte) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    if (index >= UUID_SIZE) {
        uuid_set_error("Index out of range");
        return UUID_E_INVALID_ARG;
    }
    *byte = uuid->bytes[index];
    return UUID_OK;
}

const uint8_t *uuid_bytes(const uuid_t *uuid) {
    if (!uuid) return NULL;
    return uuid->bytes;
}

/* ── Parsing ──────────────────────────────────────────────────────────── */

uuid_error_t uuid_parse_n(uuid_t *uuid, const char *str, size_t len) {
    size_t i, pos = 0;
    uint8_t bytes[UUID_SIZE];
    int need_dash[4] = {4, 5, 6, 7};
    int dash_idx = 0;
    int have_dashes = 0;

    if (!uuid || !str) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }

    /* Skip leading whitespace */
    while (pos < len && (str[pos] == ' ' || str[pos] == '\t' || str[pos] == '\n' || str[pos] == '\r')) {
        pos++;
    }

    if (pos >= len) {
        uuid_set_error("Empty string");
        return UUID_E_INVALID_FORMAT;
    }

    /* Check for braces */
    if (str[pos] == '{') {
        pos++;
        /* Skip trailing whitespace inside braces */
        while (pos < len && str[pos] == ' ') pos++;
        if (pos >= len) {
            uuid_set_error("Incomplete braced UUID");
            return UUID_E_INVALID_FORMAT;
        }
    }

    /* Try canonical format with dashes: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx */
    if (len - pos >= 36 && str[pos + 8] == '-') {
        have_dashes = 1;
    }

    pos = (str[0] == '{') ? 1 : 0;
    /* Skip leading whitespace again after brace check */
    while (pos < len && (str[pos] == ' ' || str[pos] == '\t')) pos++;

    for (i = 0; i < UUID_SIZE; i++) {
        int hi, lo;

        if (pos >= len) {
            uuid_set_error("Unexpected end of input");
            return UUID_E_INVALID_FORMAT;
        }

        /* Check for closing brace */
        if (str[pos] == '}') {
            pos++;
            break;
        }

        hi = hex_to_val(str[pos]);
        if (hi < 0) {
            uuid_set_error("Invalid hex character");
            return UUID_E_INVALID_FORMAT;
        }
        pos++;

        if (pos >= len) {
            uuid_set_error("Unexpected end of input");
            return UUID_E_INVALID_FORMAT;
        }

        lo = hex_to_val(str[pos]);
        if (lo < 0) {
            uuid_set_error("Invalid hex character");
            return UUID_E_INVALID_FORMAT;
        }
        pos++;

        bytes[i] = (uint8_t)((hi << 4) | lo);

        /* Expect dash after specific bytes */
        if (have_dashes && dash_idx < 4 && i + 1 == (size_t)(need_dash[dash_idx] + dash_idx)) {
            if (pos < len && str[pos] == '-') {
                pos++;
                dash_idx++;
            } else {
                uuid_set_error("Missing dash in canonical format");
                return UUID_E_INVALID_FORMAT;
            }
        }
    }

    /* Skip trailing whitespace */
    while (pos < len && (str[pos] == ' ' || str[pos] == '\t' || str[pos] == '\n' || str[pos] == '\r')) {
        pos++;
    }

    /* Expect closing brace if we opened one */
    if (str[0] == '{') {
        while (pos < len && str[pos] == ' ') pos++;
        if (pos >= len || str[pos] != '}') {
            uuid_set_error("Missing closing brace");
            return UUID_E_INVALID_FORMAT;
        }
        pos++;
        while (pos < len && (str[pos] == ' ' || str[pos] == '\t')) pos++;
    }

    /* Must have consumed exactly the right number of characters */
    if (pos != len) {
        uuid_set_error("Trailing garbage in UUID string");
        return UUID_E_INVALID_FORMAT;
    }

    memcpy(uuid->bytes, bytes, UUID_SIZE);
    return UUID_OK;
}

uuid_error_t uuid_parse(uuid_t *uuid, const char *str) {
    if (!str) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    return uuid_parse_n(uuid, str, strlen(str));
}

uuid_error_t uuid_parse_binary(uuid_t *uuid, const void *data, size_t len) {
    if (!uuid || !data) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    if (len < UUID_SIZE) {
        uuid_set_error("Buffer too small for binary UUID");
        return UUID_E_INVALID_LENGTH;
    }
    memcpy(uuid->bytes, data, UUID_SIZE);
    return UUID_OK;
}

uuid_error_t uuid_validate_n(const char *str, size_t len) {
    uuid_t tmp;
    return uuid_parse_n(&tmp, str, len);
}

uuid_error_t uuid_validate(const char *str) {
    if (!str) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    return uuid_validate_n(str, strlen(str));
}

/* ── Formatting ───────────────────────────────────────────────────────── */

uuid_error_t uuid_format(const uuid_t *uuid, char *buf, size_t size) {
    const char *hex = hex_lower;
    if (!uuid || !buf) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    if (size < UUID_STRING_LEN) {
        uuid_set_error("Buffer too small");
        return UUID_E_BUFFER_TOO_SMALL;
    }
    buf[0]  = hex[(uuid->bytes[0] >> 4) & 0x0F];
    buf[1]  = hex[uuid->bytes[0] & 0x0F];
    buf[2]  = hex[(uuid->bytes[1] >> 4) & 0x0F];
    buf[3]  = hex[uuid->bytes[1] & 0x0F];
    buf[4]  = hex[(uuid->bytes[2] >> 4) & 0x0F];
    buf[5]  = hex[uuid->bytes[2] & 0x0F];
    buf[6]  = hex[(uuid->bytes[3] >> 4) & 0x0F];
    buf[7]  = hex[uuid->bytes[3] & 0x0F];
    buf[8]  = '-';
    buf[9]  = hex[(uuid->bytes[4] >> 4) & 0x0F];
    buf[10] = hex[uuid->bytes[4] & 0x0F];
    buf[11] = hex[(uuid->bytes[5] >> 4) & 0x0F];
    buf[12] = hex[uuid->bytes[5] & 0x0F];
    buf[13] = '-';
    buf[14] = hex[(uuid->bytes[6] >> 4) & 0x0F];
    buf[15] = hex[uuid->bytes[6] & 0x0F];
    buf[16] = hex[(uuid->bytes[7] >> 4) & 0x0F];
    buf[17] = hex[uuid->bytes[7] & 0x0F];
    buf[18] = '-';
    buf[19] = hex[(uuid->bytes[8] >> 4) & 0x0F];
    buf[20] = hex[uuid->bytes[8] & 0x0F];
    buf[21] = hex[(uuid->bytes[9] >> 4) & 0x0F];
    buf[22] = hex[uuid->bytes[9] & 0x0F];
    buf[23] = '-';
    buf[24] = hex[(uuid->bytes[10] >> 4) & 0x0F];
    buf[25] = hex[uuid->bytes[10] & 0x0F];
    buf[26] = hex[(uuid->bytes[11] >> 4) & 0x0F];
    buf[27] = hex[uuid->bytes[11] & 0x0F];
    buf[28] = hex[(uuid->bytes[12] >> 4) & 0x0F];
    buf[29] = hex[uuid->bytes[12] & 0x0F];
    buf[30] = hex[(uuid->bytes[13] >> 4) & 0x0F];
    buf[31] = hex[uuid->bytes[13] & 0x0F];
    buf[32] = hex[(uuid->bytes[14] >> 4) & 0x0F];
    buf[33] = hex[uuid->bytes[14] & 0x0F];
    buf[34] = hex[(uuid->bytes[15] >> 4) & 0x0F];
    buf[35] = hex[uuid->bytes[15] & 0x0F];
    buf[36] = '\0';
    return UUID_OK;
}

uuid_error_t uuid_format_compact(const uuid_t *uuid, char *buf, size_t size) {
    const char *hex = hex_lower;
    size_t i;
    if (!uuid || !buf) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    if (size < UUID_COMPACT_LEN) {
        uuid_set_error("Buffer too small");
        return UUID_E_BUFFER_TOO_SMALL;
    }
    for (i = 0; i < UUID_SIZE; i++) {
        buf[i * 2]     = hex[(uuid->bytes[i] >> 4) & 0x0F];
        buf[i * 2 + 1] = hex[uuid->bytes[i] & 0x0F];
    }
    buf[UUID_COMPACT_LEN_NO_NUL] = '\0';
    return UUID_OK;
}

uuid_error_t uuid_format_braces(const uuid_t *uuid, char *buf, size_t size) {
    uuid_error_t err;
    if (!uuid || !buf) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    if (size < 39) {
        uuid_set_error("Buffer too small (need 39 bytes)");
        return UUID_E_BUFFER_TOO_SMALL;
    }
    buf[0] = '{';
    err = uuid_format(uuid, buf + 1, size - 2);
    if (err != UUID_OK) return err;
    buf[37] = '}';
    buf[38] = '\0';
    return UUID_OK;
}

uuid_error_t uuid_format_options(const uuid_t *uuid, char *buf, size_t size, int format) {
    const char *hex;
    size_t i, pos = 0;
    int need_braces = (format & UUID_FMT_BRACES) != 0;
    int uppercase = (format & UUID_FMT_UPPER) != 0;
    int compact = (format & UUID_FMT_COMPACT) != 0;

    if (!uuid || !buf) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }

    if (format & UUID_FMT_BINARY) {
        return uuid_format_binary(uuid, buf, size);
    }

    hex = uppercase ? hex_upper : hex_lower;

    if (need_braces) {
        if (size < 39) {
            uuid_set_error("Buffer too small");
            return UUID_E_BUFFER_TOO_SMALL;
        }
        buf[pos++] = '{';
    } else {
        if (compact) {
            if (size < UUID_COMPACT_LEN) {
                uuid_set_error("Buffer too small");
                return UUID_E_BUFFER_TOO_SMALL;
            }
        } else {
            if (size < UUID_STRING_LEN) {
                uuid_set_error("Buffer too small");
                return UUID_E_BUFFER_TOO_SMALL;
            }
        }
    }

    for (i = 0; i < UUID_SIZE; i++) {
        buf[pos++] = hex[(uuid->bytes[i] >> 4) & 0x0F];
        buf[pos++] = hex[uuid->bytes[i] & 0x0F];
        if (!compact) {
            if (i == 3 || i == 5 || i == 7 || i == 9) {
                buf[pos++] = '-';
            }
        }
    }

    if (need_braces) {
        buf[pos++] = '}';
    }

    buf[pos] = '\0';
    return UUID_OK;
}

uuid_error_t uuid_format_binary(const uuid_t *uuid, void *buf, size_t size) {
    if (!uuid || !buf) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    if (size < UUID_SIZE) {
        uuid_set_error("Buffer too small");
        return UUID_E_BUFFER_TOO_SMALL;
    }
    memcpy(buf, uuid->bytes, UUID_SIZE);
    return UUID_OK;
}

char *uuid_to_string(const uuid_t *uuid, char *buf, size_t size) {
    if (uuid_format(uuid, buf, size) == UUID_OK) return buf;
    return NULL;
}

/* ── MD5 Implementation (RFC 1321, for UUID v3) ───────────────────────── */

static void uuid_md5_transform(uint32_t state[4], const uint8_t block[64]) {
    uint32_t a, b, c, d, x[16];
    int i;

    for (i = 0; i < 16; i++) {
        x[i] = (uint32_t)block[i*4] | ((uint32_t)block[i*4+1] << 8) |
               ((uint32_t)block[i*4+2] << 16) | ((uint32_t)block[i*4+3] << 24);
    }

    a = state[0]; b = state[1]; c = state[2]; d = state[3];

    #define MD5_F(x,y,z) (((x) & (y)) | ((~x) & (z)))
    #define MD5_G(x,y,z) (((x) & (z)) | ((y) & (~z)))
    #define MD5_H(x,y,z) ((x) ^ (y) ^ (z))
    #define MD5_I(x,y,z) ((y) ^ ((x) | (~z)))
    #define MD5_ROT(x,n) (((x) << (n)) | ((x) >> (32-(n))))
    #define MD5_FF(a,b,c,d,x,k,s) do { (a) += MD5_F((b),(c),(d)) + (x) + (k); (a) = MD5_ROT((a),(s)); (a) += (b); } while(0)
    #define MD5_GG(a,b,c,d,x,k,s) do { (a) += MD5_G((b),(c),(d)) + (x) + (k); (a) = MD5_ROT((a),(s)); (a) += (b); } while(0)
    #define MD5_HH(a,b,c,d,x,k,s) do { (a) += MD5_H((b),(c),(d)) + (x) + (k); (a) = MD5_ROT((a),(s)); (a) += (b); } while(0)
    #define MD5_II(a,b,c,d,x,k,s) do { (a) += MD5_I((b),(c),(d)) + (x) + (k); (a) = MD5_ROT((a),(s)); (a) += (b); } while(0)

    MD5_FF(a,b,c,d,x[ 0],0xd76aa478, 7); MD5_FF(d,a,b,c,x[ 1],0xe8c7b756,12);
    MD5_FF(c,d,a,b,x[ 2],0x242070db,17); MD5_FF(b,c,d,a,x[ 3],0xc1bdceee,22);
    MD5_FF(a,b,c,d,x[ 4],0xf57c0faf, 7); MD5_FF(d,a,b,c,x[ 5],0x4787c62a,12);
    MD5_FF(c,d,a,b,x[ 6],0xa8304613,17); MD5_FF(b,c,d,a,x[ 7],0xfd469501,22);
    MD5_FF(a,b,c,d,x[ 8],0x698098d8, 7); MD5_FF(d,a,b,c,x[ 9],0x8b44f7af,12);
    MD5_FF(c,d,a,b,x[10],0xffff5bb1,17); MD5_FF(b,c,d,a,x[11],0x895cd7be,22);
    MD5_FF(a,b,c,d,x[12],0x6b901122, 7); MD5_FF(d,a,b,c,x[13],0xfd987193,12);
    MD5_FF(c,d,a,b,x[14],0xa679438e,17); MD5_FF(b,c,d,a,x[15],0x49b40821,22);

    MD5_GG(a,b,c,d,x[ 1],0xf61e2562, 5); MD5_GG(d,a,b,c,x[ 6],0xc040b340, 9);
    MD5_GG(c,d,a,b,x[11],0x265e5a51,14); MD5_GG(b,c,d,a,x[ 0],0xe9b6c7aa,20);
    MD5_GG(a,b,c,d,x[ 5],0xd62f105d, 5); MD5_GG(d,a,b,c,x[10],0x02441453, 9);
    MD5_GG(c,d,a,b,x[15],0xd8a1e681,14); MD5_GG(b,c,d,a,x[ 4],0xe7d3fbc8,20);
    MD5_GG(a,b,c,d,x[ 9],0x21e1cde6, 5); MD5_GG(d,a,b,c,x[14],0xc33707d6, 9);
    MD5_GG(c,d,a,b,x[ 3],0xf4d50d87,14); MD5_GG(b,c,d,a,x[ 8],0x455a14ed,20);
    MD5_GG(a,b,c,d,x[13],0xa9e3e905, 5); MD5_GG(d,a,b,c,x[ 2],0xfcefa3f8, 9);
    MD5_GG(c,d,a,b,x[ 7],0x676f02d9,14); MD5_GG(b,c,d,a,x[12],0x8d2a4c8a,20);

    MD5_HH(a,b,c,d,x[ 5],0xfffa3942, 4); MD5_HH(d,a,b,c,x[ 8],0x8771f681,11);
    MD5_HH(c,d,a,b,x[11],0x6d9d6122,16); MD5_HH(b,c,d,a,x[14],0xfde5380c,23);
    MD5_HH(a,b,c,d,x[ 1],0xa4beea44, 4); MD5_HH(d,a,b,c,x[ 4],0x4bdecfa9,11);
    MD5_HH(c,d,a,b,x[ 7],0xf6bb4b60,16); MD5_HH(b,c,d,a,x[10],0xbebfbc70,23);
    MD5_HH(a,b,c,d,x[13],0x289b7ec6, 4); MD5_HH(d,a,b,c,x[ 0],0xeaa127fa,11);
    MD5_HH(c,d,a,b,x[ 3],0xd4ef3085,16); MD5_HH(b,c,d,a,x[ 6],0x04881d05,23);
    MD5_HH(a,b,c,d,x[ 9],0xd9d4d039, 4); MD5_HH(d,a,b,c,x[12],0xe6db99e5,11);
    MD5_HH(c,d,a,b,x[15],0x1fa27cf8,16); MD5_HH(b,c,d,a,x[ 2],0xc4ac5665,23);

    MD5_II(a,b,c,d,x[ 0],0xf4292244, 6); MD5_II(d,a,b,c,x[ 7],0x432aff97,10);
    MD5_II(c,d,a,b,x[14],0xab9423a7,15); MD5_II(b,c,d,a,x[ 5],0xfc93a039,21);
    MD5_II(a,b,c,d,x[12],0x655b59c3, 6); MD5_II(d,a,b,c,x[ 3],0x8f0ccc92,10);
    MD5_II(c,d,a,b,x[10],0xffeff47d,15); MD5_II(b,c,d,a,x[ 1],0x85845dd1,21);
    MD5_II(a,b,c,d,x[ 8],0x6fa87e4f, 6); MD5_II(d,a,b,c,x[15],0xfe2ce6e0,10);
    MD5_II(c,d,a,b,x[ 6],0xa3014314,15); MD5_II(b,c,d,a,x[13],0x4e0811a1,21);
    MD5_II(a,b,c,d,x[ 4],0xf7537e82, 6); MD5_II(d,a,b,c,x[11],0xbd3af235,10);
    MD5_II(c,d,a,b,x[ 2],0x2ad7d2bb,15); MD5_II(b,c,d,a,x[ 9],0xeb86d391,21);

    #undef MD5_F
    #undef MD5_G
    #undef MD5_H
    #undef MD5_I
    #undef MD5_ROT
    #undef MD5_FF
    #undef MD5_GG
    #undef MD5_HH
    #undef MD5_II

    state[0] += a; state[1] += b; state[2] += c; state[3] += d;
}

void uuid_md5(const void *data, size_t data_len, uint8_t digest[16]) {
    uint32_t state[4] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
    const uint8_t *ptr = (const uint8_t *)data;
    size_t remaining = data_len;
    uint8_t block[64];
    uint64_t bit_len = (uint64_t)data_len * 8;

    while (remaining >= 64) {
        uuid_md5_transform(state, ptr);
        ptr += 64;
        remaining -= 64;
    }

    /* Padding */
    memset(block, 0, 64);
    memcpy(block, ptr, remaining);
    block[remaining] = 0x80;

    if (remaining >= 56) {
        uuid_md5_transform(state, block);
        memset(block, 0, 56);
    }

    block[56] = (uint8_t)(bit_len);
    block[57] = (uint8_t)(bit_len >> 8);
    block[58] = (uint8_t)(bit_len >> 16);
    block[59] = (uint8_t)(bit_len >> 24);
    block[60] = (uint8_t)(bit_len >> 32);
    block[61] = (uint8_t)(bit_len >> 40);
    block[62] = (uint8_t)(bit_len >> 48);
    block[63] = (uint8_t)(bit_len >> 56);
    uuid_md5_transform(state, block);

    /* Output in little-endian */
    digest[0]  = (uint8_t)(state[0]);        digest[1]  = (uint8_t)(state[0] >> 8);
    digest[2]  = (uint8_t)(state[0] >> 16);  digest[3]  = (uint8_t)(state[0] >> 24);
    digest[4]  = (uint8_t)(state[1]);        digest[5]  = (uint8_t)(state[1] >> 8);
    digest[6]  = (uint8_t)(state[1] >> 16);  digest[7]  = (uint8_t)(state[1] >> 24);
    digest[8]  = (uint8_t)(state[2]);        digest[9]  = (uint8_t)(state[2] >> 8);
    digest[10] = (uint8_t)(state[2] >> 16);  digest[11] = (uint8_t)(state[2] >> 24);
    digest[12] = (uint8_t)(state[3]);        digest[13] = (uint8_t)(state[3] >> 8);
    digest[14] = (uint8_t)(state[3] >> 16);  digest[15] = (uint8_t)(state[3] >> 24);
}

/* ── SHA-1 Implementation (FIPS 180-4, for UUID v5) ───────────────────── */

static void uuid_sha1_transform(uint32_t state[5], const uint8_t block[64]) {
    uint32_t a, b, c, d, e, w[80];
    int i;

    for (i = 0; i < 16; i++) {
        w[i] = ((uint32_t)block[i*4] << 24) | ((uint32_t)block[i*4+1] << 16) |
               ((uint32_t)block[i*4+2] << 8) | (uint32_t)block[i*4+3];
    }
    for (i = 16; i < 80; i++) {
        w[i] = w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16];
        w[i] = (w[i] << 1) | (w[i] >> 31);
    }

    a = state[0]; b = state[1]; c = state[2]; d = state[3]; e = state[4];

    for (i = 0; i < 80; i++) {
        uint32_t f, k, temp;
        if (i < 20)      { f = (b & c) | ((~b) & d); k = 0x5A827999; }
        else if (i < 40) { f = b ^ c ^ d;             k = 0x6ED9EBA1; }
        else if (i < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8F1BBCDC; }
        else              { f = b ^ c ^ d;             k = 0xCA62C1D6; }
        temp = ((a << 5) | (a >> 27)) + f + e + k + w[i];
        e = d; d = c; c = (b << 30) | (b >> 2); b = a; a = temp;
    }

    state[0] += a; state[1] += b; state[2] += c; state[3] += d; state[4] += e;
}

void uuid_sha1(const void *data, size_t data_len, uint8_t digest[20]) {
    uint32_t state[5] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};
    const uint8_t *ptr = (const uint8_t *)data;
    size_t remaining = data_len;
    uint8_t block[64];
    uint64_t bit_len = (uint64_t)data_len * 8;

    while (remaining >= 64) {
        uuid_sha1_transform(state, ptr);
        ptr += 64;
        remaining -= 64;
    }

    memset(block, 0, 64);
    memcpy(block, ptr, remaining);
    block[remaining] = 0x80;

    if (remaining >= 56) {
        uuid_sha1_transform(state, block);
        memset(block, 0, 56);
    }

    block[56] = (uint8_t)(bit_len >> 56);
    block[57] = (uint8_t)(bit_len >> 48);
    block[58] = (uint8_t)(bit_len >> 40);
    block[59] = (uint8_t)(bit_len >> 32);
    block[60] = (uint8_t)(bit_len >> 24);
    block[61] = (uint8_t)(bit_len >> 16);
    block[62] = (uint8_t)(bit_len >> 8);
    block[63] = (uint8_t)(bit_len);
    uuid_sha1_transform(state, block);

    digest[0]  = (uint8_t)(state[0] >> 24); digest[1]  = (uint8_t)(state[0] >> 16);
    digest[2]  = (uint8_t)(state[0] >> 8);  digest[3]  = (uint8_t)(state[0]);
    digest[4]  = (uint8_t)(state[1] >> 24); digest[5]  = (uint8_t)(state[1] >> 16);
    digest[6]  = (uint8_t)(state[1] >> 8);  digest[7]  = (uint8_t)(state[1]);
    digest[8]  = (uint8_t)(state[2] >> 24); digest[9]  = (uint8_t)(state[2] >> 16);
    digest[10] = (uint8_t)(state[2] >> 8);  digest[11] = (uint8_t)(state[2]);
    digest[12] = (uint8_t)(state[3] >> 24); digest[13] = (uint8_t)(state[3] >> 16);
    digest[14] = (uint8_t)(state[3] >> 8);  digest[15] = (uint8_t)(state[3]);
    digest[16] = (uint8_t)(state[4] >> 24); digest[17] = (uint8_t)(state[4] >> 16);
    digest[18] = (uint8_t)(state[4] >> 8);  digest[19] = (uint8_t)(state[4]);
}

/* ── UUID v1 (Time-based, Gregorian) ─────────────────────────────────── */

uuid_error_t uuid_v1_init(uuid_v1_state_t *state) {
    if (!state) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    state->timestamp = 0;
    if (uuid_platform_random(&state->clock_seq, sizeof(state->clock_seq)) != UUID_OK) {
        return UUID_E_RANDOM;
    }
    state->clock_seq &= 0x3FFF; /* 14 bits */
    if (uuid_platform_random(state->node, 6) != UUID_OK) {
        return UUID_E_RANDOM;
    }
    /* Set multicast bit (bit 0 of byte 10) to indicate random node */
    state->node[0] |= 0x01;
    state->initialized = 1;
    return UUID_OK;
}

uuid_error_t uuid_v1(uuid_v1_state_t *state, uuid_t *uuid) {
    uint64_t ts;
    if (!state || !uuid) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    if (!state->initialized) {
        uuid_set_error("State not initialized");
        return UUID_E_INVALID_ARG;
    }

    ts = uuid_current_time_100ns();
    if (ts <= state->timestamp) {
        /* Monotonic: increment timestamp and clock_seq */
        state->timestamp++;
        state->clock_seq = (state->clock_seq + 1) & 0x3FFF;
    } else {
        state->timestamp = ts;
    }

    /* time_low (bytes 0-3) */
    uuid->bytes[0] = (uint8_t)(state->timestamp >> 40);
    uuid->bytes[1] = (uint8_t)(state->timestamp >> 32);
    uuid->bytes[2] = (uint8_t)(state->timestamp >> 24);
    uuid->bytes[3] = (uint8_t)(state->timestamp >> 16);
    /* time_mid (bytes 4-5) */
    uuid->bytes[4] = (uint8_t)(state->timestamp >> 8);
    uuid->bytes[5] = (uint8_t)(state->timestamp);
    /* time_hi_and_version (bytes 6-7) */
    uuid->bytes[6] = (uint8_t)(((state->timestamp >> 48) & 0x0F) | (1 << 4)); /* version 1 */
    uuid->bytes[7] = (uint8_t)(state->timestamp >> 56);
    /* clock_seq_hi_and_reserved (byte 8) — variant 10xx */
    uuid->bytes[8] = (uint8_t)(((state->clock_seq >> 8) & 0x3F) | 0x80);
    /* clock_seq_low (byte 9) */
    uuid->bytes[9] = (uint8_t)(state->clock_seq & 0xFF);
    /* node (bytes 10-15) */
    memcpy(&uuid->bytes[10], state->node, 6);

    return UUID_OK;
}

uuid_error_t uuid_v1_generate(uuid_t *uuid) {
    static UUID_TLS uuid_v1_state_t tls_state = {0, 0, {0}, 0};
    uuid_error_t err;
    if (!tls_state.initialized) {
        err = uuid_v1_init(&tls_state);
        if (err != UUID_OK) return err;
    }
    return uuid_v1(&tls_state, uuid);
}

/* ── UUID v3 (Name-based, MD5) ───────────────────────────────────────── */

uuid_error_t uuid_v3(const uuid_t *namespace_uuid, const void *name,
                     size_t name_len, uuid_t *uuid) {
    uint8_t digest[16];
    if (!namespace_uuid || !name || !uuid) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }

    /* Use stack buffer for typical names, heap for large ones */
    {
        size_t total = UUID_SIZE + name_len;
        uint8_t stack_buf[512];
        uint8_t *buf = (total <= sizeof(stack_buf)) ? stack_buf : (uint8_t *)malloc(total);
        if (!buf) {
            uuid_set_error("Memory allocation failed");
            return UUID_E_NO_MEMORY;
        }
        memcpy(buf, namespace_uuid->bytes, UUID_SIZE);
        memcpy(buf + UUID_SIZE, name, name_len);
        uuid_md5(buf, total, digest);
        if (buf != stack_buf) free(buf);
    }

    /* Set version 3 */
    digest[6] = (uint8_t)((digest[6] & 0x0F) | (3 << 4));
    /* Set variant 1 (RFC 4122) */
    digest[8] = (uint8_t)((digest[8] & 0x3F) | 0x80);

    memcpy(uuid->bytes, digest, UUID_SIZE);
    return UUID_OK;
}

uuid_error_t uuid_v3_ns(uuid_namespace_t ns, const char *name, size_t name_len,
                        uuid_t *uuid) {
    uuid_t ns_uuid;
    uuid_error_t err;
    if (!name || !uuid) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    err = uuid_namespace(ns, &ns_uuid);
    if (err != UUID_OK) return err;
    return uuid_v3(&ns_uuid, name, name_len, uuid);
}

/* ── UUID v4 (Random) ────────────────────────────────────────────────── */

uuid_error_t uuid_v4(uuid_t *uuid) {
    if (!uuid) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    if (uuid_platform_random(uuid->bytes, UUID_SIZE) != UUID_OK) {
        return UUID_E_RANDOM;
    }
    /* Set version 4 */
    uuid->bytes[6] = (uint8_t)((uuid->bytes[6] & 0x0F) | (4 << 4));
    /* Set variant 1 (RFC 4122) */
    uuid->bytes[8] = (uint8_t)((uuid->bytes[8] & 0x3F) | 0x80);
    return UUID_OK;
}

/* ── UUID v5 (Name-based, SHA-1) ─────────────────────────────────────── */

uuid_error_t uuid_v5(const uuid_t *namespace_uuid, const void *name,
                     size_t name_len, uuid_t *uuid) {
    uint8_t digest[20];
    if (!namespace_uuid || !name || !uuid) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }

    {
        size_t total = UUID_SIZE + name_len;
        uint8_t stack_buf[512];
        uint8_t *buf = (total <= sizeof(stack_buf)) ? stack_buf : (uint8_t *)malloc(total);
        if (!buf) {
            uuid_set_error("Memory allocation failed");
            return UUID_E_NO_MEMORY;
        }
        memcpy(buf, namespace_uuid->bytes, UUID_SIZE);
        memcpy(buf + UUID_SIZE, name, name_len);
        uuid_sha1(buf, total, digest);
        if (buf != stack_buf) free(buf);
    }

    /* Use first 16 bytes of SHA-1 hash */
    memcpy(uuid->bytes, digest, UUID_SIZE);
    /* Set version 5 */
    uuid->bytes[6] = (uint8_t)((uuid->bytes[6] & 0x0F) | (5 << 4));
    /* Set variant 1 (RFC 4122) */
    uuid->bytes[8] = (uint8_t)((uuid->bytes[8] & 0x3F) | 0x80);

    return UUID_OK;
}

uuid_error_t uuid_v5_ns(uuid_namespace_t ns, const char *name, size_t name_len,
                        uuid_t *uuid) {
    uuid_t ns_uuid;
    uuid_error_t err;
    if (!name || !uuid) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    err = uuid_namespace(ns, &ns_uuid);
    if (err != UUID_OK) return err;
    return uuid_v5(&ns_uuid, name, name_len, uuid);
}

/* ── UUID v6 (Reordered time-based, Gregorian) ───────────────────────── */

uuid_error_t uuid_v6_init(uuid_v1_state_t *state) {
    return uuid_v1_init(state);
}

uuid_error_t uuid_v6(uuid_v1_state_t *state, uuid_t *uuid) {
    uint64_t ts;
    if (!state || !uuid) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    if (!state->initialized) {
        uuid_set_error("State not initialized");
        return UUID_E_INVALID_ARG;
    }

    ts = uuid_current_time_100ns();
    if (ts <= state->timestamp) {
        state->timestamp++;
        state->clock_seq = (state->clock_seq + 1) & 0x3FFF;
    } else {
        state->timestamp = ts;
    }

    /* v6 is same as v1 but with time fields reordered for sortability */
    /* time_high (bytes 0-3): top 32 bits of timestamp */
    uuid->bytes[0] = (uint8_t)(state->timestamp >> 56);
    uuid->bytes[1] = (uint8_t)(state->timestamp >> 48);
    uuid->bytes[2] = (uint8_t)(state->timestamp >> 40);
    uuid->bytes[3] = (uint8_t)(state->timestamp >> 32);
    /* time_mid (bytes 4-5) */
    uuid->bytes[4] = (uint8_t)(state->timestamp >> 24);
    uuid->bytes[5] = (uint8_t)(state->timestamp >> 16);
    /* time_low_and_version (bytes 6-7) */
    uuid->bytes[6] = (uint8_t)(((state->timestamp >> 8) & 0x0F) | (6 << 4));
    uuid->bytes[7] = (uint8_t)(state->timestamp);
    /* clock_seq (bytes 8-9) */
    uuid->bytes[8] = (uint8_t)(((state->clock_seq >> 8) & 0x3F) | 0x80);
    uuid->bytes[9] = (uint8_t)(state->clock_seq & 0xFF);
    /* node (bytes 10-15) */
    memcpy(&uuid->bytes[10], state->node, 6);

    return UUID_OK;
}

uuid_error_t uuid_v6_generate(uuid_t *uuid) {
    static UUID_TLS uuid_v1_state_t tls_state = {0, 0, {0}, 0};
    uuid_error_t err;
    if (!tls_state.initialized) {
        err = uuid_v6_init(&tls_state);
        if (err != UUID_OK) return err;
    }
    return uuid_v6(&tls_state, uuid);
}

/* ── UUID v7 (Unix epoch time-based, monotonic) ──────────────────────── */

uuid_error_t uuid_v7_init(uuid_v7_state_t *state) {
    if (!state) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    state->timestamp_ms = 0;
    state->counter = 0;
    if (uuid_platform_random(state->node, 6) != UUID_OK) {
        return UUID_E_RANDOM;
    }
    state->initialized = 1;
    return UUID_OK;
}

uuid_error_t uuid_v7(uuid_v7_state_t *state, uuid_t *uuid) {
    uint64_t now_ms;
    if (!state || !uuid) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    if (!state->initialized) {
        uuid_set_error("State not initialized");
        return UUID_E_INVALID_ARG;
    }

    now_ms = uuid_current_time_ms();

    if (now_ms > state->timestamp_ms) {
        state->timestamp_ms = now_ms;
        state->counter = 0;
        if (uuid_platform_random(&state->counter, sizeof(state->counter)) != UUID_OK) {
            return UUID_E_RANDOM;
        }
        state->counter &= 0x0FFF; /* 12 bits for randomness */
    } else {
        /* Same millisecond: increment counter for monotonicity */
        state->counter++;
        if (state->counter > 0x0FFF) {
            uuid_set_error("Counter overflow within same millisecond");
            return UUID_E_OVERFLOW;
        }
    }

    /* bytes 0-5: 48-bit timestamp (big-endian) */
    uuid->bytes[0] = (uint8_t)(state->timestamp_ms >> 40);
    uuid->bytes[1] = (uint8_t)(state->timestamp_ms >> 32);
    uuid->bytes[2] = (uint8_t)(state->timestamp_ms >> 24);
    uuid->bytes[3] = (uint8_t)(state->timestamp_ms >> 16);
    uuid->bytes[4] = (uint8_t)(state->timestamp_ms >> 8);
    uuid->bytes[5] = (uint8_t)(state->timestamp_ms);

    /* byte 6: version 7 + top 4 bits of rand_a */
    uuid->bytes[6] = (uint8_t)(((state->counter >> 8) & 0x0F) | (7 << 4));
    /* byte 7: bottom 8 bits of rand_a */
    uuid->bytes[7] = (uint8_t)(state->counter & 0xFF);

    /* bytes 8-9: rand_b (14 bits) + variant */
    {
        uint8_t rb[2];
        if (uuid_platform_random(rb, 2) != UUID_OK) {
            return UUID_E_RANDOM;
        }
        uuid->bytes[8] = (uint8_t)((rb[0] & 0x3F) | 0x80); /* variant 10 */
        uuid->bytes[9] = rb[1];
    }

    /* bytes 10-15: node */
    memcpy(&uuid->bytes[10], state->node, 6);

    return UUID_OK;
}

uuid_error_t uuid_v7_generate(uuid_t *uuid) {
    static UUID_TLS uuid_v7_state_t tls_state = {0, 0, {0}, 0};
    uuid_error_t err;
    if (!tls_state.initialized) {
        err = uuid_v7_init(&tls_state);
        if (err != UUID_OK) return err;
    }
    return uuid_v7(&tls_state, uuid);
}

/* ── UUID v8 (Custom/Vendor-specific) ────────────────────────────────── */

uuid_error_t uuid_v8(uuid_t *uuid, const uint8_t custom[12]) {
    if (!uuid || !custom) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }

    /* Copy 12 custom bytes into bytes 0-11 */
    memcpy(uuid->bytes, custom, 12);

    /* Set version 8 */
    uuid->bytes[6] = (uint8_t)((uuid->bytes[6] & 0x0F) | 0x80);
    /* Set variant 1 (RFC 4122) */
    uuid->bytes[8] = (uint8_t)((uuid->bytes[8] & 0x3F) | 0x80);

    return UUID_OK;
}

uuid_error_t uuid_v8_custom(uuid_t *uuid, const uint8_t custom[16]) {
    if (!uuid || !custom) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    memcpy(uuid->bytes, custom, UUID_SIZE);
    return UUID_OK;
}

/* ── Nil / Max Convenience ────────────────────────────────────────────── */

uuid_error_t uuid_nil(uuid_t *uuid) {
    return uuid_init_nil(uuid);
}

uuid_error_t uuid_max(uuid_t *uuid) {
    return uuid_init_max(uuid);
}

/* ── Namespace Helpers ────────────────────────────────────────────────── */

uuid_error_t uuid_namespace(uuid_namespace_t ns, uuid_t *uuid) {
    if (!uuid) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    if ((int)ns < 0 || (int)ns > 3) {
        uuid_set_error("Invalid namespace enum");
        return UUID_E_NAMESPACE;
    }
    memcpy(uuid->bytes, ns_uuids[(int)ns].bytes, UUID_SIZE);
    return UUID_OK;
}

uuid_error_t uuid_get_namespace(uuid_namespace_t ns, uuid_t *uuid) {
    return uuid_namespace(ns, uuid);
}

/* ── Hashing ──────────────────────────────────────────────────────────── */

uint64_t uuid_hash(const uuid_t *uuid) {
    uint64_t h = 0xcbf29ce484222325ULL; /* FNV offset basis */
    size_t i;
    if (!uuid) return 0;
    for (i = 0; i < UUID_SIZE; i++) {
        h ^= (uint64_t)uuid->bytes[i];
        h *= 0x100000001b3ULL; /* FNV prime */
    }
    return h;
}

/* ── Serialization ────────────────────────────────────────────────────── */

uuid_error_t uuid_serialize(const uuid_t *uuid, uint8_t *buf, size_t size) {
    if (!uuid || !buf) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    if (size < UUID_SIZE) {
        uuid_set_error("Buffer too small");
        return UUID_E_BUFFER_TOO_SMALL;
    }
    memcpy(buf, uuid->bytes, UUID_SIZE);
    return UUID_OK;
}

uuid_error_t uuid_deserialize(uuid_t *uuid, const uint8_t *buf, size_t size) {
    if (!uuid || !buf) {
        uuid_set_error("NULL pointer");
        return UUID_E_INVALID_ARG;
    }
    if (size < UUID_SIZE) {
        uuid_set_error("Buffer too small");
        return UUID_E_BUFFER_TOO_SMALL;
    }
    memcpy(uuid->bytes, buf, UUID_SIZE);
    return UUID_OK;
}

/* ── Sorting ──────────────────────────────────────────────────────────── */

int uuid_compare_asc(const void *a, const void *b) {
    return uuid_compare((const uuid_t *)a, (const uuid_t *)b);
}

int uuid_compare_desc(const void *a, const void *b) {
    return uuid_compare((const uuid_t *)b, (const uuid_t *)a);
}

void uuid_sort_asc(uuid_t *uuids, size_t count) {
    if (uuids && count > 1) {
        qsort(uuids, count, sizeof(uuid_t), uuid_compare_asc);
    }
}

void uuid_sort_desc(uuid_t *uuids, size_t count) {
    if (uuids && count > 1) {
        qsort(uuids, count, sizeof(uuid_t), uuid_compare_desc);
    }
}

/* ── Version Info ─────────────────────────────────────────────────────── */

const char *uuid_version_string(void) {
    return UUID_VERSION_STRING;
}

void uuid_version_info(int *major, int *minor, int *patch) {
    if (major) *major = UUID_VERSION_MAJOR;
    if (minor) *minor = UUID_VERSION_MINOR;
    if (patch) *patch = UUID_VERSION_PATCH;
}
