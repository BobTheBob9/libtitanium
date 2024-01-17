#pragma once

#include <stdint.h>
#include <stdlib.h> // for size_t
#include <math.h> // for maths funcs (pow, log, etc)
#include <limits>

/*

NOTE: NOT IN A NAMESPACE! these are largely shorthand definitions for int types
namespacing them would kind of ruin the whole point of them being shorthand! so we don't 

*/

using u8 = uint8_t;
using byte = u8;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;

static_assert( sizeof(f32) * 8 == 32, "Bad size for type f32! (should be 32 bits/4 bytes)" );
static_assert( sizeof(f64) * 8 == 64, "Bad size for type f64! (should be 64 bits/8 bytes)" );

// does this suck? unsure
// TODO:@ 
#define minof( type ) std::numeric_limits<type>::min()
#define maxof( type ) std::numeric_limits<type>::max()
