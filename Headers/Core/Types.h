/**
 * Types.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <array>
#include <cstdint>
#include <forward_list>
#include <memory>
#include <vector>

/* Integer Types */
typedef uint8_t     uint8;
typedef uint16_t    uint16;
typedef uint32_t    uint32;
typedef uint64_t    uint64;

typedef int8_t      int8;
typedef int16_t     int16;
typedef int32_t     int32;
typedef int64_t     int64;

/* Real Types */
typedef float       real32;
typedef double      real64;

/* String Types */
typedef char        utf8;
typedef char16_t    utf16;
typedef char32_t    utf32;

/* Verify Type Sizes */
static_assert(sizeof(uint8) == 1, "");
static_assert(sizeof(uint16) == 2, "");
static_assert(sizeof(uint32) == 4, "");
static_assert(sizeof(uint64) == 8, "");

static_assert(sizeof(int8) == 1, "");
static_assert(sizeof(int16) == 2, "");
static_assert(sizeof(int32) == 4, "");
static_assert(sizeof(int64) == 8, "");

static_assert(sizeof(real32) == 4, "");
static_assert(sizeof(real64) == 8, "");

static_assert(sizeof(utf8) == 1, "");
static_assert(sizeof(utf16) == 2, "");
static_assert(sizeof(utf32) == 4, "");

#endif /* TYPES_H_ */
